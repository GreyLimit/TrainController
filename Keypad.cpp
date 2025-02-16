//
//	Keypad.cpp
//	==========
//

#include "Environment.h"
#include "Parameters.h"
#include "Configuration.h"

#include "Code_Assurance.h"
#include "Keypad.h"
#include "Clock.h"
#include "Task.h"
#include "Trace.h"

#ifdef DEBUGGING_ENABLED
#include "Console.h"
#endif

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
	_scan_status = build_scancode;
}

//
//	Initialise the system and kick off the keyboard reading 
//	process.
//
void Keypad::initialise( byte i2c_address ) {

	STACK_TRACE( "void Keypad::initialise( byte i2c_address )" );

	TRACE_KEYPAD( console.print( F( "KEY address " )));
	TRACE_KEYPAD( console.println_hex( i2c_address ));
	TRACE_KEYPAD( console.print( F( "KEY gate " )));
	TRACE_KEYPAD( console.println( _gate.identity()));

	//
	//	Save our address
	//
	_adrs = i2c_address;
	_buffer = 0;
	_result = TWI::error_none;
	
	//
	//	Add this object to the task manager.
	//
	if( !task_manager.add_task( this, &_gate )) ABORT( TASK_MANAGER_QUEUE_FULL );
	
	//
	//	Release the task flag so that the setup process is
	//	initiated automatically by the task manager.
	//
	_gate.release();
}

//
//	Return the value of the next key pressed (or 0
//	if no key changes are detected).
//
byte Keypad::read( void ) {

	STACK_TRACE( "byte Keypad::read( void )" );

	byte		r;
	
	if( _queue.read( &r )) return( r );
	return( 0 );
}

//
//	This is the interface routine from the task management system.
//	This is called only when the TWI system has set this objects
//	flag to true.
//
void Keypad::process( UNUSED( byte handle )) {

	STACK_TRACE( "void Keypad::process( byte handle )" );

	//
	//	We are waiting for a successful return from the I2C scan
	//	activity.
	//
	switch( _scan_status ) {
		//
		//	State associated with setting the scan mask.
		//
		case build_scancode: {

			TRACE_KEYPAD( console.println( F( "KEY build_scancode" )));

			//
			//	Construct the buffer with the data we need to
			//	place into the IO port before we can read the
			//	result of the scan back.
			//
			_buffer = (( scan_row_mask ^ ( 1 << _scan_row )) << scan_row_lsb )|( scan_col_mask << scan_col_lsb );

			//
			//	We fall through to the code sending the data.
			//
			_scan_status = send_scancode;
			FALL_THROUGH;
		}
		case send_scancode: {

			TRACE_KEYPAD( console.print( F( "KEY send_scancode " )));
			TRACE_KEYPAD( console.print_hex( _buffer ));
			TRACE_KEYPAD( console.print( F( " to " )));
			TRACE_KEYPAD( console.println_hex( _adrs ));

			//
			//	Send the scan code:
			//
			if( twi.send_data( _adrs, &_buffer, 1, &_gate, &_result )) {

				TRACE_KEYPAD( console.println( F( "KEY ok" )));

				//
				//	Successfully scheduled transaction, so move the state to "write_complete"
				//	and wait for the TWI device to flag that the command has completed.
				//
				_scan_status = write_complete;
			}
			else {

				TRACE_KEYPAD( console.println( F( "KEY fail" )));
				
				//
				//	failed to queue the scan so we will try again shortly
				//
				if( !event_timer.delay_event( scan_delay, &_gate, false )) {

					TRACE_KEYPAD( console.println( F( "KEY delay fail" )));

					//
					//	Report schedule error.
					//
					errors.log_error( EVENT_TIMER_QUEUE_FULL, scan_delay );

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
		case write_complete: {

			TRACE_KEYPAD( console.println( F( "KEY write_complete" )));
			TRACE_KEYPAD( console.print( F( "KEY result " )));
			TRACE_KEYPAD( console.println( (byte)_result ));

			//
			//	Check the return code from send_scancode.
			//
			if( _result != TWI::error_none ) {

				TRACE_KEYPAD( console.println( F( "KEY write error" )));

				//
				//	The scan initialisation failed, so we log an error
				//	then try to reset the restart the scan.
				//
				errors.log_error( I2C_COMMS_ERROR, (word)_result );

				//
				//	Restart the keypad scan in a short while.
				//
				_scan_status = build_scancode;
				if( !event_timer.delay_event( scan_delay, &_gate, false )) {

					TRACE_KEYPAD( console.println( F( "KEY delay failed" )));

					//
					//	Report schedule error.
					//
					errors.log_error( EVENT_TIMER_QUEUE_FULL, scan_delay );

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
		case read_scancode: {

			TRACE_KEYPAD( console.println( F( "KEY read_scancode" )));

			//
			//	Initiate the read of the scan data:
			//
			if( twi.receive_byte( _adrs, &_buffer, &_gate, &_result )) {

				TRACE_KEYPAD( console.println( F( "KEY ok" )));

				//
				//	Successfully scheduled the read of the scan code, so set
				//	state to "read_complete" and await the reply.
				//
				_scan_status = read_complete;
			}
			else {

				TRACE_KEYPAD( console.println( F( "KEY fail" )));

				//
				//	Unsuccessfully scheduled a read so we try again shortly.
				//
				if( !event_timer.delay_event( scan_delay, &_gate, false )) {

					TRACE_KEYPAD( console.println( F( "KEY delay fail" )));

					//
					//	Report schedule error.
					//
					errors.log_error( EVENT_TIMER_QUEUE_FULL, scan_delay );

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
		case read_complete: {
			byte	c, r, b, x;
			bool	s;

			TRACE_KEYPAD( console.println( F( "KEY read_complete" )));

			//
			//	Check the return code form the TWI call made by the
			//	"write_complete" state.
			//
			if( _result != TWI::error_none ) {

				TRACE_KEYPAD( console.print( F( "KEY failed " )));
				TRACE_KEYPAD( console.println( (word)_result ));

				//
				//	Reading the scancode failed; Log an
				//	error, reset next state to build_scancode
				//	then schedule the task manager to call us again.
				//
				errors.log_error( I2C_COMMS_ERROR, (word)_result );

				//
				//	Restart the keypad scan in a short while.
				//
				_scan_status = read_scancode;
				if( !event_timer.delay_event( scan_delay, &_gate, false )) {

					TRACE_KEYPAD( console.println( F( "KEY delay fail" )));

					//
					//	Report schedule error.
					//
					errors.log_error( EVENT_TIMER_QUEUE_FULL, scan_delay );

					//
					//	Or immediately if we cannot schedule a delay.
					//
					_gate.release();
				}
				break;
			}
			
			//
			//	Now we work on the data in the buffer.
			//

			TRACE_KEYPAD( console.print( F( "KEY buffer " )));
			TRACE_KEYPAD( console.println_hex( _buffer ));

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
				//	received state then the button has
				//	moved (changed state).
				//
				x = r | c;

				TRACE_KEYPAD( console.print( F( "KEY button " )));
				TRACE_KEYPAD( console.println( x ));

				if( _pressed[ x ] != s ) {

					TRACE_KEYPAD( console.print( F( "KEY key " )));
					TRACE_KEYPAD( console.print( (char)progmem_read_byte( keypad_mapping[ x ])));
					TRACE_KEYPAD( console.println( s? F( " pressed" ): F( " released" )));

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
			_scan_status = build_scancode;
			if( !event_timer.delay_event( scan_delay, &_gate, false )) {

				TRACE_KEYPAD( console.println( F( "KEY delay fail" )));

				//
				//	Report schedule error.
				//
				errors.log_error( EVENT_TIMER_QUEUE_FULL, scan_delay );

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
