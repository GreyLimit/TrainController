//
//	Rotary.h
//	========
//
//	Define class that monitors the position of a rotary control
//	and returns either relative a adjustment or an absolute
//	position.
//

#ifndef _ROTARY_H_
#define _ROTARY_H_

#include "Environment.h"

//
//	Declare the class used to handle all features of a rotary
//	control (or any two sensor rotating device).
//
class Rotary {
private:
	//
	//	Store the pin numbers we are watching.
	//
	byte	_pin_a,
		_pin_b,
		_pin_button;
	
	//
	//	This is our current state
	//
	byte	_state;
	
	//
	//	Where we think we are in absolute terms
	//
	byte	_posn;

	//
	//	Return the change since the last test
	//
	sbyte change( void );
	
	//
	//	Button related code (if present)
	//
	bool	_has_button,
		_bstate;
	word	_bcount;
		
public:
	//
	//	Constructors.
	//
	Rotary( byte a, byte b );
	Rotary( byte a, byte b, byte button );
	
	//
	//	Report a movement in the control.
	//
	sbyte movement( void );
	
	//
	//	Report cuurrent position
	//
	byte position( void );
	
	//
	//	Reset current position
	//
	void reset( byte posn );
	
	//
	//	Get button press data.  This is how often this routine
	//	was called while the button was pressed.
	//
	word pressed( void );
};

#endif

//
//	EOF
//
