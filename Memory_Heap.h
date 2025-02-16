//
//	Memory_Heap.h		The definition of  a generic
//	=============		memory heap allocation (and
//				deallocation) system designed
//	to be a used in collaboration with C++ new() and delete().
//
//	Specifically new should be called thus:
//
//		type *ptr = new ( malloc( sizeof( type ))) type;
//
//	where
//
//		type is the class/type name
//
//		ptr will become the same as address
//
//	It is also valid to use the following code:
//
//		type *ptr = (type *)malloc( sizeof( type ));
//		ptr->type();
//
//	This is explicitly calling the constructor code.
//
//	When the memory is to be deleted it should NOT be
//	done through the "delete" operator, but rather directly
//	with the mechanism which initially provided the memory.
//
//	Specifically to delete an object do the following:
//
//		ptr->~type();
//		free( (void *)ptr );
//
//
//	The use of malloc() and free() above are place holders
//	for any other memory management system of choice.
//
//	Note:
//
//	This systems is explicitly granted a fixed memory space
//	upon initialisation and also provides a blanket "memory
//	reset" function which sets the memory provided back to
//	the empty state.
//

#ifndef _MEMORY_HEAP_H_
#define _MEMORY_HEAP_H_

//
//	Bring in some consistent types.
//
#include "Environment.h"
#include "Library_Types.h"

//
//	Declare the size of the heap if not defined elsewhere.  This
//	definition of 128 bytes is enough to enable firmware to compile
//	and execute, but (likely) not enough to facilitate full
//	functionity.
//
//	The value of HEAP_SIZE should be overredden in the Parameters
//	or Configuration file.
//
#ifndef HEAP_SIZE
#define HEAP_SIZE	127
#endif


//
//	The Memory Heap class.
//	======================
//
//	The Heap itself will be managed in the most simple minded
//	manner possible.  This is specifically to promote a solution
//	which is simple to write and so (hopefully) simple to
//	prove correct.  The solution should have a low memory over
//	head at the expense of a high CPU overhead working on the
//	assumption that heap activity will be low volume and
//	predominantly allocate once basis.
//
//	For high volume, fast allocation and deallocations consider
//	the Memory_Pool object.  This is specifically for temporary
//	object management.
//
//	Memory Heap data memory structure:
//
//	Heap->
//	0						size-1
//	Allocation	Allocation
//	[[size][data]]	[[size][data]]	... [0]
//
//	Where:
//
//		size	The number of heap "units" in the allocation
//			including the size.
//
//		data	The actual memory area handed over to the calling
//			code.
//
class Memory_Heap {
private:
	//
	//	Define the base type which the heap system works with
	//	internally.  This is expected to be an unsigned integer
	//	container capable of containing a pointer value.
	//
	//	This does not impact the callers use of this object.
	//
#ifdef ARDUINO
#if HEAP_SIZE < 128
	typedef byte storage_unit;
#else
	typedef word storage_unit;
#endif
#else
#if HEAP_SIZE < 32768
	typedef word storage_unit;
#else
	typedef dword storage_unit;
#endif
#endif

	//
	//	Declare the total size of the heap area in terms of the
	//	storage unit being used.  This means that we have to
	//	divide the specified heap size by the sizeof the storage
	//	unit.
	//
	static const storage_unit	size = HEAP_SIZE / sizeof( storage_unit );
	
	//
	//	The memory area we will manage.  This is a fixed area of
	//	memory which can be allocated from dynamically and released
	//	when finished with.
	//
	storage_unit			_heap[ size ];

	//
	//	Define a number of constants based on the storage_unit type.
	//
	//	We can use the top bit safely as we are always working
	//	in units of "storage_unit", a data item declared to be
	//	no less than *twice* the capacity of the byte size of
	//	the whole heap area.
	//
	static const storage_unit heap_flag	= (storage_unit)1 << (( sizeof( storage_unit ) << 3 ) -1 );
	static const storage_unit heap_data	= heap_flag -1;
	//
	static const storage_unit empty		= (storage_unit)0;
	//
	static const storage_unit invalid	= ~empty;
	//
	static const storage_unit rounding	= sizeof( storage_unit ) -1;
	
	//
	//	Define some simple "bit twiddling" functions which the
	//	heap code will require to manage the heap records.
	//
	static inline bool is_flag_set( storage_unit v ) { return( BOOL( v & heap_flag )); }
	static inline storage_unit set_flag( storage_unit v ) { return( v | heap_flag ); }
	static inline storage_unit reset_flag( storage_unit v ) { return( v & heap_data ); }
	static inline storage_unit block_size( storage_unit v ) { return( v & heap_data ); }

	//
	//	Allocation verification routine.
	//
	//	Returns true if the index is referencing a valid block,
	//	allocated or deallocated, or false if there is an issue
	//	with the index.
	//
	bool valid( storage_unit index );
	
	//
	//	Free space location routine
	//
	//	Find the best fit deallocated block and return its index
	//	or return the index of the first "never allocated"
	//	piece of heap memory.
	//
	storage_unit find( storage_unit rqd );

public:
	//
	//	The constructor
	//	===============
	//
	Memory_Heap( void );
	
	//
	//	The memory allocation routine, argument is in bytes.
	//
	void *alloc( size_t rqst );

	//
	//	The memory deallocation routine.
	//
	void free( void *block );

	//
	//	Erase the heap memory, make ready for a new cycle of
	//	allocations etc.
	//
	void erase( void );
};


//
//	Declare the memory heap itself.
//
extern Memory_Heap heap;

//
//	Declare a basic API to simplify the code associated with the
//	allocation and release of dynamic memory space on the heap.
//
template<class T> inline T *NEW( void ) {
	T *ptr = heap.alloc( sizeof( T ));
	if( ptr ) ptr->T();
	return( ptr );
}
template<class T> inline void FREE( T *ptr ) {
	ptr->~T();
	heap.free( (void *)ptr );
}


#endif

//
//	EOF
//
