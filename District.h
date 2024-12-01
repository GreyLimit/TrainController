//
//	District.h
//	==========
//
//	Declare the class which manages to actions of a single
//	district in coordination with other districts.
//

#ifndef _DISTRICT_H_
#define _DISTRICT_H_

//
//	We will need the follow systems.
//
#include "Configuration.h"
#include "Environment.h"
#include "Code_Assurance.h"
#include "ADC_Manager.h"
#include "TOD.h"
#include "Average.h"

//
//	Declare the class for handling a single district.
//
class District : public Task {
public:
	//
	//	Define the parameters to the district management code:
	//
	//
	//	We are going to use an array of values compounded upon
	//	each other to generate a series of average values which
	//	represent averages over longer and longer periods (each
	//	element of the array twice as long as the previous).
	//
	//	Define the size of the array.
	//
	static const byte	compounded_values = 10;

	//
	//	Define where the "average current load" reading is taken from.
	//
	static const byte	average_current_index = compounded_values - 1;

	//
	//	Define the size of the "spike average" we will use to
	//	identify a critical rise in power consumption signifying
	//	a short circuit.  This is an index into the
	//	compounded average table.
	//
	static const byte	spike_average_value = 1

	//
	//	Define the size of the "short average" we will use to
	//	identify a genuine rise in power consumption signifying
	//	a confirmation return.  This is an index into the
	//	compounded average table.
	//
	static const byte	short_average_value = 2


private:
	//
	//	Declare the set of states in which a district can be
	//	sitting in.
	//
	enum state {			// District...
		state_unassigned,	// ...	is not attached to any hardware.
		state_off,		// ...	is explicitly powered down.
		state_on,		// ...	is running normally.
		state_shorted,		// ...	is shorted and waiting for
					//	slot to try phase inversion.
		state_inverted,		// ...	was shorted and is waiting
					//	for confirmation that the
					//	inversion worked
		state_paused,		// ...	is down as result of short
					//	or overload.
	};
	
	//
	//	Store the state of this district.
	//
	state				_state;

	//
	//	Record the link into the driver so we can directly
	//	control our output pins.
	//
	byte				_driver;

	//
	//	Keep a copy of the ADC pin we need to check for the
	//	power monitoring facility.
	//
	Pin_IO				_adc;
	byte				_test;

	//
	//	The control area from the ADC manager.
	//
	word				_reading;
	bool				_flag;

	//
	//	Where we gather our readings over time.
	//
	Average<compounded_values>	_average;

	//
	//	Declare a single Gateway, common to all defined
	//	Districts, that controls access to code which
	//	only a single district should execute at any
	//	single time.
	//
	static Gate			_exclusive;

public:
	//
	//	Initialise the district as unassigned.
	//
	District( void ) {
		_state = state_unassigned;
	}

	//
	//	Assign enable and direction pins to this district
	//	(which will then automatically configure the
	//	driver object and initiate use of ADC manager)
	//
	bool assign( byte enable, byte direction, byte adc_pin, byte adc_number ) {
		//
		//	Set the test pin for input and reset flag.
		//
		_adc.configure( adc_pin, true );
		_test = adc_number;
		_flag = false;
		//
		//	Add our pins to the driver object.
		//
		if( !dcc_driver.add( &_driver, enable, direction )) {
			ABORT();
		}
		//
		//	Add the district to the task manager and
		//	then initiate the first ADC reading.
		//
		if( !task_manager.add_task( this, &_flag )) ABORT();
		if( !adc_manager.read( _test, &_flag, &_reading )) ABORT();
	}

	//
	//	Task Entry point from the task manager.
	//
	virtual bool process( void ) {
		//
		//	We get here because a reading is available, so
		//	add this to the average.  There are some cases
		//	where the "reading" has not been made (during
		//	a pause), but the reading variable will have been
		//	reset to zero for this period.
		//
		_average.add( _reading );
		//
		//	What we do really depends on our state.
		//
		switch( _state ) {
			case state_off: {
				//
				//	Nothing to do here.  The state
				//	will be changed by an external
				//	source.
				//
				break;
			}
			case state_on: {
				//
				//	This is the "normal" state of a
				//	district.  We should be testing to
				//	see how the load is looking.
				//
				if( _reading > INSTANT_CURRENT_LIMIT ) {
					//
					//	This is declared an immediate short, we
					//	need to head into the recovery process:
					//	We try to invert the district before we
					//	head into a power off.  However, the
					//	"risk" is that multiple districts see the
					//	same short, we have to ensure only one
					//	district tries the reverse trick.
					//
					if( _exclusive.acquired()) {
						//
						//	we have exclusive access to this code
						//	so jolly well get on with it.
						//
						dcc_driver.toggle( _driver );
						_state = state_inverted;
					}
					else {
						//
						//	Another driver has already got hold of the
						//	exclusive access, we transition to shorted
						//	state to monitor its success.
						//
						_state = state_shorted;
					}
				}
				else if( _average.read( average_current_index ) > AVERAGE_CURRENT_LIMIT ) {
					//
					//	This is an on going overload situation.  This
					//	is not the result of a short (we believe), so
					//	we go straight for the power off option.
					//
					dcc_driver.off( _driver );
					_reading = 0;
					_state = state_paused;
				}
				//
				//	Normal processing completed, state changed if necessary.
				//
				break;
			}
			case state_shorted: {
				//
				//	We are here because there was a short but we could not
				//	try flipping this district.  The short may have been resolved
				//	so test the reading for that first.
				//
				//	If still shorted try the exclusive flag one more time before
				//	heading into a full pause mode.
				//
				if( _reading > INSTANT_CURRENT_LIMIT ) {
					if( _exclusive.acquired())) {
						//
						//	we have exclusive access to this code
						//	so jolly well get on with it.
						//
						dcc_driver.toggle( _driver );
						_state = state_inverted;
					}
					else {
						//
						//	There's nothing we can do here - the
						//	district needs to be turned off.
						//
						dcc_driver.off( _driver );
						_reading = 0;
						_state = state_paused;
					}
				}
				else {
					//
					//	We're all good now - back to on state.
					//
					_state = state_on;
				}
				//
				//	Short handling completed, state determines next action.
				//
				break;
			}
			case state_inverted: {
				//
				//	We are here because we claimed the exclusive lock and
				//	flipped the phase on this district. 
				//
				if( _reading > INSTANT_CURRENT_LIMIT ) {
					//
					//	There's nothing we can do here - the
					//	district needs to be turned off.
					//
					dcc_driver.off( _driver );
					_reading = 0;
					_state = state_paused;
				}
				else {
					//
					//	We're all good now - back to the normal state.
					//
					_state = state_on;
				}
				//
				//	Final action is to release the exclusive lock
				//	before allowing the state variable to control
				//	next actions.
				//
				_exclusive.release();
				break;
			}
			case state_paused: {
				//
				//	We have come in here after being paused for
				//	a period of time.  Time to restart the district
				//	and try for normal operation.
				//
				dcc_driver.on( _driver );
				_average.reset();
				_state = state_on;
				break;
			}
			default: {
				//
				//	Nothing we can do, this is a programmer error.
				//
				ABORT();
				break;
			}
		}
		//
		//	Pausing or reading the ADC - depends on the
		//	new value state we are in.
		//
		if( _state ==  state_paused ) {
			time_of_day.add( DRIVER_RESET_PERIOD, &_flag );
		}
		else {
			adc_manager.read( _test, &_flag, &_reading );
		}
		//
		//	Done, tell the task manager that we need to keep
		//	going.
		//
		return( true );
	}

	//
	//	Control the power on this district.
	//
	void power( bool on ) {
		dcc_driver.power( _driver, on );
		_state = on? state_on: state_off;
	}
};


#endif


//
//	EOF
//
