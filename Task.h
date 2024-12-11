//
//	Task.h
//	======
//
//	Define a simple mechanism which can be used to juggle a set
//	of activities, calling each when a condition flag is set.
//

#ifndef _TASK_H_
#define _TASK_H_

//
//	We will need the Environment.
//
#include "Environment.h"
#include "Parameters.h"
#include "Configuration.h"
#include "Signal.h"
#include "Errors.h"
#include "Task_Entry.h"

//
//	My initial thoughts were that this code would need to be
//	"Critical" code (at least in some sections), but *if* the
//	code which creates tasks is *always* outside an interrupt
//	routine then this requirement is unnecessary.
//

//
//	Declare a value for the "maximum number of nested calls" to
//	this object.
//
#ifndef TASK_MAXIMUM_DEPTH
#define TASK_MAXIMUM_DEPTH	3
#endif

//
//	Define TASK_TABLE_SIZE if not already defined.  The default
//	values are hardly "special" and can be really wrong.
//
#ifndef TASK_TABLE_SIZE
#define TASK_TABLE_SIZE		SELECT_SML(4,8,16)
#endif

//
//	Define the task manager class where the argument passed in is
//	the maximum number of tasks which it will handle.
//
class TaskManager {
private:
	//
	//	Declare the maximum nesting depth and table_size.
	//
	static const byte	maximum_depth = TASK_MAXIMUM_DEPTH;
	static const byte	table_size = TASK_TABLE_SIZE;
	
	//
	//	Define a task entry and the task table.
	//
	struct task_record {
		Signal		*trigger;
		Task_Entry	*call;
		task_record	*next;
	};
	//
	//	Define our table space and the pointers we will use
	//	to control and manage the task list.
	//
	task_record	_table[ table_size ],
			*_head,
			**_tail,
			*_free;

	//
	//	Define a "depth indicator"; a counter which tracks
	//	how many times the object has been called in a "nested"
	//	fashion.  The primary purpose is to avoid cyclic/recursive
	//	calls to this module blowing up the stack space and
	//	causing the firmware to fail in a unpredictable way.
	//
	byte		_depth;

public:
	//
	//	Constructor and Destructor.
	//
	TaskManager( void );

	//
	//	This is a task polling routine and is called to see
	//	if a single task can be executed before returning.
	//
	void pole_task( void );

	//
	//	This is the task scheduler interface (called from the
	//	main loop continuously.
	//
	void run_tasks( void );

	//
	//	This is the access point where tasks are added to the system.
	//
	bool add_task( Task_Entry *call, Signal *trigger );
};

//
//	define the task_manager itself.
//
extern TaskManager task_manager;

#endif

//
//	EOF
//
