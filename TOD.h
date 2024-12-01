//
//	TOD.h
//	=====
//
//	The definition of the "time of day" module.
//
//	This has two roles:
//
//	To track the time of day at a "per second" granularity to
//	allow the passing of time to be assessed at human scales.
//
//	To provide a flag management system (used as input the
//	the task management code) where flags can be set true at a
//	human period of time in the future (65535 seconds ahead,
//	approximately 18 hours).
//

#ifndef _TOD_H_
#define _TOD_H_

//
//	Bring in the necessary definitions.
//
#include "Configuration.h"
#include "Environment.h"
#include "Task.h"
#include "Clock.h"

//
//	Define how many TOD Tasks we will manage.
//
#ifndef TIME_OF_DAY_TASKS
#define TIME_OF_DAY_TASKS	8
#endif

//
//	Declare the TOD class.
//
class TOD : public Task {
	//
	//	The TIME OF DAY element of the class
	//	------------------------------------
	//
public:
	//
	//	Define how many "steps" the counter contains.  Each step
	//	represents a progressively larger unit of time, starting
	//	with seconds.
	//
	//	Defined, so far:
	//		0		seconds,
	//		1		minutes,
	//		2		hours,
	//		3		days.
	//
	static const byte stages = 4;
	
private:
	//
	//
	//	Here we have the counter limits.
	//
	static const byte limit[ stages ] PROGMEM;

	//
	//	Here are the counters.
	//
	byte _stage[ stages ];

	//
	//	Here is the flag required as part of the scheduling
	//	process.
	//
	bool	_flag;

	//
	//	The TOD Flag Manager components.
	//
	struct pending {
		word	left;
		bool	*flag;
		task	*next;
	};
	
	//
	//	flag records and pointers.
	//
	pending	_pending[ TIME_OF_DAY_TASKS ],
		*_active,
		*_free;

public:
	//
	//	Constructor
	//
	TOD( void ) {
		//
		//	The TOD values.
		//
		for( byte i = 0; i < stages; _stage[ i++ ] = 0 );
		_flag = false;
		//
		//	The pending flags data.
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
	void start( void ) {
		event_timer.delay_event( MSECS( 1000 ), &_flag, true );
		task_manager.add_task( this, &_flag );
	}

	//
	//	Provide access to the TOD data.
	//
	byte read( byte index ) {
		if( index < stages ) return( _stage[ index ]);
		return( 0 );
	}
	bool write( byte index, byte value ) {
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
	bool add( word duration, bool *flag ) {
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
	virtual bool process( void ) {
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
			if(!( ptr->left-- )) {
				*( ptr->flag ) = true;
				_active = ptr->next;
				ptr->next = _free;
				_free = ptr;
			}
		}
		
		//
		//	Tell scheduler we wish to continue being called.
		//
		return( true );
	}
};

#endif

//
//	And here is the TOD object.
//
extern TOD time_of_day;

//
//	EOF
//
