//
//	Protocol.h
//	==========
//
//	Declare a set of symbols which capture the fixed elements
//	of the communications protocol between a controlling
//	computer and the Train Controller.
//

#ifndef _PROTOCOL_H_
#define _PROTOCOL_H_

#include "Environment.h"
#include "Parameters.h"
#include "Configuration.h"

//
//	This is not a functional class, merely an informational one.
//
class Protocol {
public:
	//
	//	The lead-in and lead-out characters.
	//
	static const char	lead_in = '[';
	static const char	lead_out = ']';
	//
	//	DCC command letters.
	//
	static const char	mobile = 'M';		// Mobile speed and direction.
	static const char	accessory = 'A';	// Accessory control.
	static const char	function = 'F';		// Mobile function control.
	static const char	rewrite_state = 'W';	// Mobile decoder state re-write.
	//
	//	Controller reporting.
	//
	static const char	error = 'E';		// Returned error report.
	//
	//	Controller configuration.
	//
	static const char	power = 'P';		// Track power control.
	static const char	eeprom ='Q';		// EEPROM accessing command.
};


#endif

//
//	EOF
//
