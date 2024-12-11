//
//	ADC_Manager.h:	This AVR implementation of the ADC reading
//			environment.
//

#ifndef _AVR_MANAGER_H_
#define _AVR_MANAGER_H_

#include "Configuration.h"
#include "Environment.h"
#include "Signal.h"

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
	//	Define how many ADC conversions we are prepared to backlog
	//
	static const byte	pending_queue = MAXIMUM_ADC_QUEUE;
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
		Signal		*flag;
		//
		//	The address of the next record.
		//
		pending		*next;
	};

	//
	//	Define the queue and free record list.
	//
	pending		_pending[ pending_queue ],
			*_active,
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
	bool read( byte pin, Signal *flag, word *result );

	//
	//	This routine is called when an ADC reading has
	//	completed.
	//
	void store( word value );
};


//
//	Declare the ADC Manager itself.
//
extern ADC_Manager adc_manager;


#endif

//
//	EOF
//
