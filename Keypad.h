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
#include "Task_Entry.h"
#include "TWI.h"
#include "Signal.h"
#include "Clock.h"
#include "Poly_Queue.h"

//
//	If not defined then we assume that we are using the default
//	address of the 8 bit interface board (32, 0x20).
//
#ifndef KEYPAD_ADDRESS
#define	KEYPAD_ADDRESS	0x20
#endif

//
//	Define the size of the internal keyboard buffer.
//
#ifndef KEYPAD_QUEUE_SIZE
#define KEYPAD_QUEUE_SIZE	8
#endif

//
//	The controlling class for the module.
//
class Keypad : public Task_Entry {
public:
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
		build_scancode,
		send_scancode,
		write_complete,
		read_scancode,
		read_complete
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
	Signal				_gate;
	
	//
	//	This is the error code set by the TWI routines.
	//
	TWI::error_code			_result;

	//
	//	The queue through which scanned buttons are returned to
	//	calling firmware.
	//
	Poly_Queue<byte,KEYPAD_QUEUE_SIZE> _queue;

public:
	//
	//	Constructor
	//
	Keypad( void );
	
	//
	//	Initialise the system and kick off the keyboard reading 
	//	process.
	//
	void initialise( byte i2c_address );

	//
	//	Return the value of the next key pressed (or 0
	//	if no key changes are detected).
	//
	byte read( void );

	//
	//	This is the interface routine from the task managent system.
	//	This is called only when the TWI system has set this objects
	//	flag to true.
	//
	virtual void process( byte handle );
};

#endif

//
//	EOF
//
