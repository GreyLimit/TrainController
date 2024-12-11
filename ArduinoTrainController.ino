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
//	Arduino Train Controller V0.2.3
//	-------------------------------
//
#define VERSION_NUMBER "0.2.3"

//
//	This version, really an extension of v0.2.2, sees the
//	following modifications:
//
//	*	Introduction of a simplified Dijkstra P/V signalling
//		system.
//
//	*	Introduction of a Signal controller task management
//		system.
//
//	*	Replace use of millis() and micros() with a Signal
//		based relative time system.
//
//	*	Conversion of the DCC generation code to a C++ class
//		permitting a more generic application of its code (in
//		future).
//
//	Lets be honest - this is a full rewrite of the whole firmware
//	as a move away from my earlier attempts to create something
//	resembling "real time" software towards something more structured
//	and portable.
//
//	This version is wholly different from the previous versions
//	containing systems for managing time in both "computer" and
//	"human" scales, a system for the causing "tasks" to be called
//	when and external (to that task) event raises a suitable flag.
//


//
//	Arduino Train Controller V0.2.2
//	-------------------------------
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
#include "Parameters.h"
#include "Configuration.h"

//
//	Include firmware modules.
//
#include "Errors.h"
#include "Constants.h"
#include "USART.h"
#include "TWI.h"
#include "LCD.h"
#include "FrameBuffer.h"
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
#include "Constants.h"
#include "Districts.h"
#include "DCC_Constant.h"
#include "TOD.h"
#include "Stats.h"
#include "HCI.h"
#include "Signal.h"
#include "Banner.h"
#include "Task.h"

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
//	the various supported baud rates.  These are all preceded
//	with a 'B'.
//
#ifndef SERIAL_BAUD_RATE
#define SERIAL_BAUD_RATE	B38400
#endif


//
//	System SETUP all starts here.
//	=============================
//

void setup( void ) {
	//
	//	Task 1:	Get the console interface live.
	//
	initialise_console( SERIAL_BAUD_RATE );

	//
	//	Task 2:	Announce what firmware this is to the console.
	//
	serial_banner( &console );

	//
	//	Task 3: Initialise the elements of the firmware.
	//
	initialise_constants();
	districts.initialise();
	dcc_generator.initialise();
	hci_control.initialise();
	protocol.initialise();
}


//
//	Finally, the main event loop which is al about running the tasks.
//
void loop( void ) {
	//
	//	Task 4:	Just do it!
	//
	task_manager.run_tasks();
}



//
//	EOF
//
