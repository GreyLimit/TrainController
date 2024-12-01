//
//	ADC_Manager.h:	This AVR implementation of the ADC reading
//			environment.
//

#ifndef _AVR_MANAGER_H_
#define _AVR_MANAGER_H_

#include "Configuration.h"
#include "Environment.h"
#include "Critical.h"

//
//	Define the number of pin ADC readings which can be queued.
//
#ifndef MAXIMUM_ADC_QUEUE
#define MAXIMUM_ADC_QUEUE	16
#endif


//
//	Macro to set the input pin the ADC will continuously read until
//	called to read another pin.
//
#define MONITOR_ANALOGUE_PIN(p)		ADMUX=bit(REFS0)|((p)&0x07);ADCSRA|=bit(ADSC)|bit(ADIE)

//
//	The AVR Analogue to Digital Conversion management class.
//
class ADC_Manager {
private:
	//
	//	The following structure is used by the ADC manager to
	//	queue up multiple conversion requests so that the ADC
	//	hardware can be 100% utilised.
	//
	struct pending {
		//
		//	The actual pin that is to be read.  This is the
		//	analogue pin number (not the digital equivalent). 
		//
		byte		pin;
		//
		//	Where the value of the reading is to be placed
		//	when obtained.
		//
		word		*save;
		//
		//	The flag to be set when the reading has been
		//	collected.
		//
		bool		*flag;
		//
		//	The address of the next record.
		//
		pending		*next;
	};

	//
	//	Define the queue and free record list.
	//
	pending		*_active,
			**_tail,
			*_free;


public:
	//
	//	Constructor
	//
	ADC_Manager( void );

	//
	//	Request that a pin has its voltage read.  When
	//	the reading is complete the value obtained is
	//	stored at result and the flag is set.
	//
	bool read( byte pin, bool *flag, word *result ) {
		pending		*ptr;
		Critical	code;
		bool		initiate;

		if(!( ptr = _free )) return( false );
		initiate = ( _active == NIL( pending ));
		_free = ptr->next;
		ptr->save = result;
		ptr->flag = flag;
		ptr->next = NIL( pending );
		*_tail = ptr;
		_tail = &( ptr->next );
		if( initiate ) MONITOR_ANALOGUE_PIN( _active->pin );
	}

	//
	//	This routine is called when an ADC reading has
	//	completed.
	//
	void store( word value ) {
		pending	*ptr;
		
		if(( ptr = _active )) {
			//
			//	Remove from the list.
			//
			if(!( _active = ptr->next )) _tail = &_active
			//
			//	Save data and set flag.
			//
			*( ptr->save ) = value;
			*( ptr->flag ) = true;
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
};


//
//	Declare the ADC Manager itself.
//
extern ADC_Manager adc_manager;


#endif

//
//	EOF
//
