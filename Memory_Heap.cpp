//
//	Memory_Heap.cpp		The declaration of  a generic
//	===============		memory heap allocation (and
//				deallocation) system designed
//	to be a used in collaboration with C++ new() and delete().
//


#include "Memory_Heap.h"
#include "Errors.h"

#include "Trace.h"
#ifdef DEBUGGING_ENABLED
#include "Console.h"
#endif

//
//	Allocation verification routine.
//
//	Returns true if the index is referencing a valid block,
//	allocated or de-allocated, or false if there is an issue
//	with the index.
//
bool Memory_Heap::valid( storage_unit index ) {

	STACK_TRACE( "bool Memory_Heap::valid( storage_unit index )" );

	TRACE_HEAP( console.print( F( "MHP valid " )));
	TRACE_HEAP( console.print( (word)index ));

	for( storage_unit look = 0; _heap[ look ]; look += block_size( _heap[ look ])) {
		if( look == index ) {
			
			TRACE_HEAP( console.println( F( " Yes" )));
			
			return( true );
		}
	}
	
	TRACE_HEAP( console.println( F( " No" )));
	
	return( false );
}
//
//	Space location routine
//
//	Find the best fit de-allocated block and return its index
//	or return the index of the first "never allocated"
//	piece of heap memory.
//
//	Yes - this is very slow - we are not meant to be doing
//	this that often.
//
Memory_Heap::storage_unit Memory_Heap::find( storage_unit rqd ) {

	STACK_TRACE( "storage_unit Memory_Heap::find( storage_unit rqd )" );

	TRACE_HEAP( console.print( F( "MHP find " )));
	TRACE_HEAP( console.println( (word)rqd ));

	storage_unit	look,
			sz,
			nxt,
			bst,
			fnd;

	look = 0;
	bst = invalid;
	fnd = 0;
	while(( sz = _heap[ look ])) {
		if( is_flag_set( sz )) {
			//
			//	Found a block with the flag set
			//	(de-allocated), so remove the
			//	flag leaving just the size to make
			//	testing and moving on simpler.
			//
			sz = block_size( sz );
			//
			//	We will do some space consolidation
			//	here by looking ahead to see if the
			//	next block can be joined into this
			//	block.
			//
			//	This loop will completely consolidate
			//	all consecutive deallocated blocks in
			//	the heap so no other approach is
			//	required.
			//
			while( is_flag_set( nxt = _heap[ look + sz ])) {
				//
				//	Yes, so do this now as it does
				//	not cost much at this point in time.
				//
				sz += block_size( nxt );
				_heap[ look ] = set_flag( sz );
			}
			//
			//	If this is large enough but smaller
			//	than previous best, then this is
			//	the new best.
			//
			if(( sz >= rqd )&&( sz < bst )) {
				bst = sz;
				fnd = look;
				if( bst == rqd ) break;
			}
		}
		look += sz;
	}
	return(( bst != invalid )? fnd: look );
}

//
//	The constructor
//	===============
//
Memory_Heap::Memory_Heap( void ) {
	//
	//	Initialise heap space as empty.
	//
	for( storage_unit i = 0; i < size; _heap[ i++ ] = 0 );
	//
	//	Done
	//
};

//
//	The memory allocation routine, argument is in bytes.
//
void *Memory_Heap::alloc( size_t rqst ) {

	STACK_TRACE( "void *Memory_Heap::alloc( size_t rqst )" );

	TRACE_HEAP( console.print( F( "MHP alloc " )));
	TRACE_HEAP( console.println( (word)rqst ));

	storage_unit	rqd,
			fnd,
			sz;

	//
	//	Space required is that for the allocation itself
	//	plus one for the length information all converted
	//	to a number of "storage_unit" data items.
	//
	rqd = (( rqst + rounding ) / sizeof( storage_unit )) + 1;
	
	//
	//	Generally, do we consider this a valid request?
	//
	if(( rqd < sizeof( storage_unit ))||( rqd >= size )) {
		errors.log_error( HEAP_ERR_INVALID_ALLOCATION, rqst );
		return( NIL( void ));
	}
	
	//
	//	Can we find something already de-allocated?
	//
	if(( sz = block_size( _heap[ fnd = find( rqd )]))) {
		//
		//	Found an existing, but de-allocated, allocation.
		//
		//	This is the "least waste" whole block which can
		//	be reused.  If we are using less than half of it
		//	then we will break the block up.
		//
		if( rqd < ( sz >> 1 )) {
			//
			//	Sub divide the space.
			//
			_heap[ fnd + rqd ] = set_flag( sz - rqd );
			_heap[ fnd ] = reset_flag( rqd );
		}
		else {
			//
			//	Allocate whole space as once piece.
			//
			_heap[ fnd ] = reset_flag( sz );
		}
	}
	else {
		//
		//	Found the end of the allocated blocks (size == 0).
		//
		//	Is there space for this block?
		//
		//	There needs to be at least enough space for the
		//	required block PLUS one for the "0" at the end
		//	of the block list.
		//
		if( rqd >= ( size - fnd )) {
			//
			//	We cannot allocate ALL of the space, we have
			//	to leave at least one '0' length at the end.
			//
			errors.log_error( HEAP_ERR_OUT_OF_MEMORY, rqd );

			TRACE_HEAP( console.println( F( "MHP ret 0" )));

			return( NIL( void ));
		}
		//
		//	All good.
		//
		_heap[ fnd ] = rqd;
	}
	//
	//	Return the allocated memory remembering that the caller
	//	is only interested in the memory after the length data.
	//
	
	TRACE_HEAP( console.print( F( "MHP ret " )));
	TRACE_HEAP( console.println( (word)fnd ));
	
	return( (void *)&( _heap[ fnd + 1 ]));
};

//
//	The memory de-allocation routine.
//
void Memory_Heap::free( void *block ) {

	STACK_TRACE( "void Memory_Heap::free( void *block )" );

	storage_unit	look;

	if( valid( look = ((storage_unit *)block - _heap ) - 1 )) {

		TRACE_HEAP( console.print( F( "MHP free " )));
		TRACE_HEAP( console.println( (word)look ));
		
		storage_unit	sz;
		
		if( is_flag_set( sz = _heap[ look ])) {
			//
			//	Cannot release what has already been released.
			//
			errors.log_error( HEAP_ERR_DUP_DEALLOCATE, look );
		}
		else {
			//
			//	Now, just mark as released/free.
			//
			//	We could enter into a minor consolidation of free
			//	space at this point, but the find() routine does
			//	a completely effective consolidation every time
			//	it is called in alloc(). 
			//
			_heap[ look ] = set_flag( sz );
		}
	}
	else {

		TRACE_HEAP( console.print( F( "MHP invalid " )));
		TRACE_HEAP( console.println( (word)look ));
		
		errors.log_error( HEAP_ERR_INVALID_ADRS, look );
	}
};

//
//	Erase the heap memory, make ready for a new cycle of
//	allocations etc.
//
void Memory_Heap::erase( void ) {

	STACK_TRACE( "void Memory_Heap::erase( void )" );

	//
	//	Simply set the whole heap to zeros.
	//
	for( storage_unit i = 0; i < size; _heap[ i++ ] = 0 );
}


//
//	Declare the memory heap itself.
//
extern Memory_Heap heap;


//
//	EOF
//
