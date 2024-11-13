//
//	TOD.h
//	=====
//
//	The definition of the "time of day" module.
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
//	Declare the TOD class.
//
class TOD : public Task {
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

public:
	//
	//	Constructor
	//
	TOD( void );

	//
	//	Return a pointer to the flag, required by the scheduler
	//	and event timer.
	//

	//
	//	Call to initialise the TOD system in the Clock and
	//	Task sub-system.
	//
	void start( void );

	//
	//	Provide access to the TOD data.
	//
	byte read( byte index );

	//
	//	The TASK entry point, called each time the flag is
	//	set true by the clock system.
	//
	virtual bool time_slice( void );

#endif

//
//	And here is the TOD object.
//
extern TOD time_of_day;

//
//	EOF
//
