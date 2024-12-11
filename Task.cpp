//
//	Task.cpp
//	========
//
//	Declare the task manager
//

#include "Task.h"

//
//	Constructor and Destructor.
//
TaskManager::TaskManager( void ) {
	_head = NIL( task_record );
	_tail = &_head;
	_free = NIL( task_record );
	for( byte i = 0; i < table_size; i++ ) {
		_table[ i ].next = _free;
		_free = &( _table[ i ]);
	}
	_depth = 0;
}

//
//	This is a task polling routine and is called to see
//	if a single task can be executed before returning.
//
void TaskManager::pole_task( void ) {
	task_record	*t;

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
		if( t->trigger->acquire()) t->call->process();
		
		//
		//	Put on the back of the task list to await
		//	its turn again.
		//
		t->next = NIL( task_record );
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
void TaskManager::run_tasks( void ) {
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
	//	the tasks have gone - which should never happen!
	//
}

//
//	This is the access point where tasks are added to the system.
//
bool TaskManager::add_task( Task_Entry *call, Signal *trigger ) {
	task_record	*t;

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
	t->next = NIL( task_record );
	*_tail = t;
	_tail = &( t->next );
	return( true );
}


//
//	define the task_manager itself.
//
TaskManager task_manager;

//
//	EOF
//
