//
//	Average.h
//	=========
//
//	Declare a class used to roll up a series of values into a
//	running average.  For the data gathered different values
//	for the average can be obtained spanning different periods
//	of time.
//


#ifndef _AVERAGE_H_
#define _AVERAGE_H_
#endif

//
//	Grab what we need.
//
#include "Configuration.h"
#include "Environment.h"


//
//	The Average mechanism.
//
template< byte span >
class Average {
public:
	//
	//	What is our defined average span.
	//
	static const byte	average_span = span;
	
private:
	//
	//	The working area.
	//
	word	_value[ average_span ];

public:
	//
	//	Provide simple ability to reset the average.
	//
	void reset( void ) {
		for( byte i = 0; i < average_span; _value[ i++ ] = 0 );
	}

	//
	//	Initialise the content.
	//
	Average( void ) {
		reset();
	}

	//
	//	Fold in a new value, returns the "oldest" averaged
	//	value.
	//
	word add( word value ) {
		for( byte i = 0; i < average_span; i++ ) value = ( _value[ i ] = ( value + _value[ i ]) >> 1 );
		return( value );
	}

	//
	//	read the average from a point in the data set.
	//
	word read( byte index ) {
		if( index < average_span ) return( _value[ index ]);
		return( _value[ average_span-1 ]);
	}
};


#endif

//
//	EOF
//
