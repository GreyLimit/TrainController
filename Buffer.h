//
//	Buffer.h
//	========
//
//	A set of routines specifically for the encoding of
//	replies through the Serial/USB connection.
//

#ifndef _BUFFER_H_
#define _BUFFER_H_

#include "Environment.h"
#include "Parameters.h"
#include "Configuration.h"
#include "Protocol.h"

//
//	Reply Construction routines.
//	----------------------------
//
//	The following set of routines provide alternatives to
//	using "sprintf()" for filling in a buffer space with
//	a reply from the firmware.
//
//	This has been done to reduce memory usage from the sprintf
//	template strings in exchange for a larger code base (as
//	there is plenty of code flash available but memory is
//	tight on the smaller MCUs).
//
template< byte size >
class Buffer {
private:
	//
	//	Declare the size of the internal buffer
	//
	static const byte	buffer_size = size;

	//
	//	Define the maximum number of digits required to convert
	//	an integer value into its text form.
	//
	//	A digit stack of 10 is sufficient for a signed (or
	//	unsigned) 32 bit number conversion.
	//
	static const byte	digit_stack = 10;

	//
	//	This is the internal buffer created.
	//
	char			_buffer[ buffer_size ];

	//
	//	Our pointers into the buffer we have been assigned.
	//
	char			*_ptr;
	byte			_left;

	//
	//	The primitives which are combined to fill in the buffer.
	//
	bool start( char code ) {
		if( buf->left > 2 ) {
			*_ptr++ = Protocol::lead_in;
			*_ptr++ = code;
			_left -= 2;
			return( true );
		}
		return( false );
	}

	bool end( void ) {
		if( buf->left > 2 ) {
			*_ptr++ = Protocol::lead_out;
			*_ptr++ = NL;
			*_ptr = EOS;
			_left -= 2;
			return( true );
		}
		return( false );
	}

	bool add( char c ) {
		if( buf->left > 1 ) {
			*_ptr++ = c;
			_left -= 1;
			return( true );
		}
		return( false );
	}
	
	bool add( int v ) {
		char	r[ digit_stack ];
		byte	c;
		bool	n;

		c = 0;
		if(( n = ( v < 0 ))) v = -v;
		if( v == 0 ) {
			r[ c++ ] = 0;
		}
		else {
			while( v ) {
				r[ c++ ] = v % 10;
				v /= 10;
			}
		}
		if( n ) {
			if( _left < 2 ) return( false );
			*_ptr++ = MINUS;
			_left -= 1;
		}
		if( _left <= c ) return( false );
		_left -= c;
		while( c ) *_ptr++ = '0' + r[ --c ];
		return( true );
	}

	bool add( word v ) {
		char	r[ digit_stack ];
		byte	c;

		c = 0;
		if( v == 0 ) {
			r[ c++ ] = 0;
		}
		else {
			while( v ) {
				r[ c++ ] = v % 10;
				v /= 10;
			}
		}
		if( _left <= c ) return( false );
		_left -= c;
		while( c ) *_ptr++ = '0' + r[ --c ];
		return( true );
	}
	
	bool add( byte v ) {
		char	r[ digit_stack ];
		byte	c;

		c = 0;
		if( v == 0 ) {
			r[ c++ ] = 0;
		}
		else {
			while( v ) {
				r[ c++ ] = v % 10;
				v /= 10;
			}
		}
		if( _left <= c ) return( false );
		_left -= c;
		while( c ) *_ptr++ = '0' + r[ --c ];
		return( true );
	}
	
public:
	void Buffer( void ) {
		_ptr = _buffer;
		_left = buffer_size;
	}

	bool format( char code, word a1 ) {
		return( start( code ) && add( a1 ) && end());
	}

	bool format( char code, word a1, word a2 ) {
		return( start( code ) && add( a1 ) && add( SPACE ) && add( a2 ) && end());
	}

	bool format( char code, word a1, word a2, word a3 ) {
		return( start( code ) && add( a1 ) && add( SPACE ) && add( a2 ) && add( a3 ) && end());
	}

	char *buffer( void ) {
		return( _buffer );
	}

	byte size( void ) {
		return( buffer_size - _left );
	}

	void copy( char *to, byte len ) {
		memcpy( to, _buffer, min( len, buffer_size ));
	}
};

#endif

//
//	EOF
//
