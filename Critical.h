//
//	Critical
//
//	This file defines a class which is used to control access to
//	a section of code (through the C++ constructor/destructor
//	mechanism).
//


#ifndef _CRTITICAL_H_
#define _CRTITICAL_H_

#include "Environment.h"
#include "Library_Types.h"

//
//	A variable of class Critical is simply declared at the start of
//	a block of code.  The Constructor and Destructor actions of the
//	class ensure that the whole block remains atomic and cannot be
//	interrupted.
//
//	Example:
//
//		{
//			Critical code;
//
//			...blah blah blah...
//
//		}
//
//	Simples.
//

#if defined( ARDUINO_ARCH_AVR ) || defined( ARDUINO_ARCH_MEGAAVR )

//
//	The Atmel AVR Implementation.
//
class Critical {
	private:
		//
		//	The bit of the status byte which controls the
		//	handling of interrupts and enables code to be
		//	flagged as critical.
		//
		//	This is SREG bit 7: ""I".
		//
		static const byte global_interrupt_enable = bit( 7 );
		
		//
		//	Somewhere to save a copy of the Status
		//	Register
		//
		byte	_sreg;

	public:
		//
		//	Called when entering the block
		//
		Critical( void ) {
			_sreg = SREG;
			SREG &= ~global_interrupt_enable;
		}
		//
		//	Called when leaving the block
		//
		~Critical() {
			SREG = _sreg;
		}
	//
	//	Simple boolean functions return true if
	//
	//		execution is taking place inside an interrupt or Critical section.
	//
	//		execution is taking place as normal code.
	//
	static inline bool critical_code( void ) {
		return(( SREG & global_interrupt_enable ) == 0 );
	}
	static inline bool normal_code( void ) {
		return(( SREG & global_interrupt_enable ) != 0 );
	}

	//
	//	Routines that provide direct access to interrupts controls.
	//	These should not be used under any normal circumstances.
	//
	static inline void enable_interrupts( void ) {
		SREG |= global_interrupt_enable;
	}
	static inline void disable_interrupts( void ) {
		SREG &= ~global_interrupt_enable;
	}
};


#else

#error "Define Class Critical for your architecture."


#endif



#endif

//
//	EOF
//
