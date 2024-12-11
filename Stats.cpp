//
//	Stats.cpp
//	=========
//
//	A system where stats are gathered at regular intervals
//	from various parts of the environment.
//


#include "Stats.h"
#include "Task.h"
#include "Clock.h"

#include "DCC.h"

//
//	Call initialise to get the system going.
//
void Stats::initialise( void ) {
	task_manager.add_task( this, &_flag );
	event_timer.delay_event( STATS_AVERAGE_PERIOD, &_flag, true );
}

//
//	The routine called once a "period" to gather
//	in more stats and .. process them.
//
void Stats::process( void ) {
	//
	//	For the moment only the DCC stats are gather this way.
	//
	_packets_sent.add( dcc_generator.packets_sent());
}


//
//	Return the packets set in the last time period.
//
word Stats::packets_sent( void ) {
	return( _packets_sent.read( STATS_AVERAGE_READINGS-1 ));
}

//
//	The stats object.
//
Stats stats;

//
//	EOF
//
