//
//	Errors:		A consolidated error logging system
//			to enable errors to be noted at point
//			of detection, but processed at a future
//			point of convenience.
//

#ifndef _ERRORS_H_
#define _ERRORS_H_

#include "Environment.h"
#include "Parameters.h"
#include "Configuration.h"
#include "Task_Entry.h"
#include "Signal.h"
#include "Console.h"
#include "Protocol.h"

//
//	Define a list of Error numbers that this code could report
//
//	Note:	Probably ought to re-group these numbers into sections
//		so adding a new error number is less disruptive.
//
#define NO_ERROR			0

//
//	Error system errors!
//
#define ERROR_QUEUE_OVERFLOW		1
#define ERROR_REPORT_FAIL		2
#define ERROR_BUFFER_OVERFLOW		3
#define ERROR_OUTPUT_FORMAT		4

//
//	DCC command construction errors.
//
#define DCC_PROTOCOL_ERROR		10
#define DCC_COMMAND_OVERFLOW		11
#define DCC_COMMAND_TRUNCATED		12
#define DCC_COMMAND_EMPTY		13

//
//	DCC command parameter errors.
//
#define INVALID_DCC_COMMAND		20
#define INVALID_ARGUMENT_COUNT		21
#define INVALID_ADDRESS			22
#define INVALID_SPEED			23
#define INVALID_DIRECTION		24
#define INVALID_STATE			25
#define INVALID_CV_NUMBER		26
#define INVALID_FUNC_NUMBER		27
#define INVALID_BIT_NUMBER		28
#define INVALID_BIT_VALUE		29
#define INVALID_BIT_MASK		30
#define INVALID_BYTE_VALUE		31
#define INVALID_WORD_VALUE		32

//
//	Operational errors.
//
#define NO_PROGRAMMING_TRACK		40
#define POWER_NOT_OFF			41
#define POWER_OVERLOAD			42
#define POWER_SPIKE			43
#define PROGRAMMING_TRACK_ONLY		44

//
//	System processing errors.
//
#define TRANSMISSION_REPORT_FAIL	50
#define TRANSMISSION_TABLE_FULL		51
#define TRANSMISSION_PENDING_FULL	52
#define TRANSMISSION_RECORD_EMPTY	53
#define BIT_TRANS_OVERFLOW		54

//
//	Process reporting errors
//
#define COMMAND_REPORT_FAIL		60

//
//	Errors relating to the (now missing)
//	structured decoder programming logic.
//
//	This is to be reinstated at a suitable
//	time in the development cycle.
//
#define PARSE_CONF_CMD_ERROR		70
#define UNRECOGNISED_CONF_CMD		71
#define READ_ONLY_VARIABLE		72
#define INVALID_CV_NAME			73
#define INVALID_CV_ACCESS		74
#define CV_CHANGE_OVERFLOWED		75
#define CV_RANGE_ERROR			76
#define CV_INDEX_ERROR			77

//
//	HCI scheduling failed.
//
#define HCI_SCHEDULE_FAILED		89

//
//	Rotary button queue full
//
#define ROTARY_BUTTON_QUEUE_FULL	90

//
//	Time of Day queue full
//
#define TIME_OF_DAY_QUEUE_FULL		91

//
//	Event time capacity exceeded.
//
#define EVENT_TIMER_QUEUE_FULL		92

//
//	Task manager errors.
//
#define TASK_DEPTH_EXCEEDED		93

//
//	LCD COMMS Errors
//
#define LCD_QUEUE_FULL			94

//
//	I2C Errors reported
//
#define I2C_COMMS_ERROR			95

//
//	Resource errors.
//
#define ERRORS_ERR_OVERFLOW		96
#define USART_IO_ERR_DROPPED		97
//
//	Code Assurance errors
//
#define CODE_ASSURANCE_ERR_ASSERT	98
#define CODE_ASSURANCE_ERR_ABORT	99


//
//	Define the Error handling class
//
class Errors : public Task_Entry {
private:
	//
	//	How many errors will we try to cache?
	//
	static const byte cache_size = 4;

	//
	//	How do we keep the errors?
	//
	struct error_record {
		byte		error,		// What?
				repeats;	// How often?
		word		arg;		// Supporting data.
	};

	//
	//	Where do we keep them?
	//
	error_record	_cache[ cache_size ];
	byte		_count,
			_in,
			_out;
			
	//
	//	The task conrtol signal.
	//
	Signal		_errors;
	
	//
	//	Drop the next error.
	//
	void drop_error( void );


public:
	//
	//	Constructor
	//
	Errors( void );
	
	//
	//	The initialisation routine for the errors object
	//
	void initialise( void );
	
	//
	//	The task manager calls this routine to handle the
	//	output of errors.  This is controlled by the "_errors"
	//	Signal.
	//
	virtual void process( void );
	
	//
	//	Log an error with the system
	//
	void log_error( byte error, word arg );
	
	//
	//	Log a terminal system error with the system.
	//
	void log_terminate( word error, const char *file_name, word line_number );
};


//
//	Externally declare the errors object.
//
extern Errors errors;

#endif
//
//	EOF
//
