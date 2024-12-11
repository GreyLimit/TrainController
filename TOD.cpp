//
//	TOD.cpp
//	=======
//
//	The implementation of the time of day tracker.
//

#include "TOD.h"
#include "Task.h"
#include "Clock.h"

//
//	Declare our limits array.
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
	//
	//	The TOD values.
	//
	for( byte i = 0; i < stages; _stage[ i++ ] = 0 );
	//
	//	The pending signals data.
	//
	_active = NIL( pending );
	_free = NIL( pending );
	for( byte i = 0; i < TIME_OF_DAY_TASKS; i++ ) {
		_pending[ i ].next = _free;
		_free = &( _pending[ i ]);
	}
}

//
//	Call to initialise the TOD system within the Clock and
//	Task sub-system.
//
void TOD::start( void ) {
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
bool TOD::write( byte index, byte value ) {
	if(( index < stages )&&( value < progmem_read_byte( limit[ index ]))) {
		_stage[ index ] = value;
		return( true );
	}
	return( false );
}

//
//	Add a flag to the list of pending flag updates.  The
//	duration is specified in whole seconds upto 65535
//	seconds into the future.  There is no "time of day"
//	based clock scheduling.
//
bool TOD::add( word duration, Signal *flag ) {
	pending	*ptr, **adrs, *look;

	if(( ptr = _free )) {
		_free = ptr->next;
		ptr->left = duration;
		ptr->flag = flag;
		adrs = &_active;
		while(( look = *adrs )) {
			if( ptr->left < look->left ) {
				look->left -= ptr->left;
				break;
			}
			ptr->left -= look->left;
			adrs = &( look->next );
		}
		ptr->next = look;
		*adrs = ptr;
		return( true );
	}
	return( false );
}

//
//	The TASK entry point, called each time the flag is
//	set true by the clock system.
//
void TOD::process( void ) {
	pending	*ptr;
	
	//
	//	Called every second to update the TOD and look
	//	at the pending list.
	//
	//	Update the time of day.
	//
	for( byte i = 0; i < stages; i++ ) {
		if( ++_stage[ i ] < progmem_read_byte( limit[ i ])) break;
		_stage[ i ] = 0;
	}

	//
	//	Check out the pending actions.
	//
	while(( ptr = _active )) {
		//
		//	Has this counter run out?  If not (ie still not zero)
		//	then exit this loop.
		//
		if( ptr->left-- ) break;
		//
		//	Counter was zero, so release the flag and return the
		//	record to the free list.
		//
		ptr->flag->release();
		_active = ptr->next;
		ptr->next = _free;
		_free = ptr;
	}
}

//
//	A human scale inline delay routine.
//
void TOD::inline_delay( word seconds ) {
	Signal	flag;

	if( add( seconds, &flag )) {
		while( !flag.acquire()) task_manager.pole_task();
	}
	else {
		errors.log_error( TIME_OF_DAY_QUEUE_FULL, 0 );
	}
}


//
//	Declare the TOD object.
//
TOD time_of_day;


//
//	EOF
//
