//
//	TOD.cpp
//	=======
//
//	The implementation of the time of day tracker.
//

#include "TOD.h"

//
//
const byte TOD::limit[ TOD::stages ] PROGMEM = {
	60,	// Seconds
	60,	// Minutes
	24,	// Hours
	100	// Days (arbitrary).
};


//
//	Constructor
//
TOD::TOD( void ) {
	for( byte i = 0; i < stages; _stage[ i++ ] = 0 );
	_flag = false;
}

//
//	Call to initialise our entry in the Clock system.
//
void start( void ) {
	event_timer.delay_event( MSECS( 1000 ), &_flag, true );
	task_manager.add_task( this, &_flag );
}

//
//	Provide access to the TOD data.
//
byte TOD::read( byte index ) {
	if( index < stages ) return( _stage[ index ]);
	return( 0 );
}

//
//	The TASK entry point, called each time the flag is
//	set true by the clock system.
//
bool TOD::time_slice( void ) {
	//
	//	Called every second.
	//

	for( byte i = 0; i < stages; i++ ) {
		if( ++_stage[ i ] < progmem_read_byte( limit[ i ])) break;
		_stage[ i ] = 0;
	}
	//
	//	Reset the flag.
	//
	_flag = false;
	
	//
	//	Tell scheduler we wish to continue being called.
	//
	return( true );
}


//
//	And here is the TOD object.
//
TOD time_of_day;


//
//	EOF
//
