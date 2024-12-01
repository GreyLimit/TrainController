///
///	ArduinoTrainController: A DCC Model Railway Controller.
///
/// 	Firmware for an Arduino Uno R3 and Motor shield (with other
///	components) to create a simple and free standing "Train
///	Controller" to be as simple as possible.
///
///	Copyright (C) 2020, Jeff Penfold, jeff.penfold@googlemail.com
///	
///	This program is free software: you can redistribute it and/or modify
///	it under the terms of the GNU General Public License as published by
///	the Free Software Foundation, either version 3 of the License, or
///	(at your option) any later version.
///
///	This program is distributed in the hope that it will be useful,
///	but WITHOUT ANY WARRANTY; without even the implied warranty of
///	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
///	GNU General Public License for more details.
///
///	You should have received a copy of the GNU General Public License
///	along with this program.  If not, see <https://www.gnu.org/licenses/>.
///

//
//	Arduino Train Controller V0.2.2
//	-------------------------------
//
#define VERSION_NUMBER "0.2.2"

//
//	This version (skipping over V0.2.1) sees the start of the
//	expansion of this code towards the combination of this
//	"Grey Box" standalone DCC controller with the "Blue Box"
//	DCC Generator used with the Fat/Thin Controller software.
//
//	The following modifications are included in this version:
//
//	o	Replace use of millis() and micros() with a flag
//		based relative time system.
//
//	o	Introduction of a flag controller task management
//		system.
//
//	o	Conversion of the DCC generation code to a C++ class
//		permitting a more generic application of its code (in
//		future).
//
//	Lets be honest - this is a full rewrite of the whole firmware
//	as a move away from my earlier attempts to create something
//	resembling "real time" software.
//
//	This version is wholey different from the previous versions
//	containing systems for managing time in both "computer" and
//	"human" scales, a system for the causing "tasks" to be called
//	when and external (to that task) event raises a suitable flag.
//

//
//	Arduino Train Controller V0.2.0
//	-------------------------------
//

//
//	September 2024
//
//	Version raised to 0.2.0 to reflect status with firmware
//	exhibiting no known bugs and, within the initial parameters,
//	being fully functional.
//
//	The subsequent version 0.2.X will be associated with development
//	for support of PoM actions (Program On Main) that will
//	facilitate *most* of the actions which a true programming track
//	permits.  However, this firmware will still not support a
//	dedicated "Programming Track".
//

//
//	Arduino Train Controller V0.1.7
//	-------------------------------
//
//	September 2024
//
//	Debugging the LCD driver code; specifically the "screen
//	buffer" code where a very specific "edge case" literally
//	put data across the screen.
//

//
//	Arduino Train Controller V0.1.6
//	-------------------------------
//
//
//	August 2024
//
//	Redesign and reorganise the display to facilitate the ability
//	to display "per object" extended information.  This is primarily
//	aimed at the "Cab" object where the need to display function
//	status information is required.
//

//
//	Arduino Train Controller V0.1.5
//	-------------------------------
//
//	August 2024
//
//	Still delaying the move to version 0.2.  This continues to be
//	effectively and "alpha" version.
//
//	Consolidate the firmware configuration aspects of the source
//	code - creation of the "Configuration.h" file.
//
//	Revise the design of the menu presentation - reduce with to
//	create a wider area for the controlled objects display.
//
//	Debug display "artifacts" that seem to be "overflowing" from
//	the objects area into the status area.
//

//
//	Arduino Train Controller V0.1.4
//	-------------------------------
//
//	August 2024
//
//	Redesign the status area of the LCD to remove the uptime component
//	and expand load display to two lines one for each district.
//

//
//	Arduino Train Controller V0.1.3
//	-------------------------------
//
//	August 2024
//
//	Version 0.1.2 was short lived, and was primary created to
//	cover the expansion of the firmware to incorperate the
//	linkage between the user interface and the under lying
//	DCC generator code.
//
//	This has now been, initially, completed with the first
//	DCC related action being initiated from the keypad (power
//	off).  Other basic actions have been coded but cannot be
//	tested.
//
//	This versions main task is the creation of the menu execution
//	routines and, most especially, the creation of the 'number
//	input' mode to enable the creation of mobile and static decoder
//	objects.
//

//
//	Arduino Train Controller V0.1.2
//	-------------------------------
//
//	August 2024
//
//	Physical HCI devices now coded and (so far) appear to work
//	reliably.  The menus and object page coding is largely
//	completed.
//
//	This version marks the start of integrating the HCI with the
//	under lying DCC generation code.  Some reorganisation of the
//	DCC code is required to enable the injection of DCC commands
//	"from the side" rather than the USB connection.
//

//
//	Arduino Train Controller V0.1.1
//	-------------------------------
//
//
//	Key physical HCI devices are now supported:
//
//	o	Rotary control
//	o	4x4 Keypad
//	o	20x4 LCD Display
//
//	The objective of this version is to establish the HCIs visual
//	and functional elements with the aim of creating the simplest
//	interaction possible while still allowing a wide range of
//	actions to be available.
//


//
//	Arduino Train Controller V0.1
//	-----------------------------
//
//	This initial version of the Train Controller takes as its base
//	the latest (as of July 2024) version of the Arduino DCC Generator
//	firmware and make the following changes:
//
//	Simplification:
//	o	The options for differrent forms of support hardware
//		are reduced to just the Arduino Uno (and by binference
//		the Nano) and the Arduino Mega2560.
//	o	The "DCC Board" is now *only* the motor driver shield.
//		The bespoke 6 district control board has been dropped.
//	o	Only a single district is supported, though this will
//		naturally support multiple engines (as per normal DCC
//		functionality).
//	o	DCC programming track support to be removed.
//
//	Extensions:
//	o	A "human interface" will be incorperated into the firmware
//		to enable the operator to interact with the firmware
//		directly:
//
//		.	A rotary knob with turn for speed and click for
//			direction control
//
//		.	A joystick up/down/left/right/click for menu
//			navigation
//
//		.	A keypad for numerical data entry
//
//	o	Restore the DCC++ serial interface (simplied) to optionally
//		restore the interface to JMRI running on a USB attached PC
//
//	The above changes will not all happen in Version 0.1.
//

//////////////////////////////////////////////////
//						//
//	Previously in "Arduino DCC Generator"	//
//	=====================================	//
//						//
//////////////////////////////////////////////////

//
//
//	Arduino DCC Generator V1.5.1
//	----------------------------
//
//	Simplify the LCD display to make 'District Power' section
//	more meaningful.
//
//	The 'Bar Chart' output by the districy letters is simply
//	too coarse to provide meaningful information.  While being
//	left in the code (selectable with a compile time flag) the
//	output will be changed to a two digit in the range 00 to 99
//	which will represent the percentage load (of maximum) being
//	experienced.
//
//	Simpler, more accurate and more informative.
//
//
//	Arduino DCC Generator V1.5
//	--------------------------
//
//	Fix the error in the algorithm that converts the logical
//	accessory addresses into the main address and sub address
//	pair.  The error was cause by initially asserting that the
//	accessory address zero (0) was the first usable address,
//	and so the actual accessory address 0:0 (main address, sub
//	address) was mapped to logical address 1.
//
//	Purchase of the ECU ECoS Command Station has revealed that
//	this assumption (through seemingly correct) is not how the
//	industry views things, where the accessory address 1:0 is
//	mapped to logical address 1.
//
//	Arduino DCC Generator V1.4.3
//	----------------------------
//
//	Added in the 'stationary' symbol to the LCD status screen
//	so that engines doing speed '0' have their direction shown
//	as '=' in keeping with the FatControllers displays.
//
//	Arduino DCC Generator V1.4.2
//	----------------------------
//
//	Extend symbolic CV programming table to include
//
//	Arduino DCC Generator V1.4.1
//	----------------------------
//
//	Extended the timing in the "Logical CV" modification code
//	to allow time for any reply indication to take place before
//	the next command is sent.
//
//	Arduino DCC Generator V1.4
//	--------------------------
//
//	Functional extensions for this version:
//
//	December 2023
//
//		Added new "{...}" protocol to support the configuration
//		of DCC decoders using logical parameter names rather than
//		explicit direct CV updates.
//
//		This change has been brought in to make it simpler to
//		configure decoders with just the DCC Generator at hand
//		and not require an additional host software package to
//		achieve this.
//
//		This is not to say that this system will provide an entire
//		range of abilities in this field, but will ensure that
//		basic programming can be more easily achieved.
//
//		Removed March 2023 'doubling up' code when realising that
//		the code *already* repeats commands multiple times, by
//		default so the March 2023 change was pointless and stupid.
//
//	Arduino DCC Generator V1.3.3
//	----------------------------
//
//	Modifications for this version:
//
//	December 2023
//
//		Added the "Write State" to mobile decoder command.  See
//		the 'W' command for details.
//
//		Removed all pretense of needing to provide DCC++
//		compatibility.  Who was I kidding?
//

//
//	Arduino DCC Generator V1.3.3
//	----------------------------
//
//	March 2023
//
//		Flexible constant handling using the EEPROM to keep
//		'configured' constants.  This has required a migration
//		of constants towards the top of the source file so that
//		the default values are easily tuned.
//
//		Constant configuration accessed and modified via an
//		extension to the communications protocol (see the
//		'Q' command).
//
//	August 2023
//		
//		Added adjustable constant value to permit control of
//		the dynamic load reporting (constant name "dynamic_load
//		_updates).  Primarily implement to facilitate stopping
//		stream of output data to simplify debugging.
//

//
//	Arduino DCC Generator V1.3.2
//	----------------------------
//
//	March 2023
//
//	Included the set CV bit command '[U cv bnum value]' to the system
//	and also doubled up the command *inside* the command transmission
//	as there is some evidence to suggest that duplicate sequential
//	commands are required before the decode is obliged to reply.
//

//
//	Arduino DCC Generator V1.3.1
//	----------------------------
//
//	March 2023
//
//	Incorporated improved confirmation detections code which (largely)
//	reduces the averaging codes impact in the detection process.
//

//
//
//	Arduino DCC Generator V1.3
//	--------------------------
//
//	Feb 2023
//
//	This version sees a rewrite of the "programming confirmation
//	mechanism" and an extension of the mechanism that translates
//	DCC byte encoded commands into bit streams as required by the
//	generator code in the interrupt routine.
//
//	Changes in 1.3:
//
//	o	Source code now starting to be placed into separate
//		modules.
//
//	o	System Serial Library replaced with own configurable
//		USART interrupt driven solution.
//
//	o	The "long preamble" flag has been replaced in the DCC
//		command processing (the conversion from byte codes to
//		bit streams) with "preamble bits" and "postamble bits"
//		values.  This change has enabled the CV programming
//		code to introduce a meaningful delay after a specific
//		DCC command in which any reply from the decoder is more
//		readily and reliably determined.
//
//	All-in-all the change in version number to 1.3 is justified as
//	these changes are significant and affect a number of systems in
//	the firmware. 
//
//	Arduino DCC Generator V1.2
//	--------------------------
//
//	Feb 2023
//
//	There has been no real version tracking within this sketch for
//	no honestly good reason.  Since version 0.x and through versions
//	1.0 and 1.2 there have been many changes.  This version should
//	probably be called version 1.5 or 1.6, but isn't.  This source
//	code is in retirement phase awaiting replacement with the DCC
//	generator V2.1 (why no V2.0, because that approach was also set
//	to one side).
//
//	Anyway..
//


//
//	Arduino Generator
//	=================
//
//	A sketch for generating the DCC signal through
//	an Arduino motor shield.
//
//	Minimum hardware required:
//
//		Arduino UNO R3 (or Mega 2560)
//		Arduino Motor Shield or bespoke Driver board
//		15 volt (max) DC power supply
//


//
//	Include Local and System Libraries
//	==================================
//

//
//	Bring in the firmware configuration with respect to the
//	hardware being targetted.
//
#include "Environment.h"
#include "Configuration.h"
#include "Parameters.h"
#include "Trace.h"

//
//	A little math support.
//
#include "mul_div.h"

//
//	Include firmware modules.
//
#include "Code_Assurance.h"
#include "Errors.h"
#include "Constants.h"
#include "USART.h"
#include "TWI.h"
#include "LCD.h"
#include "Layout.h"
#include "Keypad.h"
#include "Rotary.h"
#include "Menu.h"
#include "Console.h"
#include "DCC.h"
#include "Function.h"
#include "Buffer.h"
#include "Formatting.h"
#include "Protocol.h"

//
//	The ROTARY device
//	=================
//
static Rotary		speed_dial( ROTARY_A, ROTARY_B, ROTARY_BUTTON );

//
//	The KEYPAD device.
//	==================
//
static Keypad		keypad;


//
//	Liquid Crystal Display
//	======================
//

//
//	Define a set of single character symbols to represent
//	actions/directions applied to decoders/accessories when
//	displayed on the LCD.
//
#define LCD_ACTION_FORWARD	'>'
#define LCD_ACTION_BACKWARDS	'<'
#define LCD_ACTION_STATIONARY	'='
#define LCD_ACTION_ENABLE	'^'
#define LCD_ACTION_DISABLE	'v'
#define LCD_ACTION_TOGGLE	'~'

//
//	Finally, on LCDs..
//
//	From a wiring perspective (apart from +5V and Ground) the
//	display is attached to the Arduino Uno* via the pass-through pins
//	D18/A4 and D19/A5 on the motor shield.  These equate to
//	the I2C/TWI interface pins SDA (D18) and SCL (D19).  These will
//	be different on other platforms.
//
//	To keep memory allocation to a minimum (at least on the basic
//	Arduino UNO) these "Lite" versions of the Wire, TWI and LCD_I2C
//	libraries are required (found with this code).
//
//	*/ Arduino Mega uses different pins: D20/SDA and D21/SCL.  These
//	are outside the footprint of a standard motor shield and need to
//	picked up directly from the Mega itself.
//

//
//	Boot Splash Screen
//	==================
//
//	Do we want a splash screen on power up and
//	what should it say?
//
//	Define SPLASH_ENABLE to include splash code,
//	and SPLASH_LINE_1, _2, _3, _4 (as appropriate
//	for the target display) as text string to
//	display on boot.  Define SPLASH_WAIT as a
//	pause period (ms) before rolling on to the
//	the firmware.
//
#define SPLASH_ENABLE
#define SPLASH_LINE_1	"Train Controller " VERSION_NUMBER
#define SPLASH_LINE_2	"MCU: " HW_TITLE
#define SPLASH_LINE_3	"Baud: " SERIAL_BAUD_RATE_STR
#define SPLASH_LINE_4	"Build: " __DATE__
#define SPLASH_WAIT	3000

//
//	Serial Host Connectivity
//	========================
//

//
//	Define the Speed and buffer size used when accessing the
//	serial port.
//
//	Example baud rates:
//
//		9600 14400 19200 38400 57600 115200
//
//	The new USART module uses specific constant values for
//	the various supported baud rates.  These are all preceeded
//	with a 'B'.
//
#ifdef SYNCHRONOUS_BUFFER
#define SERIAL_BAUD_RATE	B115200
#define SERIAL_BAUD_RATE_STR	"115200"
#else
#define SERIAL_BAUD_RATE	B38400
#define SERIAL_BAUD_RATE_STR	"38400"
#endif


//
//	LCD structure
//	-------------
//

//
//	Create the LCD interface object.
//
static LCD_TWI_IO lcd( LCD_DISPLAY_ADRS, LCD_DISPLAY_COLS, LCD_DISPLAY_ROWS );

//
//	Allocate a static frame buffer
//
static byte lcd_frame_buffer[ LCD_FRAME_BUFFER ];



//
//	Error Reporting Code.
//	---------------------
//
//	Define a mechanism for the firmware to collate reports
//	of detected errors and forward them back to the host
//	computers as a suitable opportunity arises.
//

//
//	Define size of the buffer where the returned error messages
//	are constructed.
//
#define ERROR_OUTPUT_BUFFER 32

//
//	Flush the errors routine.
//
static void flush_error_queue( void ) {
	word	error, arg;
	byte	repeats;
	
	//
	//	Flush a single error to the output queue
	//	if there is an error pending and there is
	//	enough space in the output queue.
	//
	//	The error message is in the form:
	//
	//		"[ENN AAAAA]\n"
	//
	//	which should always be shorter than
	//	ERROR_OUTPUT_BUFFER characters.
	//
	if( errors.peek_error( &error, &arg, &repeats )) {

		Buffer<ERROR_OUTPUT_BUFFER>	buffer;
	
		//
		//	Build error report, and send it only if there
		//	is enough space.
		//
		if( buffer.format( Protocol::error, error, arg )) {
			//
			//	Send if space is available.
			//
			//	We DO NOT call the console routine and use its
			//	successful result (true) to optionally remove
			//	the error from the queue because this could flood
			//	the console buffer with partial errors, endlessly.
			//
			//	This approach, while potentially meaning an error
			//	does not get out of the firmware, does avoid the
			//	creation of a cyclic loop that would keep the
			//	console output buffer permanently full.
			//
			if( console.space() >= buffer.size()) {
				FIRMWARE_OUTPUT( console.print( buffer.buffer()));
				errors.drop_error();
			}
		}
	}
}


//
//	In native mode we update the host computer with the district status
//	as changes occur.  This is the routine used to do that.
//
static void report_driver_status( void ) {
	//
	//	Actually only do this in Native mode.
	//
#define REPORT_DRIVER_BUFFER	(4 + SHIELD_OUTPUT_DRIVERS * 2)

	char	buffer[ REPORT_DRIVER_BUFFER ];
	int	v[ SHIELD_OUTPUT_DRIVERS ];

	for( byte i = 0; i < SHIELD_OUTPUT_DRIVERS; i++ ) {
		switch( output_load[ i ].status ) {
			case DRIVER_ON:
			case DRIVER_ON_GRACE: {
				v[ i ] = 1;
				break;
			}
			case DRIVER_FLIPPED: {
				v[ i ] = 2;
				break;
			}
			case DRIVER_BLOCKED: {
				v[ i ] = 3;
				break;
			}
			case DRIVER_OFF: {
				v[ i ] = 4;
				break;
			}
			default: {
				v[ i ] = 0;
				break;
			}
		}
	}
	if( reply_n( buffer, REPORT_DRIVER_BUFFER, 'D', SHIELD_OUTPUT_DRIVERS, v )) {
		if( !FIRMWARE_OUTPUT( console.print( buffer ))) errors.log_error( COMMAND_REPORT_FAIL, 'D' );
	}
}

//
//	This routine is called every time track electrical load data
//	becomes available.  The routine serves several purposes:
//
//		Monitor for overload and spike conditions
//
//		Manage the phase flipping protocol
//
//		Detect return signals from devices attached
//		to the DCC bus.
//
//	How this code operates is dependent on the status of driver, outlined
//	in the following explanation:
//
//	o	Each driver output is managed in one of five states:
//
//		ON		District supplying power, no issues
//		OFF		District enabled, but temporarily suspended
//		FLIPPED		System is trying phase flip approach to
//				address an issue
//		BLOCKED		System *wants* to flip this district, but another
//				district has this code locked out.
//		DISABLED	District is not scheduled to supply power.
//
//	o	Ignoring DISABLED, the following state change table indicates
//		an individual district should progress through the states
//		each time it has it power/load measured.  The intersection in
//		the table gives the new state and accompanying actions.
//
//						Load Condition
//				Retry		--------------
//		In State	Time	Normal		Overloaded	Spike
//		--------	-----	------		----------	-----
//
//		ON		n/a	ON		OFF+		FLIPPED+
//							Schedule	invert
//							retry		phasing
//
//		FLIPPED		OFF	ON		OFF+		OFF+
//							Schedule	Schedule
//							retry		retry
//
//		BLOCKED		OFF	ON		OFF+		FLIPPED+
//							Schedule	Schedule
//							retry		retry
//
//		OFF		ON	OFF		OFF		OFF
//				
//
//
static void monitor_current_load( int amps ) {
	DRIVER_LOAD	*dp;

	//
	//	Set shortcut pointer into the output_load[] array.
	//
	dp = &( output_load[ output_index ]);

	//
	//	Check status of the flip lock:
	//
	//	*	If flip_lock is the same as this record, then
	//		this record must be in FLIPPED state
	//
	//	*	If the flip_lock is different to the current
	//		record (and this includes being NULL) then
	//		this record must NOT be in FLIPPED state.
	//
	//	The net result of this test is that there should be
	//	only ONE record in FLIPPED state when flip_lock is
	//	not NULL, and that record must be the one flip_lock
	//	points to.
	//
	ASSERT((( flip_lock == dp )&&( dp->status == DRIVER_FLIPPED ))||(( flip_lock != dp )&&( dp->status != DRIVER_FLIPPED )));

	//
	//	The whole power management system has to be suspended
	//	for a period of time after a district has been switched
	//	on.  This is the "grace period" during which time the
	//	state of the district is "DRIVER_ON_GRACE".
	//
	if( dp->status != DRIVER_ON_GRACE ) {
		//
		//	Compound the new figure into the averages.
		//
		for( byte i = 0; i < COMPOUNDED_VALUES; i++ ) {
			amps = dp->compound_value[ i ] = ( amps + dp->compound_value[ i ]) >> 1;
		}
		//
		//	The code in the remainder of this routine needs to cover
		//	all of the cases outlined in the table described above.
		//
		//	Firstly, an instantaneous short circuit?
		//
		//	This covers off all of the rows under the column "Spike".
		// 
		if( dp->compound_value[ SPIKE_AVERAGE_VALUE ] > INSTANT_CURRENT_LIMIT ) {
			//
			//	How we handle this is dependent on what
			//	has happened before.
			//
			switch( dp->status ) {
				case DRIVER_ON: {
					//
					//	This is the first time this district has seen
					//	a power spike.  What we do here is dependent
					//	on the state of the flip lock.
					//
					//	If the flip lock is taken, we need to start a
					//	controlled period of waiting before we simply
					//	close down the district.
					//
					//	If it is clear then we can head into the trying
					//	to invert the phase of this output before doing
					//	anything more drastic (like closing down the
					//	district).
					//
					if( flip_lock ) {
						//
						//	Some other district is already "flipping"
						//
						//	Put this district into BLOCKED state and
						//	set a short timeout before we set the
						//	district to OFF.
						//
						dp->status = DRIVER_BLOCKED;
						dp->recheck = millis() + DRIVER_PHASE_PERIOD;
					}
					else {
						//
						//	Nobody has the flip lock, so this district
						//	can initiate the phase flipping logic.
						//
						//	For an Arduino motor shield solution we simply invert the
						//	corresponding output_phase[] value.
						//
						output_phase[ output_index ] ^= true;
						//
						//	Lock the flip code and note change of state.
						//
						flip_lock = dp;
						dp->status = DRIVER_FLIPPED;
						dp->recheck = millis() + DRIVER_PHASE_PERIOD;
					}
					//
					//	Now, log an error to give the reason for the
					//	change in status.
					//
					errors.log_error( POWER_SPIKE, output_index );
					break;
				}
				case DRIVER_FLIPPED: {
					//
					//	If we get here then the power condition has persisted, despite
					//	this district having had the phase flipped.
					//
					//	What we do depends on the time.
					//
					//	If the DRIVER_PHASE_PERIOD has elapsed since this district went
					//	into FLIPPED state, then we must shutdown this district.
					//
					if( millis() > dp->recheck ) {
						//
						//	This has failed long enough.  Turn it OFF.
						//
						digitalWrite( progmem_read_byte( shield_output[ output_index ].enable ), LOW );
						//
						//	We flatten the power averaging data to simplify
						//	power up restarting and schedule the restart.
						//
						for( byte i = 0; i < COMPOUNDED_VALUES; dp->compound_value[ i++ ] = 0 );
						//
						//	Clear the flip lock.
						//
						ASSERT( flip_lock == dp );
						flip_lock = NULL;
						//
						//	Note changes in status and schedule re checking status.
						//
						dp->status = DRIVER_OFF;
						dp->recheck = millis() + DRIVER_RESET_PERIOD;
						//
						//	Report new driver state
						//
						report_driver_status();
					}
					break;
				}
				case DRIVER_BLOCKED: {
					//
					//	We have got here because the power condition has persisted
					//	despite some district (other than this one) going through a
					//	process of flipping its phase.
					//
					//	First we test to see if the flip_lock has become available.
					//
					if( flip_lock ) {
						//
						//	Flip lock still held.  Is it time to stop waiting?
						//
						if( millis() > dp->recheck ) {
							//
							//	This has failed long enough so we turn it OFF.
							//
							digitalWrite( progmem_read_byte( shield_output[ output_index ].enable ), LOW );
							//
							//	We flatten the power averaging data to simplify
							//	power up restarting and schedule the restart.
							//
							for( byte i = 0; i < COMPOUNDED_VALUES; dp->compound_value[ i++ ] = 0 );
							//
							//	Note changes in status and schedule re checking status.
							//
							dp->status = DRIVER_OFF;
							dp->recheck = millis() + DRIVER_RESET_PERIOD;
							//
							//	Report change in status
							//
							report_driver_status();
						}
					}
					else {
						//
						//	We can phase flip because the flip lock has become free.
						//
						//	For an Arduino motor shield solution we simply invert the
						//	corresponding output_phase[] value.
						//
						output_phase[ output_index ] ^= true;
						//
						//	Lock the flip code and note change of state but
						//	do not reset the recheck time.  Any time lost
						//	in BLOCKED state is lost.
						//
						flip_lock = dp;
						dp->status = DRIVER_FLIPPED;
						//
						//	Report new driver state
						//
						report_driver_status();
					}
					break;
				}
				default: {
					//
					//	Turn off the output, but do not
					//	change the state information.
					//
					digitalWrite( progmem_read_byte( shield_output[ output_index ].enable ), LOW );
					break;
				}
			}
		}
		else {
			//
			//	Secondly, basic overload?
			//
			//	This covers off all of the rows under the column "Overloaded".
			//
			if( dp->compound_value[ COMPOUNDED_VALUES - 1 ] >  AVERAGE_CURRENT_LIMIT ) {
				//
				//	Cut the power here because there is some sort of long
				//	term higher power drain.
				//
				switch( dp->status ) {
					case DRIVER_DISABLED:
					case DRIVER_OFF: {
						//
						//	Turn off the output, but do not
						//	change the state information or
						//	any rechecking time which might
						//	have been set.
						//
						digitalWrite( progmem_read_byte( shield_output[ output_index ].enable ), LOW );
						break;
					}
					default: {
						//
						//	For all other states, we turn off the
						//	output, set a re-test time and clear the
						//	flip lock if necessary.
						//
						digitalWrite( progmem_read_byte( shield_output[ output_index ].enable ), LOW );
						//
						//	We flatten the power averaging data to simplify
						//	power up restarting and schedule the restart.
						//
						for( byte i = 0; i < COMPOUNDED_VALUES; dp->compound_value[ i++ ] = 0 );
						//
						//	Flip lock needs clearing, if it pointed
						//	to this record.
						//
						if( flip_lock == dp ) flip_lock = NULL;
						//
						//	Note what we have done and schedule rechecking
						//
						dp->status = DRIVER_OFF;
						dp->recheck = millis() + DRIVER_RESET_PERIOD;
						//
						//	Report error/action to host computer
						//
						errors.log_error( POWER_OVERLOAD, amps );
						report_driver_status();
					}
				}
			}
			else {
				//
				//	This is the "nominal" operation code where the load
				//	reading received is within all normal bounds.
				//

				//
				//	Finally, power level are not of any concern, but still
				//	other things to do.
				//
				//	If the state is either FLIPPED or BLOCKED we can
				//	reset it to ON and clear any retry.  Do not forget
				//	to clear the flip lock if necessary.
				//
				switch( dp->status ) {
					case DRIVER_FLIPPED: {
						//
						//	Phase flipping must have worked!
						//

						ASSERT( flip_lock == dp );

						flip_lock = NULL;
						//
						//	We are deliberately going to fall through
						//	to the following case to complete the
						//	common actions.
						//
						__attribute__(( fallthrough ));
					}
					case DRIVER_BLOCKED: {
						//
						//	Another district flipping must have fixed this.
						//
						dp->status = DRIVER_ON_GRACE;
						dp->recheck = millis() + POWER_GRACE_PERIOD;

						//
						//	Let the world know the good news.
						//
						report_driver_status();
						break;
					}
					case DRIVER_OFF: {
						//
						//	We expect to get here every time with a district
						//	which is OFF.  All we do is check to see if we
						//	need to restart the district.
						//
						if( millis() > dp->recheck ) {
							//
							//	Restart the district.
							//
							//	Turn on output.
							//
							digitalWrite( progmem_read_byte( shield_output[ output_index ].enable ), HIGH );
							//
							//	reset status etc.
							//
							dp->status = DRIVER_ON_GRACE;
							dp->recheck = millis() + POWER_GRACE_PERIOD;
							//
							//	Let the world know the good news.
							//
							report_driver_status();
						}
						break;
					}
					default: {
						//
						//	Nothing to do here
						//
						break;
					}
				}
			}
		}
	}
	else {
		//
		//	We are in the Power On Grace period.
		//
		if( millis() > dp->recheck ) {
			dp->status = DRIVER_ON;
			dp->recheck = 0;
		}
	}
	
	//
	//	Move to the next driver..
	//
	if(( output_index += 1 ) >= SHIELD_OUTPUT_DRIVERS ) output_index = 0;

	//
	//	..and start a new reading on that.
	//
	MONITOR_ANALOGUE_PIN( progmem_read_byte( shield_output[ output_index ].analogue ));
}

//
//	Routine called periodically (if enabled) to report
//	track power dynamically.
//
static void report_track_power( bool enabled ) {
	if( enabled ) {
		char	buffer[ 16 ];
		word	h;

		//
		//	Find the highest load value...
		//
		h = 0;
		for( byte i = 0; i < SHIELD_OUTPUT_DRIVERS; i++ ) {
			word t = output_load[ i ].compound_value[ COMPOUNDED_VALUES-1 ];
			if( t > h ) h = t;
		}

		//
		//	...then send the percentage report back through
		//	the console.
		//
		if( reply_1( buffer, 16, 'L', mul_div<word>( h, 100, AVERAGE_CURRENT_LIMIT ))) {
			if( !FIRMWARE_OUTPUT( console.print( buffer ))) errors.log_error( COMMAND_REPORT_FAIL, 'L' );
		}
	}
}



//
//	Firmware initialisation routines
//	--------------------------------
//


//
//	Controller Menu definitions.
//	============================
//
//	Declare the pointers into the object page data structure and the menus.
//
static PAGE_DATA	*this_page;
static byte		this_page_index;
static OBJECT_DATA	*this_object;
static byte		this_object_line;
static const MENU_PAGE	*this_menu;
static byte		this_menu_index;

//
//	Here we will track the state of the shift keys so thier
//	modifying effects can be applied.
//
static bool	menu_shift = false;
static bool	page_shift = false;
static bool	input_mode = false;
static bool	input_mobile = false;

//
//	Are we displaying the status data or the object data?
//
static bool	display_status = true;

//
//	Reset all of the defined object states to 0.  Performed whan the
//	firmware starts as the "saved sate" (brought in from EEPROM) may
//	have non-zero states embedded in it.
//
static void initialise_object_states( void ) {
	for( byte p = 0; p < PAGE_COUNT; p++ ) {
		for( byte o = 0; p < OBJECT_COUNT; p++ ) {
			PAGE_MEMORY_VAR.page[ p ].object[ o ].state = 0;
		}
	}
}


//
//	Redrawing routines.
//
static void redraw_object_area( void ) {
	char	buffer[ LCD_DISPLAY_STATUS_WIDTH ];

	//
	//	Redraw the whole object area based on the state of
	//	the current object.
	//
	if( this_object->adrs > 0 ) {
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
		for( byte f = DCC::minimum_func_number; f <= DCC::maximum_func_number; f++ ) {
			if( get_function( this_object->adrs, f, true )) {
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
					lcd.setPosn( LCD_DISPLAY_STATUS_COLUMN, r++ );
					lcd.writeBuf( buffer, LCD_DISPLAY_STATUS_WIDTH );
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
			lcd.setPosn( LCD_DISPLAY_STATUS_COLUMN, r++ );
			lcd.writeBuf( buffer, LCD_DISPLAY_STATUS_WIDTH );
			memset( buffer+1, SPACE, LCD_DISPLAY_STATUS_WIDTH-1 );
			while( r < LCD_DISPLAY_ROWS ) {
				lcd.setPosn( LCD_DISPLAY_STATUS_COLUMN, r++ );
				lcd.writeBuf( buffer, LCD_DISPLAY_STATUS_WIDTH );
			}
		}
	}
	else if( this_object->adrs < 0 ) {
		//
		//	A Static/Accessory decoder object.
		//
		//	For the moment nothing to display.
		//
		buffer[ 0 ] = '|';
		memset( buffer+1, SPACE, LCD_DISPLAY_STATUS_WIDTH-1 );
		for( byte r = 0; r < LCD_DISPLAY_ROWS; r++ ) {
			lcd.setPosn( LCD_DISPLAY_STATUS_COLUMN, r );
			lcd.writeBuf( buffer, LCD_DISPLAY_STATUS_WIDTH );
		}
	}
	else {
		//
		//	An empty object, and empty status display.
		//
		buffer[ 0 ] = '|';
		memset( buffer+1, SPACE, LCD_DISPLAY_STATUS_WIDTH-1 );
		for( byte r = 0; r < LCD_DISPLAY_ROWS; r++ ) {
			lcd.setPosn( LCD_DISPLAY_STATUS_COLUMN, r );
			lcd.writeBuf( buffer, LCD_DISPLAY_STATUS_WIDTH );
		}
	}
}
static void redraw_menu_area( void ) {
	for( byte r = 0; r < ITEM_COUNT; r++ ) {
		char const	*s;
		byte		l;
		
		lcd.setPosn( LCD_DISPLAY_MENU_COLUMN, r );
		l = MENU_ITEM_SIZE;
		s = this_menu->item[ r ].item;
		while( l-- ) lcd.writeChar( progmem_read_byte_at( s++ ));
	}
}
static void redraw_page_line( byte r ) {

#if LCD_DISPLAY_PAGE_WIDTH != 10
#error "This routine needs hand coding for new display size"
#endif

	char		line[ LCD_DISPLAY_PAGE_WIDTH ];
	OBJECT_DATA	*o;
	
	o = &( this_page->object[ r ]);

	//
	//	Update the edge character
	//
	line[ 0 ] = ( r == this_object_line )? ( input_mode? '#': '>' ): '|';

	if( o->adrs > 0 ) {
		sbyte		s;
		bool		r;
		
		//
		//	Mobile object.
		//
		line[ 1 ] = 'C';	// A "Cab" Object
		//
		//	Layout:	A=Address, D=Direction, S=Speed
		//
		//		|CAAAAADSS
		//		01234567890
		//
		(void)backfill_int_to_text( line+2, 5, o->adrs );
		if(( s = o->state ) == 0 ) {
			//
			//	For zero state we display no speed
			//	or direction information.
			//
			memset( line+7, SPACE, 3 );
		}
		else {
			//
			//	For "non-zero" states we display the
			//	speed (correctly adjusted).
			//
			if(( r = ( s < 0 ))) s = -s;
			s -= 1;				// Remember state as a speed means value 1 is speed ZERO!
			line[ 7 ] = r? '<': '>';
			if( !backfill_byte_to_text( line+8, 2, (byte)s )) {
				memset( line+8, HASH, 2 );
			}
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
			line[ 1 ] = 'A';
			(void)backfill_int_to_text( line+2, 5, -o->adrs );
			line[ 7 ] = SPACE;
			line[ 8 ] = ( o->state > 0 )? 'Y': (( o->state < 0 )? 'N': SPACE );
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
	lcd.setPosn( LCD_DISPLAY_PAGE_COLUMN, r );
	lcd.writeBuf( line, LCD_DISPLAY_PAGE_WIDTH );
}
static void redraw_page_area( void ) {
	for( byte r = 0; r < ITEM_COUNT; r++ ) redraw_page_line( r );
}

//
//	Initialisation of the menu and object structures.
//
static void init_user_interface( void ) {
	//
	//	Set the pointer to thier start condition.
	//
	this_page_index = 0;
	this_page = &( PAGE_MEMORY_VAR.page[ this_page_index ]);
	this_object_line = 0;
	this_object = &( this_page->object[ this_object_line ]);
	this_menu_index = 0;
	this_menu = &( menus.menu[ this_menu_index ]);
	//
	//	Kick of an initial redraw of both areas.
	//
	redraw_menu_area();
	redraw_page_area();
}

//
//	System SETUP all starts here.
//	=============================
//

void setup( void ) {
	//
	//	Initialise all the constant values
	//
	initialise_constants();
	initialise_object_states();
	
	//
	//	Initialise the console device.
	//
	initialise_console( SERIAL_BAUD_RATE );
	
	//
	//	Set up the data structures.
	//
	initialise_data_structures();
	init_function_cache();

	//
	//	Initialise all the defined districts.
	//
	districts.initialise();

	//
	//	Optional hardware initialisations
	//
	keypad.initialise( KEYPAD_ADDRESS );

	//
	//	LCD Display.
	//	------------
	//
	//	This might be a warm restart for the display,
	//	so reset everything to a clean slate.
	//
	
	//
	//	Kick off the LCD display, now that the TWI
	//	interface is enabled, and add the frame
	//	buffer.
	//
	lcd.begin();
	lcd.setBuffer( lcd_frame_buffer, LCD_FRAME_BUFFER );

	//
	//	Display the SPLASH screen if configured.
	//
#ifdef SPLASH_ENABLE
	{
		static const char banner_line[] PROGMEM = {
			SPLASH_LINE_1 "\n" SPLASH_LINE_2 "\n" SPLASH_LINE_3 "\n" SPLASH_LINE_4 "\n"
		};
		byte	l;
		char	*s, c;

		//
		//	Display banner to the LCD for a short period.
		//
		l = 0;
		s = (char *)banner_line;
		
		lcd.clear();

		FIRMWARE_OUTPUT( console.println());
		while(( c = progmem_read_byte_at( s++ )) != EOS ) {
			if( c == '\n' ) {
				
				lcd.setPosn( 0, ++l );

				FIRMWARE_OUTPUT( console.println());
			}
			else {
				
				lcd.writeChar( c );

				FIRMWARE_OUTPUT( console.print( c ));
			}
		}
		FIRMWARE_OUTPUT( console.println());
		
		lcd.synchronise( SPLASH_WAIT );
		lcd.clear();
	}
#endif

	twi_errorReporting( twi_error_callback );
	
	//
	//	Finally we initialise the user interface.
	//
	init_user_interface();
}

//
//	Liquid Crystal Display
//	======================
//
//	Update the LCD with pertinent data about the operation of the
//	DCC Generator.
//

//
//	Time of next call to display update (ms).
//
static unsigned long next_lcd_update = 0;

//
//	The next line of screen to be updated.
//
static byte next_lcd_line = 0;

//	This is called frequently.  It is the responcibility of this
//	routine to return immediately 99% of the time.
//
//	The update of the LCD has been spread over a number of calls
//	to reduce the "dead time" that the MCU spends in this routine
//	at any single time.
//
//	The variable "next_lcd_line" is stepped from 0 to LCD_DISPLAY_ROWS
//	(inclusive), on divisions of LINE_REFRESH_INTERVAL microseconds.
//
//	During steps 0 to LCD_DISPLAY_ROWS-1 the corresponding lines of
//	the STATUS and DISTRICT areas are updated.  On the final count of
//	"LCD_DISPLAY_ROWS" the BUFFER area is fully updated.
//
static void update_dcc_status_line( byte line ) {
	char		buffer[ LCD_DISPLAY_STATUS_WIDTH ];

	//	 0....0....1....1....2
	//	 0    5    0    5    0
	//	+--------------------+	The STATUS area of the display, showing:
	//	|              SSSSSS|	The highest district power (L)oad (percent) of A
	//	|              SSSSSS|	...of B districts
	//	|              SSSSSS|	The available (F)ree bit buffers and (P)ower status
	//	|              SSSSSS|	DCC packets (T)ransmitted sent per second
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
			DRIVER_LOAD	*load;

			//
			//	Rows 0 and 1, Power status for the districts A/0 and B/1.
			//
			//	Set load to the right record.
			//
			load = &( output_load[ line ]);
			//
			//	Fill out the buffer.
			//
			buffer[ 1 ] = 'A' + line;
			switch( load->status ) {
				case DRIVER_ON: {
					if( !backfill_int_to_text( buffer+2, LCD_DISPLAY_STATUS_WIDTH-3, (int)( mul_div<word>( output_load[ line ].compound_value[ COMPOUNDED_VALUES-1 ], 100, AVERAGE_CURRENT_LIMIT )))) {
						memset( buffer+2, HASH, LCD_DISPLAY_STATUS_WIDTH-3 );
					}
					buffer[ LCD_DISPLAY_STATUS_WIDTH-1 ] = '%';
					break;
				}
				case DRIVER_ON_GRACE:
				case DRIVER_FLIPPED:
				case DRIVER_BLOCKED: {
					memset( buffer+2, '*', LCD_DISPLAY_STATUS_WIDTH-2 );
					break;
				}
				case DRIVER_OFF: {
					memset( buffer+2, '_', LCD_DISPLAY_STATUS_WIDTH-2 );
					break;
				}
				case DRIVER_DISABLED: {
					memset( buffer+2, SPACE, LCD_DISPLAY_STATUS_WIDTH-2 );
					break;
				}
			}
			lcd.setPosn( LCD_DISPLAY_STATUS_COLUMN, line );
			lcd.writeBuf( buffer, LCD_DISPLAY_STATUS_WIDTH );
			break;
		}
		case 2: {
			byte		c;

			//
			//	Row 1, (P)ower status and (F)ree bit buffers
			//
			c = 0;
			for( byte i = 0; i < TRANSMISSION_BUFFERS; i++ ) {
				if( circular_buffer[ i ].state == TBS_EMPTY ) {
					c++;
				}
			}
			buffer[ 1 ] = 'P';
			buffer[ 2 ] = power_state? '1': '0';
			buffer[ 3 ] = 'F';
			if( !backfill_byte_to_text( buffer+4, LCD_DISPLAY_STATUS_WIDTH-4, c )) {
				memset( buffer+4, HASH, LCD_DISPLAY_STATUS_WIDTH-4 );
			}
			lcd.setPosn( LCD_DISPLAY_STATUS_COLUMN, 2 );
			lcd.writeBuf( buffer, LCD_DISPLAY_STATUS_WIDTH );
			break;
		}
		case 3: {
			static bool	spinner = false;

			//
			//	Row 3, DCC packets (T)ransmitted sent per second
			//
			buffer[ 1 ] = 'T';
			if( !backfill_int_to_text( buffer+2, LCD_DISPLAY_STATUS_WIDTH-3, (int)( mul_div<word>( lcd_statistic_packets, 1000, LCD_UPDATE_INTERVAL )))) {
				memset( buffer+2, HASH, LCD_DISPLAY_STATUS_WIDTH-2 );
			}
			//
			//	Spinner, so we can see that the firmware is still running.
			//
			buffer[ LCD_DISPLAY_STATUS_WIDTH-1 ] = ( spinner = !spinner )? SPACE: '.';
			//
			//	Place the data.
			//
			lcd.setPosn( LCD_DISPLAY_STATUS_COLUMN, 3 );
			lcd.writeBuf( buffer, LCD_DISPLAY_STATUS_WIDTH );
			//
			//	Reset stats for the next period.
			//
			lcd_statistic_packets = 0;
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
static void update_dcc_status( bool refresh ) {
	//
	//	Is this a full refresh?
	//
	if( refresh ) {
		//
		//	Just push out all lines.
		//
		for( byte l = 0; l < LCD_DISPLAY_ROWS; update_dcc_status_line( l++ ));
	}
	else {
		//
		//	If it time to update the LCD again then do so
		//
		if( millis() > next_lcd_update ) {
			//
			//	Update the next line on the display
			//
			update_dcc_status_line( next_lcd_line++ );
			
			//
			//	Return to the top of the display?
			//
			if( next_lcd_line >= LCD_DISPLAY_ROWS ) next_lcd_line = 0;

			//
			//	Calculate time of next line update
			// 
			next_lcd_update += LINE_REFRESH_INTERVAL;
		}
	}
}


//
//	Input Processing routines
//	=========================
//

//
//	While it seems pointless to create a different protocol
//	from the DCCpp Firmware, doing so offers a couple of
//	benefits:
//
//	o	Using an Arduino with one firmware against software
//		expecting the other will obviously fail, rather the
//		seem to work, then fail in unexpected ways.
//
//	o	The firmware coded in this sketch is unable to
//		implement some of the higher level primitives
//		that the DCCpp firmware supports.  These commands are
//		not direct DCC primitive operations but synthesized
//		actions which can be implemented within the host computer
//		software.
//
//	o	The syntax and structure of the conversation between
//		the DCC Generator protocol and host computer software
//		will be more aligned with the operation of the
//		firmware itself.
//
//	Having said that, in high level terms, both protocols have
//	basically the same structure.
//

//
//	Define simple "in string" number parsing routine.  This
//	effectively does the bulk of the syntax parsing for the
//	commands so it a little more fiddly than strictly
//	necessary.
//
//	Return the address of the next unparsed character, this
//	be EOS if at the end of the string.
//
static char *parse_number( char *buf, bool *found, int *value ) {
	int	v, n, c;

	//
	//	Skip any leading white space
	//
	while( isspace( *buf )) buf++;
	//
	//	Intentional assignment, remember if we are handling
	//	a negative number.
	//
	if(( n = ( *buf == '-' ))) buf++;
	//
	//	gather up a decimal number
	//
	c = 0;
	v = 0;
	while( isdigit( *buf )) {
		v = v * 10 + ( *buf++ - '0' );
		c++;
	}
	if( n ) v = -v;
	//
	//	Skip any trailing white space
	//
	while( isspace( *buf )) buf++;
	//
	//	Set up returned data
	//
	*found = ( c > 0 );
	*value = v;
	//
	//	Return address of next unprocessed character.
	//
	return( buf );
}

//
//	Define a routine which scans the input text and returns the
//	command character and a series of numeric arguments
//
//	Routine returns the number of arguments (after the command).
//
static int parse_input( char *buf, char *cmd, int *arg, int max ) {
	int	args,
		value;
	bool	found;

	ASSERT( max > 0 );

	//
	//	Copy out the command character and verify
	//
	*cmd  = *buf++;
	if( !isalnum( *cmd )) return( ERROR );
	//
	//	Step through remainder of string looking for
	//	numbers.
	//
	args = 0;
	found = true;
	while( found ) {
		buf = parse_number( buf, &found, &value );
		if( found ) {
			if( args >= max ) return( ERROR );
			arg[ args++ ] = value;
		}
	}
	//
	//	Done.
	//
	return( args );
}


//
//	The maximum number of arguments in a DCC command:
//
//	New (vers 1.3.4) 'W' command has 7 arguments post command
//	letter.
//
#define MAX_DCC_ARGS	7

//
//	Define the size of a reply buffer
//
#define MAXIMUM_REPLY_SIZE	32
//
//	The command interpreting routine.
//
static void scan_cmd_line( char *buf ) {
	//
	//	Where we build the command
	//
	char	cmd;
	int	arg[ MAX_DCC_ARGS ], args;

	//
	//	Take the data proved and parse it into useful pieces.
	//
	if(( args = parse_input( buf, &cmd, arg, MAX_DCC_ARGS )) != ERROR ) {
		//
		//	We have at least a command and (possibly) some arguments.
		//
		switch( cmd ) {

			//
			//	Power management commands
			//	-------------------------
			//
			case Protocol::power: {
				process_power_command( cmd, arg, args );
				break;
			}
			
			//
			//	Cab/Mobile decoder commands
			//	---------------------------
			//
			case Protocol::mobile: {
				process_mobile_command( cmd, arg, args );
				break;
			}

			//
			//	Accessory Commands
			//	------------------
			//
			case Protocol::accessory: {
				process_accessory_command( cmd, arg, args );
				break;
			}

			//
			//	Mobile decoder functions
			//	------------------------
			//
			case Protocol::function: {
				process_function_command( cmd, arg, args );
				break;
			}
			//	Write Mobile State (Operations Track)
			//	-------------------------------------
			//
			//	Overwrite the entire "state" of a specific mobile decoder
			//	with the information provided in the arguments.
			//
			//	While this is provided as a single DCC Generator command
			//	there is no single DCC command which implements this functionality
			//	so consequently the command has to be implemented as a tightly
			//	coupled sequence of commands.  This being said, the implementation
			//	of the commannd should ensure that either *all* of these commands
			//	are transmitted or *none* of them are.  While this does not
			//	guarantee that the target decoder gets all of the updates
			//	it does increase the likelihood that an incomplete update is
			//	successful.
			//
			case Protocol::rewrite_state: {
				process_state_command( cmd, arg, args );
				break;
			}
			//
			//	EEPROM configurable constants
			//
			case Protocol::eeprom: {
				char	*n;
				word	*w;
				byte	*b;
				
				//
				//	Accessing EEPROM configurable constants
				//
				//	[Q] -> [Q N]			Return number of tunable constants
				//	[Q C] ->[Q C V NAME]		Access a specific constant C (range 0..N-1)
				//	[Q C V V] -> [Q C V NAME]	Set a specific constant C to value V,
				//					second V is to prevent accidental
				//					update.
				//	[Q -1 -1] -> [Q -1 -1]		Reset all constants to default.
				//
				switch( args ) {
					case 0: {
						FIRMWARE_OUTPUT( console.print( PROT_IN_CHAR ));
						FIRMWARE_OUTPUT( console.print( 'Q' ));
						FIRMWARE_OUTPUT( console.print( CONSTANTS ));
						FIRMWARE_OUTPUT( console.print( PROT_OUT_CHAR ));
						FIRMWARE_OUTPUT( console.println());
						break;
					}
					case 1: {
						if( find_constant( arg[ 0 ], &n, &b, &w ) != ERROR ) {
							FIRMWARE_OUTPUT( console.print( PROT_IN_CHAR ));
							FIRMWARE_OUTPUT( console.print( 'Q' ));
							FIRMWARE_OUTPUT( console.print( arg[ 0 ]));
							FIRMWARE_OUTPUT( console.print( SPACE ));
							if( b ) {
								FIRMWARE_OUTPUT( console.print( (word)(*b )));
							}
							else {
								FIRMWARE_OUTPUT( console.print( *w ));
							}
							FIRMWARE_OUTPUT( console.print( SPACE ));
							FIRMWARE_OUTPUT( console.print_PROGMEM( n ));
							FIRMWARE_OUTPUT( console.print( PROT_OUT_CHAR ));
							FIRMWARE_OUTPUT( console.println());
						}
						break;
					}
					case 2: {
						if(( arg[ 0 ] == -1 )&&( arg[ 1 ] == -1 )) {
							reset_constants();
							FIRMWARE_OUTPUT( console.print( PROT_IN_CHAR ));
							FIRMWARE_OUTPUT( console.print( 'Q' ));
							FIRMWARE_OUTPUT( console.print( -1 ));
							FIRMWARE_OUTPUT( console.print( SPACE ));
							FIRMWARE_OUTPUT( console.print( -1 ));
							FIRMWARE_OUTPUT( console.print( PROT_OUT_CHAR ));
							FIRMWARE_OUTPUT( console.println());
						}
						break;
					}
					case 3: {
						if(( arg[ 1 ] == arg[ 2 ])&&( find_constant( arg[ 0 ], &n, &b, &w ) != ERROR )) {
							if( b ) {
								*b = (byte)arg[ 1 ];
							}
							else {
								*w = arg[ 1 ];
							}
							record_constants();
							FIRMWARE_OUTPUT( console.print( PROT_IN_CHAR ));
							FIRMWARE_OUTPUT( console.print( 'Q' ));
							FIRMWARE_OUTPUT( console.print( arg[ 0 ]));
							FIRMWARE_OUTPUT( console.print( SPACE ));
							if( b ) {
								FIRMWARE_OUTPUT( console.print( (word)( *b )));
							}
							else {
								FIRMWARE_OUTPUT( console.print( *w ));
							}
							FIRMWARE_OUTPUT( console.print( SPACE ));
							FIRMWARE_OUTPUT( console.print_PROGMEM( n ));
							FIRMWARE_OUTPUT( console.print( PROT_OUT_CHAR ));
							FIRMWARE_OUTPUT( console.println());
						}
						break;
					}
					default: {
						errors.log_error( INVALID_ARGUMENT_COUNT, cmd );
						break;
					}
				}
				break;
			}
			default: {
				//
				//	Here we capture any unrecognised command letters.
				//
				errors.log_error( UNRECOGNISED_COMMAND, cmd );
				break;
			}
		}
	}
	else {
		errors.log_error( PARSE_COMMAND_ERROR, 0 );
	}
	//
	//	Done.
	//
}


//
//	The command buffer:
//
static char cmd_buf[ MAXIMUM_DCC_CMD+1 ];	// "+1" to allow for EOS with checking code.
static byte cmd_used = 0;
	
static bool in_packet_status = false;

//
//	The character input routine.
//
static void process_input( byte count ) {
	while( count-- ) {
		char	c;

		switch(( c = console.read())) {
			case PROT_IN_CHAR: {
				//
				//	Found the start of a command, regardless of what we thought
				//	we were doing we clear out the buffer and start collecting
				//	the content of the packet.
				//
				cmd_used = 0;
				in_packet_status = true;
				break;
			}
			case PROT_OUT_CHAR: {
				//
				//	If we got here and we were in a command packet then we have
				//	something to work with, maybe.
				//
				if( in_packet_status ) {
					//
					//	Process line and reset buffer.
					//
					cmd_buf[ cmd_used ] = EOS;
					scan_cmd_line( cmd_buf );
				}
				//
				//	Now reset the buffer space.
				//
				cmd_used = 0;
				in_packet_status = false;
				break;
			}
			default: {
				//
				//	If we are inside a command packet then we save character, if there is space.
				//
				if( in_packet_status ) {
					if(( c < SPACE )||( c >= DELETE )) {
						//
						//	Invalid character for a command - abandon the current command.
						//
						cmd_used = 0;
						in_packet_status = false;
					}
					else {
						//
						//	"Valid" character (at least it is a normal character), so
						//	save it if possible.
						//
						if( cmd_used < MAXIMUM_DCC_CMD-1 ) {
							cmd_buf[ cmd_used++ ] = c;
						}
						else {
							//
							//	we have lost some part of the command.  Report error and
							//	drop the malformed command.
							//
							errors.log_error( DCC_COMMAND_OVERFLOW, MAXIMUM_DCC_CMD );
							cmd_used = 0;
							in_packet_status = false;
						}
					}
				}
				break;
			}
		}
	}
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

static void process_menu_option( byte m ) {
	switch( progmem_read_byte( this_menu->item[ m ].action )) {
		case ACTION_NEW_MOBILE: {
			//
			//	Set up for entering a new mobile address
			//
			input_mode = true;
			input_mobile = true;
			this_object->adrs = 0;
			this_object->state = 0;
			redraw_page_line( this_object_line );
			break;
		}
		case ACTION_NEW_STATIC: {
			//
			//	Set up for entering a new mobile address
			//
			input_mode = true;
			input_mobile = false;
			this_object->adrs = 0;
			this_object->state = 0;
			redraw_page_line( this_object_line );
			break;
		}
		case ACTION_ERASE: {
			//
			//	Delete the current object
			//
			this_object->adrs = 0;
			this_object->state = 0;
			redraw_page_line( this_object_line );
			break;
		}
		case ACTION_NEXT: {
			//
			//	Move to the next menu.
			//
			if(( this_menu_index += 1 ) >= MENU_COUNT ) this_menu_index = 0;
			this_menu = &( menus.menu[ this_menu_index ]);
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
			int	z;
			
			//
			//	Power OFF
			//
			z = 0;
			process_power_command( 'P', &z, 1 );
			break;
		}
		case ACTION_START: {
			int	z;
			
			//
			//	Power ON
			//
			z = 1;
			process_power_command( 'P', &z, 1 );
			break;
		}
		case ACTION_TOGGLE: {
			int	z;
			
			//
			//	Toggle Power OF->ON->OFF etc
			//
			z = power_state? 0: 1;
			process_power_command( 'P', &z, 1 );
			break;
		}
		case ACTION_STATUS: {
			//
			//	Toggle the display of the status information.
			//
			if(( display_status ^= true )) {
				update_dcc_status( true );
			}
			else {
				redraw_object_area();
			}
			break;
		}
		default: {
			break;
		}
	}
}

//
//	When not in "input_mode" we will calculate the time for function
//	number has been pressed for to determine if this is an ON or
//	and OFF request.
//
static unsigned long fkey_down_at = 0;

static void user_key_event( bool down, char key ) {
	byte	i, j;
	int	args[ 3 ];
	
	if( key == KEYPAD_PAGE_SHIFT ) {
		//
		//	Force reset of input mode.
		//
		input_mode = false;
		//
		//	Modify the page shift status.
		//
		if(( page_shift = down )) {
			//
			//	Is this a power short cut?
			//
			if( menu_shift ) {
				int	z;
				
				//
				//	Yup!.
				//
				z = 0;
				process_power_command( 'P', &z, 1 );
			}
		}
		return;
	}
	if( key == KEYPAD_MENU_SHIFT ) {
		//
		//	Force reset of input mode.
		//
		if( down ) input_mode = false;
		//
		//	Modify the menu shift status.
		//
		if(( menu_shift = down )) {
			//
			//	Is this a power short cut?
			//
			if( page_shift ) {
				int	z;
				
				//
				//	Yup!.
				//
				z = 0;
				process_power_command( 'P', &z, 1 );
			}
		}
		return;
	}

	//
	//	If both shift keys are down we ignore everything.
	//
	if( menu_shift && page_shift ) return;

	if( LAYOUT_IS_LETTER( key )) {
		//
		//	Force reset of input mode.
		//
		input_mode = false;
		//
		//	We are only concerned if the key pressed, only
		//	when released.
		//
		if( down ) return;
		//
		//	A letter has been pressed.  What we do depends on if
		//	there is a shift key down.
		//
		//	Convert letter to index (0..N-1)
		//
		i = KEYPAD_LETTER_INDEX( key );
		//
		//	deal with shifted options first.
		//
		if( menu_shift ) {
			//
			//	Select a menu option.
			//
			process_menu_option( i );
		}
		else if( page_shift ) {
			//
			//	Select a specific page.
			//
			this_page_index = i;
			this_page = &( PAGE_MEMORY_VAR.page[ this_page_index ]);
			this_object_line = 0;
			this_object = &( this_page->object[ this_object_line ]);
			redraw_page_area();
			if( !display_status ) redraw_object_area();
		}
		else {
			//
			//	Select an object on the page.
			//
			j = this_object_line;
			this_object_line = i;
			this_object = &( this_page->object[ this_object_line ]);
			redraw_page_line( j );
			redraw_page_line( i );
			if( !display_status ) redraw_object_area();
		}
		return;
	}
	
	if( !LAYOUT_IS_NUMBER( key )) return;
	//
	//	When a number key is pressed we note the time
	//	it was pressed so that when it is released we
	//	will know how long it was down for.
	//
	if( down ) {
		fkey_down_at = millis();
		return;
	}
	//
	//	Get the key number index.
	//
	i = KEYPAD_NUMBER_INDEX( key );
	//
	//	Are we in input mode?
	//
	if( input_mode ) {
		//
		//	Adjusting an object address.
		//
		if( input_mobile ) {
			//
			//	Modify a mobile address
			//
			word	a;
			
			a = this_object->adrs * 10 + i;
			if( a <= DCC::maximum_address ) this_object->adrs = a;
		}
		else {
			//
			//	Modify an accessory address
			//
			word	a;
			
			a = (-this_object->adrs) * 10 + i;
			if( a <= MAX_ACCESSORY_EXT_ADDRESS ) this_object->adrs = -a;
		}
		redraw_page_line( this_object_line );
	}
	else {
		//
		//	So we are in FUNCTION KEY mode.
		//
		//	How we handle this depends on the type of object.
		//
		if( this_object->adrs > 0 ) {
			//
			//	Mobile Decoder
			//
			//	Adjust the decoder function number.
			//
			if( menu_shift ) i += 10;
			if( page_shift ) i += 20;
			//
			//	Send out the function request.
			//
			//	If the key was down for less than a second
			//	then toggle the function, otherwise it is
			//	forced OFF.
			//
			args[ 0 ] = this_object->adrs;
			args[ 1 ] = i;
			if(( millis() - fkey_down_at ) > LONG_KEY_PRESS ) {
				//
				//	Force function off.
				//
				args[ 2 ] = 0;
			}
			else {
				//
				//	Toggle function.
				//
				args[ 2 ] = get_function( this_object->adrs, i, 1 ) ^ 1;
			}
			//
			//	Apply and update status page (optionally).
			//
			process_function_command( 'F', args, 3 );
			if( !display_status ) redraw_object_area();
		}
		else {
			//
			//	Accessory Decoder.
			//
			//	Just use the lower bit to turn on/off the
			//	accessory.
			//
			if(( i & 1 ) == 0 ) {
				//
				//	OFF
				//
				this_object->state = -1;
				args[ 1 ] = 0;
			}
			else {
				//
				//	ON
				//
				this_object->state = 1;
				args[ 1 ] = 1;
			}
			args[ 0 ] = -this_object->adrs;
			redraw_page_line( this_object_line );
			process_accessory_command( 'A', args, 2 );
		}
	}
}

//
//	Respond to the button (on the rotary knob) being pushed.
//
//	For mobile decoders pushing reverses the direction.
//
//	For accessories to reverses its position.
//
static void user_button_pressed( UNUSED( word duration )) {
	//
	//	If we are in input mode we do nothing.
	//
	if( input_mode ) return;
	//
	//	Now act accordingly.
	//
	if( this_object->adrs > 0 ) {
		//
		//	Mobile decoder, should be simple.
		//
		int	args[ 3 ];
		sbyte	s;
		bool	r;
		
		//
		//	We have a mobile decoder, huzzar!
		//
		//	Grab the speed, note direction and negate
		//	if speed is negative (ie reverse) remembering
		//	also that the change needs to be negated!
		//
		if(( s = this_object->state )) {
			//
			//	non-zero s value.
			//
			if(( r = ( s < 0 ))) s = -s;
			//
			//	having extracted direction, reduce
			//	speed by 1.
			//
			s -= 1;
		}
		else {
			//
			//	A zero speed is ok, it means forwrds
			//	at speed zero.
			//
			r = false;
		}
		//
		//	We simply reverse the extracted direction.
		//
		r = !r;
		//
		//	Initiate DCC command.
		//
		args[ 0 ] = this_object->adrs;
		args[ 1 ] = s;
		s += 1;				// move the state back "up" 1
		if( r ) {
			args[ 2 ] = 0;
			this_object->state = -s;
		}
		else {
			args[ 2 ] = 1;
			this_object->state = s;
		}
		redraw_page_line( this_object_line );
		process_mobile_command( 'M', args, 3 );
	}
	else if( this_object->adrs < 0 ) {
		int	args[ 2 ];
		sbyte	s;
		
		//
		//	Accessory needs flipping.
		//
		switch( this_object->state ) {
			case 1: {
				//
				//	ON -> OFF
				//
				s = 0;
				this_object->state = -1;
				break;
			}
			case -1: {
				//
				//	OFF -> ON
				//
				s = 1;
				this_object->state = 1;
				break;
			}
			default: {
				//
				//	Unknown -> OFF
				//
				s = 0;
				this_object->state = -1;
				break;
			}
		}
		args[ 0 ] = -this_object->adrs;
		args[ 1 ] = s;
		redraw_page_line( this_object_line );
		process_accessory_command( 'A', args, 2 );
	}
}

//
//	The rotary control has been turned, is there
//	anything this needs to do.
//
//	For the moment the rotary control turning motion
//	is only applicable to mobile decoders and their speed.
//
static void user_rotary_movement( sbyte change ) {
	//
	//	If we are in input mode we do nothing.
	//
	if( input_mode ) return;
	//
	//	Now act accordingly.
	//
	if( this_object->adrs > 0 ) {
		sbyte	s, t;
		bool	r;
		
		//
		//	We have a mobile decoder, huzzar!
		//
		//	Calculate the change then, if the result is
		//	valid, then apply it.
		//
		//	Grab the speed, note direction and negate
		//	if speed is negative (ie reverse).
		//
		//	Note/	When storing the speed of the mobile
		//		decoder in the state variable we need
		//		to accommodate the potential for both
		//		"forwards at speed zero" and "backwards
		//		at speed zero" - something a signed
		//		value cannot do. Therefore all speeds
		//		are 1 off their true value.
		//
		if(( s = this_object->state ) == 0 ) {
			//
			//	A zero state is ok, it means forwards
			//	at speed zero.
			//
			r = false;
		}
		else {
			//
			//	non-zero s value.
			//
			if(( r = ( s < 0 ))) {
				s = -s;
			}
			//
			//	having extracted direction into r we
			//	reduce speed by 1.
			//
			s -= 1;
		}
		//
		//	Adjust the speed appropiately, and if changes
		//	are valid save the new speed and issue the DCC
		//	command.
		//
		t = s + change;
		//
		//	Reducing the speed underflows in a reliable
		//	manner.  We can easily correct this.
		//
		//	Adding to the speed has the potential to both
		//	overflow the DCC speed range (126), but also the
		//	signed byte base type (127) which would roll it into
		//	negative values.  This must be checked for.
		//
		//	We will hit this is a single step: If we have stepped
		//	outside the DCC speed range we will reset the value
		//	based on the sign of the change applied.
		//
		if(( t < DCC::minimum_speed )||( t > DCC::maximum_speed )) t = ( change < 0 )? DCC::minimum_speed: DCC::maximum_speed;
		//
		//	Have we changed the speed?  If so then do it.
		//
		if( t != s ) {
			//
			//	Initiate DCC command.
			//
			int	args[ 3 ];
			
			args[ 0 ] = this_object->adrs;
			args[ 1 ] = t;
			t += 1;				// move the state back "up" 1
			if( r ) {
				args[ 2 ] = 0;
				this_object->state = -t;
			}
			else {
				args[ 2 ] = 1;
				this_object->state = t;
			}
			redraw_page_line( this_object_line );
			process_mobile_command( 'M', args, 3 );
		}
	}
}


//
//	Firmware main processing loop.
//	==============================
//

//
//	'dynamic_loading' is used to control the periodic output
//	of district loads asynchronously from the DCC Generator.
//
static unsigned long	dynamic_loading = 0;

//
//	The rotary control can produce far too much data, and has a habit
//	of flipping backwards and forwards.  We will implement a buffering
//	system so that the updates are consolidated and applied less
//	prequently and more evenly.
//
static unsigned long	rotary_update = 0;
static sbyte		rotary_average = 0;

//
//	Finally, the main event loop:
//
void loop( void ) {
	byte		ready;
	word		duration;

	//
	//	Service those background facilities which need regular
	//	attention (all relating to the I2C bus)
	//
	twi_eventProcessing();
	lcd.service();
	keypad.service();

	//
	//	Is there Serial data to be processed...
	//
	if(( ready = console.available())) process_input( ready );
	
	//
	//	Every time we spin through the loop we give the
	//	management service routine a slice of the CPU time
	//	so that buffer transitions between states are
	//	synchronised correctly.
	//
	management_service_routine();

	//
	//	Power related actions triggered only when data is ready
	//
	if( reading_is_ready ) monitor_current_load( track_load_reading );

	//
	//	Perform the load reporting process, periodically.
	//
	if( millis() > dynamic_loading ) {
		//
		//	Forward out an asynchronous power update.
		//
		dynamic_loading = millis() + DYNAMIC_LOAD_PERIOD;
		report_track_power( DYNAMIC_LOAD_REPORTS );
	}

	//
	//	Send updates to the LCD.
	//
	if( display_status ) update_dcc_status( false );
	
	//
	//	lets see how the rotary controller is working.
	//
	rotary_average += speed_dial.movement();
	if( millis() > rotary_update ) {
		user_rotary_movement( rotary_average );
		rotary_average = 0;
		rotary_update = millis() + ROTARY_UPDATE_PERIOD;
	}
	
	//
	//	Has there been a button pressed?
	//
	if(( duration = speed_dial.pressed())) user_button_pressed( duration );

	//
	//	Now lets see if we are talking to the keyboard
	//
	if(( ready = keypad.read())) user_key_event((( ready & 0x80 ) != 0 ), (char)( ready & 0x7f ));

	//
	//	Then we give the Error management system an
	//	opportunity to queue some output data.
	//
	flush_error_queue();

}

//
//	EOF
//
