//
//	Signal.h
//	========
//
//
//	Declare the "signal" class which is used to send notifications
//	between feeder objects and the consumer objects.
//
//	The "control" of processing power between the feed/consumer
//	code is via the Task Manager.
//
//	The name "Signal" is a nod towards the P/V signal system created
//	by the Edsger W. Dijkstra, after which this system is formed. 
//

#ifndef _SIGNAL_H_
#define _SIGNAL_H_

//
//	We *must* exclusive control over the CPU inside this class
//	so we include the Critical code module.
//
#include "Environment.h"
#include "Parameters.h"
#include "Configuration.h"

//
//	Declare the signal type forming the basis for the system.
//
class Signal {
private:
	//
	//	Define the value limits for the counter.
	//
	static const byte minimum_count_value = 0;
	static const byte maximum_count_value = MAXIMUM_BYTE;
	
	//
	//	This is the Counter that forms the controlled data element.
	//
	byte	_count;

public:
	//
	//	Constructors - default initialises the signal to
	//	zero, forcing the system to wait until a "signal"
	//	is raised.  The following permits the counter to
	//	be reloaded with a know value and allows the signal
	//	to be used as a resource meter.
	//
	Signal( void );
	Signal( byte load );

	//
	//	This routine "returns" a resource to the signal:
	//
	//		The internal count is increased by one.
	//
	void release( void );

	//
	//	This routine "claims" a resource from the signal:
	//
	//		The internal count is decreased by one.
	//
	void claim( void );

	//
	//	This routine is used to test and claim a resource in
	//	a single activity.  If the function returns true then
	//	a resource *has already been* claimed as if claim()
	//	had been successfully called.
	//
	bool acquire( void );
};

#endif

//
//	EOF
//
