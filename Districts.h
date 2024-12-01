//
//	Districts.h
//	===========
//
//	Declare the module which defines and creates the set
//	of districts supported by the firmware.
//

#ifndef _DISTRICTS_H_
#define _DISTRICTS_H_

//
//	Bring in the studd we need.
//
#include "Configuration.h"
#include "Environment.h"
#include "District.h"

//
//	Define the object holding all of the districts and providing
//	the "high level" access and control of the districts.
//
class Districts {
public:
	//
	//	Define the number of districts which we are going to handle.
	//
	//	This number here is really limited by the number of ADC lines which
	//	the MCU has available.  On Arduino UNO/Nano this is typically 6
	//	(that is 8 less the I2C SDA/SCL lines).
	//
	//	That being said, this is also limited by the number of H-Bridge
	//	driver devices which can be attached to the MCU.  With the
	//	standard "Arduino Motor Driver Shield" This is actually a
	//	much lower number; 2.
	//
	static const byte	districts = 2;
	
private:
	//
	//	Declare the set of districts which we will be managing.
	//
	District	_district[ districts ];

	//
	//	Declare the data structure that provides the input data
	//	to facilitate the configuration of the individual
	//	districts.
	//
	struct district_data {
		byte	enable,
			direction,
			adc_pin,
			adc_test,
			brake;
	};
	static const district_data _district_data[ districts ] PROGMEM;

	//
	//	Our state regarding power: on / off.
	//
	bool		_powered_on;

public:
	//
	//	Allow this to build itself empty first.
	//
	Districts( void ) {
		_powered_on = false;
	}

	//
	//	Initialise the districts and set them into action.
	//
	void initialise( void ) {
		for( byte i = 0; i < districts; i++ ) {
			district_data	*d;
			Pin_IO		brake;

			d = &( _district_data[ i ]);
			
			brake.configure( progmem_read_byte( d->brake ), false );
			brake.low();
			
			d->assign(	progmem_read_byte( d->enable ),
					progmem_read_byte( d->direction ),
					progmem_read_byte( d->adc_pin ),
					progmem_read_byte( d->adc_test ));
		}
	}

	//
	//	Set the power status of the districts.
	//
	void power( bool on ) {
		for( i = 0; i < districts; _district[ i++ ].power( on ));
	}
};

//
//	Declare the controlling object.
//
extern Districts districts;



#endif

//
//	EOF
//
