//
//	TOD.cpp
//	=======
//
//	The implementation of the time of day tracker.
//

#include "TOD.h"

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
//	Declare the TOD object.
//
TOD time_of_day;


//
//	EOF
//
