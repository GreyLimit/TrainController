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
#include "Environment.h"
#include "Configuration.h"
#include "Parameters.h"
#include "Trace.h"

//
//	Environment for this module
//
#include "Code_Assurance.h"
#include "Rotary.h"
#include "Task.h"
#include "Clock.h"
#include "Errors.h"

#ifdef DEBUGGING_ENABLED
#include "Console.h"
#endif

//
//	Rotary scan period.
//
//	When debugging this needs to be *much* slower as the events
//	come into the firmware fast than they can be processed.
//
#ifndef ROTARY_SCAN_PERIOD
#define ROTARY_SCAN_PERIOD	MSECS( DEBUGGING_OPTION( 50, 5 ))
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

	STACK_TRACE( "void Rotary::initialise( byte a, byte b, byte button )" );
	
	ASSERT( a != b );
	ASSERT( b != button );
	ASSERT( a != button );

	TRACE_ROTARY( console.print( F( "ROT a = " )));
	TRACE_ROTARY( console.println( a ));
	TRACE_ROTARY( console.print( F( "ROT b = " )));
	TRACE_ROTARY( console.println( b ));
	TRACE_ROTARY( console.print( F( "ROT button = " )));
	TRACE_ROTARY( console.println( button ));
	TRACE_ROTARY( console.print( F( "ROT flag " )));
	TRACE_ROTARY( console.println( _flag.identity()));

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
	if( !task_manager.add_task( this, &_flag )) ABORT( TASK_MANAGER_QUEUE_FULL );
	if( !event_timer.delay_event( ROTARY_SCAN_PERIOD, &_flag, true )) ABORT( EVENT_TIMER_QUEUE_FULL );
}

//
//	Scan the rotary controller
//
void Rotary::process( UNUSED( byte handle )) {

	STACK_TRACE( "void Rotary::process( UNUSED( byte handle ))" );

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
			//
			//	Button has been released and the count is
			//	greater than zero so this is the first
			//	time we have been here after the button
			//	was pressed.
			//

			TRACE_ROTARY( console.print( F( "ROT pressed " )));
			TRACE_ROTARY( console.println( _bcount ));

			if( !_presses.write( _bcount )) errors.log_error( ROTARY_BUTTON_QUEUE_FULL, ROTARY_BUTTON_QUEUE );

			_bcount = 0;
		}
	}
	else {
		//
		//	Zero means the button is down, all we do here is
		//	simply count how long it is being held down for.
		//
		if(( _bcount += 1 ) == 0 ) _bcount--;
	}
}



//
//	Return the change since the last test
//
sbyte Rotary::change( void ) {

	STACK_TRACE( "sbyte Rotary::change( void )" );

	sbyte step;

	_state = (( _state & 3 ) << 2 )|( _pin_a.read()? 2: 0 )|( _pin_b.read()? 1: 0 );

	ASSERT( _state < 16 );

	step = (sbyte)progmem_read_byte( state_change[ _state ]);

	TRACE_ROTARY( if( step ))
	{
		TRACE_ROTARY( console.print( F( "ROT " )));
		TRACE_ROTARY( console.print_hex( _state ));
		TRACE_ROTARY( console.print( SPACE ));
		TRACE_ROTARY( console.println( step ));
	}

	return( step );
}


//
//	Get button press data.  This is how often this routine
//	was called while the button was pressed.
//
word Rotary::pressed( void ) {

	STACK_TRACE( "word Rotary::pressed( void )" );

	word	count;

	if( _presses.read( &count )) return( count );
	return( 0 );
}

//
//	Report a movement in the control.
//
sbyte Rotary::movement( void ) {

	STACK_TRACE( "sbyte Rotary::movement( void )" );

	sbyte	result;

	result = _posn;
	_posn = 0;
	return( result );
}


//
//	EOF
//

