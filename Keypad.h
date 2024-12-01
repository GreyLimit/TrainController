//
//	Keypad - Arduino library to read a 4x4 matrix keypad
//	via the TWI_IO Library
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


#ifndef _KEYPAD_H_
#define _KEYPAD_H_

#include "Environment.h"
#include "Parameters.h"
#include "Configuration.h"
#include "Layout.h"
#include "Task.h"
#include "TWI.h"
#include "Clock.h"

//
//	The controlling class for the module.
//
class Keypad : public Task {
private:
	//
	//	Define some constant values used in this class.
	//
	static const byte		rows		= LAYOUT_ROWS;
	static const byte		cols		= LAYOUT_COLS;
	static const byte		keys		= LAYOUT_KEYS;

	//
	//	Define the bits which form the row setting and column
	//	reading pins.
	//
	static const byte		scan_row_lsb	= LAYOUT_ROW_LSB;
	static const byte		scan_col_lsb	= LAYOUT_COL_LSB;

	//
	//	The constructed buffer value is a consolidation of
	//	the output rows bits and the input col bits, but,
	//	remembering that the pseudo-bidirectional nature
	//	of the I2C 8-bit IO port means that it is better to
	//	work with "grounded" activies (0s) rather than "raised"
	//	actives (1s).
	//
	//	The result is that the row being tested is set to
	//	'0' with all other rows being '1's in combination
	//	with all the input lines being initialised to '1'.
	//
	//	Any key pressed on the selected gounded row will
	//	pull down the corresponding pin when the value is
	//	read back.
	//
	//	Define the base mask values for rows and columns
	//	(as if their LSB value is 0).
	//
	static const byte		scan_row_mask	= LAYOUT_ROW_MASK;
	static const byte		scan_col_mask	= LAYOUT_COL_MASK;

	//
	//	Define the number of bits required to "cover" the
	//	rows and cols values (ie this is the log base 2 of
	//	values).
	//
	static const byte		scan_rows_bits	= LAYOUT_ROW_BITS;
	static const byte		scan_cols_bits	= LAYOUT_COL_BITS;
	
	//
	//
	static constexpr word		scan_delay = MSECS( LAYOUT_SCAN_DELAY );

	//
	//	Define the value of an empty scan return.
	//
	static const byte		empty_scan	= 0xff;

	//
	//	Define the "pressed"/"released" bit in the returned
	//	value.  This bit is set when a button is initially
	//	pressed, and reset when the button is released.
	//
	//	The inference is that a keypad can only return 126
	//	usedful values as any value with this bit set is
	//	invalid, and any value equivalent to the none value
	//	should be ignored.
	//
	//	This still leaves all of the useful ASCII codes.
	//
	static const byte		pressed		= 0x80;

	//
	//	Remember the address the keypad is attached to.
	//
	byte				_adrs;

	//
	//	The following variables are used to drive through
	//	the scanning process and monitor which keys are
	//	pressed and which are not.  It also allows multiple
	//	keys to be pressed "at once" and be returned in series.
	//
	byte				_scan_row;

	//
	//	Remember which keys have been pressed on the keypad.
	//
	bool				_pressed[ keys ];

	//
	//	The state variable used to track the conversation with
	//	the TWI attached IO port.
	//
	enum scan_status : byte {
		send_scancode,
		resend_scancode,
		read_scancode,
		reread_scancode,
		scan_completed,
		scan_processing
	}				_scan_status;

	//
	//	This is the buffer space used by the TWI routines, yes
	//	just a single byte.
	//
	byte				_buffer;
	
	//
	//	This is the flag used by the TWI interface routines
	//	to signal that the transaction has been completed.
	//
	bool				_flag;
	
	//
	//	This is the error code set by the TWI routines.
	//
	TWI::error_code			_result;

	//
	//	Define the size of a queue through which returned
	//	keys are buffered.
	//
	static const byte		queue_size	= keys;

	//
	//	The queue through which scanned buttons are returned to
	//	calling firmware.
	//
	//	JEFF	I should replace this with the Byte_Queue class.
	//		Yes, obviously.
	//
	byte				_queued,
					_queue_in,
					_queue_out,
					_queue[ queue_size ];
public:
	//
	//	Constructor!
	//	============
	//
	Keypad( void ) {
		byte	i;
		
		//
		//	Reset everything to starting positions.
		//
		_adrs = 0;
		for( i = 0; i < keys; _pressed[ i++ ] = false );
		_scan_row = 0;
		_scan_status = send_scancode;
		_queued = 0;
		_queue_in = 0;
		_queue_out = 0;
	}
	//
	//	Initialise the system and kick off the keyboard reading 
	//	process.
	//
	void initialise( byte i2c_address ) {
		//
		//	Save our address
		//
		_adrs = i2c_address;
		//
		//	pre-set the task flag to true so that
		//	the setup process is initiated automatically
		//	by the task manager.
		//
		_buffer = 0;
		_flag = true;
		_result = TWI::error_none;
		//
		//	Add this object to the task manager.
		//
		task_manager.add_task( this, &_flag );
	}

	//
	//	Return the value of the next key pressed (or 0
	//	if no key changes are detected).
	//
	byte read( void ) {
		if( _queued ) {
			byte		r;

			r = _queue[ _queue_out++ ];
			if( _queue_out >= queue_size ) _queue_out = 0;
			_queued--;
			return( r );
		}
		return( 0 );
	}

	//
	//	Push a keyboard button press into the queue.  Although
	//	available as part of the external interface this is
	//	intended primarily for the internal operations.
	//
	bool write( byte key ) {
		if( _queued < queue_size ) {
			_queue[ _queue_in++ ] = key;
			if( _queue_in >= queue_size ) _queue_in = 0;
			_queued++;
			return( true );
		}
		return( false );
	}

	//
	//	This is the interface routine from the task managent system.
	//	This is called only when the TWI system has set this objects
	//	flag to true.
	//
	virtual bool process( void ) {
		//
		//	We are waiting for a successful return from the I2C scan
		//	activity.
		//
		switch( _scan_status ) {
			//
			//	State associated with setting the scan mask.
			//
			case send_scancode: {
				//
				//	Construct the buffer with the data we need to
				//	place into the IO port before we can read the
				//	result of the scan back.
				//
				_buffer = (( scan_row_mask ^ ( 1 << _scan_row )) << scan_row_lsb )|( scan_col_mask << scan_col_lsb );
				//
				//	We fall through to the code sending the data.
				//
				FALL_THROUGH;
			}
			case resend_scancode: {
				//
				//	Send the scan code:
				//
				if( twi.send_data( _adrs, &_buffer, 1, &_flag, &_result )) {
					//
					//	Successfully scheduled transaction, so move the state to "read_scancode"
					//	and wait for the TWI device to flag that the command has completed.
					//
					_scan_status = read_scancode;
				}
				else {
					//
					//	failed to queue the scan so we will try again shortly
					//
					_scan_status = resend_scancode;
					if( !event_timer.delay_event( scan_delay, &_flag, false )) _flag = true;
				}
				break;
			}
			//
			//	States associated with reading back the scan data.
			//
			case read_scancode: {
				//
				//	Check the return code from the TWI call made
				//	by the "send_scancode" state.
				//
				if( _result != TWI::error_none ) {
					//
					//	The scan initialisation failed, so we log an error
					//	then try to reset the restart the scan.
					//
					errors.log_error( I2C_COMMS_ERROR, (word)_result );
					//
					//	Restart the keypad scan in a short while.
					//
					_scan_status = send_scancode;
					if( !event_timer.delay_event( scan_delay, &_flag, false )) _flag = true;
					break;
				}
				//
				//	Since setting the scan worked, now read the reply.
				//
				FALL_THROUGH;
			}
			case reread_scancode: {
				//
				//	Initiate the read of the scan data:
				//
				if( twi.receive_byte( _adrs, &_buffer, &_flag, &_result )) {
					//
					//	Successfully scheduled the read of the scan code, so set
					//	state to "scan_completed" and await the reply.
					//
					_scan_status = scan_completed;
				}
				else {
					//
					//	Unsuccessfully scheduled a read so we try again shortly.
					//
					_scan_status = reread_scancode;
					if( !event_timer.delay_event( scan_delay, &_flag, false )) _flag = true;
				}
				break;
			}
			//
			//	States associated with processing the scan data.
			//
			case scan_completed: {
				//
				//	Check the return code form the TWI call made by the
				//	"read_scancode" state.
				//
				if( _result != TWI::error_none ) {
					//
					//	Reading the scancode failed; Log an
					//	error, reset next state to send_scancode
					//	then schedule the task manager to call us again.
					//
					errors.log_error( I2C_COMMS_ERROR, (word)_result );
					//
					//	Restart the keypad scan in a short while.
					//
					_scan_status = reread_scancode;
					if( !event_timer.delay_event( scan_delay, &_flag, false )) _flag = true;
					break;
				}
				//
				//	Fall through to processing the scan.
				//
				FALL_THROUGH;
			}
			case scan_processing: {
				//
				//	Now we work on the data in the buffer.
				//
				byte	c, r, b, x;
				bool	s;

				//
				//	We need to run through all the returned
				//	scan bits since we are watching for keys
				//	being released as well as pressed.
				//
				r = _scan_row << scan_cols_bits;
				b = 1 << scan_col_lsb;
				for( c = 0; c < cols; c++ ) {
					//
					//	Logic here is twisted, be warned;
					//	a '0' bit is pressed, a '1' is
					//	release.
					//
					s = (( _buffer & b ) == 0 );
					
					//
					//	If the saved state is different to
					//	recieved state then the button has
					//	moved (changed state).
					//
					x = r | c;
					if( _pressed[ x ] != s ) {
						//
						//	Queue detected change then
						//	(if queued successful) change
						//	saved state for this key.
						//
						if( write( progmem_read_byte( keypad_mapping[ x ]) | ( s? pressed: 0 ))) _pressed[ x ] = s;
					}

					//
					//	move to next column bit.
					//
					b <<= 1;
				}
				//
				//	move the scan row along one to cover
				//	the next set of keys.
				//
				if(( _scan_row += 1 ) >= rows ) _scan_row = 0;
				//
				//	Finally we move back to the start state
				//
				_scan_status = send_scancode;
				break;
			}
		}
		//
		//	Keep this task in the system.
		//
		return( true );
	}

};

#endif

//
//	EOF
//
