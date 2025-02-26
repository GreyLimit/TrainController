//
//	HCI.h
//	=====
//
//	The Human Computer Interface.
//

#include "HCI.h"
#include "Rotary.h"
#include "Keypad.h"
#include "DCC_Constant.h"
#include "DCC.h"
#include "Errors.h"
#include "Clock.h"
#include "Constants.h"
#include "Task_Entry.h"
#include "Banner.h"
#include "District.h"
#include "Districts.h"
#include "Stats.h"
#include "Function.h"
#include "TOD.h"
#include "Task.h"
#include "Trace.h"

#ifdef DEBUGGING_ENABLED
#include "Console.h"
#endif

//
//	Define a set of single character symbols to represent
//	actions/directions applied to decoders/accessories when
//	displayed on the LCD.
//
#define LCD_ACTION_FORWARD	'>'
#define LCD_ACTION_BACKWARDS	'<'
#define LCD_ACTION_STATIONARY	'='
#define LCD_ACTION_ENABLE	'Y'
#define LCD_ACTION_DISABLE	'N'

#define LCD_CAB_OBJECT		'C'
#define LCD_ACCESSORY_OBJECT	'A'


//
//	Redrawing routines.
//
void HCI::redraw_object_area( void ) {

	STACK_TRACE( "void HCI::redraw_object_area( void )" );

	char	buffer[ LCD_DISPLAY_STATUS_WIDTH ];

	//
	//	Redraw the whole object area based on the state of
	//	the current object.
	//
	if( _this_object->adrs > 0 ) {
		bool	left;
		byte	r;
		
		//
		//	A Cab/Mobile decoder object
		//
		
#if LCD_DISPLAY_STATUS_WIDTH != 6
#error "The mobile decoder code is written specifically for a 6 column area"
#endif

		//
		//	Set up the output index.
		//
		buffer[ 0 ] = '|';
		left = true;
		r = 0;
		for( byte f = DCC_Constant::minimum_func_number; f <= DCC_Constant::maximum_func_number; f++ ) {
			if( function_cache.get( _this_object->adrs, f, true )) {
				if( left ) {
					//
					//	left column.
					//
					(void)backfill_byte_to_text( buffer+1, 2, f );
					left = false;
				}
				else {
					//
					//	right column.
					//
					(void)backfill_byte_to_text( buffer+3, 3, f );
					//
					//	display
					//
					_display.set_posn( r++, LCD_DISPLAY_STATUS_COLUMN );
					_display.write_buf( buffer, LCD_DISPLAY_STATUS_WIDTH );
					left = true;
					//
					//	Overflow?
					//
					if( r >= LCD_DISPLAY_ROWS ) break;
				}
			}
		}
		//
		//	Need to blank some part of the buffer.
		//
		if( left )  {
			memset( buffer+1, SPACE, LCD_DISPLAY_STATUS_WIDTH-1 );
		}
		else {
			memset( buffer+3, SPACE, 3 );
		}
		if( r < LCD_DISPLAY_ROWS ) {
			_display.set_posn( r++, LCD_DISPLAY_STATUS_COLUMN );
			_display.write_buf( buffer, LCD_DISPLAY_STATUS_WIDTH );
			memset( buffer+1, SPACE, LCD_DISPLAY_STATUS_WIDTH-1 );
			while( r < LCD_DISPLAY_ROWS ) {
				_display.set_posn( r++, LCD_DISPLAY_STATUS_COLUMN );
				_display.write_buf( buffer, LCD_DISPLAY_STATUS_WIDTH );
			}
		}
	}
	else if( _this_object->adrs < 0 ) {
		//
		//	A Static/Accessory decoder object.
		//
		//	For the moment nothing to display.
		//
		buffer[ 0 ] = '|';
		memset( buffer+1, SPACE, LCD_DISPLAY_STATUS_WIDTH-1 );
		for( byte r = 0; r < LCD_DISPLAY_ROWS; r++ ) {
			_display.set_posn( r, LCD_DISPLAY_STATUS_COLUMN );
			_display.write_buf( buffer, LCD_DISPLAY_STATUS_WIDTH );
		}
	}
	else {
		//
		//	An empty object, and empty status display.
		//
		buffer[ 0 ] = '|';
		memset( buffer+1, SPACE, LCD_DISPLAY_STATUS_WIDTH-1 );
		for( byte r = 0; r < LCD_DISPLAY_ROWS; r++ ) {
			_display.set_posn( r, LCD_DISPLAY_STATUS_COLUMN );
			_display.write_buf( buffer, LCD_DISPLAY_STATUS_WIDTH );
		}
	}
}


void HCI::redraw_menu_area( void ) {

	STACK_TRACE( "void HCI::redraw_menu_area( void )" );

	for( byte r = 0; r < ITEM_COUNT; r++ ) {
		_display.set_posn( r, LCD_DISPLAY_MENU_COLUMN );
		_display.write_PROGMEM( _this_menu->item[ r ].text, MENU_ITEM_SIZE );
	}
}


void HCI::redraw_page_line( byte r ) {

#if LCD_DISPLAY_PAGE_WIDTH != 10
#error "This routine needs hand coding for new display size"
#endif

	STACK_TRACE( "void HCI::redraw_page_line( byte r )" );

	char		line[ LCD_DISPLAY_PAGE_WIDTH ];
	object_data	*o;
	
	o = &( _this_page->object[ r ]);

	//
	//	Update the edge character
	//
	line[ 0 ] = ( r == _this_object_line )? ( _input_mode? '#': '>' ): '|';

	if( o->adrs > 0 ) {
		//
		//	Mobile object.
		//
		line[ 1 ] = LCD_CAB_OBJECT;	// A "Cab" Object
		
		//
		//	Layout:	A=Address, D=Direction, S=Speed
		//
		//		|CAAAAADSS
		//		01234567890
		//
		(void)backfill_int_to_text( line+2, 5, o->adrs );
		if( speed_dir_valid( o->state )) {
			line[ 7 ] = read_direction( o->state )? LCD_ACTION_FORWARD: LCD_ACTION_BACKWARDS;
			if( !backfill_byte_to_text( line+8, 2, read_speed( o->state ))) memset( line+8, HASH, 2 );
		}
		else {
			memset( line+7, SPACE, 3 );
		}
	}
	else {
		if( o->adrs < 0 ) {
			//
			//	Static object.
			//
			//	Layout:	A=Address, D=Direction, S=Speed
			//
			//		|A__AAA_S_
			//		01234567890
			//
			line[ 1 ] = LCD_ACCESSORY_OBJECT;
			(void)backfill_int_to_text( line+2, 5, -o->adrs );
			line[ 7 ] = SPACE;
			if( accessory_valid( o->state )) {
				line[ 8 ] = read_accessory( o->state)? LCD_ACTION_ENABLE: LCD_ACTION_DISABLE;
			}
			else {
				line[ 8 ] = SPACE;
			}
			line[ 9 ] = SPACE;
		}
		else {
			//
			//	Empty object.
			//
			memset( line+1, SPACE, LCD_DISPLAY_PAGE_WIDTH-1 );
		}
	}
	//
	//	Display this line in the page
	//
	_display.set_posn( r, LCD_DISPLAY_PAGE_COLUMN );
	_display.write_buf( line, LCD_DISPLAY_PAGE_WIDTH );
}

void HCI::redraw_page_area( void ) {

	STACK_TRACE( "void HCI::redraw_page_area( void )" );

	for( byte r = 0; r < ITEM_COUNT; r++ ) redraw_page_line( r );
}

//
//	The update a line of the LCD with the status of the DCC Generator.
//
void HCI::update_dcc_status_line( byte line ) {

	STACK_TRACE( "void HCI::update_dcc_status_line( byte line )" );

	char		buffer[ LCD_DISPLAY_STATUS_WIDTH ];

	if( !_display_status ) return;

	//	 0....0....1....1....2
	//	 0    5    0    5    0
	//	+--------------------+	The STATUS area of the display, showing:
	//	|              SSSSSS|	The highest district power (L)oad (percent) of A
	//	|              SSSSSS|	... and of B districts
	//	|              SSSSSS|	The available (F)ree bit buffers and (P)ower status
	//	|              SSSSSS|	DCC packets (T)ransmitted packets per second
	//	+--------------------+
	//
	//	The status area can also be flip between the over all system
	//	status and the specific object status.
	//
	//	Now complete each of the rows in LCD_DISPLAY_STATUS_WIDTH-1 characters.
	//
	//	We do need to start with the edge of the area.
	//
	buffer[ 0 ] = '|';
	switch( line ) {
		case 0:
		case 1: {
			//
			//	Rows 0 and 1, Power status for the districts A/0 and B/1.
			//
			//	We are, probably erroneously (and because the values lined
			//	up), going to use the line index as the district index too.
			//
			//	Fill out the buffer.
			//
			buffer[ 1 ] = 'A' + line;
			switch( districts.state( line )) {
				case District::state_on: {
					if( !backfill_int_to_text( buffer+2, LCD_DISPLAY_STATUS_WIDTH-3, (int)districts.load_average( line ))) {
						memset( buffer+2, HASH, LCD_DISPLAY_STATUS_WIDTH-3 );
					}
					buffer[ LCD_DISPLAY_STATUS_WIDTH-1 ] = '%';
					break;
				}
				case District::state_off: {
					memset( buffer+2, '_', LCD_DISPLAY_STATUS_WIDTH-2 );
					break;
				}
				case District::state_shorted:
				case District::state_inverted: {
					memset( buffer+2, '*', LCD_DISPLAY_STATUS_WIDTH-2 );
					break;
				}
				case District::state_paused: {
					memset( buffer+2, SPACE, LCD_DISPLAY_STATUS_WIDTH-2 );
					break;
				}
				default: {
					memset( buffer+2, '?', LCD_DISPLAY_STATUS_WIDTH-2 );
					break;
				}
			}
			_display.set_posn( line, LCD_DISPLAY_STATUS_COLUMN );
			_display.write_buf( buffer, LCD_DISPLAY_STATUS_WIDTH );
			break;
		}
		case 2: {
			//
			//	Row 1, Active (P)ower/Zone and (F)ree bit buffers
			//
			buffer[ 1 ] = 'P';
			buffer[ 2 ] = '0' + districts.zone();
			buffer[ 3 ] = 'F';
			if( !backfill_byte_to_text( buffer+4, LCD_DISPLAY_STATUS_WIDTH-4, (int)dcc_generator.free_buffers())) {
				memset( buffer+4, HASH, LCD_DISPLAY_STATUS_WIDTH-4 );
			}
			_display.set_posn( 2, LCD_DISPLAY_STATUS_COLUMN );
			_display.write_buf( buffer, LCD_DISPLAY_STATUS_WIDTH );
			break;
		}
		case 3: {
			static byte	opt = 0;

			//
			//	Row 3, DCC packets (T)ransmitted sent per second
			//
			//	Rotate between the set of options this line shows.
			//
			if(( ++opt > 1 )) opt = 0;
			switch( opt ) {
				case 0: {
					buffer[ 1 ] = 'T';
					if( !backfill_int_to_text( buffer+2, LCD_DISPLAY_STATUS_WIDTH-2, stats.packets_sent())) {
						memset( buffer+2, HASH, LCD_DISPLAY_STATUS_WIDTH-2 );
					}
					break;
				}
				case 1: {
					buffer[ 1 ] = 'M';
					if( !backfill_int_to_text( buffer+2, LCD_DISPLAY_STATUS_WIDTH-2, heap.free_memory())) {
						memset( buffer+2, HASH, LCD_DISPLAY_STATUS_WIDTH-2 );
					}
					break;
				}
			}
			//
			//	Place the data.
			//
			_display.set_posn( 3, LCD_DISPLAY_STATUS_COLUMN );
			_display.write_buf( buffer, LCD_DISPLAY_STATUS_WIDTH );
			break;
		}
		default: {
			//
			//	Should never get here.
			//
			break;
		}
	}
}

void HCI::update_dcc_status( void ) {

	STACK_TRACE( "void HCI::update_dcc_status( void )" );

	//
	//	Just push out all lines.
	//
	for( byte l = 0; l < LCD_DISPLAY_ROWS; update_dcc_status_line( l++ ));
}


//
//	User Interface Input Processing
//	===============================
//
//	These following routines handle the inputs from the user and
//	call up systems within the firmware to execute the requests
//	implied by the users input.
//

//
//	The inputs to the user interface arrive in these routines:
//

void HCI::process_menu_option( byte m ) {

	STACK_TRACE( "void HCI::process_menu_option( byte m )" );

	switch( progmem_read_byte( _this_menu->item[ m ].action )) {
		case ACTION_NEW_MOBILE: {
			//
			//	Set up for entering a new mobile address
			//
			_input_mode = true;
			_input_mobile = true;
			_this_object->adrs = 0;
			_this_object->state = 0;
			redraw_page_line( _this_object_line );
			break;
		}
		case ACTION_NEW_STATIC: {
			//
			//	Set up for entering a new accessory address
			//
			_input_mode = true;
			_input_mobile = false;
			_this_object->adrs = 0;
			_this_object->state = 0;
			redraw_page_line( _this_object_line );
			break;
		}
		case ACTION_ERASE: {
			//
			//	Delete the current object
			//
			_this_object->adrs = 0;
			_this_object->state = 0;
			redraw_page_line( _this_object_line );
			break;
		}
		case ACTION_NEXT: {
			//
			//	Move to the next menu.
			//
			if(( _this_menu_index += 1 ) >= MENU_COUNT ) _this_menu_index = 0;
			_this_menu = &( menus.page[ _this_menu_index ]);
			redraw_menu_area();
			break;
		}
		case ACTION_SAVE: {
			//
			//	Save the current state of the controller.
			//
			record_constants();
			break;
		}
		case ACTION_STOP: {
			//
			//	Power OFF
			//
			districts.power( 0 );
			break;
		}
		case ACTION_START: {
			//
			//	Power ON
			//
			districts.power( 1 );
			break;
		}
		case ACTION_TOGGLE: {
			//
			//	Toggle Power OF->ON->OFF etc
			//
			switch( districts.zone()) {
				case 0: {
					//
					//	All zones off - light up zone 1.
					//
					districts.power( 1 );
					break;
				}
				case 1: {
					//
					//	Main zone one - turn zones off.
					//
					districts.power( 0 );
					break;
				}
				default: {
					//
					//	If the zone if neither 0 or 1 then
					//	we will assume the controller is
					//	being computer controlled and something
					//	else is going on so we leave well alone.
					//
					break;
				}
			}
			break;
		}
		case ACTION_STATUS: {
			//
			//	Toggle the display of the status information.
			//
			if(( _display_status ^= true )) {
				update_dcc_status();
			}
			else {
				redraw_object_area();
			}
			break;
		}
		default: {
			//
			//	Ignore anything we do not understand.
			//	This should be an error really.
			//
			break;
		}
	}
}

void HCI::user_key_event( bool down, char key ) {

	STACK_TRACE( "void HCI::user_key_event( bool down, char key )" );

	TRACE_HCI( console.print( F( "HCI key " )));
	TRACE_HCI( console.print( key ));
	TRACE_HCI( console.print( F( " down " )));
	TRACE_HCI( console.println( down ));

	byte	i, j;

	//
	//	PAGE Shift key?
	//
	if( key == LAYOUT_PAGE_SHIFT ) {
		//
		//	Force reset of input mode.
		//
		_input_mode = false;
		
		//
		//	Modify the page shift status.
		//
		if(( _page_shift = down )) {
			//
			//	Is this a power short cut?
			//
			if( _menu_shift ) districts.power( 0 );
		}
		return;
	}

	//
	//	MENU Shift key?
	//
	if( key == LAYOUT_MENU_SHIFT ) {
		//
		//	Force reset of input mode.
		//
		if( down ) _input_mode = false;
		
		//
		//	Modify the menu shift status.
		//
		if(( _menu_shift = down )) {
			//
			//	Is this a power short cut?
			//
			if( _page_shift ) districts.power( 0 );
		}
		return;
	}

	//
	//	If both shift keys are down we ignore everything.
	//
	if( _menu_shift && _page_shift ) return;

	//
	//	Alphabetic LETTER
	//
	if( LAYOUT_IS_LETTER( key )) {
		//
		//	Force reset of input mode.
		//
		_input_mode = false;
		
		//
		//	We are only concerned if the key pressed has
		//	been released.
		//
		if( down ) return;
		
		//
		//	A letter has been pressed.  What we do depends on if
		//	there is a shift key down.
		//
		//	Convert letter to index (0..N-1)
		//
		i = LAYOUT_LETTER_INDEX( key );
		
		//
		//	deal with shifted options first.
		//
		if( _menu_shift ) {
			//
			//	Select a menu option.
			//
			process_menu_option( i );
		}
		else if( _page_shift ) {
			//
			//	Select a specific page.
			//
			_this_page_index = i;
			_this_page = &( PAGE_MEMORY.page[ _this_page_index ]);
			_this_object_line = 0;
			_this_object = &( _this_page->object[ _this_object_line ]);
			redraw_page_area();
			if( !_display_status ) redraw_object_area();
		}
		else {
			//
			//	Select an object on the page.
			//
			j = _this_object_line;
			_this_object_line = i;
			_this_object = &( _this_page->object[ _this_object_line ]);
			redraw_page_line( j );
			redraw_page_line( i );
			if( !_display_status ) redraw_object_area();
		}
		return;
	}

	//
	//	Pretty sure this test is actually redundant.
	//
	if( !LAYOUT_IS_NUMBER( key )) return;

	//
	//	For the moment we are only interested when the numeric
	//	key is being released.
	//
	if( down ) return;
	
	//
	//	Get the key number index.
	//
	i = LAYOUT_NUMBER_INDEX( key );
	
	//
	//	Are we in input mode?
	//
	if( _input_mode ) {
		//
		//	Adjusting an object address.
		//
		if( _input_mobile ) {
			//
			//	Modify a mobile address
			//
			word	a;
			
			a = _this_object->adrs * 10 + i;
			if( a <= DCC_Constant::maximum_address ) _this_object->adrs = a;
		}
		else {
			//
			//	Modify an accessory address
			//
			word	a;
			
			a = (-_this_object->adrs) * 10 + i;
			if( a <= DCC_Constant::maximum_ext_address ) _this_object->adrs = -a;
		}
		redraw_page_line( _this_object_line );
	}
	else {
		//
		//	So we are in FUNCTION KEY mode.
		//
		//	How we handle this depends on the type of object.
		//
		if( _this_object->adrs > 0 ) {
			//
			//	Mobile Decoder
			//
			//	Adjust the decoder function number.
			//
			if( _menu_shift ) i += 10;
			if( _page_shift ) i += 20;

			//
			//	Ignore any combination that gives an
			//	invalid function key number.
			//
			if( i > DCC_Constant::maximum_func_number ) return;
			
			//
			//	Get function status and toggle it.
			//
			j = function_cache.get( _this_object->adrs, i, 1 ) ^ 1;
			
			//
			//	Apply and update status page (optionally).
			//
			if( dcc_generator.function_command( _this_object->adrs, i, j )) {
				//
				//	Update display, if appropriate.
				//
				if( !_display_status ) redraw_object_area();
			}
		}
		else if( _this_object->adrs < 0 ) {
			bool	s;

			//
			//	Accessory Decoder.
			//
			//	Just use the lower bit to turn the
			//	accessory on or off.
			//
			s = i & 1;
			if( dcc_generator.accessory_command( -_this_object->adrs,( s? DCC_Constant::accessory_on: DCC_Constant::accessory_off ))) {
				_this_object->state = accessory_state( s );
				redraw_page_line( _this_object_line );
			}
		}
	}
}

//
//	Respond to the button (on the rotary knob) being pushed.
//
//	For mobile decoders pushing reverses the direction.
//
//	For accessories it reverses the position.
//
void HCI::user_button_pressed( UNUSED( word duration )) {

	STACK_TRACE( "void HCI::user_button_pressed( UNUSED( word duration ))" );

	//
	//	If we are in input mode we do nothing.
	//
	if( _input_mode ) return;
	
	//
	//	Now act accordingly.
	//
	if( _this_object->adrs > 0 ) {
		byte	s;
		bool	d;

		TRACE_HCI( console.print( F( "HCI rev mob " )));
		TRACE_HCI( console.println( _this_object->adrs ));
		
		//
		//	Mobile decoder, will be simple; change direction
		//	bit and re-apply the speed to the DCC generator.
		//
		if( speed_dir_valid( _this_object->state )) {
			d = !read_direction( _this_object->state );
			s = read_speed( _this_object->state );
		}
		else {
			d = true;
			s = 0;
		}
		
		//
		//	Initiate DCC command.
		//
		if( dcc_generator.mobile_command( _this_object->adrs, s, ( d? DCC_Constant::direction_forwards: DCC_Constant::direction_backwards ))) {
			_this_object->state = speed_dir_state( s, d );
			redraw_page_line( _this_object_line );
		}
	}
	else if( _this_object->adrs < 0 ) {
		bool	s;
		
		TRACE_HCI( console.print( F( "HCI rev acc " )));
		TRACE_HCI( console.println( -_this_object->adrs ));
		
		//
		//	Accessory needs flipping.
		//
		if( accessory_valid( _this_object->state )) {
			s = !read_accessory( _this_object->state );
		}
		else {
			s = false;
		}
		//
		//	Send out the command..
		//
		if( dcc_generator.accessory_command( -_this_object->adrs,( s? DCC_Constant::accessory_on: DCC_Constant::accessory_off ))) {
			_this_object->state = accessory_state( s );
			redraw_page_line( _this_object_line );
		}
	}
}

//
//	The rotary control has been turned, is there
//	anything this needs to do.
//
//	For the moment the rotary control turning motion
//	is only applicable to mobile decoders and their speed.
//
void HCI::user_rotary_movement( sbyte change ) {

	STACK_TRACE( "void HCI::user_rotary_movement( sbyte change )" );

	TRACE_HCI( console.print( F( "HCI rotary " )));
	TRACE_HCI( console.println( change ));

	//
	//	If we are in input mode we do nothing.
	//
	//	JEFF	We *could* use the rotary control to adjust the
	//		selected number up or down .. possibly?
	//
	if( _input_mode ) return;

	//
	//	Now act accordingly.
	//
	if( _this_object->adrs > 0 ) {
		byte	s, t;
		bool	d;

		TRACE_HCI( console.println( F( "HCI mobile" )));

		//
		//	We have a mobile decoder, huzzar!
		//
		if( speed_dir_valid( _this_object->state )) {
			s = read_speed( _this_object->state );
			d = read_direction( _this_object->state );
		}
		else {
			s = 0;
			d = true;
		}

		//
		//	Adjust the speed appropiately, and if changes
		//	are valid save the new speed and issue the DCC
		//	command.
		//
		t = s + change;

		//
		//	Correct any overflow from the calculation.  We are
		//	being a little cocky here; if the calculation
		//	under flows (i.e. speed reduced below 0) the unsigned
		//	type of t means this becomes a large positive
		//	so the correction we make is based on the direction
		//	the speed was adjusted.
		//
		//	Also note that, at this point in the code, we are
		//	dealing with the human understanding of speed
		//	values (0-126) and not the DCC command understanding
		//	of speed values (0,2-127).
		//
		if( t > maximum_speed ) t = ( change < 0 )? minimum_speed: maximum_speed;

		//
		//	Have we changed the speed?  If so then do it.
		//
		if( t != s ) {
			//
			//	Update the state value, then convert the
			//	target value to the DCC speed range (0,2-127).
			//
			s = t;
			if( t ) t++;

			//
			//	Initiate DCC command.
			//
			if( dcc_generator.mobile_command( _this_object->adrs, t, ( d? DCC_Constant::direction_forwards: DCC_Constant::direction_backwards ))) {
				_this_object->state = speed_dir_state( s, d );
				redraw_page_line( _this_object_line );
			}
		}
	}
}


//
//	Called to check keypad for input
//
void HCI::keypad_reader( void ) {

	STACK_TRACE( "void HCI::keypad_reader( void )" );

	byte	key;

	if(( key = _keypad.read())) {
		bool	down;

		//
		//	Something has changed .. decode and process.
		//
		if(( down = BOOL( key & Keypad::pressed ))) key &= ~Keypad::pressed;

		TRACE_HCI( console.print( F( "key " )));
		TRACE_HCI( console.print( (char)key ));
		TRACE_HCI( console.println( down? F( " down" ): F( " up" )));

		//
		//	... process
		//
		user_key_event( down, key );
	}
}

//
//	Called to process rotary actions.
//
void HCI::rotary_updater( void ) {

	STACK_TRACE( "void HCI::rotary_updater( void )" );

	word	d;
	sbyte	m;
	
	if(( d = _dial.pressed())) user_button_pressed( d );
	if(( m = _dial.movement())) user_rotary_movement( m );
}

void HCI::process( byte handle ) {

	STACK_TRACE( "void HCI::process( byte handle )" );

	switch( handle ) {
		case rotary_handle: {
			rotary_updater();
			break;
		}
		case keypad_handle: {
			keypad_reader();
			break;
		}
		case display_handle: {
			update_dcc_status_line( _display_line++ );
			if( _display_line >= LCD_DISPLAY_ROWS ) _display_line = 0;
			break;
		}
		default: {
			ABORT( PROGRAMMER_ERROR_ABORT );
			break;
		}
	}
}

//
//	Organise the HCI into action!
//
//	This still feels too complex but is better than it was before!
//
void HCI::initialise( void ) {

	STACK_TRACE( "void HCI::initialise( void )" );

	TRACE_HCI( console.print( F( "HCI display flag " )));
	TRACE_HCI( console.println( _display_flag.identity()));
	TRACE_HCI( console.print( F( "HCI keypad flag " )));
	TRACE_HCI( console.println( _keypad_flag.identity()));
	TRACE_HCI( console.print( F( "HCI rotary flag " )));
	TRACE_HCI( console.println( _rotary_flag.identity()));

	//
	//	Set up all the elements that form to HCI control.
	//
	_lcd.initialise( LCD_DISPLAY_ADRS, LCD_DISPLAY_ROWS, LCD_DISPLAY_COLS );
	_display.initialise( &_lcd );
	_dial.initialise( ROTARY_A, ROTARY_B, ROTARY_BUTTON );
	_keypad.initialise( KEYPAD_ADDRESS );

	//
	//	Display the banner screen on the LCD.
	//
	framebuffer_banner( &_display );
	time_of_day.inline_delay( BANNER_DISPLAY_TIME );
	_display.clear();
	
	//
	//	Set the pointers to their start conditions.
	//
	_this_page_index = 0;
	_this_page = &( PAGE_MEMORY.page[ _this_page_index ]);
	_this_object_line = 0;
	_this_object = &( _this_page->object[ _this_object_line ]);
	_this_menu_index = 0;
	_this_menu = &( menus.page[ _this_menu_index ]);
	
	//
	//	Reset all of the defined object states to 0.  Performed when the
	//	firmware starts as the "saved sate" (brought in from EEPROM) will
	//	have non-zero states embedded in it.
	//
	for( byte p = 0; p < PAGE_COUNT; p++ ) {
		for( byte o = 0; p < OBJECT_COUNT; p++ ) {
			PAGE_MEMORY.page[ p ].object[ o ].state = 0;
		}
	}
	
	//
	//	Redraw everything!
	//
	redraw_menu_area();
	redraw_page_area();
	
	//
	//	Now kick off the events processing.
	//
	_display_line = 0;
	if( !event_timer.delay_event( MSECS( LINE_REFRESH_INTERVAL ), &_display_flag, true )) ABORT( EVENT_TIMER_QUEUE_FULL );
	if( !task_manager.add_task( this, &_display_flag, display_handle )) ABORT( TASK_MANAGER_QUEUE_FULL );

	//
	//	Initialise the keypad scanning code.
	//
	if( !event_timer.delay_event( MSECS( KEYPAD_READING_INTERVAL ), &_keypad_flag, true )) ABORT( EVENT_TIMER_QUEUE_FULL );
	if( !task_manager.add_task( this, &_keypad_flag, keypad_handle )) ABORT( TASK_MANAGER_QUEUE_FULL );

	//
	//	Initialise the rotary dial control.
	//
	if( !event_timer.delay_event( MSECS( ROTARY_UPDATE_PERIOD ), &_rotary_flag, true )) ABORT( EVENT_TIMER_QUEUE_FULL );
	if( !task_manager.add_task( this, &_rotary_flag, rotary_handle )) ABORT( TASK_MANAGER_QUEUE_FULL );
}


//
//	Declare the HCI object itself.
//
HCI hci_control;


//
//	EOF
//
