//
//	Clock
//	=====
//
//	The module provides alternative mechanism to the built-in
//	millis() and micros() system calls, by providing a system
//	based on relative timing over absolute timing so that "roll
//	round" errors/issues are avoided.
//
//	This also removes the necessity for many, many unsigned 32 bit
//	comparisons, and should be much faster.
//

#ifndef _CLOCK_H_
#define _CLOCK_H_

//
//	We will need to Environment and Critical modules to define
//	the clock code.
//
#include "Parameters.h"
#include "Environment.h"
#include "Critical.h"

//
//	Define, in microseconds, the duration of a "tick", the unit of
//	time which this module will operate in.
//
#define CLOCK_TICK	50

//
//	Define (if not already defined) the maximum number of clock
//	events which the system will handle.
//
#ifndef CLOCK_EVENTS
#define CLOCK_EVENTS	8
#endif

//
//	Define our base clock tick dependent on the underlying hardware.
//
#if defined( __AVR_ATmega328__ )| defined( __AVR_ATmega328P__ )| defined( __AVR_ATmega328PB__ )
//
//	Nano or Uno R3 configuration
//	============================
//

//
//	Map Clock symbols onto target Timer hardware
//
#define CLK_COUNTER_BITS	8
#define CLK_TCCRnA		TCCR0A
#define CLK_TCCRnB		TCCR0B
#define CLK_TIMERn_COMPA_vect	TIMER0_COMPA_vect
#define CLK_TCNTn		TCNT0
#define CLK_OCRnA		OCR0A
#define CLK_WGMn1		WGM01
#define CLK_CSn0		CS00
#define CLK_CSn1		CS01
#define CLK_TIMSKn		TIMSK0
#define CLK_OCIEnA		OCIE0A


#elif defined( __AVR_ATmega2560__ )
//
//	Mega 2560 configuration
//	=======================
//

//
//	Map Clock symbols onto target Timer hardware
//
#define CLK_COUNTER_BITS	8
#define CLK_TCCRnA		TCCR0A
#define CLK_TCCRnB		TCCR0B
#define CLK_TIMERn_COMPA_vect	TIMER0_COMPA_vect
#define CLK_TCNTn		TCNT0
#define CLK_OCRnA		OCR0A
#define CLK_WGMn1		WGM01
#define CLK_CSn0		CS00
#define CLK_CSn1		CS01
#define CLK_TIMSKn		TIMSK0
#define CLK_OCIEnA		OCIE0A

#else

#error "Clock timer has not been configured for this board"

#endif

//
//	Using the define CPU cycles per second, F_CPU, calculate how
//	many clock cycles are required to make up a period of CLOCK_TICK
//	usecs.
//
#define CYCLES_PER_TICK	(CLOCK_TICK*(F_CPU/1000000))

//
//	Calculate the range of the timers counter
//
#define CLK_RANGE	(1<<CLK_COUNTER_BITS)

//
//	Determine the clock pre-scaler required to get this into
//	an 8-bit timer counter.
//
//	The CSn2, CSn2 and CSn0 bits of the TCCRnB register select
//	the pre-scaler using the following values:
//
//		CSn2	CSn1	CSn0	Decimal	Clock Effect
//		----	----	----	-------	------------
//
//		0	0	0	0	No clock source (Timer/Counter stopped)
//		0	0	1	1	clk (no prescaling)
//		0	1	0	2	clk/8 (from prescaler)
//		0	1	1	3	clk/64 (from prescaler)
//		1	0	0	4	clk/256 (from prescaler)
//		1	0	1	5	clk/1024 (from prescaler)
//		1	1	0	6	External clock source on T0 pin. Clock on falling edge.
//		1	1	1	7	External clock source on T0 pin. Clock on rising edge.
//

//
//	Try a pre-scaler of 1.
//	----------------------
//
#if CYCLES_PER_TICK < CLK_RANGE

#define CLK_CSn		1
#define CLK_TICKS	CYCLES_PER_TICK

//
//	Try a pre-scaler of 8.
//	----------------------
//
#elif CYCLES_PER_TICK < (CLK_RANGE*8)

#define CLK_CSn		2
#define CLK_TICKS	(CYCLES_PER_TICK/8)

//
//	Try a pre-scaler of 64.
//	----------------------
//
#elif CYCLES_PER_TICK < (CLK_RANGE*64)

#define CLK_CSn		3
#define CLK_TICKS	(CYCLES_PER_TICK/64)

//
//	Call any other option an error.
//
#else

#error "Clock timer calculation error."

#endif

//
//	The following two macros convert Millisecond and Microsecond
//	values into a number of CLOCK_TICK units.  For Milliseconds these
//	will always be accurate (assuming CLOCK_TICK is a factor of 1000).
//	For microseconds this cannot be the case since the value of
//	CLOCK_TICK is defined in microseconds there is always going to
//	be a rounding effect.
//
//	Working on the premise that microsecond delays are for
//	the purpose of timing delays with hardware, then the calculation
//	will always round up to the next CLOCK_TICK; A microsecond
//	duration should be read as "not less than X microseconds".
//
//	It is advised that these macros are only ever used with constant
//	values that can be pre-calculated at compile time (or the calculation
//	could overwhelm the delays requested!)
//
#define MSECS(t)	(((t)*1000)/CLOCK_TICK)
#define USECS(t)	(((t)+(CLOCK_TICK-1))/CLOCK_TICK)

//
//	Define the software interface to the clock routines.  These
//	routines facilitate the setting of boolean flags "at a set
//	distance" into the future.  These are not intended for "cycle
//	accurate" timed events, but for the sequencing of software
//	based activities which are paced through time in a controlled
//	fashion.
//
class Clock {
private:
	//
	//	Our task managing structure.
	//
	struct clock_event {
		word		left,		// Ticks left to event.
				repeats;	// Repeating value if non-zero.
		bool		*event;		// The event to set.
		clock_event	*next;
	}
	//
	//	Our task managing pointers and the array of records
	//	which we have to manage.
	//
	clock_event	*_active,
			*_free,
			_events[ CLOCK_EVENTS ];

	//
	//	Insert an event into the active list according to the
	//	number of ticks specified in the left field.
	//
	void insert( clock_event *ptr ) {
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

public:
	Clock( void ) {
		//
		//	Set up the memory space we have been granted.
		//
		_active = NIL( clock_event );
		_free = NIL( clock_event );
		for( byte i = 0; i < CLOCK_EVENTS; i++ ) {
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
	void tick( void ) {
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
			//	Trigger the event.
			//
			*( ptr->event ) = true;
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
	bool delay_event( word ticks, bool *event, bool repeating ) {
		Critical	code;
		clock_event	*ptr;

		if(!( ptr = _free )) return( false );
		
		_free = ptr->next;
		ptr->left = ticks;
		ptr->repeats = repeating? ticks: 0;
		ptr->event = event;
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
	void inline_delay( word ticks ) {
		if( Critical::normal_code()) {
			volatile bool flag = false;
			
			if( delay_event( ticks, &flag, false )) {
				while( !flag ) task_manager.pole_task();
			}
		}
	}
};


//
//	Finally we define the actual clock instance.
//
extern Clock	event_timer;


#endif

//
//	EOF
//
