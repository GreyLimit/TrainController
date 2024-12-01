//
//	Driver.h
//	========
//
//	Define the interface between the DCC Generator code and the
//	physical pins through which the signal is modulated.
//

#ifndef _DRIVER_H_
#define _DRIVER_H_

//
//	Get the environment.
//
#include "Configuration.h"
#include "Environment.h"
#include "Pin_IO.h"

//
//	Set the maximum number of districts we want to be able to handle.
//
#ifndef MAXIMUM_DISTRICTS
#define MAXIMUM_DISTRICTS	8
#endif

//
//	Define the class that encapsulates the output pin code.
//
//
//	This implementation adopts the "pin at a time" mechanism
//	so allowing for output pins to be arranged to suite the MCU.
//
class Driver {
private:
	//
	//	What we need to know about each "district" of the DCC
	//	environment.
	//
	struct driver {
		Pin_IO		enable,
				direction;
	};
	//
	//	Define the array of pins which we will be using to
	//	access to real world.
	//
	driver	_district[ MAXIMUM_DISTRICTS ];
	byte	_districts;


public:
	//
	//	Constructor
	//
	Driver( void ) {
		_districts = 0;
	}

	//
	//	Add a new pin to the configuration of the driver.
	//
	bool add( byte *index, byte enable, byte direction ) {
		driver	*d;
		
		//
		//	This is a pin referenced by the platform pin number.
		//
		if( _districts >= MAXIMUM_DISTRICTS ) return( false );

		//
		//	Locate a new record (and save its index number).
		//
		d = &( _district[(( *index = _districts++ ))]);

		//
		//	Update record and initialise the pins.
		//
		d->enable.configure( enable, false );
		d->enable.low();
		d->direction.configure( direction, false );
		d->direction.low()

		//
		//	Done!
		//
		return( true );
	}
	bool add( byte *index, byte enable_dev, byte enable_bitno, byte direction_dev, byte direction_bitno ) {
		driver	*d;
		
		//
		//	This is the pin referenced by port number and bit
		//	within port number.
		//
		if( _districts >= MAXIMUM_DISTRICTS ) return( false );

		//
		//	Locate a new record (and save its index number).
		//
		d = &( _district[(( *index = _districts++ ))]);

		//
		//	Update record and initialise the pins.
		//
		d->enable.configure( enable_dev, enable_bitno, false );
		d->enable.low();
		d->direction.configure( direction_dev, direction_bitno, false );
		d->direction.low()

		//
		//	Done.
		//
		return( true );
	}

	//
	//	Now the DCC generator will use the following API calls.
	//
	
	//
	//	Turn on all or one district
	//
	void on( void ) {
		for( byte i = 0; i < _districts; _district[ i++ ].enable.high());
	}
	void on( byte index ) {
		if( index < _districts ) _district[ index ].enable.high());
	}

	//
	//	Turn off all or one district
	//
	void off( void ) {
		for( byte i = 0; i < _districts; _district[ i++ ].enable.low());
	}
	void off( byte index ) {
		if( index < _districts ) _district[ index ].enable.low());
	}

	//
	//	Toggle the output signal of all or a single district.
	//
	void toggle( void ) {
		for( byte i = 0; i < _districts; _district[ i++ ].direction.toggle());
	}
	void toggle( byte index ) {
		if( index < _districts ) _district[ index ].direction.toggle());
	}

	//
	//	Generic power on/off call.
	//
	void power( bool on ) {
		if( on ) {
			for( byte i = 0; i < _districts; _district[ i++ ].enable.high());
		}
		else {
			for( byte i = 0; i < _districts; _district[ i++ ].enable.low());
		}
	}
	void power( byte index, bool on ) {
		if( index < _districts ) {
			if( on ) {
				_district[ index ].enable.high());
			}
			else {
				_district[ index ].enable.low());
			}
		}
	}

};

//
//	Here we declare the driver object.
//
extern Driver dcc_driver;

#endif

//
//	EOF
//
