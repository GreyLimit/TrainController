///
///	TWI.h - A simplified TWI/I2C API library for Arduino
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

//
//	This collection of routines will (within reasonable efforts) implement
//	the protocol as outlined in the following document:
//
//		http://smbus.org/specs/SMBus_3_1_20180319.pdf
//
//	How this document has been interpreted, and the design decisions which
//	derive from these interpretations will be captured in the its
//	implementation file.
//

#ifndef _TWI_H_
#define _TWI_H_

#include <Arduino.h>
#include <inttypes.h>
#include <util/twi.h>

#include "Environment.h"
#include "Configuration.h"
#include "Parameters.h"
#include "Trace.h"
#include "Critical.h"
#include "Console.h"
#include "Task.h"

//
//	This code divides all TWI clock frequencies by 10K
//	to make them fit inside a single byte.
//
//	This "relationship" between actual frequency and
//	its internal representation is not mathematical as
//	the clock speed applied to the TWI hardware is
//	determined using a lookup table.  It would, therefore,
//	be perfectly feasible to have a non-linear association
//	as long as larger numbers meant faster speeds.
//
//	Default Frequency is 100KHz.
//
#ifndef TWI_FREQ
#define TWI_FREQ		10
#endif

//
//	Allow configuration of the internal queue length.  This
//	define the maximum number of pending TWI actions that the
//	driver can sustain.  In reality this effectively defines
//	the number of TWI attached devices which can be communicated
//	with in a parallel manner, but not the actual maximum number
//	of attached devices.
//
#ifndef TWI_MAX_QUEUE_LEN
#define TWI_MAX_QUEUE_LEN	4
#endif

//
//	Specify a delay period used during a hardware reset.  This is
//	the time between "turning off" the device and the following
//	re-enable actions.  This is (hopefully) enough time for the
//	TWI attached device causing trouble to reset itself and so enable
//	the bus to return to normal operation.
//
#ifndef TWI_HARDWARE_RESET
#define TWI_HARDWARE_RESET	1000
#endif

//
//	Design Discussion
//	=================
//
//	The "SMBus_3_1_20180319.pdf" (http://smbus.org/specs/SMBus_3_1_20180319.pdf)
//	document provides (in a somewhat less than entirely clear manner)
//	two levels of information that can be viewed as different levels
//	(or tiers) of any "networking" protocol stack.  These are:
//
//	o	Abstract data format guide lines for the nature of the
//		conversation between devices on an I2C/TWI bus
//
//	o	Hardware specific action sequences dictating the "how"
//		to enables two devices on an I2C/TWI bus to successfully
//		exchange data which was outlined above.
//
//	Of these two items the second is a mandatory element that must be
//	implemented accurately to ensure communication with the widest
//	range of devices.  These routines do this.
//
//	The implementation of the "content" of the communication will be
//	deligated to the software using these routines.
//

//
//	From "SMBus_3_1_20180319.pdf" The following two structural elements
//	have been specifically extracted for encoding within these routines:
//
//	o	Transaction Diagrams detailing the *exact* hardware level
//		exchanges between master and slave devices.
//
//	o	Exchange "modes" which name and capture the various ways
//		a master and a slave can talk.  Each named mechanism will
//		be detailed using Transaction Diagrams.
//
//		The document above lists the following "protocols" that
//		can be used between master and slave devices:
//
//		6.5.1	Quick Commands
//
//		6.5.2	Send Byte
//		6.5.3	Receive Byte
//		6.5.4	Write Byte/Word
//		6.5.5	Read Byte/Word
//
//		6.5.6	Process Call
//
//		6.5.7	Block Write/Read
//		6.5.8	Block Write-Block Read Process Call
//
//		6.5.9	SMBus Host Notify protocol
//
//		6.5.10	Write 32 protocol
//		6.5.11	Read 32 protocol
//		6.5.12	Write 64 protocol
//		6.5.13	Read 64 protocol
//
//	It is worth noting that the I2C/TWI hardware protocol is significantly
//	asymetric in the following ways:
//
//	o	There is a master/slave relationship between two devices
//		communicating though any device can initiate an exchange
//		and will therefore become the master for that exchange.
//
//	o	The nature of the above means that while it is possible for
//		master to transmit a variable amount of data to the slave
//		with the slave clearly being able to determine the end of the
//		data.  It is (apparently) impossible for the master to
//		receive a variable amount of data as there is no underlying
//		hardware based mechanism which enables the slave to say to
//		the master "end of data".
//
//		The consequence of this is that the API is also asymmetric
//		requiring the master to always know, ahead of time, how much
//		data the slave will send back.  This must become a feature
//		of the application level protocol ensuring both master and
//		slave agree on transfer sizes.
//

//
//	Two Wire Status values
//	----------------------
//
//	In the file <util/twi.h> are the definitions for the
//	status values which can be found in the top 5 bits of the
//	TWSR register (with the bottom 3 being used for other purposes).
//
//	The defines made in this file are fixed as they are derived from
//	hardware embedded in the TWI implementation in the AVR chips.
//
//	A quick analysis of these values shows that once shifted right 3
//	bits there is little "logical" meaning to the individual bits, but
//	form a number sequence from 1 to 25.  Within this range they codes
//	are grouped into four groups and a scattering of "others".  These
//	are:
//
//		Master Initialisation	TW_START, TW_REP_START
//		Master Transmit		TW_MT_*
//		Master Receive		TW_MR_*
//		Slave Receive		TW_SR_*
//		Slave Transmit		TW_ST_*
//		"Others"		No Info, Bus Error
//
//	It is worth noting that within TW_MT_ and TW_MR_ there is
//	a duplicated entry: "TW_MT_ARB_LOST" and "TW_MR_ARB_LOST"
//	with the same value: 0x38.  This is, however, unique to
//	the Master set of codes as the Slave codes contain their
//	own specific versions.
//
//	To simplify some handling of the TWI status values the following
//	macros are provided to allow "classification" of the status code
//	before specific code-by-code analysis is undertaken.
//

//
//	Master related status values:
//
//						TWSR & 0xf8	TWSR >> 3
//
//		TW_START			0x08		1
//		TW_REP_START			0x10		2
//
//		TW_MT_SLA_ACK			0x18		3
//		TW_MT_SLA_NACK			0x20		4
//		TW_MT_DATA_ACK			0x28		5
//		TW_MT_DATA_NACK			0x30		6
//
//		TW_MT_ARB_LOST			0x38		7
//		TW_MR_ARB_LOST			0x38		7
//
//		TW_MR_SLA_ACK			0x40		8
//		TW_MR_SLA_NACK			0x48		9
//		TW_MR_DATA_ACK			0x50		10
//		TW_MR_DATA_NACK			0x58		11
//
#define TW_MASTER_STATUS(x)	(((x)>=TW_START)&&((x)<=TW_MR_DATA_NACK))

//
//		Slave related status values:
//
//						TWSR & 0xf8	TWSR >> 3
//
//		TW_SR_SLA_ACK			0x60		12
//		TW_SR_ARB_LOST_SLA_ACK		0x68		13
//		TW_SR_GCALL_ACK			0x70		14
//		TW_SR_ARB_LOST_GCALL_ACK	0x78		15
//		TW_SR_DATA_ACK			0x80		16
//		TW_SR_DATA_NACK			0x88		17
//		TW_SR_GCALL_DATA_ACK		0x90		18
//		TW_SR_GCALL_DATA_NACK		0x98		19
//		TW_SR_STOP			0xA0		20
//
//		TW_ST_SLA_ACK			0xA8		21
//		TW_ST_ARB_LOST_SLA_ACK		0xB0		22
//		TW_ST_DATA_ACK			0xB8		23
//		TW_ST_DATA_NACK			0xC0		24
//		TW_ST_LAST_DATA			0xC8		25
//
#define TW_SLAVE_STATUS(x)	(((x)>=TW_SR_SLA_ACK)&&((x)<=TW_ST_LAST_DATA))

//
//	"Others"
//
//						TWSR & 0xf8	TWSR >> 3
//
//		TW_NO_INFO			0xF8		31	
//		TW_BUS_ERROR			0x00		0
//

//
//	In addition to the values defined in the file <util/twi.h>
//	these code specific value are also set into this variable.
//
//	Looking at the above table it can be seen that the values from
//	26 to 30 are not used, and could be hijacked for synthetic
//	status values, if that were required.
//
#define TW_NO_ERROR	0xff


//
//	Define the Twin Wire Interface (TWI) also known as I2C or IIC.
//
class TWI {
public:
	//
	//	Declare the frequency of the TWI interface to the
	//	attached devices.
	//
	static const byte	frequency = TWI_FREQ;

	//
	//	Provide a symbol which will mean "the highest possible
	//	speed"
	//
	//	At the moment "255" would mean approximately 2.5 MHz
	//	which is an improbable TWI speed.
	//
	static const byte	maximum_frequency = 255;

	//
	//	Define a period of Microseconds which the firmware will
	//	wait during a hardware reset of the TWI
	//
	//	This is a last resort attempt to put things back into an
	//	operation state.
	//
	static const byte	reset_delay_us = 50;

	//
	//	Define the Lowest and Highest valid addresses.
	//
	static const byte	lowest_address = 16;
	static const byte	Highest_address = 127;
	
	//
	//	Define the size of the pending queue implemented by these functions.
	//
	static const byte	maximum_queue = TWI_MAX_QUEUE_LEN;

	//
	//	Define the number of micro-seconds we pause operation
	//	of the TWI device in the event of a hardware reset.
	//
	static const word	hardware_reset = TWI_HARDWARE_RESET;

	//
	//	These are the error codes passed to the error reporting
	//	routine.
	//
	enum error_code : byte {
		error_none = 0,		//	No Error, success!
		error_timed_out,	//	Action timed out
		error_queue_full,	//	Master transaction queue full
		error_hw_reset,		//	TWI Hardware has been reset
		error_address,		//	Slave address not acknowledged
		error_write_fail,	//	Write data to slave not acknowledged
		error_read_fail,	//	n/a
		error_transaction,	//	Unexpected state change in transaction
		error_starting,		//	Transaction start failed
		error_truncated,	//	Slave transaction truncated
		error_overflow,		//	Slave recv buffer overflowed, data lost
		error_underflow,	//	Slave send buffer too small, zero filling
		error_no_buffer,	//	No buffer space has been allocated
		error_dropped		//	Transaction was dropped before completion
	};

private:
	//
	//	Declare the data structure the TWI code relies upon.
	//
	
	//
	//	The following PROGMEM table facilitates the implementation of
	//	bit rate configuration.
	//
	//	Note: the TWI clock speed is calculated from:
	//
	//		TWI_CLOCK = F_CPU / ( 16 + 2 * TWBR * 4 ^ TWPS )
	//
	//	Re-arranged to give TWBR:
	//
	//		 TWBR = ( F_CPU / TWI_CLOCK - 16 ) / ( 2 * 4 ^ TWPS )
	//
	//	All divisions are rounded down (floor'd) to the next lowest integer.
	//
	//	A table is used as this removes the requirement for the
	//	mathematics library, and also allows for a (more) simplified
	//	setting of the scale factor at the same time.
	//
	struct bitrate {
		byte	freq,		// freq / 10Khz
			twbr,		// Range 10 .. 255
			twps;		// Range 0 .. 3
	};
	//
	//	The bitrate table:
	//
	static const bitrate bitrates[] PROGMEM;


	//
	//	The following enumeration enable the Transaction Diagrams
	//	to be directly encoded and used as the basis for the
	//	interrupt service routine and supporting service routines.
	//

	enum machine_state : byte {
		//
		//		Emit start condition (handled by event loop code)
		//		Emit a restart condition.
		//		Start or restart has completed.
		//
		state_start,
		state_restart,
		state_start_complete,

		//
		//		Emit a stop condition.
		//
		state_stop,

		//
		//		Emit target address with Read.
		//		Emit target address with Write.
		//		Wait for address Acknowledgement.
		//
		state_adrs_read,
		state_adrs_write,
		state_adrs_ack,

		//
		//		Send one byte of data
		//		Wait for slave Acknowledgement and...
		//		...loop back 1 if data still to send.
		//
		state_send_byte,
		state_send_ack_loop,

		//
		//		Tell TWI hardware that we are waiting
		//		for data.  Set to use NAck if this will
		//		be the last byte, set for Ack if this is
		//		not the last byte.
		//		The receive action actually gets the data
		//		and loops back if the byte was Ackd or
		//		rolls to the next action if it was NAckd.
		//
		state_recv_ready,
		state_recv_byte_loop,

		//
		//	These two action indicate the "end" of a set of
		//	actions and are used to indicate a transaction
		//	is complete (with appropriate callback executed).
		//
		//		Execute successful master callback routine
		//		Execute unsuccessful master callback routine
		//
		//	 Both handled by event loop code.
		//
		state_good_callback,
		state_fail_callback
	};

	//
	//	The following static tables provide the state machines
	//	which handle the execution of any given TWI activity
	//	as defined in the SMBus Specification.
	//
	static const machine_state mode_quick_read[] PROGMEM;
	static const machine_state mode_quick_write[] PROGMEM;
	static const machine_state mode_send_data[] PROGMEM;
	static const machine_state mode_receive_byte[] PROGMEM;
	static const machine_state mode_data_exchange[] PROGMEM;
	static const machine_state abort_transaction[] PROGMEM;

	//
	//	Define a data structure used to queue pending
	//	exchange requests initiated from this MCU as a
	//	master device.
	//
	struct transaction {
		//
		//	The list of actions this needs to implement, and the
		//	reset address if the transaction needs to be restarted
		//	mid flight.
		//
		const machine_state	*action;
		//
		//	Who are we trying to exchange data with?
		//
		byte			target;
		//
		//	The address of the buffer space to be used as
		//	the source data and repository for the subsequent
		//	reply.  Next forms a pointer used to access the
		//	buffer space.
		//
		byte			*buffer;
		//
		//	The next field is used to "step through" the buffer
		//	when either sending or receiving data.
		//
		byte			next;
		//
		//	The number of bytes in the buffer space which form
		//	the data to be sent from the master to the slave
		//
		byte			send;
		//
		//	The number of bytes expected back from the slave
		//	which will be placed into the buffer
		//
		byte			recv;
		//
		//	The address of the fllag to be set TRUE when the
		//	actions has been completed (success or fail).
		//
		void			*flag;
		//
		//	The address  where the error code of action is placed.
		//
		error_code		*result;
	};

	//
	//	Define the pending exchange queue and indexes into it.
	//
	transaction	_queue[ maximum_queue ];	// This is the queue
	byte		_queue_len,			// number of queued exchanges
			_queue_in,			// index of next free slot
			_queue_out;			// index of next pending exchange

	//
	//	The following pointer is used by the ISR
	//	as the pointer to the "active" exchange
	//	record.
	//
	transaction	*_active;

	//
	//	Declare the flag value used to notify the task management
	//	this object requires CPU time to process an outstanding
	//	event.
	//
	bool		*_flag;

	//
	//	The following TWI "primitive" operations are based on the
	//	content of the document:
	//
	//		ATmega48A/PA/88A/PA/168A/PA/328/P
	//		megaAVR ® Data Sheet
	//
	//	Produced by Microchip, downloaded from:
	//
	//	https://ww1.microchip.com/downloads/en/DeviceDoc/ATmega48A-PA-88A-PA-168A-PA-328-P-DS-DS40002061B.pdf
	//

	//
	//	void start( void )
	//	------------------
	//
	//	Send start condition (also doubles up as a restart)
	//
	static inline void start( void ) {
		//
		//	TWI Control Register, TWCR
		//
		//	Bit	Value	Meaning
		//
		//	TWINT	1	Initiate action (resets bit to 0)
		//	TWEA	slave	TWI Slave enable / (N)Ack reply
		//	TWSTA	1	Transmit Start condition
		//	TWSTO	0	Transmit Stop condition
		//	TWWC	X	TWDR Write Collision detected
		//	TWEN	1	TWI Hardware enable
		//	TWIE	int	TWI Interrupt enable
		//
		TRACE_TWI( console.write( 'S' ));
		//
		TWCR = bit( TWIE ) | bit( TWEN ) | bit( TWSTA ) | bit( TWINT );
	}

	//
	//	bool action_complete( void )
	//	-------------------------------
	//
	//	Checks the value of the Initiate flag (TWINT) which will
	//	return to 1 when the previous activity has been completed.
	//
	static inline bool action_complete( void ) {
		//
		//	TWCR	TWI Control Register
		//
		//	TWINT	Action complete flag.
		//
		//		Returns 0 while an action is in
		//		progress, and becomes 1 when the
		//		action has completed and the TWI
		//		is ready for the next action.
		//
		TRACE_TWI( console.write( 'C' ));
		//
		return( TWCR & bit( TWINT ));
	}

	//
	//	void send_byte( byte data )
	//	------------------------------
	//
	//	Transmit a byte of data
	//
	static inline void send_byte( byte data ) {
		//
		//	TWI data IO register, TWDR
		//
		//	TWI Control Register, TWCR
		//
		//	Bit	Value	Meaning
		//
		//	TWINT	1	Initiate action (resets bit to 0)
		//	TWEA	slave	TWI Slave enable / (N)Ack reply
		//	TWSTA	0	Transmit Start condition
		//	TWSTO	0	Transmit Stop condition
		//	TWWC	X	TWDR Write Collision detected
		//	TWEN	1	TWI Hardware enable
		//	TWIE	int	TWI Interrupt enable
		//
		TRACE_TWI( console.write( '>' ));
		TRACE_TWI( console.print_hex( data ));
		//
		TWDR = data;
		TWCR = bit( TWIE ) | bit( TWEN ) | bit( TWINT );
	}

	//
	//	void send_target( byte adrs, bool writing )
	//	----------------------------------------------
	//
	//	Send the address byte of the slave being targetted by
	//	this transmission. The argument writing indicates if
	//	the following transmission involves the master writing
	//	to the slave (true) or readin from thw slave (false).
	//
	static inline void send_target( byte adrs, bool writing ) {
		//
		TRACE_TWI( console.print_hex( adrs ));
		TRACE_TWI( console.write( writing? 'W': 'R' ));
		//
		//	send_byte(( adrs << 1 )|( writing? TW_WRITE: TW_READ ));
		//
		TWDR = ( adrs << 1 )|( writing? TW_WRITE: TW_READ );
		TWCR = bit( TWIE ) | bit( TWEN ) | bit( TWINT );
	}

	//
	//	void read_ack( bool ack )
	//	----------------------------
	//
	//	This routine is only about sending an Acknowledgement
	//	or NOT Acknowledgement as a result of a byte being
	//	delivered to us.
	//
	static inline void read_ack( bool ack ) {
		//
		//	TWI Control Register, TWCR
		//
		//	Bit	Value	Meaning
		//
		//	TWINT	1	Initiate action (resets bit to 0)
		//	TWEA	ack	TWI Slave enable / (N)Ack reply
		//	TWSTA	0	Transmit Start condition
		//	TWSTO	0	Transmit Stop condition
		//	TWWC	X	TWDR Write Collision detected
		//	TWEN	1	TWI Hardware enable
		//	TWIE	int	TWI Interrupt enable
		//
		TRACE_TWI( console.write( ack? 'A': 'N' ));
		//
		TWCR = bit( TWIE ) |( ack? bit( TWEA ): 0 )| bit( TWEN ) | bit( TWINT );
	}

	//
	//	byte read_byte( void )
	//	----------------------
	//
	//	Return the byte which has just been delivered to the system
	//
	static inline byte read_byte( void ) {
	#ifdef ENABLE_TRACE_TWI
		//
		//	For debugging..
		//
		byte	data;
		
		data = TWDR;
		//
		TRACE_TWI( console.write( '<' ));
		TRACE_TWI( console.print_hex( data ));
		//
		//	TWI Data Register, TWDR
		//
		return( data );
	#else
		//
		//	TWI Data Register, TWDR
		//
		return( TWDR );
	#endif
	}

	//
	//	void stop( void )
	//	---------------------
	//
	//	Tell the remote slave device that we are done, and
	//	release the bus for other devices to use.
	//
	static inline void stop( void ) {
		//
		//	TWI Control Register, TWCR
		//
		//	Bit	Value	Meaning
		//
		//	TWINT	1	Initiate action (resets bit to 0)
		//	TWEA	X	TWI Slave enable / (N)Ack reply
		//	TWSTA	0	Transmit Start condition
		//	TWSTO	1	Transmit Stop condition
		//	TWWC	X	TWDR Write Collision detected
		//	TWEN	1	TWI Hardware enable
		//	TWIE	X	TWI Interrupt enable
		//
		TRACE_TWI( console.write( 'P' ));
		TRACE_TWI( console.println());
		//
		TWCR = bit( TWIE ) | bit( TWEN ) | bit( TWSTO ) | bit( TWINT );
	}

	//
	//	void clear_bus( void )
	//	-------------------------
	//
	//	Return the TWI Bus to a "quiessed" state ready for the next
	//	transmission.
	//
	static inline void clear_bus( void ) {
		//
		//	TWI Control Register, TWCR
		//
		//	Bit	Value	Meaning
		//
		//	TWINT	1	Initiate action (resets bit to 0)
		//	TWEA	X	TWI Slave enable / (N)Ack reply
		//	TWSTA	0	Transmit Start condition
		//	TWSTO	0	Transmit Stop condition
		//	TWWC	X	TWDR Write Collision detected
		//	TWEN	1	TWI Hardware enable
		//	TWIE	X	TWI Interrupt enable
		//
		TWCR = bit( TWIE ) | bit( TWEN ) | bit( TWINT );
	}

	//
	//	void reset_hardware( void )
	//	------------------------------
	//
	//	This is a final "let us try power off then on" the TWI
	//	hardware to regain control of the system.
	//
	static void reset_hardware( void ) {
		byte	twcr;
		
		//
		//	We are only focused on the hardware, we will not
		//	change any firmware settings.
		//
		//	Save current value.
		//
		twcr = TWCR;
		//
		//	Clear the control register to reset hardware.
		//
		TWCR = 0;
		//
		//	Pause execution of the TWI code.  For this
		//	to simplify this code we use the "in-line"
		//	delay provided by the event_timer module. 
		//
		event_timer.inline_delay( USECS( hardware_reset ));
		
		//
		//	Now put the control register back, but without
		//	a TWSTA, TWSTO or TWINT set.
		//
		TWCR = twcr & ~( TWSTA | TWSTO | TWINT );
	}

	//
	//	byte twi_state( void )
	//	---------------------------
	//
	static inline byte state( void ) {
		return( TWSR & 0xf8 );
	}


	//
	//	void drop_action( void )
	//	------------------------
	//
	//	Routine to capture the dropping of the current master
	//	transaction and returning it to the pending queue as
	//	a free record.
	//
	static void drop_action( void ) {
		if( _active ) {
			Critical code;

			//
			//	We need to tell the caller of the action that
			//	it has been dropped, so...
			//
			_action->result = error_dropped;
			_action->flag = true;

			//
			//	Now remove from active transactions
			//
			_active = NIL( transaction );
			_queue_len -= 1;
			if(( _queue_out += 1 ) >= maximum_queue ) _queue_out = 0;
		}
	}

	//
	//	void next_action( void )
	//	----------------------------------
	//
	//	Routine to capture the dropping of the current master
	//	transaction and returning it to the pending queue as
	//	a free record.
	//
	static void next_action( void ) {
		Critical code;
		
		//
		//	Are we replacing a now finished job?
		//
		if( _active ) {
			//
			//	Yes.
			//
			_queue_len -= 1;
			if(( _queue_out += 1 ) >= maximum_queue ) _queue_out = 0;
		}
		//
		//	Is there anything left in the queue?
		//
		if( _queue_len ) {
			//
			//	Yes.
			//
			_active = &( _queue[ _queue_out ]);
		}
		else {
			//
			//	No.
			//
			_active = NIL( transaction );
		}
	}


	//
	//	bool queue_transaction( const machine_state *action, byte address, byte *buffer, byte send, byte recv, bool *flag, error_code *result )
	//	------------------------------------------------------------------------------------------------------------------------------------------
	//
	//	Initiates an asynchronous data exchange with a specified slave device, with the parameters supplying
	//	the required details:
	//
	//		action		The list of actions which implement this transaction
	//		address		The slave address being targeted
	//		buffer		The address of a byte array where sent and returned data will be stored.
	//		send		The number of bytes to send from the buffer area
	//		recv		The number of bytes which will be returned into the buffer area
	//		flag		Address of a flag to set to true when the command completes
	//		result		Address of an error_code variable where the status of the completed command is placed
	//
	//	Returns true if the exchange has been successfully queued, false otherwise.
	//
	//	Note:	The buffer space address passed into this routine is kept and accessed
	//		asynchronously from the main flow of the program.  To this end the area
	//		must be allocated in such a way that it remains valid from the point of
	//		calling this routine to the point at which the flag is set.
	//
	//		Also note that if multiple exchange requests are queued, then each must
	//		have its own distinct buffer space to avoid data being over written.
	//
	//	The API reserves the right to implement a pending exchange queue of arbitrary length
	//	from zero entries.  The implication is that with a queue of length zero all
	//	exchange requests will fail if any exchange is currently taking place.  The code
	//	must be prepared for this possibility as the same situation would apply with any
	//	size queue which has been filled with pending requests.
	//
	bool queue_transaction( const machine_state *action, byte address, byte *buffer, byte send, byte recv, bool *flag, byte *result ) {
		transaction	*ptr;

		ASSERT( flag != NIL( bool ));
		ASSERT( result != NIL( error_code ));

		//
		//	Is there space for a new request?
		//
		if( twi_queue_len >= maximum_queue ) return( false );
		//
		//	Locate the next available slot and adjust queue
		//	appropriately.
		//
		ptr = &( twi_queue[ _queue_in++ ]);
		if( _queue_in >= maximum_queue ) _queue_in = 0;
		//
		//	"ptr" is the address of our queue record, so we can now
		//	fill in this record with the supplied details.
		//
		ptr->action = action;
		ptr->target = address;
		ptr->buffer = buffer;
		ptr->next = 0;			// Also set up by the START and RESTART actions
		ptr->send = send;
		ptr->recv = recv;
		ptr->flag = flag;
		ptr->result = result;
		//
		//	Last action, increase the queue length.
		//
		_queue_len++;
		return( true );
	}

public:
	//
	//	void set_frequency( byte freq )
	//	-------------------------------
	//
	//	Set the TWI Clock frequency by setting the TWBR and
	//	TWPS (TWSR 1:0) registers.
	//
	//	The frequency is (currently) passed in as a multiple
	//	of 10KHz (i.e. if freq == 25 then clock = 250,000 Hz)
	//
	//	There is an assumption, a reasonable one I believe, that
	//	the table that this routine accesses contains at least ONE line
	//	for a valid TWI/I2C clock rate.  Failure to ensure this will
	//	render any use of the TWI hardware pretty much impossible.
	//
	void set_frequency( byte freq ) {
		const bitrate	*look;
		byte		found,
				twbr,
				twps;

		twbr = 0;
		twps = 0;
		look = bitrates;
		while(( found = progmem_read_byte( look->freq ))) {
			twbr = progmem_read_byte( look->twbr );
			twps = progmem_read_byte( look->twps );
			if( found <= freq ) break;
			look++;
		}
		TWSR = twps;
		TWBR = twbr;
	}

	//
	//	byte best_frequency( byte freq )
	//	--------------------------------
	//
	//	Return the identity of the best available
	//	clock speed at (or below) the value passed in.
	//
	byte best_frequency( byte freq ) {
		const bitrate	*look;
		byte		found;

		look = bitrates;
		while(( found = progmem_read_byte( look->freq ))) {
			if( found <= freq ) break;
			look++;
		}
		return( found );
	}

	//
	//	byte lowest_frequency( void )
	//	-----------------------------
	//
	//	Return the identity of the slowest available
	//	clock speed.  This could be useful when a master
	//	and a slave negotiate the best available
	//	common speed .. you need to start from somewhere.
	//
	//	This *should* always come out as "1", and (currently)
	//	reflects a speed of 10Kbps
	//
	byte lowest_frequency( void ) {
		const bitrate	*look;
		byte		found,
				last;

		look = bitrates;
		last = maximum_frequency;
		while(( found = progmem_read_byte( look->freq ))) {
			last = found;
			look++;
		}
		return( last );
	}


	//
	//	Constructor initialises the TWI to the default
	//
	void TWI( void ) {
		//
		//	Prepare the queue as empty.
		//
		_queue_len = 0;
		_queue_in = 0;
		_queue_out = 0;

		//
		//	Initially there is no active exchange.
		//
		_active = NIL( transaction );

		//
		//	Disable slave configuration
		//
		TWAR = 0;

		//
		//	Optionally enable internal pull-up resistors required
		//	for TWI protocol.
		//
		pinMode( SDA, INPUT_PULLUP );
		pinMode( SCL, INPUT_PULLUP );

		//
		//	Initialise TWI pre-scaler and default bit rate.
		//
		set_frequency( frequency );

		//
		//	Attach ourselves to the task manager so that
		//	we process the interrupts asynchronously.
		//
		_flag = false;
		task_manager.add_task( this, &_flag );

		//
		//	Enable TWI module and TWI interrupt.
		//
		TWCR = bit( TWIE ) | bit( TWEN );
	}

	//
	//	The (6.5.1) Quick Commands
	//	--------------------------
	//
	bool quick_read( byte adrs, bool *flag, error_code *result ) {
		return( queue_transaction( mode_quick_read, adrs, NULL, 0, 0, flag, result ));
	}

	bool quick_write( byte adrs, bool *flag, error_code *result ) {
		return( queue_transaction( mode_quick_write, adrs, NULL, 0, 0, flag, result ));
	}

	//
	//	All "just send data" commands
	//	-----------------------------
	//	(6.5.2) Send Byte
	//	(6.5.4) Write Byte/Word
	//	(6.5.7) Block Write/Read
	//	(6.5.10) Write 32 protocol
	//	(6.5.12) Write 64 protocol
	//
	bool send_data( byte adrs, byte *buffer, byte send, bool *flag, error_code *result ) {
		return( queue_transaction( mode_send_data, adrs, buffer, send, 0, flag, result ));
	}

	//
	//	The (6.5.3) Receive Byte
	//	------------------------
	//
	bool receive_byte( byte adrs, byte *buffer, bool *flag, error_code *result ) {
		return( queue_transaction( twi_mode_receive_byte, adrs, buffer, 0, 1, flag, result ));
	}

	//
	//	All "exchange data" commands
	//	----------------------------
	//	(6.5.4) Write Byte/Word
	//	(6.5.5) Read Byte/Word
	//	(6.5.6) Process Call
	//	(6.5.8) Block Write-Block Read Process Call
	//	(6.5.9) SMBus Host Notify protocol
	//	(6.5.11) Read 32 protocol
	//	(6.5.13) Read 64 protocol
	//
	bool exchange( byte adrs, byte *buffer, byte send, byte recv, bool *flag, error_code *result ) {
		return( queue_transaction( twi_mode_data_exchange, adrs, buffer, send, recv, link, reply ));
	}


	//
	//	void state_machine( TWI_TRANSACTION *active, byte twsr )
	//	------------------------------------------------------------
	//
	//	Using the TWI state supplied drive the transaction record forward.
	//	When the routine returns something will have been done.
	//
	void state_machine( TWI_TRANSACTION *active, byte twsr ) {
		//
		//	This routine is always handed a TWI state obtained after
		//	TWINT becomes 1 (either through being called by the ISR
		//	or though polling the TWINT flag).
		//
		//	The purpose of this routine is to handle the new state
		//	by moving on the master transaction machine forward.
		//
	masterLoop:
		switch( progmem_read_byte_at( active->action )) {
			case TWI_HW_RESTART: {
				//
				//	Generate a restart condition and move the
				//	action pointer forward to the next step.
				//
				twi_start();
				active->next = 0;	// Reset index into the buffer
				active->action++;
				break;
			}
			case TWI_HW_START_COMPLETE: {
				//
				//	We are waiting for the start (or restart) to complete successfully.
				//	If it has completed then move to the next step in the machine.
				//      If not then redirect machine to the abort transaction code.
				//
				switch( twsr ) {
					case TW_START:
					case TW_REP_START: {
						//
						//	Move to next step in the machine.
						//
						active->action++;
						break;
					}
					default: {
						//
						//	Not a state we are anticipating, abort transaction
						//
						twi_error = twsr;
						active->action = twi_abortTransaction;
						twi_sendError( TWI_ERR_STARTING );
					}
				}
				//
				//	We have not "done" anything so we go back to the start of the
				//	routine and read the next step in the machine.
				//
				goto masterLoop;
			}
			case TWI_HW_STOP: {
				//
				//	Generate a stop condition and move the
				//	action pointer forward to the next step.
				//
				twi_stop();
				active->action++;
				break;
			}
			case TWI_HW_ADRS_READ: {
				//
				//	We now send out the slave address from
				//	which we want to read data.
				//
				//	If we are only getting a single byte, then
				//	the Ack flag needs to be false, otherwise
				//	we set it to true.
				//
				twi_sendTarget( active->target, false );
				active->action++;
				break;
			}
			case TWI_HW_ADRS_WRITE: {
				//
				//	We now send out the slave address with
				//	WRITE signified.
				//
				twi_sendTarget( active->target, true );
				active->action++;
				break;
			}
			case TWI_HW_ADRS_ACK: {
				//
				//	We are waiting for the address byte to
				//	complete, then we check the status to
				//	see if we were ack'd or nack'd by the slave.
				//
				switch( twsr ) {
					case TW_MT_SLA_ACK:
					case TW_MR_SLA_ACK: {
						//
						//	The slave has Ackd their address byte
						//	for either read or write (this step in the
						//	master machine does not care which).  Move
						//	to the next step in the machine.
						//
						active->action++;
						break;
					}
					case TW_MT_SLA_NACK:
					case TW_MR_SLA_NACK: {
						//
						//	The slave has NOT Ackd their address byte, so
						//	log an error and then move to the next step in
						//	the machine.
						//
						twi_error = twsr;
						active->action = twi_abortTransaction;
						twi_sendError( TWI_ERR_ADDRESS );
						break;
					}
					default: {
						//
						//	..and again, but a different error.
						//
						twi_error = twsr;
						active->action = twi_abortTransaction;
						twi_sendError( TWI_ERR_TRANSACTION );
						break;
					}
				}
				goto masterLoop;
			}
			case TWI_HW_SEND_BYTE: {
				//
				//	We have to send a byte to the slave
				//
				twi_sendByte( active->buffer[ active->next++ ]);
				active->action++;
				break;
			}
			case TWI_HW_SEND_ACK_LOOP: {
				//
				//	Has the data been received?  The ack
				//	will tell us.
				//
				switch( twsr ) {
					case TW_MT_DATA_ACK: {
						//
						//	The sent data byte has been Ackd.  If there
						//	are more data bytes to send go BACK one step,
						//	otherwise move forwards to the next step.
						//
						if( active->next < active->send ) {
							active->action--;
						}
						else {
							active->action++;
						}
						break;
					}
					case TW_MT_DATA_NACK: {
						//
						//	The data byte was NAckd, the write has failed.
						//
						twi_error = twsr;
						active->action = twi_abortTransaction;
						twi_sendError( TWI_ERR_WRITE_FAIL );
						break;
					}
					default: {
						//
						//	Other state, generate another error.
						//
						twi_error = twsr;
						active->action = twi_abortTransaction;
						twi_sendError( TWI_ERR_TRANSACTION );
					}
				}
				goto masterLoop;
			}
			case TWI_HW_RECV_READY: {
				//
				//	Here we let the system know that we are
				//	ready to receive another byte from the
				//	slave and (through Ack/NAck) if this will
				//	be the last byte.
				//
				twi_readAck( active->next < ( active->recv-1 ));
				active->action++;
				break;
			}
			case TWI_HW_RECV_BYTE_LOOP: {
				//
				//	Save that byte of data!
				//
				if( active->next < active->recv ) {
					active->buffer[ active->next++ ] = twi_readByte();
				}
				else {
					(void)twi_readByte();
				}
				//
				//	Now choose what to do.
				//
				switch( twsr ) {
					case TW_MR_DATA_ACK: {
						//
						//	We have a byte of data, the system
						//	has already sent an "Ack" (so this
						//	is not the last byte).  We roll
						//	back to the previous action.
						//
						active->action--;
						break;
					}
					case TW_MR_DATA_NACK: {
						//
						//	We have a byte of data, the system
						//	has already sent an "NAck" (so this
						//	IS the last byte).  Move onto the
						//	next action.
						//
						active->action++;
						break;
					}
					default: {
						//
						//	Some sort of transaction error.
						//
						twi_error = twsr;
						active->action = twi_abortTransaction;
						twi_sendError( TWI_ERR_TRANSACTION );
						break;
					}
				}
				goto masterLoop;
			}
			default: {
				//
				//	Anything we do not recognise we take to
				//	be handled by another routine, and so our
				//	roll here is to do nothing.
				//
				break;
			}
		}
	}
	
	//
	//	void process( void )
	//	--------------------------
	//
	//	This routine is called (via the task manager) as a
	//	consequence of the ISR setting "_flag" to true, which
	//	happens as a result of the TWI device changing state.
	//
	virtual void process( void ) {
		byte	twsr;

		//
		//	Grab a copy of the status register
		//
		twsr = TWSR;

		//
		//	Since we do not support slave operation we
		//	are only interested in master based actions.
		//
		if( TW_MASTER_STATUS( twsr )) {
			//
			//	MASTER Actions
			//	--------------
			//
			//	Use state_machine to implement the consequences
			//	of this state change.
			//
			if( _active ) {
				//
				//	We have a transaction lined up, so lets move
				//	this on.
				//
				state_machine( _active, twsr );
			}
			else {
				//
				//	We are getting a master state, but without any
				//	associated master transaction: Execute a bus
				//	release and try to return the bus to "normal"
				//
				reset_hardware();
			}
		}
	}

	//
	//	void manager( void )
	//	--------------------
	//
	//	This routine is to be called as part of the main event loop of an
	//	application, ideally as frequently as possible.  This is the
	//	routine which will call the "reply" routines ensuring that
	//	these are not "caught up" in the time and space restrictions that
	//	being part of an ISR call would create.
	//
	//	This will also, if TWI polling is enabled, drive forward any
	//	in progress transaction (slave or master).
	//
	void manager( void ) {
		//
		//	Master Processing
		//	-----------------
		//
		//	The value of "_active" indicates (if non-zero) that there
		//	transaction in progress with the details of the transaction
		//	is at the address indicated.
		//
		if( _active ) {
			//
			//	This points to a transaction record so we look to see
			//	if there are actions which we have to execute on its
			//	behalf.
			//
			//	These fall into one of two categorise:  Either we need
			//	start the master transmission, or we need to complete it.
			//
			//	We only run the callback when the action indicates this
			//	and then subsequently delete the record.
			//
			switch( progmem_read_byte_at( _active->action )) {
				case state_state: {
					//
					//	Generate the start condition and move the
					//	action pointer forward to the next step.
					//
					if( _has_timeout ) _start_time = micros();
					_active->next = 0;	// New index into the buffer
					_active->action++;
					_start();
					break;
				}
				case state_good_callback: {
					//
					//	Make a good call back (if the callback is not NULL)
					//
					*( _active->result ) = error_none;
					*( _active->flag ) = true;
					//
					//	Throw the transaction away.
					//
					_loadNextAction();
					break;
				}
				case state_fail_callback: {
					//
					//	Make a bad call back (if the callback is not NULL)
					//
					*( _active->result ) = error_;
					*( _active->flag ) = true;
					//
					//	Throw the transaction away.
					//
					next_action();
					break;
				}
				default: {
					//
					//	Any other action we simply ignore as none
					//	of our business (as this will be dealt with
					//	through another mechanism).
					//
					break;
				}
			}
		}
		else {
			//
			//	There is no active record, so if the queue is not
			//	empty then set the pointer to the next available
			//	record and initialise the time out new value.
			//
			if( _queue_len > 0 ) next_action();
		}
	}
};



//
//	Define the object controlling the TWI device
//
extern TWI twi;

#endif

//
//	EOF
//
