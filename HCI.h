//
//	HCI.h
//	=====
//
//	The Human Computer Interface.
//

#ifndef _HCI_H_
#define _HCI_H_

#include "Environment.h"
#include "Parameters.h"
#include "Configuration.h"
#include "Menu.h"
#include "LCD.h"
#include "FrameBuffer.h"
#include "Keypad.h"
#include "Rotary.h"
#include "Formatting.h"

//
//	Declare the class containing the HCI control systems
//
class HCI {
private:
	//
	//	We define here the human interpretation of DCC speeds
	//	as stored in the status field of the HCI.  for this
	//	purpose the following definitions apply:
	//
	//	Bit(s)	Role	Meaning
	//	------	----	-------
	//	0-6	Speed	0 is stationary, 126 is maximum
	//			(not to be confused with underlying
	//			DCC values, which are similar).
	//	7	Dir	1: Forwards, 0:Backwards
	//	8	Valid	0:Invalid, 1:Valid
	//
	static const word	minimum_speed = 0;
	static const word	maximum_speed = 126;
	//
	static inline byte read_speed( word state ) { return( state & 0x7F ); }
	static inline bool read_direction( word state ) { return(( state & 0x80 ) == 0x00 ); }
	static inline bool speed_dir_valid( word state ) { return(( state & 0x100 ) != 0x000 ); }
	static inline word speed_dir_state( byte speed, bool dir ) { return( 0x100 |( dir? 0x80: 0x00 )|( speed & 0x7F )); }

	//
	//	Define here how we store the state of an accessory.  This
	//	is easier than a mobile decoder speed as we only need to know
	//	if the state is valid, and what the state is.
	//
	static inline bool accessory_valid( word state ) { return(( state & 0x20 ) != 0x00 ); }
	static inline bool read_accessory( word state ) { return(( state & 0x01 ) != 0x00 ); }
	static inline word accessory_state( bool state ) { return( 0x20 |( state? 0x01: 0x00 )); }
	
	//
	//	These are the other components which form the HCI.
	//
	LCD		_lcd;
	byte		_frame_buffer[ LCD_FRAME_BUFFER ];
	FrameBuffer	_display;
	Rotary		_dial;
	Keypad		_keypad;

	//
	//	These are the "pointers" into the HCI structures
	//	which track where we are and what is being displayed.
	//
	page_data	*_this_page;
	byte		_this_page_index;
	object_data	*_this_object;
	byte		_this_object_line;
	const menu_page	*_this_menu;
	byte		_this_menu_index;

	//
	//	Here we will track the state of the shift keys so their
	//	modifying effects can be applied.
	//
	bool		_menu_shift = false;
	bool		_page_shift = false;
	bool		_input_mode = false;
	bool		_input_mobile = false;

	//
	//	Are we displaying the status data or the object data?
	//
	bool		_display_status = true;

public:
	//
	//	Redrawing..
	//
	void redraw_object_area( void );
	void redraw_menu_area( void );
	void redraw_page_line( byte r );
	void redraw_page_area( void );

	void update_dcc_status_line( byte line );
	void update_dcc_status( void );

	void process_menu_option( byte m );

	//
	//	Keypad inputs here
	//
	void user_key_event( bool down, char key );

	//
	//	Rotary inputs here.
	//
	void user_button_pressed( word duration );
	void user_rotary_movement( sbyte change );

	//
	//	The initialisation routine
	//
	void initialise( void );

	//
	//	Called to check keypad for input
	//
	void keypad_reader( void );

	//
	//	Called to process rotary actions.
	//
	void rotary_updater( void );
};


//
//	Declare the HCI object itself.
//
extern HCI hci_control;

#endif

//
//	EOF
//
