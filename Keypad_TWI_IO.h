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


#ifndef _KEYPAD_TWI_IO_H_
#define _KEYPAD_TWI_IO_H_

#include "Arduino.h"
#include "Keypad.h"

//
//	The controlling class for the module.
//
class Keypad_TWI_IO {
private:
	//
	//	Define some constant values used in this class.
	//
	static const byte		rows		= KEYPAD_ROWS;
	static const byte		cols		= KEYPAD_COLS;
	static const byte		keys		= KEYPAD_KEYS;

	//
	//	Define the bits which form the row setting and column
	//	reading pins.
	//
	static const byte		scan_row_lsb	= KEYPAD_ROW_LSB;
	static const byte		scan_col_lsb	= KEYPAD_COL_LSB;

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
	static const byte		scan_row_mask	= KEYPAD_ROW_MASK;
	static const byte		scan_col_mask	= KEYPAD_COL_MASK;

	//
	//	Define the number of bits required to "cover" the
	//	rows and cols values (ie this is the log base 2 of
	//	values).
	//
	static const byte		scan_rows_bits	= KEYPAD_ROW_BITS;
	static const byte		scan_cols_bits	= KEYPAD_COL_BITS;

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
	enum {
		setup_required,
		setup_pending,
		scan_required,
		scan_pending,
		scan_completed
	}				_scan_status;

	//
	//	This is the buffer space used by the TWI routines, yes
	//	just a single byte.
	//
	byte				_buffer;

	//
	//	Define the size of a queue through which returned
	//	keys are buffered.
	//
	static const byte		queue_size	= keys;

	//
	//	The queue through which scanned buttons are returned to
	//	calling firmware.
	//
	byte				_queued,
					_queue_in,
					_queue_out,
					_queue[ queue_size ];
public:
	//
	//	Constructor for the Keypad object.
	//
	Keypad_TWI_IO( byte i2c_address );

	//
	//	Return the value of the next key pressed (or "none"
	//	if no key changes are detected).
	//
	byte read( void );

	//
	//	Push a keyboard button press into the queue.  Although
	//	available as part of the external interface this is
	//	intended primarily for the internal operations.
	//
	bool write( byte key );

	//
	//	IMPORTANT
	//	=========
	//
	//	This routine must be called at least once
	//	each time through the "loop()" routine
	//	to ensure that the processes supporting the
	//	KEYPAD are driven forwards generating the
	//	intended output.
	//
	void service( void );		// Use inside loop();

	//
	//	The routine through which the result of a scan
	//	is passed back.
	//
	void done( bool valid );
};

#endif

//
//	EOF
//
