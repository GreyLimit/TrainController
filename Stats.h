//
//	Stats.h
//	=======
//
//	A system where stats are gathered at regular intervals
//	from various parts of the environment.
//


#ifndef _STATS_H_
#define _STATS_H_

#include "Environment.h"
#include "Parameters.h"
#include "Configuration.h"
#include "Average.h"
#include "Task_Entry.h"
#include "Signal.h"

//
//	Define the averaging range we are going to empty for the
//	stats.
//
#ifndef STATS_AVERAGE_READINGS
#define STATS_AVERAGE_READINGS	4
#endif

//
//	Define the number of Milliseconds overwhich the readings are
//	spanned.
//
#ifndef STATS_AVERAGE_PERIOD
#define STATS_AVERAGE_PERIOD	1000
#endif

//
//	Declare the stats object.
//
class Stats : public Task_Entry {
private:
	//
	//	The internal stats we are keeping.
	//
	Average< STATS_AVERAGE_READINGS >	_packets_sent;

	//
	//	The control signal used to schedule this object.
	//
	Signal		_flag;

public:
	//
	//	Call initialise to get the system going.
	//
	void initialise( void );

	//
	//	The routine called once a "period" to gather
	//	in more stats and .. process them.
	//
	virtual void process( void );

	//
	//	Return the packets set in the last time period.
	//
	word packets_sent( void );
};

//
//	The stats object.
//
extern Stats stats;

#endif

//
//	EOF
//
