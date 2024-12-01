//
//	Function.h
//	==========
//
//	Module to Cache and manage the "function state" of mobile
//	decoders.
//

#ifndef _FUNCTION_H_
#define _FUNCTION_H_

//
//	We will need the following files
//
#include "Environment.h"
#include "Parameters.h"
#include "Configuration.h"
#include "DCC.h"

//
//	Decoder Function value cache
//	----------------------------
//
//	Native DCC Generator code requires a function status cache to
//	support modification of an individual decoder function.  This
//	is required as (as far as I can tell) there is no mechanism to
//	allow individual function adjustment without setting/resetting
//	between 3 to 7 other functions at the same time.
//


//
//	Declare a "database" which will be used to keep the functional
//	state of the "most recently" accessed mobile decoders.
//
class Function {
private:
	//
	//	Define a number of cache records and bytes for bit storage in
	//	each record.  We calculate FUNCTION_BIT_ARRAY based on the
	//	MIN and MAX function numbers provided (the 7+ ensures correct
	//	rounding in boundary cases).
	//
	//	We will base the function cache size on the maximum number of
	//	DCC mobile decoders we can have active in parallel.
	//
	static constexpr byte	cache_size	= DCC::transmission_buffers * 2;
	static constexpr byte	bit_array	= (( 1 + DCC::maximum_func_number - DCC::minimum_func_number ) + 7 ) >> 3;

	//
	//	The structure used to cache function values per decoder
	//	so that the "block" function setting DCC packet can be
	//	used (because that is the only way).
	//
	struct cache {
		int		target;
		byte		bits[ bit_array ];
		cache		*next,
				**prev;
	};

	//
	//	This is the array of cache records and the pointer into the head
	//	of it (the most recently accessed).
	//
	cache		_record[ cache_size ],
			*_cache;

	//
	//	Define the lookup and manage cache code.
	//
	cache *find( int target ) {
		cache	**adrs,
			*last,
			*ptr;

		ASSERT( target >= DCC::minimum_address );
		ASSERT( target <= DCC::maximum_address );

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
		for( byte i = 0; i < FUNCTION_BIT_ARRAY; last->bits[ i++ ] = 0 );
		
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


public:
	//
	//	Function to initialise the cache records empty.
	//
	//	We will "pre-fill" the cache with empty records so that the code can
	//	always assume that there are records in the cache, because there are.
	//
	void Function( void ) {
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
	bool update( int target, byte func, bool state ) {
		cache	*ptr;
		byte	i, b; 

		ASSERT( func >= DCC::minimum_func_number );
		ASSERT( func <= DCC::maximum_func_number );

		ptr = find( target );
		i = ( func - DCC::minimum_func_number ) >> 3;
		b = 1 << (( func - DCC::minimum_func_number ) & 7 );

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
	static byte get( int target, byte func, byte val ) {
		cache	*ptr;
		byte	i, b; 

		ASSERT( func >= DCC::minimum_func_number );
		ASSERT( func <= DCC::maximum_func_number );

		ptr = find( target );
		i = ( func - DCC::minimum_func_number ) >> 3;
		b = 1 << (( func - DCC::minimum_func_number ) & 7 );

		if( ptr->bits[ i ] & b ) return( val );
		return( 0 );
	}


};

//
//	Define the function cache.
//
extern Function function_cache;

#endif

//
//	EOF
//




