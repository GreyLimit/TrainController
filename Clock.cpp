//
//	Clock
//	=====
//
//	The implementation of the Clock module.
//

#include "Clock.h"

//
//	The actual clock instance.
//
Clock< CLOCK_EVENTS >	event_timer;

//
//	and its associated ISR code.
//
ISR( CLK_TIMERn_COMPA_vect ) {
	event_timer.tick();
}


//
//	EOF
//
