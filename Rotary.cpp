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
#include "Clock.h"
#include "Errors.h"

//
//	Rotary scan period.
//
#ifndef ROTARY_SCAN_PERIOD
#define ROTARY_SCAN_PERIOD	MSECS(5)
#endif

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
void Rotary::initialise( byte a, byte b, byte button ) {
	
	ASSERT( a != b );
	ASSERT( b != button );
	ASSERT( a != button );

	//
	//	Configure our pins.
	//
	_pin_a.configure( a, true, true );
	_pin_b.configure( b, true, true );
	_pin_button.configure( button, true, true );
	//
	//	Set our states
	//
	_state = 0;
	_posn = 0;
	_bstate = false;
	_bcount = 0;
	//
	//	Set up the regular scanning.
	//
	event_timer.delay_event( ROTARY_SCAN_PERIOD, &_flag, true );
}

//
//	Scan the rotary controller
//
void Rotary::process( void ) {
	//
	//	Capture the rotation of the knob - no need to do
	//	anything else.
	//
	_posn += change();

	//
	//	Lets checkout the button status.
	//
	if( _pin_button.read()) {
		//
		//	Non-zero means the button has been released, and
		//	if the count is non-zero then we need to add the
		//	count value to the queue.
		//
		if( _bcount ) {
			if( !_presses.write( _bcount )) errors.log_error( ROTARY_BUTTON_QUEUE_FULL, ROTARY_BUTTON_QUEUE );
			_bcount = 0;
		}
	}
	else {
		//
		//	Zero means the button is down, all we do here is
		//	simply count how long it is being held down for.
		//
		_bcount++;
	}
}



//
//	Return the change since the last test
//
sbyte Rotary::change( void ) {
	_state = (( _state & 3 ) << 2 )|( _pin_a.read()? 2: 0 )|( _pin_b.read()? 1: 0 );
	
	ASSERT( _state < 16 );
	
	return( (sbyte)progmem_read_byte( state_change[ _state ]));
}


//
//	Get button press data.  This is how often this routine
//	was called while the button was pressed.
//
word Rotary::pressed( void ) {
	word	count;
	
	if( _presses.read( &count )) return( count );
	return( 0 );
}

//
//	Report a movement in the control.
//
sbyte Rotary::movement( void ) {
	sbyte	result;

	result = _posn;
	_posn = 0;
	return( result );
}


//
//	EOF
//

