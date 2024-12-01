//
//	Districts.cpp
//	=============
//
//	The declaration of the districts manager.
//


#include "Districts.h"

//
//	Current configured for a basic Arduino Motor Shield
//
//	A and B drivers available, with 4 pins allocated to each.
//
//	These are the pin numbers "as per the motor shield".
//
//	IMPORTANT:
//
//		You *must* cut the VIN CONNECT traces on the
//		back of the Motor Shield.  You will need to
//		power the shield separately from the Arduino
//		with 15 volts DC, and leaving the VIN CONNECT
//		in place will put 15 volts across the Arduino
//		and probably damage it.
//
//		Previous versions of this firmware (and the DCC++
//		firmware) also required that the BRAKE feature
//		(for both A and B H-Bridges) should be cut too.
//		This is no longer required as this firmware
//		explicitly sets these LOW and are not touched
//		after that.
//
//		Also (unlike the DCC++ firmware) this firmware
//		requires no additional jumpers to support its
//		intended operation.
//
#define SHIELD_DRIVER_A_DIRECTION	12
#define SHIELD_DRIVER_A_ENABLE		3
#define SHIELD_DRIVER_A_BRAKE		9
#define SHIELD_DRIVER_A_LOAD		A0
#define SHIELD_DRIVER_A_ANALOGUE	0

#define SHIELD_DRIVER_B_DIRECTION	13
#define SHIELD_DRIVER_B_ENABLE		11
#define SHIELD_DRIVER_B_BRAKE		8
#define SHIELD_DRIVER_B_LOAD		A1
#define SHIELD_DRIVER_B_ANALOGUE	1


const Districts::district_data Districts::_district_data[ Districts::districts ] PROGMEM = {
	//
	//	enable			direction			adc_pin			adc_test			brake
	//	------			---------			-------			--------			-----
	//
	{	SHIELD_DRIVER_A_ENABLE,	SHIELD_DRIVER_A_DIRECTION,	SHIELD_DRIVER_A_LOAD,	SHIELD_DRIVER_A_ANALOGUE,	SHIELD_DRIVER_A_BRAKE	},
	{	SHIELD_DRIVER_B_ENABLE,	SHIELD_DRIVER_B_DIRECTION,	SHIELD_DRIVER_B_LOAD,	SHIELD_DRIVER_B_ANALOGUE,	SHIELD_DRIVER_B_BRAKE	}
}

//
//	The districts manager.
//
Districts	districts;


//
//	EOF
//
