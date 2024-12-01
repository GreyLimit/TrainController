//
//	Formatting.cpp
//	==============
//
//	Implementation of said formatting routines.
//


#include "Environment.h"
#include "Parameters.h"
#include "Configuration.h"
#include "Formatting.h"


//
//	Numerical output formatting routines
//	------------------------------------
//
//	This "back fill" integer to text routine is used only
//	by the LCD update routine.  Returns false if there was
//	an issue with the conversion (and remedial action needs
//	to be done) or true if everything worked as planned.
//
//	The "int" version handles signed 16 bit numbers, the byte
//	version unsigned 8 bit values.
//
bool backfill_int_to_text( char *buf, byte len, int v ) {
	bool	n;	// Negative flag.

	ASSERT( buf != NULL );
	ASSERT( len > 0 );

	//
	//	Cut out the "0" case as it need special handling.
	//
	if( v == 0 ) {
		//
		//	Zero case easy to handle. Remember that
		//	we pre-decrement 'len' as the value is
		//	the number of byte left, not the index of
		//	the last byte.
		//
		//	We know len is at least 1 byte.
		//
		buf[ --len ] = '0';
	}
	else {
		//
		//	Prepare for handling negative number
		//
		if(( n = ( v < 0 ))) v = -v;
		
		//
		//	loop round pealing off the digits
		//
		while( len-- ) {
			//
			//	While() test conveniently checks the
			//	number of bytes left is greater than
			//	zero, before moving len down to the
			//	index of the next charater to fill in.
			//
			buf[ len ] = '0' + ( v % 10 );
			if(( v /= 10 ) == 0 ) {
				//
				//	We break out here if v is zero
				//	as our work is done!
				//
				break;
			}
		}
		//
		//	If v is not zero, or if len is zero and
		//	the negative flag is set, then we cannot
		//	fit the data into the available space.
		//
		if( v ||( n && ( len < 1 ))) return( false );
		
		//
		//	Insert negative symbol if required.
		//
		if( n ) buf[ --len ] = '-';
	}
	//
	//	Space pad rest of buffer.  Remember here, too, that
	//	len is (effectively) pre-decremented before being used
	//	as the index.
	//
	while( len-- ) buf[ len ] = SPACE;
	//
	//	Done!
	//
	return( true );
}

//
//	Again for unsigned bytes.
//
bool backfill_byte_to_text( char *buf, byte len, byte v ) {

	ASSERT( buf != NULL );
	ASSERT( len > 0 );

	//
	//	An unsigned byte specific version of the above routine.
	//
	if( v == 0 ) {
		buf[ --len ] = '0';
	}
	else {
		while( len ) {
			buf[ --len ] = '0' + ( v % 10 );
			if(( v /= 10 ) == 0 ) break;
		}
	}
	while( len ) buf[ --len ] = SPACE;
	return( v == 0 );
}



//
//	Now front fill a value with unsigned byte, return number of
//	character used or 0 on error.
//
byte byte_to_text( char *buf, byte len, byte v ) {
	byte	l;

	ASSERT( buf != NULL );
	ASSERT( len > 0 );

	if( v == 0 ) {
		buf[ 0 ] = '0';
		return( 1 );
	}
	l = 0;
	while( len-- ) {
		buf[ l++ ] = '0' + ( v % 10 );
		if(( v /= 10 ) == 0 ) break;
	}
	return(( v > 0 )? 0: l );
}

//
//	Now front fill a value with unsigned byte, return number of
//	character used or 0 on error.
//
byte word_to_text( char *buf, byte len, word v ) {
	byte	l;

	ASSERT( buf != NULL );
	ASSERT( len > 0 );

	if( v == 0 ) {
		buf[ 0 ] = '0';
		return( 1 );
	}
	l = 0;
	while( len-- ) {
		buf[ l++ ] = '0' + ( v % 10 );
		if(( v /= 10 ) == 0 ) break;
	}
	return(( v > 0 )? 0: l );
}







//
//	EOF
//
