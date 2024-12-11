//
//	FrameBuffer.cpp
//	===============
//
//	Implementation of the frame buffer
//

#include "FrameBuffer.h"
#include "Code_Assurance.h"
#include "Task.h"

//
//	Declare simple macros for tagging and clearing the "dirty"
//	bit in the frame buffer
//
#define TAG_DIRTY(b)	((b)|(byte)0x80)
#define IS_DIRTY(b)	((b)&(byte)0x80)
#define TAG_CLEAN(b)	((b)&(byte)0x7F)

FrameBuffer::FrameBuffer( void ) {
	_lcd = NIL( LCD );
	_buffer = NIL( byte );
	_rows = 0;
	_cols = 0;
	_size = 0;
	_chk_r = 0;
	_chk_c = 0;
	_cursor = 0;
	_sync = false;
}

void FrameBuffer::initialise( LCD *lcd, byte *buffer, byte size, byte rows, byte cols ) {

	ASSERT( lcd != NIL( LCD ));
	ASSERT( buffer != NIL( byte ));
	ASSERT( rows > 0 );
	ASSERT( cols > 0 );

	//
	//	Save the frame buffer dimensions.
	//
	_lcd = lcd;
	_buffer = buffer;
	_rows = rows;
	_cols = cols;
	_size = _rows * _cols;

	ASSERT( size == _size );

	//
	//	Empty the buffer and tag data as
	//	needing to be updated.
	//
	for( byte i = 0; i < _size; _buffer[ i++ ] = TAG_DIRTY( SPACE ));

	//
	//	Add ourselves to the task list.
	//
	task_manager.add_task( this, &_flag );

	//
	//	We now initiate processing by
	//	releasing the flag.
	//
	_flag.release();
}

void FrameBuffer::clear( void ) {
	//
	//	Empty the buffer and tag data as
	//	needing to be updated.
	//
	for( byte i = 0; i < _size; _buffer[ i++ ] = TAG_DIRTY( SPACE ));
	//
	//	Set cursor to home.
	//
	_cursor = 0;
}

	
void FrameBuffer::set_posn( byte row, byte col ) {

	ASSERT( _size );
	ASSERT( _buffer );

	//
	//	Simply set the cursor index somewhere inside the frame
	//	buffer.
	//
	if(( _cursor = ( row * _cols + col )) >=  _size ) _cursor = 0;
}

void FrameBuffer::write_char( char val ) {

	ASSERT( _size );
	ASSERT( _buffer );

	//
	//	Place character into the frame buffer at the cursor
	//	position and move the cursor forward one position.
	//	Wrap the cursor round to the top of the screen if
	//	you head off the end of the last line.
	//

	byte v = TAG_CLEAN( val );
	
	//
	//	Place value into the frame buffer but only if
	//	it is different from the value already there.
	//
	if( _buffer[ _cursor ] != v ) _buffer[ _cursor ] = TAG_DIRTY( v );
	//
	//	Move the cursor forward, and wrap to the top
	//	if we fall off the bottom.
	//
	if(( _cursor += 1 ) >= _size ) _cursor = 0;
}

void FrameBuffer::write_str( const char *str ) {
	char	c;
	
	while(( c = *str++ ) != EOS ) write_char( c );
}

void FrameBuffer::write_buf( const char *buf, byte len ) {
	while( len-- ) write_char( *buf++ );
}

void FrameBuffer::write_PROGMEM( const char *buf, byte len ) {
	while( len-- ) write_char( progmem_read_byte_at( buf++ ));
}

void FrameBuffer::fill( char val, byte len ) {
	while( len-- ) write_char( val );
}


//
//	The procedure that provides the "dynamic" background updates
//	from the frame buffer to the LCD thus disconnecting the firmware
//	requirement to output data from the displays ability to accept it.
//


//	JEFF
//	NOTE/	This routine will be .. slow .. since it is effectively
//		synchronising its updates with the throughput of the
//		LCD driver (using the _flag signal to hand control back
//		and forth between them).  It *may* be faster to implement
//		a system of "over driving" the LCD with data - it does have
//		a queue for this purpose.
//
//		This will require some sort of counter used to monitor the
//		size of the "over drive".
//
//		Another thought - why not keep a count of "dirty" bytes
//		and fill int the LCD queue until it is full?
//
//	Need to be smart here - really possibility of make a spin for ever loop!
//
void FrameBuffer::process( void ) {
	byte	i, c;
	
	//
	//	Frame buffer support.
	//	=====================
	//
	//	The _chk_r and _chk_c variables give the current position
	//	of where we are looking in the buffer.
	//
	//	Is there something to do at the current scan position?
	//
	c = _buffer[( i = _chk_r * _cols + _chk_c )];
	if( IS_DIRTY( c )) {
		//
		//	All "pending" updates in the frame buffer have
		//	their top bit set. Once a position has been updated
		//	This bit is reset.
		//
		if( _sync ) {
			//
			//	We can just directly output the character
			//	to the display.
			//
			c = TAG_CLEAN( c );
			if( _lcd->write( c, &_flag )) {
				//
				//	Success, update frame buffer, move
				//	next and last on wards.
				//
				_buffer[ i ] = c;
				//
				//	Now move on the next and last values.
				//
				//	But .. remembering that the LCD does
				//	not wrap across the edge of the display.
				//
				if(( _chk_c += 1 ) >= _cols ) {
					//
					//	If we "run off" the edge of the
					//	line then we will have to get the
					//	LCD to re-position its cursor.
					//
					_sync = false;
					_chk_c = 0;
					if(( _chk_r += 1 ) >= _rows ) {
						_chk_r = 0;
					}
				}
			}
		}
		else {
			//
			//	We need to move the output cursor on the LCD
			//	to where we need to output a pending character.
			//
			if( _lcd->index( i, &_flag )) {
				//
				//	If the index function is successful then
				//	we can move the LCD note that the LCD and
				//	scan positions are synchronised.
				//
				_sync = true;
			}
		}
	}
	else {
		//
		//	Any character we have "skipped" over means that
		//	the output LCD cursor is in the wrong place, so
		//	we reset for sync flag.
		//
		_sync = false;
		//
		//	Move on to the next byte, and wrap if we get to
		//	the bottom of the display.
		//
		if(( _chk_c += 1 ) >= _cols ) {
			_chk_c = 0;
			if(( _chk_r += 1 ) >= _rows ) {
				_chk_r = 0;
			}
		}
		//
		//	Finally we "self schedule ourselves again because
		//	we haven't sent anything out yet (so the LCD code
		//	will not wake us up).
		//
		//	This *WILL* lead to wasted cycles as the frame
		//	buffer spin loops when the buffer is static and
		//	unchanging.
		//
		//	JEFF - bad design here 
		//
		_flag.release();
	}
}

//
//	EOF
//
