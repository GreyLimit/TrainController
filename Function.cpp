//
//	Function.cpp
//	============
//
//	Module to Cache and manage the "function state" of mobile
//	decoders.
//


#include "Function.h"

//
//	Define the lookup and manage cache code.
//
Function::cache *Function::find( word target ) {
	cache	**adrs,
		*last,
		*ptr;

	ASSERT( target >= DCC_Constant::minimum_address );
	ASSERT( target <= DCC_Constant::maximum_address );

	//
	//	Start at the top, the most recently accessed record.
	//
	adrs = &_cache;
	last = NIL( cache );

	//
	//	Extract address of the next record to check..
	//
	while(( ptr = *adrs )) {
		//
		//	Is this the one?
		//
		if( target == ptr->target ) {
			//
			//	Yes, so move to top of the list (if not already there)
			//	so that access to this record is as quick as possible
			//	for subsequent requests.
			//
			if( _cache != ptr ) {
				//
				//	Detach from the list.
				//
				if(( *( ptr->prev ) = ptr->next )) ptr->next->prev = ptr->prev;
				//
				//	Add to head of list.
				//
				ptr->next = _cache;
				_cache->prev = &( ptr->next );
				_cache = ptr;
				ptr->prev = &_cache;
			}
			//
			//	Return the pointer to the desired cache record.
			//
			return( ptr );
		}
		//
		//	Note last record we saw.
		//
		last = ptr;
		adrs = &( ptr->next );
	}
	//
	//	Nothing found, so we re-use the oldest record in the list.
	//	Start by unlinking it from the end of the list.
	//
	*( last->prev ) = NULL;
	
	//
	//	Replace with new target and empty function settings (since
	//	we know nothing about them).
	//
	last->target = target;
	for( byte i = 0; i < bit_array; last->bits[ i++ ] = 0 );
	
	//
	//	Link onto head of cache.
	//
	last->next = _cache;
	_cache->prev = &( last->next );
	_cache = last;
	last->prev = &_cache;
	//
	//	Done.
	//
	return( last );
}


//
//	Function to initialise the cache records empty.
//
//	We will "pre-fill" the cache with empty records so that the code can
//	always assume that there are records in the cache, because there are.
//
Function::Function( void ) {
	cache	**tail,
		*ptr;
	
	tail = &_cache;
	for( byte i = 0; i < cache_size; i++ ) {
		//
		//	Note current record.
		//
		ptr = &( _record[ i ]);
		
		//
		//	Empty the record.
		//
		ptr->target = 0;
		for( byte j = 0; j < bit_array; ptr->bits[ j++ ] = 0 );
		
		//
		//	Link in the record.
		//
		*tail = ptr;
		ptr->prev = tail;
		tail = &( ptr->next );
	}
	//
	//	Finally we terminate the list.
	//
	*tail = NULL;
}

//
//	Routine applies a boolean value for a specified function
//	on a specified target number.
//
//	This returns true if the function status changed, false
//	otherwise.
//
bool Function::update( word target, byte func, bool state ) {
	cache	*ptr;
	byte	i, b; 

	//ASSERT( func >= DCC_Constant::minimum_func_number );
	ASSERT( func <= DCC_Constant::maximum_func_number );

	ptr = find( target );
	i = ( func - DCC_Constant::minimum_func_number ) >> 3;
	b = 1 << (( func - DCC_Constant::minimum_func_number ) & 7 );

	if( state ) {
		//
		//	Bit set already?
		//
		if( ptr->bits[ i ] & b ) return( false );
		//
		//	Yes.
		//
		ptr->bits[ i ] |= b;
		return( true );
	}
	//
	//	Bit clear already?
	//
	if(!( ptr->bits[ i ] & b )) return( false );
	//
	//	Yes.
	//
	ptr->bits[ i ] &= ~b;
	return( true );
}

//
//	Routine returns 0 or the supplied value based on the
//	supplied function number being off or on.
//
byte Function::get( word target, byte func, byte val ) {
	cache	*ptr;
	byte	i, b; 

	//ASSERT( func >= DCC_Constant::minimum_func_number );
	ASSERT( func <= DCC_Constant::maximum_func_number );

	ptr = find( target );
	i = ( func - DCC_Constant::minimum_func_number ) >> 3;
	b = 1 << (( func - DCC_Constant::minimum_func_number ) & 7 );

	if( ptr->bits[ i ] & b ) return( val );
	return( 0 );
}





//
//	Declare the function cache.
//
Function function_cache;

//
//	EOF
//
