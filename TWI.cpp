///
///	TWI.cpp - A simplified TWI/I2C API library for Arduino
///
///	Copyright (c) 2021 Jeff Penfold.  All right reserved.
///
///	This library is free software; you can redistribute it and/or
///	modify it under the terms of the GNU Lesser General Public
///	License as published by the Free Software Foundation; either
///	version 2.1 of the License, or (at your option) any later version.
///
///	This library is distributed in the hope that it will be useful,
///	but WITHOUT ANY WARRANTY; without even the implied warranty of
///	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
///	Lesser General Public License for more details.
///
///	You should have received a copy of the GNU Lesser General Public
///	License along with this library; if not, write to the Free Software
///	Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301
///	USA
///

#include <limits.h>
#include <stdlib.h>
#include <inttypes.h>
#include <avr/pgmspace.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <compat/twi.h>

//
//	The configuration of the firmware.
//
#include "Configuration.h"
#include "Trace.h"

//
//	Environment for this module.
//
#include "Arduino.h"
#include "pins_arduino.h"
#include "Environment.h"
#include "TWI.h"

//
//	The following data structures capture a simplified view of the
//	above titles and maps them onto its functional implementation.
//
//	Each will be supported by its Transaction Diagram where the following
//	layout and symbols will be used:
//
//		Master:	master transmission components
//		Slave:	slave transmission components
//
//			time >>>--->
//
//	The components of a transmission are:
//
//		S	Start condition
//		T	Repeat Start condition
//		P	Stop condition
//		R	RW bit set for Reading (1)
//		W	RW bit set for Writing (0)
//		A	Acknowledge bit (?)
//		N	Not-Acknowledge bit (?)
//		X	Value Ignored
//
//		a	7 bits of the slave address
//		d	8 data bits
//		^	Repeated section of the diagram
//
//	The (6.5.1) Quick Commands boil down to one of two possible
//	single bytes transmission from the master to the slave.  This
//	is effectively just the target slave address with the Read
//	bit either 1 (for reading) or 0 (for writing).  No data is read
//	of transmitted.
//
//	TWI_MODE_QUICK_READ
//	-------------------
//	Master:	SaaaaaaaR P
//	Slave:           A
//
//	TWI_MODE_QUICK_WRITE
//	--------------------
//	Master:	SaaaaaaaW P
//	Slave:           A
//
static const TWI::machine_state TWI::mode_quick_read[] PROGMEM = {
	state_state, state_start_complete,
	state_adrs_read, state_adrs_ack,
	state_stop, state_good_callback
};
static const TWI::machine_state TWI::mode_quick_write[] PROGMEM = {
	state_state, state_start_complete,
	state_adrs_write, state_adrs_ack,
	state_stop, state_good_callback
};

//
//	All "just send data" commands (6.5.2 Send Byte, 6.5.4 Write
//	Byte/Word, 6.5.7 Block Write/Read, 6.5.10 Write 32 protocol,
//	6.5.12 Write 64 protocol) conform to the following underlying
//	hardware exchange:
//
//	TWI_MODE_SEND_DATA
//	------------------
//	Master:	SaaaaaaaW dddddddd P
//	Slave:	         A        A
//	Repeat:	          ^^^^^^^^^
//
static const TWI::machine_state TWI::mode_send_data[] PROGMEM = {
	state_state, state_start_complete,
	state_adrs_write, state_adrs_ack,
	state_send_byte, state_send_ack_loop,
	state_stop, state_good_callback
};
//
//	The (6.5.3) Receive Byte is the only command which can directly
//	obtain any data form the slave *without* first asking for
//	something:
//
//	TWI_MODE_RECEIVE_BYTE
//	---------------------
//	Master:	SaaaaaaaR         NP
//	Slave:	         Adddddddd
//
static const TWI::machine_state TWI::mode_receive_byte[] PROGMEM = {
	state_state, state_start_complete,
	state_adrs_read, state_adrs_ack,
	state_recv_ready, state_recv_byte_loop,
	state_stop, state_good_callback
};
//
//	All of the remaining commands (6.5.4 Write Byte/Word, 6.5.5
//	Read Byte/Word, 6.5.6 Process Call, 6.5.8 Block Write-Block
//	Read Process Call, 6.5.9 SMBus Host Notify protocol, 6.5.11
//	Read 32 protocol, 6.5.13 Read 64 protocol) conform to a common
//	data exchange protocol:
//
//	TWI_MODE_DATA_EXCHANGE
//	----------------------
//	Master:	SaaaaaaaW dddddddd TSaaaaaaaR         A          NP
//	Slave:	         A        A          Adddddddd   dddddddd
//	Repeat:	          ^^^^^^^^^           ^^^^^^^^^
//
static const TWI::machine_state TWI::mode_data_exchange[] PROGMEM = {
	state_state, state_start_complete,
	state_adrs_write, state_adrs_ack,
	state_send_byte, state_send_ack_loop,
	state_restart, state_start_complete,
	state_adrs_read, state_adrs_ack,
	state_recv_ready, state_recv_byte_loop,
	state_stop, state_good_callback
};

//
//	twi_abortTransaction
//	--------------------
//
//	This incomplete sequence is not applied in the first instance
//	but is used to "redirect" a transaction which has failed and
//	needs to completed with subsequent tidy up.
//
static const TWI::machine_state abort_transaction[] PROGMEM = {
	state_stop, state_fail_callback
};


//
//	The bitrate table
//	-----------------
//
//	The order of the entries in this table is important; they
//	must be in sorted freq order, largest first.  The
//	routine will always return the result which is the speed
//	requested or nearest (but slower) speed available.
//
//	All speeds which require the TWBR to be less than 10
//	are explicitly excluded as these are (allegedly) too fast
//	for the MCU to support reliably.
//
static const TWI::bitrate TWI::bitrates[] PROGMEM = {
#if	F_CPU == 20000000
	{	40,	17,	0	},
	{	35,	20,	0	},
	{	30,	25,	0	},
	{	25,	32,	0	},
	{	20,	42,	0	},
	{	15,	58,	0	},
	{	10,	92,	0	},
	{	9,	103,	0	},
	{	8,	117,	0	},
	{	7,	33,	1	},
	{	6,	39,	1	},
	{	5,	48,	1	},
	{	4,	60,	1	},
	{	3,	81,	1	},
	{	2,	123,	1	},
	{	1,	62,	2	},
	
#elif	F_CPU == 16000000
	{	40,	12,	0	},
	{	35,	14,	0	},
	{	30,	18,	0	},
	{	25,	24,	0	},
	{	20,	32,	0	},
	{	15,	45,	0	},
	{	10,	72,	0	},
	{	9,	80,	0	},
	{	8,	92,	0	},
	{	7,	106,	0	},
	{	6,	125,	0	},
	{	5,	38,	1	},
	{	4,	48,	1	},
	{	3,	64,	1	},
	{	2,	98,	1	},
	{	1,	49,	2	},
	
#elif	F_CPU == 12000000
	{	30,	12,	0	},
	{	25,	16,	0	},
	{	20,	22,	0	},
	{	15,	32,	0	},
	{	10,	52,	0	},
	{	9,	58,	0	},
	{	8,	67,	0	},
	{	7,	77,	0	},
	{	6,	92,	0	},
	{	5,	112,	0	},
	{	4,	35,	1	},
	{	3,	48,	1	},
	{	2,	73,	1	},
	{	1,	37,	2	},
	
#elif	F_CPU == 8000000
	{	20,	12,	0	},
	{	15,	18,	0	},
	{	10,	32,	0	},
	{	9,	36,	0	},
	{	8,	42,	0	},
	{	7,	49,	0	},
	{	6,	58,	0	},
	{	5,	72,	0	},
	{	4,	92,	0	},
	{	3,	125,	0	},
	{	2,	48,	1	},
	{	1,	98,	1	},
	
#elif	F_CPU == 4000000
	{	10,	12,	0	},
	{	9,	14,	0	},
	{	8,	17,	0	},
	{	7,	20,	0	},
	{	6,	25,	0	},
	{	5,	32,	0	},
	{	4,	42,	0	},
	{	3,	58,	0	},
	{	2,	92,	0	},
	{	1,	48,	1	},
	
#elif	F_CPU == 2000000
	{	5,	12,	0	},
	{	4,	17,	0	},
	{	3,	25,	0	},
	{	2,	42,	0	},
	{	1,	92,	0	},
	
#elif	F_CPU == 1000000
	{	2,	17,	0	},
	{	1,	42,	0	},
#else
#error "Unrecognised CPU Clock Speed"
#endif
	//
	//	End of list
	//
	{	0,	0,	0	}
};


//
//	The TWI Object
//	==============
//
//	Define the object controlling the TWI device
//
TWI twi;

//
//	Two Wire Interrupt Service Routine.
//	===================================
//
//	This routine simply calls the generic event/state handler
//	passing in the state value being notified.
//
//
ISR( TWI_vect ) {
	twi.process_event();
}


//
//	EOF
//
