//
//	Keypad.h
//	========
//
//	Specify key fact about the keypad
//
//

#ifndef _KEYPAD_H_
#define _KEYPAD_H_

//
//	Define the size of the keypad in rows and cols.  The
//	rows (and cols) are indexed from 0 to N-1.
//
#define KEYPAD_ROWS			4
#define KEYPAD_COLS			4

//
//	Define the total number of keys on the keypad
//
#define KEYPAD_KEYS			(KEYPAD_ROWS*KEYPAD_COLS)

//
//	Define the number of bits required to hold a row and column
//	index.
//
#define KEYPAD_ROW_BITS			2
#define KEYPAD_COL_BITS			2

//
//	Binary bit values related to the row and column values and
//	how they map into the 8 bit device IO port.
//
#define KEYPAD_ROW_MASK			0x0f
#define KEYPAD_ROW_LSB			4
#define KEYPAD_COL_MASK			0x0f
#define KEYPAD_COL_LSB			0


//
//	Declare the mapping between "row x col" and the ASCII (7-bit)
//	value returned.
//
extern const byte 			keypad_mapping[ KEYPAD_KEYS ] PROGMEM;

//
//	Declare symbolic values for elements of the keypad:
//
//	Note the number of letters ('A'..'D' in this example) and also
//	the two "control/shift" keys providing alternate meanings to
//	keypad buttons.
//
#define KEYPAD_LETTERS			4
#define KEYPAD_PAGE_SHIFT		'#'
#define KEYPAD_MENU_SHIFT		'*'

#define IS_KEYPAD_LETTER(k)		(((k)>='A')&&((k)<='D'))
#define KEYPAD_LETTER_INDEX(k)		((k)-'A')

#define IS_KEYPAD_NUMBER(k)		(((k)>='0')&&((k)<='9'))
#define KEYPAD_NUMBER_INDEX(k)		((k)-'0')


#endif

//
//	EOF
//
