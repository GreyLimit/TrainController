//
//	Districts.cpp
//	=============
//
//	The declaration of the districts manager.
//


#include "Districts.h"
#include "Code_Assurance.h"
#include "Trace.h"

#ifdef DEBUGGING_ENABLED
#include "Console.h"
#endif

//
//	Currently configured for a basic Arduino Motor Shield
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
#define SHIELD_DRIVER_A_ZONE		1

#define SHIELD_DRIVER_B_DIRECTION	13
#define SHIELD_DRIVER_B_ENABLE		11
#define SHIELD_DRIVER_B_BRAKE		8
#define SHIELD_DRIVER_B_LOAD		A1
#define SHIELD_DRIVER_B_ANALOGUE	1
#define SHIELD_DRIVER_B_ZONE		1


const Districts::district_data Districts::_district_data[ Districts::districts ] PROGMEM = {
	//
	//	enable			direction			adc_pin			adc_test			brake			Zone
	//	------			---------			-------			--------			-----			----
	//
	{	SHIELD_DRIVER_A_ENABLE,	SHIELD_DRIVER_A_DIRECTION,	SHIELD_DRIVER_A_LOAD,	SHIELD_DRIVER_A_ANALOGUE,	SHIELD_DRIVER_A_BRAKE,	SHIELD_DRIVER_A_ZONE	},
	{	SHIELD_DRIVER_B_ENABLE,	SHIELD_DRIVER_B_DIRECTION,	SHIELD_DRIVER_B_LOAD,	SHIELD_DRIVER_B_ANALOGUE,	SHIELD_DRIVER_B_BRAKE,	SHIELD_DRIVER_B_ZONE	}
};

//
//	Allow this to build itself empty first.
//
Districts::Districts( void ) {
	_zone = 0;
}

//
//	Initialise the districts and set them into action.
//
void Districts::initialise( void ) {
	
	STACK_TRACE( "void Districts::initialise( void )" );
	
	//
	//	Set up the districts according to the built in
	//	table.
	//
	for( byte i = 0; i < districts; i++ ) {
		const district_data	*d;
		Pin_IO			brake;

		d = &( _district_data[ i ]);
		
		brake.configure( progmem_read_byte( d->brake ), false );
		brake.low();

		//
		//	Ensure zone 0 is never used (zone 0 is all off).
		//
		ASSERT( progmem_read_byte( d->zone ) > 0 );
		
		_district[ i ].assign(	progmem_read_byte( d->enable ),
					progmem_read_byte( d->direction ),
					progmem_read_byte( d->adc_pin ),
					progmem_read_byte( d->adc_test ));
	}
	
	//
	//	Ensure everything is off.
	//
	for( byte i = 0; i < districts; _district[ i++ ].power( false ));
	_zone = 0;
}


//
//	Return the number of the zone currently being operated.
//
byte Districts::zone( void ) {
	
	STACK_TRACE( "byte Districts::zone( void )" );
	
	return( _zone );
}



//
//	Set the power to target zone.
//
bool Districts::power( byte zone ) {
	
	STACK_TRACE( "bool Districts::power( byte zone )" );
	
	byte	count;

	_zone = zone;
	count = 0;
	for( byte i = 0; i < districts; i++ ) {
		if( progmem_read_byte( _district_data[ i ].zone ) == zone ) {
			count++;
			_district[ i ].power( true );
		}
		else {
			_district[ i ].power( false );
		}
	}
	return(( zone == 0 )||( count > 0 ));
}

//
//	Return current load average (0-100) for indicated district
//
byte Districts::load_average( byte index ) {
	
	STACK_TRACE( "byte Districts::load_average( byte index )" );
	
	if( index >= districts ) return( 0 );
	return( _district[ index ].load_average());
}

//
//	Return the state of this district
//
District::district_state Districts::state( byte index ) {
	
	STACK_TRACE( "District::district_state Districts::state( byte index )" );
	
	if( index >= districts ) return( District::state_unassigned );
	return( _district[ index ].state());
}


//
//	The districts manager.
//
Districts	districts;


//
//	EOF
//
