//
//	Keypad_TWI_IO - Arduino library to read a 4x4 matrix keypad
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

//
//	The configuration of the firmware.
//
#include "Configuration.h"
#include "Trace.h"

//
//	The environment for this module.
//
#include "Environment.h"
#include "Critical.h"
#include "Errors.h"
#include "TWI_IO.h"
#include "Keypad.h"
#include "Keypad_TWI_IO.h"

//
//	Constructor!
//	============
//
Keypad_TWI_IO::Keypad_TWI_IO( byte i2c_address ) {
	byte	i;
	
	_adrs = i2c_address;
	for( i = 0; i < keys; _pressed[ i++ ] = false );
	_scan_row = 0;
	_scan_status = setup_required;
	_queued = 0;
	_queue_in = 0;
	_queue_out = 0;
}

//
//	Return the value of the next key pressed (or 0
//	if no key changes are detected).
//
byte Keypad_TWI_IO::read( void ) {
	if( _queued ) {
		Critical	code;
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
bool Keypad_TWI_IO::write( byte key ) {
	if( _queued < queue_size ) {
		Critical	code;

		_queue[ _queue_in++ ] = key;
		if( _queue_in >= queue_size ) _queue_in = 0;
		_queued++;
		return( true );
	}
	return( false );
}

//
//	This is an "interrupt" style routine called
//	when TWI transactions have been completed
//
void Keypad_TWI_IO::done( bool valid ) {
	switch( _scan_status ) {
		case setup_pending: {
			_scan_status = valid? scan_required: setup_required;
			break;
		}
		case scan_pending: {
			_scan_status = valid? scan_completed: scan_required;
			break;
		}
		case setup_required:
		case scan_required:
		case scan_completed: {
			_scan_status = setup_required;
			break;
		}
	}
}

//
//	This is the callback routine which is passed into the TWI IO
//	system along with a (void *) cast copy of the "this" pointer
//
//	This will allow the KEYPAD code to know when the data has been
//	actually sent and the machine can be moved forwards.
//
//	This routine cannot be part of the object itself as it gets
//	called "outside" the objects framework.  Hence the need for
//	casting the link point back to the right type.
//
static void twi_callback( bool valid, void *link, UNUSED( byte *buffer ), UNUSED( byte len )) {
	((Keypad_TWI_IO *)link)->done( valid );
}

//
//	Call this routine regularily to ensure that the key pad is
//	scanned frequently.
//
void Keypad_TWI_IO::service( void ) {
	//
	//	We are waiting for a successful return from the I2C scan
	//	activity.
	//
	switch( _scan_status ) {
		case setup_required: {
			//
			//	Build the buffer up with the data we need to
			//	place into the IO port before we can read the
			//	result of the scan back.
			//
			_buffer = (( scan_row_mask ^ ( 1 << _scan_row )) << scan_row_lsb )|( scan_col_mask << scan_col_lsb );
			//
			//	.. then try to send it setting _scan_complete to
			//	false if the transmission was accepted.
			//
			if( twi_cmd_send_data( _adrs, &_buffer, 1, (void *)this, twi_callback )) _scan_status = setup_pending;
			break;
		}
		case setup_pending: {
			//
			//	Here we are simply waiting for events to happend behind the scenes
			//
			break;
		}
		case scan_required: {
			//
			//	The setup has completed correctly so initiate the read of the scan.
			//
			if( twi_cmd_receive_byte( _adrs, &_buffer, (void *)this, twi_callback )) _scan_status = scan_pending;
			break;
		}
		case scan_pending: {
			//
			//	Here we are simply waiting for more events to happend behind the scenes
			//
			break;
		}
		case scan_completed: {
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
			_scan_status = setup_required;
			break;
		}
	}
}



//
//	EOF
//
