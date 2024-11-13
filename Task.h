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

//
//	My initial thoughts were that this code would need to be
//	"Critical" code (at least in some sections), but *if* the
//	code which creates tasks is *always* outside an interrupt
//	routine then this requirement is unnecessary.
//

//
//	Define a virtual class which forms the interface between
//	the implementation of a task and the task manager.
//
class Task {
public:
	//
	//	This routine is called every time a task is given CPU
	//	time, and should return TRUE if the task should be
	//	rescheduled or FALSE if it should be dropped.
	//
	virtual bool time_slice( void ) = 0;
};

//
//	Define the task manager class where the argument passed in is
//	the maximum number of tasks which it will handle.
//
class TaskManager {
private:
	//
	//	Define a task entry and the task table.
	//
	struct task {
		bool	*trigger;
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
	}

	//
	//	This is the task scheduler interface (called from the
	//	main loop continuously.
	//
	void run_tasks( void ) {
		//
		//	Scan for task to call based on the value of
		//	their trigger flag.
		//
		while(( t = _head ) != NIL( task )) {
			task	*t;

			//
			//	Found a task so unlink from the task list
			//	then test the trigger.
			//
			_head = t.next;
			if( *( t->trigger )) {
				//
				//	Task triggered, so call it.
				//
				if( t->call->time_slice()) {
					//
					//	Reschedule task to tail
					//	of the task list.
					//
					t->next = NIL( task );
					*_tail = t;
					_tail = &( t->next );
				}
				else {
					//
					//	Task is dead, forget it.
					//
					t->next = _free;
					_free = &t;
				}
			}
			else {
				//
				//	Task not triggered, put on the
				//	back of the task list for a future
				//	test.
				//
				t->next = NIL( task );
				*_tail = t;
				_tail = &( t->next );
			}
		}
		//
		//	The run_tasks() function only returns when all
		//	the tasks have "died".  Until that point the
		//	tasks are called repeatedly while any remain to
		//	test and execute.
		//
	}

	//
	//	This is the access point where tasks are added to the system.
	//
	bool add_task( Task *call, bool *trigger ) {
		task	*t;

		//
		//	Find an empty task record, fail if there are none
		//	left.
		//
		if(( t = _free ) == NIL( task )) return( false );
		
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
