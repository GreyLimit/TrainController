//
//	Errors:		A consolidated error logging system
//			to enable errors to be noted at point
//			of detection, but processed at a future
//			point of convenience.
//


//
//	Bring in our environment
//
#include "Errors.h"
#include "Task.h"
#include "Clock.h"
#include "Code_Assurance.h"
#include "Buffer.h"
#include "DCC.h"
#include "Protocol.h"

//
//	Define the Error handling class
//
void Errors::drop_error( void ) {
	
	ASSERT( _count > 0 );

	//
	//	Reduce the count.
	//
	_count--;
	
	//
	//	Move the output index on.
	//
	if( ++_out >= cache_size ) _out = 0;
	
	//
	//	Signal the reduction in available errors.
	// 
	_errors.claim();
}

//
//	Constructor
//
Errors::Errors( void ) {
	for( byte i = 0; i < cache_size; i++ ) {
		_cache[ i ].error = 0;
		_cache[ i ].arg = 0;
		_cache[ i ].repeats = 0;
	}
	_count = 0;
	_in = 0;
	_out = 0;
}

//
//	The initialisation routine for the errors object
//
void Errors::initialise( void ) {
	task_manager.add_task( this, &_errors );
}

//
//	The task manager calls this routine to handle the
//	output of errors.  This is controlled by the "_errors"
//	Signal.
//
void Errors::process( void ) {
	error_record			*ptr;
	Buffer<DCC::maximum_output>	reply;

	//
	//	Sanity Check.
	//
	ASSERT( _count > 0 );

	//
	//	Set a point to the error record we are handling
	//
	ptr = &( _cache[ _out ]);
	
	//
	//	Fill in the error record.
	//
	if( !reply.format( Protocol::error, ptr->error, ptr->arg, ptr->repeats )) {
		//
		//	If this happens then there is nothing we can really do.
		//	throw away the error, but log another error formatting
		//	error!
		//
		drop_error();
		log_error( ERROR_OUTPUT_FORMAT, ptr->error );
		return;
	}
	
	//
	//	Send the error to the console and delete the
	//	error on success.
	//
	if( reply.send( &console )) drop_error();
	
	//
	//	Done.
	//
}

//
//	Log an error with the system
//
void Errors::log_error( byte error, word arg ) {
	byte	i, j;

	//
	//	Has this error been reported before?
	//
	i = _out;
	for( j = 0; j < _count; j++ ) {
		if(( _cache[ i ].error == error )&&( _cache[ i ].arg == arg )) {
			//
			//	Found the same error logged already, just add to the count.
			//
			if( _cache[ i ].repeats < ERROR_BYTE ) _cache[ i ].repeats++;
			//
			//	Done as there is nothing else to do.
			//
			return;
		}
		//
		//	Try next error...
		//
		if(( i += 1 ) >= cache_size ) i = 0;
	}
	//
	//	No, this is a new error, can we create it?
	//
	if( _count >= cache_size ) {
		//
		//	Here we have a "Cache full" situation
		//	where errors are being logged faster than
		//	they can be reported.
		//
		//	Start by looking for a previous "cache full"
		//	error to extend.
		//
		for( i = 0; i < cache_size; i++ ) {
			if( _cache[ i ].error == ERRORS_ERR_OVERFLOW ) {
				//
				//	We increment the argument (count of
				//	overflows) but only if it does not
				//	wrap round to zero.
				//
				if( _cache[ i ].arg < ERROR_WORD ) _cache[ i ].arg++;
				//
				//	Good, the lost errors count has been
				//	incremented.
				//
				return;
			}
		}
		//
		//	Nothing found so we ditch the most recent error
		//	(on the grounds that older errors are more
		//	pertinent to working out what's gone wrong).
		//
		//	Identify the index of the previous error
		//	regardless of what it was.
		//
		if(( i = _in )) {	// Assign intentional
			i--;
		}
		else {
			i = cache_size-1;
		}
		//
		//	What ever it was is gone.
		//
		_cache[ i ].error = ERRORS_ERR_OVERFLOW;
		_cache[ i ].repeats = 1;
		_cache[ i ].arg = 1;
		//
		//	Done.
		//
		return;
	}
	//
	//	Create new record.
	//
	_cache[ _in ].error = error;
	_cache[ _in ].arg = arg;
	_cache[ _in ].repeats = 0;
	if(( _in += 1 ) >= cache_size ) _in = 0;
	_count++;

	//
	//	Signal that a new error has been inserted into
	//	the cache.
	//
	_errors.release();
	
	//
	//	Done.
	//
}

//
//	Log a terminal system error with the system.
//
void Errors::log_terminate( word error, const char *file_name, word line_number ) {
	//
	//	This is a terminal error where we are supplied the error, filename and
	//	line number.  This routine is not expected to return, ever, and must be
	//	cognisant that is *may* be called from within a Critical section of code.
	//
	//	For simplicity's sake, just re-enable interrupts as we will need this to
	//	get data out of the micro controller.
	//
	Critical::enable_interrupts();
	
	//
	//	Set console into synchronous (no data
	//	loss) mode - only effective on the print
	//	methods.
	//
	console.synchronous( true );
	
	//
	//	Now we start an infinite loop..
	//
	while( true ) {
		byte	i, j;
		
		//
		//	Output our point of failure.
		//
		console.print( (int)error );
		console.print( TAB );
		console.print( file_name );
		console.print( TAB );
		console.println( line_number );
		//
		//	Now output the error cache.
		//
		i = _out;
		for( j = 0; j < _count; j++ ) {
			console.print( (int)( _cache[ i ].error ));
			console.print( TAB );
			console.print( (int)( _cache[ i ].arg ));
			console.print( TAB );
			console.println( (int)( _cache[ i ].repeats ));
			if(( i += 1 ) >= cache_size ) i = 0;
		}
		//
		//	Try to delay for a fixed period.
		//
		event_timer.inline_delay( MSECS( 1000 ));
	}
	//
	//	We should never get here as the above code should
	//	run indefinitely.
	//
}



//
//	Declare the errors object.
//
Errors errors;


//
//	EOF
//
