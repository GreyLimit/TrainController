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
#include "Task_Entry.h"

//
//	This class defines and handles the communications protocol
//	between a computer and the Train Controller (via the USB link).
//
class Protocol : public Task_Entry {
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

	//
	//	Define the size of the input buffer area.
	//
	static const byte	buffer_size = 32;

private:
	//
	//	Define the input state and buffer variables.
	//
	bool		_inside,
			_valid;
	char		_buffer[ buffer_size ];
	byte		_len;
	
	//
	//	Define simple "in string" number parsing routine.
	//
	char *parse_number( char *buf, bool *found, int *value );

	//
	//	Parse an input buffer.
	//
	void parse_buffer( char *buf, int len );

public:
	//
	//	Constructor.
	//
	Protocol( void );
	
	//
	//	Set up this object and link into other systems.
	//
	void initialise( void );

	//
	//	The task entry point.
	//
	virtual void process( void );
};

//
//	Here is the protocol engine.
//
extern Protocol protocol;

#endif

//
//	EOF
//
