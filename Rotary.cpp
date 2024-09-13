//
//	Rotary.cpp
//	==========
//
//	Implement the class that monitors the position of a rotary
//	control and returns either relative a adjustment or an
//	absolute position.
//

//
//	The configuration of the firmware.
//
#include "Configuration.h"
#include "Trace.h"

//
//	Environment for this module
//
#include "Environment.h"
#include "Rotary.h"
#include "Code_Assurance.h"


//
//	The static state translation table.
//
//	This needs to capture the meaning of the state/signal
//	transitions the two pins (a and b) make as the control
//	is rotated.
//
//	This is best captured with the following ASCII diagram:
//		  ___     ___     ___     ___    
//	A	_|   |___|   |___|   |___|   |__
//		    ___     ___     ___     ___ 
//	B	___|   |___|   |___|   |___|   |
//
//
static constexpr sbyte state_change[ 16 ] PROGMEM {
//
//	Effect		Prev a	Prev b	New a	New b	Meaning
//	------		------	------	-----	-----	-------
	0,	//	0	0	0	0	No Change
	-1,	//	0	0	0	1	AntiClockwise
	1,	//	0	0	1	0	Clockwise
	0,	//	0	0	1	1	Error
	1,	//	0	1	0	0	Clockwise
	0,	//	0	1	0	1	No Change
	0,	//	0	1	1	0	Error
	-1,	//	0	1	1	1	AntiClockwise
	-1,	//	1	0	0	0	AntiClockwise
	0,	//	1	0	0	1	Error
	0,	//	1	0	1	0	No Change
	1,	//	1	0	1	1	Clockwise
	0,	//	1	1	0	0	Error
	1,	//	1	1	0	1	Clockwise
	-1,	//	1	1	1	0	AntiClockwise
	0	//	1	1	1	1	No Change
	
};

//
//	Constructor.
//
Rotary::Rotary( byte a, byte b ) {
	
	ASSERT( a != b );
	
	_pin_a = a;
	_pin_b = b;
	_pin_button = 255;		// An invalid pin number (hopefully).
	
	pinMode( a, INPUT_PULLUP );
	pinMode( b, INPUT_PULLUP );
	
	_state = 0;
	_posn = 0;
	
	_has_button = false;
	_bstate = false;
}
Rotary::Rotary( byte a, byte b, byte button ) {
	
	ASSERT( a != b );
	ASSERT( b != button );
	ASSERT( a != button );
	
	_pin_a = a;
	_pin_b = b;
	_pin_button = button;
	
	pinMode( a, INPUT_PULLUP );
	pinMode( b, INPUT_PULLUP );
	
	_state = 0;
	_posn = 0;
	
	pinMode( button, INPUT_PULLUP );
	
	_has_button = true;
	_bstate = false;
}

//
//	Return the change since the last test
//
sbyte Rotary::change( void ) {
	_state = (( _state & 3 ) << 2 )|(( digitalRead( _pin_a ) == HIGH )? 2: 0 )|(( digitalRead( _pin_b ) == HIGH )? 1: 0 );
	
	ASSERT( _state < 16 );
	
	return( (sbyte)progmem_read_byte( state_change[ _state ]));
}


//
//	Report a movement in the control.
//
sbyte Rotary::movement( void ) {
	sbyte	e;
	
	_posn += ( e = change());
	return( e );
}

//
//	Report cuurrent position
//
byte Rotary::position( void ) {
	return( _posn += change());
}

//
//	Reset current position
//
void Rotary::reset( byte posn ) {
	_posn = posn;
}

//
//	Get button press data.  This is how often this routine
//	was called while the button was pressed.
//
word Rotary::pressed( void ) {
	if( _has_button ) {
		if( digitalRead( _pin_button ) == LOW ) {
			//
			//	Button is being held down (pin is shorted to earth)
			//
			if( _bstate ) {
				//
				//	was already down, keep counting.
				//
				if(!( _bcount += 1 )) _bcount -= 1;
			}
			else {
				//
				//	Just pressed - start counting.
				//
				_bstate = true;
				_bcount = 0;
			}
		}
		else {
			//
			//	Button is not held down (pin pulled high by internal resistor).
			//
			if( _bstate ) {
				//
				//	Was held down before, so flag released and return count.
				//
				_bstate = false;
				return( _bcount );
			}
		}
	}
	return( 0 );
}


//
//	EOF
//

