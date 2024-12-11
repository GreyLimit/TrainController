//
//	Clock
//	=====
//
//	The implementation of the Clock module.
//

#include "Clock.h"

#include "Critical.h"
#include "Task.h"
#include "Code_Assurance.h"

//
//	Insert an event into the active list according to the
//	number of ticks specified in the left field.
//
void Clock::insert( clock_event *ptr ) {
	clock_event	*look,
			**adrs;

	adrs = &_active;
	while(( look = *adrs )) {
		if( ptr->left < look->left ) {
			ptr->next = look;
			look->left -= ptr->left;
			*adrs = ptr;
			return;
		}
		ptr->left -= look->left;
		adrs = &( look->next );
	}
	ptr->next = NIL( clock_event );
	*adrs = ptr;
}


Clock::Clock( void ) {
	//
	//	Set up the memory space we have been granted.
	//
	_active = NIL( clock_event );
	_free = NIL( clock_event );
	for( byte i = 0; i < clock_events; i++ ) {
		_events[ i ].next = _free;
		_free = &( _events[ i ]);
	}
	//
	//	Set up the clock interrupt so that we can do
	//	our job; this is critical code, no interrupts.
	//
	{
		Critical	code;

		//
		//		Set Timer to default empty values.
		//
		CLK_TCCRnA = 0;		// Set entire HW_TCCRnA register to 0
		CLK_TCCRnB = 0;		// Same for HW_TCCRnB
		CLK_TCNTn  = 0;		// Initialise counter value to 0

		//
		//	Set compare match register to
		//	generate the correct tick duration.
		//
		CLK_OCRnA = CLK_TICKS;

		//
		//		Turn on CTC mode
		//
		CLK_TCCRnA |= bit( CLK_WGMn1 );

		//
		//	Set the CSn bits (0..2) in HW_TCCRnB to select
		//	the desired pre-scaler.
		//
		CLK_TCCRnB |= CLK_CSn;
		
		//
		//	Enable timer compare interrupt
		//
		CLK_TIMSKn |= bit( CLK_OCIEnA );
	}
}

//
//	This is the interrupt routine, called every single
//	tick of the clock.  This needs to be as short as
//	possible to avoid bogging down the whole firmware.
//
void Clock::tick( void ) {
	clock_event	*ptr;
	
	//
	//	Test for active events - fall through if there
	//	are none pending.
	//
	while(( ptr = _active )) {
		//
		//	Decrement the time left on the event, if
		//	it was not zero the exit the interrupt
		//	routine.
		//
		if( ptr->left-- ) {
			//
			//	Leaving from here means that there
			//	is still time before the next event
			//	is anticipated.
			//
			return;
		}
		//
		//	The event time has arrived (left was 0).
		//	Start by unhooking it from the _active list.
		//
		_active = ptr->next;
		
		//
		//	Signal that the timer has passed (effectively
		//	the resource is released).
		//
		ptr->gate->release();
		
		//
		//	If the repeat field is non-zero then...
		//
		if(( ptr->left = ptr->repeats )) {
			//
			//	... we need to reschedule the event.
			//
			insert( ptr );
		}
		else {
			//
			//	.. or put it back in the free list.
			//
			ptr->next = _free;
			_free = ptr;
		}
	}
	//
	//	If we exit here then all the timed events have
	//	been notified and there is nothing left to do.
	//
}

//
//	This is the interface into the clock allowing new
//	"one time" or "repeating" events to be created.
//
bool Clock::delay_event( word ticks, Signal *gate, bool repeating ) {
	Critical	code;
	clock_event	*ptr;

	ASSERT( gate != NIL( Signal ));

	if(!( ptr = _free )) return( false );
	
	_free = ptr->next;
	ptr->left = ticks;
	ptr->repeats = repeating? ticks: 0;
	ptr->gate = gate;
	insert( ptr );
	return( true );
}

//
//	An "in-line" delay of a specified number of ticks.
//
//	This call will automatically return if it is running
//	inside an interrupt or critical code section without
//	any pause being experienced.
//
void Clock::inline_delay( word ticks ) {
	if( Critical::normal_code()) {
		Signal	gate;
		
		if( delay_event( ticks, &gate, false )) {
			while( !gate.acquire()) task_manager.pole_task();
		}
	}
}


//
//	The actual clock instance.
//
Clock	event_timer;

//
//	and its associated ISR code.
//
ISR( CLK_TIMERn_COMPA_vect ) {
	event_timer.tick();
}


//
//	EOF
//
