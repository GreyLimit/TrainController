//
//	FrameBuffer.h
//	=============
//
//	Capture the functionality of a directly addressed
//	LCD display in a "memory mapped" methodology.
//

#ifndef _FRAME_BUFFER_H_
#define _FRAME_BUFFER_H_


#include "Environment.h"
#include "Parameters.h"
#include "Configuration.h"

#include "LCD.h"
#include "Signal.h"
#include "Task_Entry.h"

//
//	Declare the frame buffer
//
class FrameBuffer : public Task_Entry {
private:
	//
	//	Our target LCD.
	//
	LCD		*_lcd;
	
	//
	//	The following variables contain and manage the current "frame buffer"
	//	for the LCD.  These are only activated if a piece of memory is provided
	//	to be the frame buffer (the object does not allocate or contain any
	//	memory space for this purpose).
	//
	//	_buffer		Where the frame buffer is in memory.
	//
	//	_size		The size of the frame buffer (or 0 if not defined,
	//			use as indication of a buffer being available)
	//
	//	_chk_r		The Row and Col position of the buffer
	//	_chk_c		scanning code.
	//
	//	_cursor		Insert point for next output text.
	//
	//	_sync		True if the LCD position is synchronised
	//			with the buffer scan position (r,c).
	//
	byte		*_buffer,			// Buffer area.
			_rows,				// Dimension of frame buffer
			_cols,				//   rows and cols.
			_size,				// Size of the buffer (total bytes).
			_chk_r,				// The Row and Col position of
			_chk_c,				//   the buffer scanning code.
			_cursor;			// index into buffer
	bool		_sync;				// Cursor synchronised with scan?

	//
	//	Declare the control flag used to interface with the
	//	LCD driver.
	//
	Signal		_flag;
	
public:
	FrameBuffer( void );
	void initialise( LCD *lcd, byte *buffer, byte size, byte rows, byte cols );
	void set_posn( byte row, byte col );
	void clear( void );
	void write_char( char val );
	void write_str( const char *str );
	void write_buf( const char *buf, byte len );
	void write_PROGMEM( const char *buf, byte len );
	void fill( char val, byte len );

	virtual void process( void );
};


#endif

//
//	EOF
//
