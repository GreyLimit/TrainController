//
//	ADC_IO.cpp:	The implementation of the AVR ADC_IO object
//

//
//	Bring in the library elements required.
//
#include "ADC_Manager.h"

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
	adc_manager.store( HL_TO_W( high, low ))
}

//
//	EOF
//



