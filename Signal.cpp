//
//	Signal.cpp
//	==========
//
//	The implementation of the Signal module.
//


#include "Signal.h"
#include "Task.h"
#include "Critical.h"
#include "Code_Assurance.h"

//
//	Constructors - default initialises the signal to
//	zero, forcing the system to wait until a "signal"
//	is raised.  The following permits the counter to
//	be reloaded with a know value and allows the signal
//	to be used as a resource meter.
//
Signal::Signal( void ) {
	_count = 0;
}
Signal::Signal( byte load ) {
	_count = load;
}

//
//	This routine "returns" a resource to the signal:
//
//		The internal count is increased by one.
//
void Signal::release( void ) {
	//
	//	Are we inside a Critical code section/ISR?
	//
	if( Critical::critical_code()) {
		//
		//	Simply add 1 to the counter.
		//
		_count++;

		//
		//	Final check to see if we have "wrapped".
		//	Saying this "should not" happen does not
		//	mean it will never happen.
		//
		ASSERT( _count > minimum_count_value );
	}
	else {
		//
		//	If not then we can apply a more flexible approach
		//	handling the signal.
		//
		while( true ) {
			//
			//	Perform a small section of Critical code.
			//
			{
				Critical code;

				if( _count < maximum_count_value ) {
					_count++;
					break;
				}
			}
			//
			//	If we get here then we need to give other
			//	pieces of code opportunity to execute.
			//
			task_manager.pole_task();
			
			//
			//	Rise and repeat ...
			//
		}
	}
}

//
//	This routine "claims" a resource from the signal:
//
//		The internal count is decreased by one.
//
void Signal::claim( void ) {
	//
	//	Are we inside a Critical code section/ISR?
	//
	if( Critical::critical_code()) {

		//
		//	Initially check to see if we will "unwrap".
		//	Saying this "should not" happen does not
		//	mean it will never happen.
		//
		ASSERT( _count > minimum_count_value );
		
		//
		//	Simply subtract 1 from the counter.
		//
		_count--;
	}
	else {
		//
		//	If not then we can apply a more flexible approach
		//	handling the signal.
		//
		while( true ) {
			//
			//	Perform a small section of Critical code.
			//
			{
				Critical code;

				if( _count > minimum_count_value ) {
					_count--;
					break;
				}
			}
			//
			//	If we get here then we need to give other
			//	pieces of code opportunity to execute.
			//
			task_manager.pole_task();
			
			//
			//	Rise and repeat ...
			//
		}
	}
}

//
//	This routine is used to test and claim a resource in
//	a single activity.  If the function returns true then
//	a resource *has already been* claimed as if claim()
//	had been successfully called.
//
bool Signal::acquire( void ) {
	bool	result;

	ASSERT( Critical::normal_code());

	{
		Critical code;

		if(( result = ( _count > minimum_count_value ))) _count--;
	}

	return( result );
}




//
//	EOF
//
