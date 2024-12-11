//
//	ADC_IO.cpp:	The implementation of the AVR ADC_IO object
//

//
//	Bring in the library elements required.
//
#include "ADC_Manager.h"
#include "Critical.h"
#include "Code_Assurance.h"

//
//	Constructor
//
ADC_Manager::ADC_Manager( void ) {
	_active = NIL( pending );
	_tail = &_active;
	_free = NIL( pending );
	for( byte i = 0; i < pending_queue; i++ ) {
		_pending[ i ].next = _free;
		_free = &( _pending[ i ]);
	}
}

//
//	Request that a pin has its voltage read.  When
//	the reading is complete the value obtained is
//	stored at result and the flag is set.
//
bool ADC_Manager::read( byte pin, Signal *flag, word *result ) {
	pending		*ptr;
	bool		initiate;
	
	Critical	code;

	//
	//	Can we schedule this?
	//
	if(!( ptr = _free )) return( false );

	//
	//	Will we need to "kick off" the conversion?
	//
	initiate = ( _active == NIL( pending ));

	//
	//	Grab a free pending record and set it up.
	//
	_free = ptr->next;
	ptr->pin = pin;
	ptr->save = result;
	ptr->flag = flag;
	ptr->next = NIL( pending );
	*_tail = ptr;
	_tail = &( ptr->next );

	ASSERT( _active != NIL( pending ));

	//
	//	Fire off conversion if required.
	//
	if( initiate ) MONITOR_ANALOGUE_PIN( _active->pin );

	//
	//	Success!
	//
	return( true );
}

//
//	This routine is called when an ADC reading has
//	completed.
//
void ADC_Manager::store( word value ) {
	pending	*ptr;
	
	if(( ptr = _active )) {
		//
		//	Remove from the list.
		//
		if(!( _active = ptr->next )) _tail = &_active;
		//
		//	Save data and signal new data.
		//
		*( ptr->save ) = value;
		ptr->flag->release();
		//
		//	Return to the free list.
		//
		ptr->next = _free;
		_free = ptr;
		//
		//	Kick off the next ADC request?
		//
		if( _active ) MONITOR_ANALOGUE_PIN( _active->pin );
	}
}


//
//	Declare the ADC Manager itself.
//
ADC_Manager adc_manager;

//
//	Define the Interrupt Service Routine (ISR) which will
//	read the data and store it.  Restarting another ADC
//	conversion is done elsewhere.
//
ISR( ADC_vect ) {
	byte	low, high;

	//
	//	We have to read ADCL first; doing so locks both ADCL
	//	and ADCH until ADCH is read.  reading ADCL second would
	//	cause the results of each conversion to be discarded,
	//	as ADCL and ADCH would be locked when it completed.
	//
	low = ADCL;
	high = ADCH;
	//
	//	Store the reading in the ADC manager.
	//
	adc_manager.store( HL_TO_W( high, low ));
}

//
//	EOF
//



