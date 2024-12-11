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
#include "Parameters.h"
#include "Configuration.h"
#include "Pin_IO.h"
#include "Signal.h"
#include "Poly_Queue.h"
#include "Task_Entry.h"

//
//	Make up the size of the internal rotary button queue.
//
#ifndef ROTARY_BUTTON_QUEUE
#define ROTARY_BUTTON_QUEUE	4
#endif

//
//	Declare the class used to handle all features of a rotary
//	control (or any two sensor rotating device).
//
class Rotary : public Task_Entry {
private:
	//
	//	Store the pin numbers we are watching.
	//
	Pin_IO		_pin_a,
			_pin_b,
			_pin_button;
	
	//
	//	This is our current state
	//
	byte		_state;
	
	//
	//	This is the rotary adjustment since the object was
	//	last asked about it.
	//
	sbyte		_posn;

	//
	//	Return the change since the last test
	//
	sbyte change( void );
	
	//
	//	Button related code.
	//
	bool		_bstate;
	word		_bcount;

	//
	//	Include a small byte buffer for the button presses.
	//
	Poly_Queue< word, ROTARY_BUTTON_QUEUE >
			_presses;

	//
	//	This is the flag that calls up the regular checking of
	//	the rotary knob.
	//
	Signal		_flag;
		
public:
	//
	//	Set up the control knob.
	//
	void initialise( byte a, byte b, byte button );

	//
	//	Scan the rotary controller
	//
	virtual void process( void );
	
	//
	//	Report a movement in the control.
	//
	sbyte movement( void );

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
