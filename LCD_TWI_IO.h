//
//	LCD_TWI_IO - Arduino library to control an LCD via the TWI_IO Library
//
//	Copyright(C) 2021 Jeff Penfold <jeff.penfold@googlemail.com>
//
//	This program is free software : you can redistribute it and /or modify
//	it under the terms of the GNU General Public License as published by
//	the Free Software Foundation, either version 3 of the License, or
//	(at your option) any later version.
//
//	This program is distributed in the hope that it will be useful,
//	but WITHOUT ANY WARRANTY; without even the implied warranty of
//	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.See the
//	GNU General Public License for more details.
//
//	You should have received a copy of the GNU General Public License
//	along with this program.  If not, see < https://www.gnu.org/licenses/>.
//


#ifndef _LCD_TWI_IO_H_
#define _LCD_TWI_IO_H_

#include "Arduino.h"

//
//	The controlling class for the module.
//
class LCD_TWI_IO {
private:
	//
	//	The "machine" which will be used to transmit the data
	//	to the LCD is formed from the following instructions:
	//
	enum mc_state : byte {
		mc_idle = 0,		// Machine at idle
		mc_reset,		// Send reset byte (value=0x00)
		mc_inst_high_enable,	// send high nybble with E=1 as inst
		mc_inst_high_disable,	// send high nybble with E=0 as inst
		mc_inst_low_enable,	// send low nybble with E=1 as inst
		mc_inst_low_disable,	// send low nybble with E=0 as inst
		mc_data_high_enable,	// send high nybble with E=1 as data
		mc_data_high_disable,	// send high nybble with E=0 as data
		mc_data_low_enable,	// send low nybble with E=1 as data
		mc_data_low_disable,	// send low nybble with E=0 as data
		mc_status_enable,	// send status request with E=1 as inst
		mc_status_disable,	// send status request with E=0 as inst
		mc_transmit_buffer,	// send content of the buffer
		mc_wait_on_done,	// Wait for TWI confirmation
		mc_read_buffer,		// read the interface into the buffer
		mc_store_high_data,	// place buffer into data high bits
		mc_store_low_data,	// place buffer into data high bits
		mc_begin_wait,		// Note the top of the wait loop
		mc_wait_loop,		// If not ready loop again


		//
		//	These should be redundant, eventually.
		//
		mc_set_delay_40000us,	// Set delay countdown to 40000us
		mc_set_delay_4200us,	// 4200us
		mc_set_delay_1600us,	// 1600us
		mc_set_delay_150us,	// 150us
		mc_set_delay_41us,	// 41us
		mc_set_delay_37us,	// 37us
		mc_set_delay_10us,	// 10us
		mc_delay_wait,		// Wait until the delay period has expired
	};

	//
	//	Capture some hard details about the LCD this object is
	//	addressing.
	//
	byte	_address,
		_cols,
		_rows;

	//
	//	Define a structure to be used to deliver a single data
	//	byte or instruction to the LCD.
	//
	struct pending {
		byte		value;
		const mc_state	*program;
	};
	static const byte max_pending = 16;
	//
	//	This is the queue of pending bytes heading out of the
	//	object towards the LCD.
	//
	struct pending	_queue[ max_pending ];
	byte		_queue_len,
			_queue_in,
			_queue_out;
	//
	//	Routine used to add another action to the queue
	//
	bool	queueTransfer( const mc_state *program, byte value );
	void	queueTransferWait( const mc_state *program, byte value );
	
	//
	//	Define the variables holding the "state" details of the
	//	LCD.
	//
	byte	_back_light;	// Backlight? (obviously)
	byte	_display_state;	// Display on? Cursor on? Blinking?
	byte	_entry_state;	// Text L->R? Auto Scroll?
	
	//
	//	The machine "programs" which tell the system how to
	//	handle each of the mechanisms for talking to the LCD.
	//
	static const mc_state mc_idle_program[] PROGMEM;
	static const mc_state mc_reset_program[] PROGMEM;
	static const mc_state mc_init_long_delay[] PROGMEM;
	static const mc_state mc_init_medium_delay[] PROGMEM;
	static const mc_state mc_init_short_delay[] PROGMEM;
	static const mc_state mc_send_inst[] PROGMEM;
	static const mc_state mc_send_data[] PROGMEM;

	//
	//	The variables which the current "program" are operating against.
	//
	const mc_state	*_fsm_instruction,
			*_fsm_loop;
	byte		_fsm_data_byte,
			_fsm_buffer;
	unsigned long	_fsm_wait_ends;
	bool		_fsm_twi_returns,
			_fsm_twi_success;

	//
	//	The following variables contain and manage the current "frame buffer"
	//	for the LCD.  These are only activated if a piece of memory is provided
	//	to be the frame buffer (the object does not allocate or contain any
	//	memory space for this purpose).
	//
	//	_frame_buffer		Where the frame buffer is in memory (or NULL)
	//
	//	_frame_size		The size of the frame buffer (or 0 if not defined,
	//				use as indication of a buffer being available)
	//
	//	_frame_chk_r		The Row and Col position of the buffer
	//	_frame_chk_c		scanning code.
	//
	//	_frame_cursor		Insert point for next output text.
	//
	//	_frame_sync		True if the LCD position is synchronised
	//				with the buffer scan position (r,c).
	//
	byte		*_frame_buffer,
			_frame_size,
			_frame_chk_r,
			_frame_chk_c,
			_frame_cursor;
	bool		_frame_sync;
			

public:
	//
	//	Constructor for the LCD object.
	//
	LCD_TWI_IO( byte address, byte cols, byte rows );
	
	//
	//	Routine to "kick off" the LCD code
	//	and a service routine which needs to be
	//	called regularly in the main loop code.
	//
	void begin( void );		// Use inside setup();

	//
	//	IMPORTANT
	//	=========
	//
	//	This routine must be called at least once
	//	each time through the "loop()" routine
	//	to ensure that the processes supporting the
	//	LCD are driven forwards generating the
	//	intended output.
	//
	void service( void );		// Use inside loop();

	//
	//	Return space available in the queue
	//
	byte	queueCapacity( void );

	//
	//	This is an "interrupt" style routine called
	//	when TWI transactions have been completed
	//
	//	Never call directly.
	//
	void done( bool ok );
	
	//
	//	Routines to act directly upon the LCD.
	//
	//	These return true if the command/action was
	//	successfully queued, false otherwise.
	//
	bool backlight( bool on );
	bool clear( void );
	bool home( void );
	bool leftToRight( bool l2r );
	bool autoscroll( bool on );
	bool display( bool on );
	bool cursor( bool on );
	bool blink( bool on );
	bool position( byte col, byte row );
	bool index( byte posn );
	bool write( byte val );

	//
	//	Routines which act upon the frame buffer.
	//
	//	The frame buffer hijacks the top bit of each
	//	byte to indicate update status.  As a result
	//	only 7-ASCII can be displayed through this
	//	mechanism.
	//
	bool setBuffer( byte *buffer, byte size );
	void setPosn( byte col, byte row );
	void writeChar( char val );
	void writeStr( const char *str );
	void writeBuf( const char *buf, byte len );
	void fill( char val, byte len );

	//
	//	Pause activities and wait until the
	//	frame buffer has been updated.
	//
	//	Either run until all jobs apear to have
	//	been completed (no arguments), or
	//	run for a fixed period of time specified
	//	as milliseconds in the argument.
	//
	void synchronise( void );
	void synchronise( word ms );
};

#endif

//
//	EOF
//
