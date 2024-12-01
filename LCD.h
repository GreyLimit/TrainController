//
//	LCD - Arduino library to control an LCD via the TWI_IO Library
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


#ifndef _LCD_H_
#define _LCD_H_

#include "Arduino.h"

//
//	The controlling class for the module.
//
template< byte bus_adrs, byte lcd_rows, byte lcd_cols >
class LCD {
private:
	//
	//	The I2C 8-Bit IO Expander directly connects to 8 of the available
	//	LCD IO pins.  These are:
	//
	//	Bit	Name	Meaning
	//
	//	0	RS	Register Select
	//			0	Instruction Register (IR) on WRITE
	//			0	Address Counter (AC) on READ
	//			1	Data Register (DR) for READ or WRITE
	//
	//	1	RW	0	Write Data
	//			1	Read Data
	//
	//	2	E	Enable data read/write
	//
	//			It is the *transition* from 1 -> 0 in this bit which
	//			triggers activity on the LCD.  There for all activity
	//			with the LCD involves writing a "data byte" with this
	//			bit as 1 followed by writing the *same* byte, but with
	//			this bit as 0.
	//
	//	3	LED	Backlight off
	//			0	Off
	//			1	On
	//
	//	4	D0	Data bit 0
	//	5	D1	Data bit 1
	//	6	D2	Data bit 2
	//	7	D3	Data bit 3
	//		 
	//	Because the LCD is set to 4-bit mode, 4 bits of the I2C output are for the control outputs
	//	while the other 4 bits are for the 8 bits of data which are send in parts using the enable
	//	output.
	//

	//
	//	"Output State"
	//	==============
	//
	//	This definition deals with the "top level" access to the
	//	LCD.  All other access has to go through the "output state"
	//	definition.
	//
	//	These values enable the software to "operate" the LCD at
	//	its *hardware* interface level.  ALl of the definitions
	//	after this relate to "operating" the firmware within the
	//	LCD.
	//
	//	The following sequences indicate "how" to access the LCDs
	//	internal registers and so (using the instructions outlined
	//	below) how to make the LCD do what you want!
	//
	//	The following show the steps required to execute specific
	//	actions upon the LCD, and represet the bytes of data which
	//	have to be written to the I2C I/O expander which is attached
	//	to the LCD.
	//
	//	In the following:
	//		"?" means according to requirements
	//		"hhhh" means the high nybble of the data
	//		"llll" means the low nybble of the data
	//
	//	Some form of delay will be required between each data byte
	//	transmitted.
	//
	//		Data	LED	Enable	RW	RS
	//	Bit	7654	3	2	1	0
	//		----	---	------	--	--
	//
	//	Write Byte to Instruction Register
	//
	//		hhhh	?	1	0	0
	//		hhhh	?	0	0	0
	//		llll	?	1	0	0
	//		llll	?	0	0	0
	//
	//
	//	Place a character onto the LCD
	//
	//		hhhh	?	1	0	1
	//		hhhh	?	0	0	1
	//		llll	?	1	0	1
	//		llll	?	0	0	1
	//
	//	
	//
	static const byte	output_state	= 0b00000000;
	//
	//	Name the meaning of each bit position
	//
	static const byte	register_select	= 0;
	static const byte	read_write	= 1;
	static const byte	enable		= 2;
	static const byte	backlight	= 3;

	//
	//	All of the following definitions relate to
	//	values which are applied to the LCD Instruction
	//	Register, and act upon the display rather than
	//	place data into the display.
	//

	//
	//	"Entry State"
	//	=============
	//
	//	The "Entry State" instructions dictate how new
	//	text is inserted into the LCD display. Is text
	//	added left to right or right to left.  Does the
	//	LCD automatically scroll when the bottom is reached?
	//
	//	entry_state		000001ab
	//
	//	a	Left/right additon of new text
	//	b	Auto scrolling enabled
	//
	static const byte	entry_state	= 0b00000100;
	static const byte	auto_scroll	= 0;
	static const byte	left_right	= 1;

	//
	//	"Display State"
	//	===============
	//
	//	The "Display State" instructions control how the
	//	LCD display "looks".  Is it on? Is there a cursor?
	//	Does the cursor flash?
	//
	//	display_state	00001abc
	//
	//	a	Display on?
	//	b	Cursor on?
	//	c	Blinking cursor?
	//
	static const byte	display_state	= 0b00001000;
	static const byte	blink_on	= 0;
	static const byte	cursor_on	= 1;
	static const byte	display_on	= 2;

	//
	//	"Direct Instructions"
	//	=====================
	//
	//	Direct LCD instructions
	//
	static const byte	clear_screen	= 0b00000001;
	static const byte	home_screen	= 0b00000010;
	static const byte	scroll_left	= 0b00011000;
	static const byte	scroll_right	= 0b00011100;
	static const byte	set_position	= 0b10000000;

	//
	//	Macros to extract high and low nybble of a value
	//	suitable for sending to display.
	//
	inline byte low_nybble( byte v ) { return(( v & 0x0f ) << 4 ); }
	inline byte high_nybble( byte v ) { return( v & 0xf0 ); }


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
	//	bus_adrs		I2C/TWI target address.
	//
	//	lcd_rows / lcd_cols	Display size.
	//
	//	buffer_size		The size of the screen buffer.
	//
	static constexpr byte	buffer_size = lcd_rows * lcd_cols;

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
	//	bool queueTransfer( const byte *program, byte value )
	//	-----------------------------------------------------
	//
	//	Routine used to add another action to the queue, return true
	//	if the action has been queued, false otherwise.
	//
	bool queueTransfer( const mc_state *program, byte value ) {
		if( _queue_len < max_pending ) {
			_queue[ _queue_in ].value = value;
			_queue[ _queue_in ].program = program;
			if( ++_queue_in >= max_pending ) _queue_in = 0;
			_queue_len++;
			return( true );
		}
		return( false );
	}

	//
	//	bool queueTransferWait( const byte *program, byte value )
	//	---------------------------------------------------------
	//
	//	Routine used to add another action to the queue but waits until the
	//	action has been successfully added to the queue.  This will block
	//	the execution of the firmware at this point.
	//
	void queueTransferWait( const mc_state *program, byte value ) {
		while( !queueTransfer( program, value )) task_manager.pole_task();
	}

	
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
	byte		_frame_buffer[ buffer_size ],
			_frame_chk_r				// The Row and Col position of the buffer
			_frame_chk_c				// scanning code.
			_frame_cursor,				// index into buffer
	bool		_frame_sync;				// index into buffer.
			

public:
	//
	//	Constructor!
	//	============
	//
	LCD( void ) {
		//
		//	Empty the pending queue in preparation for the
		//	initial initialisation code to execute.
		//
		_queue_len = 0;
		_queue_in = 0;
		_queue_out = 0;
		//
		//	Set the back light off and the state variables
		//	as empty.
		//
		_back_light = 0;
		_display_state = 0;
		_entry_state = 0;
		//
		//	Initialise the program machine which controls
		//	data transmission to the LCD
		//
		_fsm_instruction = mc_idle_program;
		_fsm_loop = NIL( mc_state );
		_fsm_data_byte = 0;
		_fsm_buffer = 0;
		_fsm_wait_ends = 0;
		_fsm_twi_returns = false;
		_fsm_twi_success = false;
		//
		//	Set up the frame buffer as empty.
		//
		for( byte i = 0; i < _frame_size; _frame_buffer[ i++ ] = ( (byte)SPACE | 0x80 ));
		_frame_chk_r = 0;
		_frame_chk_c = 0;
		_frame_cursor = 0;
		_frame_sync = false;
	}
	
	void begin( void ) {
		//
		//	Clear i2c adapter, then wait more than 40ms after powerOn.
		//
		queueTransferWait( mc_reset_program, 0b00000000 );
		//
		//	See HD44780U datasheet "Initializing by Instruction" Figure 24 (4-Bit Interface)
		//
		//	Now set up the LCD
		//
		//	Function set flags
		//
		//	DL
		//	--
		//	0	4-bit data IO
		//	1	8-bit data IO
		//
		//	4-bit "Function Set" opcode is
		//
		//		0-0-1-DL
		//
		//	N F	Lines	Font
		//	- -	-----	----
		//	0 0	1	5x8
		//	0 1	1	5x10
		//	1 0	2	5x8
		//
		//	8-bit "Function Set" opcode is
		//
		//		0-0-1-DL-N-F-X-X
		//
		//	These are all written to the Instruction Register
		//	of the LCD (RS = 0, R/W = 0)
		//	
		queueTransferWait( mc_init_long_delay, 0b00110000 );	// Function Set + 8 bit mode (As a 4 bit instruction)
		queueTransferWait( mc_init_medium_delay, 0b00110000 );	// Function Set + 8 bit mode (As a 4 bit instruction)
		queueTransferWait( mc_init_short_delay, 0b00110000 );	// Function Set + 8 bit mode (As a 4 bit instruction)
		queueTransferWait( mc_init_short_delay, 0b00100000 );	// Function Set + 4 bit mode (As a 4 bit instruction)
		if( _rows == 1 ) {
			//
			//	1 line displays get initialised here
			//
			queueTransferWait( mc_send_inst, 0b00100000 );	// Function Set + 4 bit mode + 1 line + 5x8 font (As an 8 bit instruction)
		}
		else {
			//
			//	2 and 4 line displays here (as 4 line displays
			//	are doubled up 2 line displays).
			//
			queueTransferWait( mc_send_inst, 0b00101000 );	// Function Set + 4 bit mode + 2 lines + 5x8 font (As an 8 bit instruction)
		}
		//
		//	And pause..
		//
		synchronise( 1000 );
		//
		//	Now some ordinary tidy up steps.
		//
		display( true );
		clear();
		leftToRight( true );
		cursor( false );
		blink( false );
		backlight( true );
	}

	//
	//	This routine called each time round loop to support going
	//	LCD processes.
	//
	void service( void ) {
		//
		//	Everything done in this routine is under the control
		//	of the current LCD program.  This is, effectively,
		//	where all the actual work gets done.
		//
		switch( progmem_read_byte_at( _fsm_instruction )) {
			case mc_idle: {			// machine at idle
				//
				//	If we are here and there is something in the queue
				//	we load up the necessary components and run the
				//	program.
				//
				if( _queue_len ) {
					//
					//	Load the _fsm_ variables with the required
					//	data to run this job and remove the job from
					//	the queue.
					//
					_fsm_data_byte = _queue[ _queue_out ].value;
					_fsm_instruction = _queue[ _queue_out ].program;
					if( ++_queue_out >= max_pending ) _queue_out = 0;
					_queue_len--;
				}
				break;
			};
			case mc_reset: {		// Resetting the device by sending
				_fsm_buffer = 0;	// an empty byte.
				_fsm_instruction++;
				break;
			}
			case mc_inst_high_enable: {	// send high nybble with E=1 as inst
				//
				//	RS = 0	(Select IR)
				//	R/W = 0 (Write)
				//	E = 1	(Enable)
				//
				_fsm_buffer = high_nybble( _fsm_data_byte ) | _back_light | bit( enable );
				_fsm_instruction++;
				break;
			};
			case mc_inst_high_disable: {	// send high nybble with E=0 as inst
				//
				//	RS = 0	(Select IR)
				//	R/W = 0 (Write)
				//	E = 0	(Disable)
				//
				_fsm_buffer = high_nybble( _fsm_data_byte ) | _back_light;
				_fsm_instruction++;
				break;
			};
			case mc_inst_low_enable: {	// send low nybble with E=1 as inst
				//
				//	RS = 0	(Select IR)
				//	R/W = 0 (Write)
				//	E = 1	(Enable)
				//
				_fsm_buffer = low_nybble( _fsm_data_byte ) | _back_light | bit( enable );
				_fsm_instruction++;
				break;
			};
			case mc_inst_low_disable: {	// send low nybble with E=0 as inst
				//
				//	RS = 0	(Select IR)
				//	R/W = 0 (Write)
				//	E = 0	(Disable)
				//
				_fsm_buffer = low_nybble( _fsm_data_byte ) | _back_light;
				_fsm_instruction++;
				break;
			};
			case mc_data_high_enable: {	// send high nybble with E=1 as data
				//
				//	RS = 1	(Select DR)
				//	R/W = 0 (Write)
				//	E = 1	(Enable)
				//
				_fsm_buffer = high_nybble( _fsm_data_byte ) | _back_light | bit( register_select ) | bit( enable );
				_fsm_instruction++;
				break;
			};
			case mc_data_high_disable: {	// send high nybble with E=0 as data
				//
				//	RS = 1	(Select DR)
				//	R/W = 0 (Write)
				//	E = 0	(Disable)
				//
				_fsm_buffer = high_nybble( _fsm_data_byte ) | _back_light | bit( register_select );
				_fsm_instruction++;
				break;
			};
			case mc_data_low_enable: {	// send low nybble with E=1 as data
				//
				//	RS = 1	(Select DR)
				//	R/W = 0 (Write)
				//	E = 1	(Enable)
				//
				_fsm_buffer = low_nybble( _fsm_data_byte ) | _back_light | bit( register_select ) | bit( enable );
				_fsm_instruction++;
				break;
			};
			case mc_data_low_disable: {	// send low nybble with E=0 as data
				//
				//	RS = 1	(Select DR)
				//	R/W = 0 (Write)
				//	E = 0	(Disable)
				//
				_fsm_buffer = low_nybble( _fsm_data_byte ) | _back_light | bit( register_select );
				_fsm_instruction++;
				break;
			};
			case mc_status_enable: {	// send status request with E=1 as inst
				//
				//	RS = 0	(Select IR)
				//	R/W = 1 (Read)
				//	E = 1	(Enable)
				//
				_fsm_buffer = 0xf0 | _back_light | bit( read_write ) | bit( enable );
				_fsm_instruction++;
				break;
			};
			case mc_status_disable: {	// send status request with E=0 as inst
				//
				//	RS = 0	(Select IR)
				//	R/W = 1 (Read)
				//	E = 0	(Disable)
				//
				_fsm_buffer = 0xf0 | _back_light | bit( read_write );
				_fsm_instruction++;
				break;
			};
			case mc_read_buffer: {		// read the interface into the buffer
				//
				//	We read and read again until this returns true then
				//	move to the next instruction
				//
				_fsm_twi_returns = false;
				if( twi_cmd_receive_byte( _address, &_fsm_buffer, (void *)this, twi_callback )) _fsm_instruction++;
				break;
			}
			case mc_store_high_data: {	// place buffer into data high bits
				_fsm_data_byte = ( _fsm_buffer & 0xf0 )|( _fsm_data_byte & 0x0f );
				_fsm_instruction++;
				break;
			}
			case mc_store_low_data: {	// place buffer into data high bits
				_fsm_data_byte = (( _fsm_buffer & 0xf0 ) >> 4 )|( _fsm_data_byte & 0xf0 );
				_fsm_instruction++;
				break;
			}
			case mc_transmit_buffer: {	// send the content of the buffer
				//
				//	We send and send again until this returns true then
				//	move to the next instruction
				//
				_fsm_twi_returns = false;
				if( twi_cmd_send_data( _address, &_fsm_buffer, 1, (void *)this, twi_callback )) _fsm_instruction++;
				break;
			}
			case mc_wait_on_done: {		// Wait for the TWI action to complete
				if( _fsm_twi_returns ) {
					//
					//	Transmission completed
					//
					if( _fsm_twi_success ) {
						//
						//	Successfully
						//
						_fsm_instruction++;
					}
					else {
						//
						//	Unsuccessfully
						//
						//	What do we do?  We are (probably)
						//	in the middle of a series of a series
						//	of data transfers such that "losing"
						//	one will break the activity.
						//
						//	Best option is to drop all pending
						//	activities and execute a reset.
						//
						_queue_len = 0;
						_queue_in = 0;
						_queue_out = 0;
						_fsm_instruction = mc_reset_program;
						_fsm_loop = NIL( mc_state );
					}
				}
				break;
			}
			case mc_begin_wait: {		// Note the top of the wait loop
				//
				//	Just need to remember the instruction pointer
				//	after this one (no need to re-run this code
				//	for each loop.
				//
				_fsm_loop = ++_fsm_instruction;
				break;
			}
			case mc_wait_loop: {		// If not ready loop again
				//
				//	The Busy Flag is the top bit of the data recovered from
				//	the LCD.
				//
				if( _fsm_data_byte & 0x80 ) {
					_fsm_instruction++;
				}
				else {
					_fsm_instruction = _fsm_loop;
				}
				break;
			}
			case mc_set_delay_40000us: {	// Set delay countdown to 40000us
				_fsm_wait_ends = micros() + 40000;
				_fsm_instruction++;
				break;
			}
			case mc_set_delay_4200us: {	// 4200us
				_fsm_wait_ends = micros() + 4200;
				_fsm_instruction++;
				break;
			};
			case mc_set_delay_1600us: {	// 1600us
				_fsm_wait_ends = micros() + 1600;
				_fsm_instruction++;
				break;
			};
			case mc_set_delay_150us: {	// 150us
				_fsm_wait_ends = micros() + 150;
				_fsm_instruction++;
				break;
			};
			case mc_set_delay_41us: {	// 41us
				_fsm_wait_ends = micros() + 41;
				_fsm_instruction++;
				break;
			};
			case mc_set_delay_37us: {	// 37us
				_fsm_wait_ends = micros() + 37;
				_fsm_instruction++;
				break;
			};
			case mc_set_delay_10us: {	// 10us
				_fsm_wait_ends = micros() + 10;
				_fsm_instruction++;
				break;
			};
			case mc_delay_wait: {	// Wait until the delay period has expired
				if( micros() > _fsm_wait_ends ) _fsm_instruction++;
				break;
			};
			default: {
				//
				//	This should not happen, but if it does then
				//	there is a real problem.  All I can do is
				//	log an error (if possible) and reset the program.
				//	Error logging not implement yet.
				//
				_fsm_instruction = mc_idle_program;
				break;
			}
		}
		//
		//	If there is frame buffer defined, then we need to continue a scan
		//	of its content to find updated elements, and then work to get these
		//	transmitted to the LCD.
		//
		if( _frame_size ) {
			byte	c, i;
			
			//
			//	The _frame_chk_r and _frame_chk_c variables gives the
			//	current position of where we are looking in the buffer.
			//
			//	Is there something to do at the current scan position?
			//
			c = _frame_buffer[( i = _frame_chk_r * _cols + _frame_chk_c )];
			if( c & 0x80 ) {
				//
				//	All "pending" updates in the frame buffer have
				//	their top bit set. Once a position has been updated
				//	This bit is reset.
				//
				if( _frame_sync ) {
					//
					//	We can just directly output the character
					//	to the display.
					//
					c &= 0x7f;
					if( write( c )) {
						//
						//	Success, update frame buffer, move
						//	next and last onwards.
						//
						_frame_buffer[ i ] = c;
						//
						//	Now move on the next and last values.
						//
						//	But .. remembering that the LCD does
						//	not wrap across the edge of the display.
						//
						if(( _frame_chk_c += 1 ) >= _cols ) {
							//
							//	If we "run off" the edge of the
							//	line then we will have to get the
							//	LCD to re-position its cursor.
							//
							_frame_sync = false;
							_frame_chk_c = 0;
							if(( _frame_chk_r += 1 ) >= _rows ) {
								_frame_chk_r = 0;
							}
						}
					}
				}
				else {
					//
					//	We need to move the output cursor on the LCD
					//	to where we need to output a pending character.
					//
					if( index( i )) {
						//
						//	If the index function is successful then
						//	we can move the LCD note that the LCD and
						//	scan positions are synchronised.
						//
						_frame_sync = true;
					}
				}
			}
			else {
				//
				//	Any character we have "skipped" over means that
				//	the output LCD cursor is in the wrong place, so
				//	we reset for sync flag.
				//
				_frame_sync = false;
				//
				//	Move on to the next byte, and wrap if we get to
				//	the bottom of the display.
				//
				if(( _frame_chk_c += 1 ) >= _cols ) {
					_frame_chk_c = 0;
					if(( _frame_chk_r += 1 ) >= _rows ) {
						_frame_chk_r = 0;
					}
				}
			}
		}
	}

	//
	//	byte queueCapacity( void )
	//	--------------------------
	//
	//	Return space available in the queue
	//
	byte queueCapacity( void ) {
		return( max_pending - _queue_len );
	}


	//
	//	This is an "interrupt" style routine called
	//	when TWI transactions have been completed
	//
	void done( bool ok ) {
		//
		//	Note last TWI action completed
		//
		_fsm_twi_success = ok;
		_fsm_twi_returns = true;
	}

	
	//
	//	Functional Routines
	//	===================
	//
	//	These return true if the command/action was
	//	successfully queued, false otherwise.
	//
	bool backlight( bool on ) {
		//
		//	The back light is explicitly controlled by
		//	one of the pins on the LCDs primary hardware
		//	interface.  The _back_light variable is
		//	used as part of the mask when writing to
		//	the LCD so the LCD is controlled simply by
		//	setting (or resetting) this specific bit.
		//
		bitWrite( _back_light, backlight, on );
		//
		//	The change in backlight status will be
		//	implemented on any following command to
		//	the LCD
		//
		return( true );
	}
	
	bool clear( void ) {

		TRACE_LCD( console.write( 'C' ));
		
		if( queueTransfer( mc_send_inst, clear_screen )) {
			if( _frame_size ) {
				for( byte i = 0; i < _frame_size; _frame_buffer[ i++ ] = ( SPACE | 0x80 ));
				_frame_chk_r = 0;
				_frame_chk_c = 0;
				_frame_cursor = 0;
			}
			return( true );
		}
		return( false );
	}
	
	bool home( void ) {

		TRACE_LCD( console.write( 'H' ));
		
		if( queueTransfer( mc_send_inst, home_screen )) {
			if( _frame_size ) {
				_frame_chk_r = 0;
				_frame_chk_c = 0;
				_frame_cursor = 0;
			}
			return( true );
		}
		return( false );
	}

	bool leftToRight( bool l2r ) {
		//
		//	Note that this facility does not apply consistently
		//	to "wrapping" between lines.  If you write off the end
		//	of line 0 it rolls into line 2.  Like wise rolling off
		//	line 1 rolls into line 3.
		//
		//	This is caused entirely by the memory mapping between
		//	the driver chip and the actual display.
		//
		bitWrite( _entry_state, left_right, l2r );
		return( queueTransfer( mc_send_inst, entry_state | _entry_state ));
	}

	bool autoscroll( bool on ) {
		bitWrite( _entry_state, auto_scroll, on );
		return( queueTransfer( mc_send_inst, entry_state | _entry_state ));
	}

	bool display( bool on ) {
		bitWrite( _display_state, display_on, on );
		return( queueTransfer( mc_send_inst, display_state | _display_state ));
	}

	bool cursor( bool on ) {
		bitWrite( _display_state, cursor_on, on );
		return( queueTransfer( mc_send_inst, display_state | _display_state ));
	}

	bool blink( bool on ) {
		bitWrite( _display_state, blink_on, on );
		return( queueTransfer( mc_send_inst, display_state | _display_state ));
	}

	bool position( byte col, byte row ) {
		byte newAddress;

		TRACE_LCD( console.println());
		TRACE_LCD( console.write( 'P' ));
		TRACE_LCD( console.print_hex( col ));
		TRACE_LCD( console.print_hex( row ));

		//
		//	Start with the required column position
		//
		newAddress = col;
		//
		//	For ODD rows we add the fixed offset
		//
		if( row & 1 ) newAddress += 0x40;
		//
		//	For the repeated rows add the total number of columns
		//
		if( row & 2 ) newAddress += _cols;
		//
		//	Transmit command
		//
		return( queueTransfer( mc_send_inst, set_position | newAddress ));
	}

	bool index( byte posn ) {

		TRACE_LCD( console.println());
		TRACE_LCD( console.write( 'I' ));
		TRACE_LCD( console.print_hex( posn ));

		//
		//	similar to the position() routine, but views
		//	the buffer simply as a series of bytes starting
		//	at the top left and finishing at the bottom
		//	right.
		//
		//	Line 0
		//
		if( posn < _cols ) return( queueTransfer( mc_send_inst, set_position | posn ));
		//
		//	Line 1
		//
		if(( posn -= _cols ) < _cols ) return( queueTransfer( mc_send_inst, set_position |( 0x40 + posn )));
		//
		//	Line 2
		//
		if(( posn -= _cols ) < _cols ) return( queueTransfer( mc_send_inst, set_position |( _cols + posn )));
		//
		//	Line 3
		//
		if(( posn -= _cols ) < _cols ) return( queueTransfer( mc_send_inst, set_position |( 0x40 + _cols + posn )));
		//
		//	Out of bounds, put it top left, position 0.
		//
		return( queueTransfer( mc_send_inst, set_position ));
	}

	bool write( byte val ) {

		TRACE_LCD( console.write( 'W' ));
		TRACE_LCD( console.print_hex( val ));

		return( queueTransfer( mc_send_data, val ));
	}

JEFF
	void setPosn( byte col, byte row ) {
		//
		//	Simply set the cursor index if a frame buffer
		//	has been allocated.
		//
		if( _frame_size ) if(( _frame_cursor = ( row * _cols + col )) >=  _frame_size ) _frame_cursor = 0;
	}

	void writeChar( char val ) {
		//
		//	Place character into the frame buffer at the cursor
		//	position and move the cursor forward one position.
		//	Wrap the cursor round to the top of the screen if
		//	you head off the end of the last line.
		//
		if( _frame_size ) {
			byte v = val & 0x7f;
			
			//
			//	Place value into the frame buffer but only if
			//	it is different from the value already there.
			//
			if( _frame_buffer[ _frame_cursor ] != v ) _frame_buffer[ _frame_cursor ] = v | 0x80;
			//
			//	Move the cursor forward, and wrap to the top
			//	if we fall off the bottom.
			//
			if(( _frame_cursor += 1 ) >= _frame_size ) _frame_cursor = 0;
		}
	}

	void writeStr( const char *str ) {
		char	c;
		
		while(( c = *str++ ) != EOS ) writeChar( c );
	}

	void writeBuf( const char *buf, byte len ) {
		while( len-- ) writeChar( *buf++ );
	}

	void fill( char val, byte len ) {
		while( len-- ) writeChar( val );
	}

	//
	//	Pause activities and wait until the
	//	frame buffer has been updated.
	//
	//	Either run until all jobs apear to have
	//	been completed (no arguments), or
	//	run for a fixed period of time specified
	//	as milliseconds in the argument.
	//
	void synchronise( void ) {
		while( _queue_len ) {
			service();
			twi_eventProcessing();
		}
	}
	void synchronise( word ms ) {
		unsigned long	start;

		start = millis();
		while(( millis() - start ) < ms ) synchronise();
	}

};


#endif

//
//	EOF
//
