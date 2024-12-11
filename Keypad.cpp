//
//	Keypad.cpp
//	==========
//

#include "Keypad.h"
#include "Clock.h"
#include "Task.h"

//
//	Constructor!
//	============
//
Keypad::Keypad( void ) {
	byte	i;
	
	//
	//	Reset everything to starting positions.
	//
	_adrs = 0;
	for( i = 0; i < keys; _pressed[ i++ ] = false );
	_scan_row = 0;
	_scan_status = send_scancode;
}

//
//	Initialise the system and kick off the keyboard reading 
//	process.
//
void Keypad::initialise( byte i2c_address ) {
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
	_gate.release();
	_result = TWI::error_none;
	//
	//	Add this object to the task manager.
	//
	task_manager.add_task( this, &_gate );
}

//
//	Return the value of the next key pressed (or 0
//	if no key changes are detected).
//
byte Keypad::read( void ) {
	byte		r;
	
	if( _queue.read( &r )) return( r );
	return( 0 );
}

//
//	This is the interface routine from the task management system.
//	This is called only when the TWI system has set this objects
//	flag to true.
//
void Keypad::process( void ) {
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
			if( twi.send_data( _adrs, &_buffer, 1, &_gate, &_result )) {
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
				if( !event_timer.delay_event( scan_delay, &_gate, false )) {
					//
					//	Or immediately if we cannot schedule a delay.
					//
					_gate.release();
				}
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
				if( !event_timer.delay_event( scan_delay, &_gate, false )) {
					//
					//	Or immediately if we cannot schedule a delay.
					//
					_gate.release();
				}
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
			if( twi.receive_byte( _adrs, &_buffer, &_gate, &_result )) {
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
				if( !event_timer.delay_event( scan_delay, &_gate, false )) {
					//
					//	Or immediately if we cannot schedule a delay.
					//
					_gate.release();
				}
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
				if( !event_timer.delay_event( scan_delay, &_gate, false )) {
					//
					//	Or immediately if we cannot schedule a delay.
					//
					_gate.release();
				}
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
					if( _queue.write( progmem_read_byte( keypad_mapping[ x ]) | ( s? pressed: 0 ))) _pressed[ x ] = s;
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
			if( !event_timer.delay_event( scan_delay, &_gate, false )) {
				//
				//	Or immediately if we cannot schedule a delay.
				//
				_gate.release();
			}
			break;
		}
	}
}



//
//	EOF
//
