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
//	Define a virtual class which forms the interface between
//	the implementation of a task and the task manager.
//
class Task {
public:
	//
	//	This routine is called every time the associated signal
	//	shows that a resource is available.
	//
	virtual void process( void ) = 0;
};

//
//	Define the task manager class where the argument passed in is
//	the maximum number of tasks which it will handle.
//
class TaskManager {
private:
	//
	//	Declare the maxium nesting depth.
	//
	static const byte	maximum_depth = TASK_MAXIMUM_DEPTH;
	
	//
	//	Define a task entry and the task table.
	//
	struct task {
		Signal	*trigger;
		Task	*call;
		task	*next;
	};
	//
	//	Define our table space and the pointers we will use
	//	to control and manage the task list.
	//
	task	_table[ TASK_TABLE_SIZE ],
		*_head,
		**_tail,
		*_free;

	//
	//	Define a "depth indicator"; a counter which tracks
	//	how many times the object has been called in a "nested"
	//	fashion.  The primary purpose is to avoid cyclic/recursive
	//	calls to this module blowing up the stack space and
	//	causing the firmware to fail.
	//
	byte	_depth;

public:
	//
	//	Constructor and Destructor.
	//
	TaskManager( void ) {
		_head = NIL( task );
		_tail = &_head;
		_free = NIL( task );
		for( byte i = 0; i < TASK_TABLE_SIZE; i++ ) {
			_table[ i ].next = _free;
			_free = &( _table[ i ]);
		}
		_depth = 0;
	}

	//
	//	This is a task polling routine and is called to see
	//	if a single task can be executed before returning.
	//
	void pole_task( void ) {
		task	*t;

		//
		//	Perform our "anti-recursion" depth trap.
		//
		if( _depth >= maximum_depth ) {
			errors.log_error( TASK_DEPTH_EXCEEDED, _depth );
			return;
		}

		//
		//	We can only do anything if there is something to do.
		//
		//	It *is* possible for the queue to be empty if there
		//	are a very small number of tasks that (unwittingly)
		//	directly call this routine.
		//
		if(( t = _head )) {
			//
			//	Note our new nesting depth.
			//
			_depth++;
			
			//
			//	Found a task so unlink from the task list
			//	then test the trigger.
			//
			if(!( _head = t->next )) _tail = &_head;
			
			//
			//	Test the signal - call if resource available.
			//	The called process is responsible for claiming
			//	the resource (or resources as appropriate).
			//
			if( t->available()) call->process();
			
			//
			//	Put on the back of the task list to await
			//	its turn again.
			//
			t->next = NIL( task );
			*_tail = t;
			_tail = &( t->next );

			//
			//	Restore nesting depth to previous value.
			//
			_depth--;
		}
	}

	//
	//	This is the task scheduler interface (called from the
	//	main loop continuously.
	//
	void run_tasks( void ) {
		//
		//	We do not allow this routine to run if the depth
		//	is anything other than zero.  This would be a
		//	coding mistake.
		//
		if( _depth ) return;
		
		//
		//	Scan for task to call based on the value of
		//	their trigger flag.
		//
		while( _head ) pole_task();
		//
		//	The run_tasks() function only returns when all
		//	the tasks have "died".  Until that point the
		//	tasks are checked repeatedly while any remain to
		//	test and execute.
		//
	}

	//
	//	This is the access point where tasks are added to the system.
	//
	bool add_task( Task *call, Signal *trigger ) {
		task	*t;

		//
		//	Find an empty task record, fail if there are none
		//	left.
		//
		if(!( t = _free )) return( false );
		
		//
		//	Fill in record and append to the task list.
		//
		_free = t->next;
		t->trigger = trigger;
		t->call = call;
		t->next = NIL( task );
		*_tail = t;
		_tail = &( t->next );
		return( true );
	}

#endif

//
//	define the task_manager itself.
//
extern TaskManager task_manager;

//
//	EOF
//
