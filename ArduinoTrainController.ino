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
//	Arduino Train Controller V0.2.0
//	-------------------------------
//
#define VERSION_NUMBER "0.2.0"

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
//	Bring in the necessary IO and Interrupt definitions.
//
#include <avr/io.h>
#include <avr/interrupt.h>

//
//	Include the AVR Progmem access facilities.
//
#include <avr/pgmspace.h>

//
//	A little math support.
//
#include "mul_div.h"

//
//	Bring in the firmware configuration with respect to the
//	hardware being targetted.
//
#include "Configuration.h"
#include "Trace.h"

//
//	Include Modules from the "Library" solution...
//
#include "Code_Assurance.h"
#include "Errors.h"
#include "Constants.h"
#include "USART.h"
#include "TWI_IO.h"
#include "LCD_TWI_IO.h"
#include "Keypad_TWI_IO.h"
#include "Rotary.h"
#include "Menu.h"
#include "Console.h"


//
//	Hardware Specific Configuration Definitions
//	===========================================
//
//	The following definitions are used to abstract the differences
//	between each of the boards.
//
//	The Macro "SELECT_SML(s,m,l)" will be used to select alternate
//	configuration values based on the apparent "size" of the target
//	micro controller.  The parameter "s" represents small MCUs with
//	2 KBytes SRAM.  "m" represents systems with between
//	2 and 4 KBytes SRAM.  All other systems will have the "l" value
//	applied.
//


#if defined( __AVR_ATmega328__ )| defined( __AVR_ATmega328P__ )| defined( __AVR_ATmega328PB__ )
//
//	Standard Nano or Uno R3 configuration
//	=====================================
//
#define HW_TITLE		"AVR ATmega328"

//
//	SRAM = 2 KBytes
//
#define SELECT_SML(s,m,l)	s

//
//	Map Timer symbols onto target Timer hardware
//
#define HW_TCCRnA		TCCR2A
#define HW_TCCRnB		TCCR2B
#define HW_TIMERn_COMPA_vect	TIMER2_COMPA_vect
#define HW_TCNTn		TCNT2
#define HW_OCRnA		OCR2A
#define HW_WGMn1		WGM21
#define HW_CSn0			CS20
#define HW_CSn1			CS21
#define HW_TIMSKn		TIMSK2
#define HW_OCIEnA		OCIE2A


#elif defined( __AVR_ATmega2560__ )
//
//	Standard Mega 2560 configuration
//	================================
//
#define HW_TITLE		"AVR ATmega2560"

//
//	SRAM = 8 KBytes
//
#define SELECT_SML(s,m,l)	l

//
//	Map Timer symbols onto target Timer hardware
//
#define HW_TCCRnA		TCCR2A
#define HW_TCCRnB		TCCR2B
#define HW_TIMERn_COMPA_vect	TIMER2_COMPA_vect
#define HW_TCNTn		TCNT2
#define HW_OCRnA		OCR2A
#define HW_WGMn1		WGM21
#define HW_CSn0			CS20
#define HW_CSn1			CS21
#define HW_TIMSKn		TIMSK2
#define HW_OCIEnA		OCIE2A

#else
//
//	Firmware has not been configured for this board.
//
#error "Firmware has not been configured for this board"

#endif



//
//	The ROTARY device
//	=================
//
static Rotary		speed_dial( ROTARY_A, ROTARY_B, ROTARY_BUTTON );

//
//	The KEYPAD device.
//
static Keypad_TWI_IO	keypad( KEYPAD_ADDRESS );

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
//	General universal constants
//	===========================
//

//
//	Define the size of a generic small textual buffer for
//	use on the stack.
//
#define TEXT_BUFFER SELECT_SML(8,12,16)

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
//	The following definitions allow for the similarity which is
//	present to be capitalised on and not cause the source code to
//	become unnecessarily convoluted.
//
//	PROT_IN_CHAR	Define the start and end characters of a sentence
//	PROT_OUT_CHAR	in the target operating system mode syntax.
//	
#define PROT_IN_CHAR		'['
#define PROT_OUT_CHAR		']'

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
//	High Level Configuration values.
//	================================
//

//
//	Define the maximum number of bytes in a DCC packet that we
//	can handle.
//
//	Note:	This is different from the number of bit transitions
//		which the system will support.  This figure (see
//		BIT_TRANSITIONS below) is based on a DCC command of
//		just four bytes.
//
#define MAXIMUM_DCC_COMMAND	6

//
//	Define the maximum number of bytes that can be accepted as
//	a complete input command.
//
#define MAXIMUM_DCC_CMD		32

//
//	Define a maximum number of characters that are required to
//	formulate the host reply to a command being received successfully.
//
#define MAXIMUM_DCC_REPLY	16

//
//	Define the number of buffers set aside for each of the
//	use cases for the buffers:
//
//		Accessory and transient commands
//		Mobile decoder speed and direction
//
//	The program allocates buffers as follows (where A represents
//	the number of Accessory buffers and M the corresponding
//	number of mobile buffers):
//
//		0 .. A-1	Accessory, transient DCC packets
//		A .. A+M-1	Mobile, persistent DCC packets
//
#define ACCESSORY_TRANS_BUFFERS	SELECT_SML( 4, 6, 16 )
#define MOBILE_TRANS_BUFFERS	SELECT_SML( 4, 6, 16 )

//
//	Define short hand base buffer numbers for each section.
//
#define ACCESSORY_BASE_BUFFER	0
#define MOBILE_BASE_BUFFER	(ACCESSORY_BASE_BUFFER+ACCESSORY_TRANS_BUFFERS)

//
//	Define the total number of transmission buffers that will
//	form the circular transmission loop.
//
//	This is a composition of the number of buffers allocated
//	to each section.
//
#define TRANSMISSION_BUFFERS	(ACCESSORY_TRANS_BUFFERS+MOBILE_TRANS_BUFFERS)

//
//	Various DCC protocol based values
//
#define MAXIMUM_DCC_ADDRESS	10239
#define MAXIMUM_SHORT_ADDRESS	127
#define MINIMUM_DCC_ADDRESS	1
//
#define MAXIMUM_DCC_SPEED	126
#define MINIMUM_DCC_SPEED	0
#define EMERGENCY_STOP		(-1)
//
#define DCC_FORWARDS		1
#define DCC_BACKWARDS		0
//
//	Internal DCC Accessory addresses.  The address structure
//	as defined in the DCC protocol.
//
#define MIN_ACCESSORY_ADDRESS	0
#define MAX_ACCESSORY_ADDRESS	511
#define MIN_ACCESSORY_SUBADRS	0
#define MAX_ACCESSORY_SUBADRS	3
//
//	External combined DCC accessory address.  The address range
//	as commonly used by external software systems.
//
#define MIN_ACCESSORY_EXT_ADDRESS	1
#define MAX_ACCESSORY_EXT_ADDRESS	2044
//
#define ACCESSORY_ON		1
#define ACCESSORY_OFF		0
//
//	The DCC standard specifies CV values between 1 and 1024,
//	but the actual "on wire" protocol utilised the values 0
//	to 1023.
//
#define MINIMUM_CV_ADDRESS	1
#define MAXIMUM_CV_ADDRESS	1024
//
//	Function numbers within a decoder
//
#define MIN_FUNCTION_NUMBER	0
#define MAX_FUNCTION_NUMBER	28
//
#define FUNCTION_OFF		0
#define FUNCTION_ON		1
#define FUNCTION_TOGGLE		2

//
//	Motor Shield definitions.
//	=========================
//
//	This DCC Generator firmware now supports multiple DCC
//	Districts in addition to an optional programming track.
//
//	This create a selection of coding, performance and optimisation
//	challenges which are all closely related to the type and
//	configuration of the Motor Shield installed upon the Arduino.
//
//	The following definitions provide the higher level guidance
//	to the source code allowing alternative methods and
//	optimisations to be selected as permitted by the installed
//	motor shield.
//
//	SHIELD_OUTPUT_DRIVERS	This is set to the number of independent
//				H-Bridge driver circuits which the
//				motor shield contains.
//
//	The configuration of the motor shield is captured using the following
//	structure definition which should be stored in the Arduino program
//	memory.
//
#define SHIELD_DRIVER struct shield_driver
SHIELD_DRIVER {
	byte		direction,	// Which pin controls the polarity
					// of the output.  When SHIELD_PORT_DIRECT
					// is defined this becomes the binary
					// mask of the bit in that port that
					// controls the direction for this
					// driver (ie bit 3 == 0b00001000)
					// The "bit()" can be used to simplify
					// this definition process.
			enable,		// Which pin is used to enable the
					// output from the driver.
			brake,		// Pin to clear on startup.
			load,		// Pin to read driver loading from.
			analogue;	// Same pin as above but numerically
					// suitable for use with setting up
					// the asynchronous ADC interrupt.
};
#define BRAKE_NOT_AVAILABLE 255


//
//	Arduino Motor Shield
//	--------------------
//

//
//	The following definitions are based on the hardware
//	characteristics of the "Deek-Robot" Motor Shield, which
//	should be compatible with the Arduino version (and others).
//
#define SHIELD_OUTPUT_DRIVERS	2

//
//	A and B drivers available, with 4 pins allocated to each.
//
//	These are the pin numbers "as per the motor shield".
//
//	IMPORTANT:
//
//		You *must* cut the VIN CONNECT traces on the
//		back of the Motor Shield.  You will need to
//		power the shield separately from the Arduino
//		with 15 volts DC, and leaving the VIN CONNECT
//		in place will put 15 volts across the Arduino
//		and probably damage it.
//
//		Previous versions of this firmware (and the DCC++
//		firmware) also required that the BRAKE feature
//		(for both A and B H-Bridges) should be cut too.
//		This is no longer required as this firmware
//		explicitly sets these LOW and are not touched
//		after that.
//
//		Also (unlike the DCC++ firmware) this firmware
//		requires no additional jumpers to support its
//		intended operation.
//
#define SHIELD_DRIVER_A_DIRECTION	12
#define SHIELD_DRIVER_A_ENABLE		3
#define SHIELD_DRIVER_A_BRAKE		9
#define SHIELD_DRIVER_A_LOAD		A0
#define SHIELD_DRIVER_A_ANALOGUE	0

#define SHIELD_DRIVER_B_DIRECTION	13
#define SHIELD_DRIVER_B_ENABLE		11
#define SHIELD_DRIVER_B_BRAKE		8
#define SHIELD_DRIVER_B_LOAD		A1
#define SHIELD_DRIVER_B_ANALOGUE	1

//
//	Define the motor shield attached to the Arduino for
//	the firmware.
//
static const SHIELD_DRIVER shield_output[ SHIELD_OUTPUT_DRIVERS ] PROGMEM = {
	{
		//
		//	Main track (district 1)
		//
		SHIELD_DRIVER_A_DIRECTION,
		SHIELD_DRIVER_A_ENABLE,
		SHIELD_DRIVER_A_BRAKE,
		SHIELD_DRIVER_A_LOAD, SHIELD_DRIVER_A_ANALOGUE
	},
	{
		//
		//	Main track (district 2)
		//
		SHIELD_DRIVER_B_DIRECTION,
		SHIELD_DRIVER_B_ENABLE,
		SHIELD_DRIVER_B_BRAKE,
		SHIELD_DRIVER_B_LOAD, SHIELD_DRIVER_B_ANALOGUE
	}
};

//
//	Timing, Protocol and Data definitions.
//	======================================
//

//
//	The following paragraphs and numerical calculations are
//	based on a spread sheet used to analyse the possible
//	subdivisions of the DCC signal timing (spreadsheet not
//	included).
//
//	The key point of the analysis is to find a common divisor
//	between the duration of half a "1" bit (58 us) and half a
//	"0" bit (100 us) based on the basic clock frequency of
//	the Arduino and ideally giving an interval count sum that
//	fits into an 8-bit interrupt counter.
//
//	The analysis determined that dividing the "1" unit of time
//	(58 us) by 4 gave a period of 14.5 us, which divides into
//	the "0" unit of time into 7 with an acceptable margin of
//	error (~1.5%).
//

//
//	These macros define the number of Interrupt Cycles that the
//	interrupt timer must count before raising an interrupt
//	to create a target interval of 14.5 us.
//
//	This period we are calling a "tick", multiples of which
//	are used to build up the intervals required to create
//	the DCC signal.
//
//	Simplistically the "MHz" clock rate of the board multiplied
//	by the selected "tick" duration (in microseconds) gives the
//	base starting point.
//

#if F_CPU == 16000000
//
//	Arduino Uno (or equivalent)
//
//		16 (MHz) x 14.5 (microseconds) = 232 (clock cycles)
//
//	This fits inside the 8-bit interrupt timer limit (255), so
//	no interrupt pre-scaler is required (so equal to 1).
//
//	Within the DCC standard, the half time of a "1" bit is
//	58 us:
//		4 ticks of 14.5 us are exactly 58 us (0% error).
//
//	Like wise, the nominal half time of a "0" bit is 100 us,
//	so:
//		7 ticks of 14.5 us gives a time of 101.5 us (1.5% error).
//
#define TIMER_INTERRUPT_CYCLES	232
#define TIMER_CLOCK_PRESCALER	1

#else

#if F_CPU == 20000000
//
//	Arduino Mega (or equivalent)
//
//		20 (MHz) x 14.5 (microseconds) = 290 (clock cycles)
//
//	This is too big for the 8 bit timer we are using, so we select
//	the smallest available pre-scaler: 8
//
//		290 (clock cycles) / 8 = 36.25
//
//	Round down to 36 and multiply out to determine the resulting
//	tick interval:
//
//		36 * 8 / 20 = 14.4 us
//
//	This makes the time interval the Interrupt Service Routine
//	must complete in marginally shorter, but is easily offset with
//	the higher intrinsic clock rate.
//
//	Therefore the we get the "half time" of a "1" as:
//
//		4 ticks of 14.4 us is 57.6 us (0.6% error)
//
//	Like wise, the nominal "half time" of a "0" bit is 100 us,
//	so:
//		7 ticks of 14.4 us gives a time of 100.8 us (0.8% error)
//
#define TIMER_INTERRUPT_CYCLES	36
#define TIMER_CLOCK_PRESCALER	8

#else

//
//	The target MCU clock frequency has not been accounted for.
//
#error "MCU Clock speed calculation needs to be calculate for this clock rate."

//
//	The calculations outlined above need to be carried out and appropriate
//	results captured in the definitions of TIMER_INTERRUPT_CYCLES and
//	TIMER_CLOCK_PRESCALER values.
//

#endif
#endif

//
//	The following two macros return the number of interrupt
//	ticks which are required to generate half of a "1" or
//	half of a "0" bit in the DCC signal.
//
#define TICKS_FOR_ONE	4
#define TICKS_FOR_ZERO	7


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
//	Pending DCC Packet data structure.
//	----------------------------------
//
//	The following constant and structure definitions provide
//	a structured and extensible mechanism for inserting one
//	or more (as a series) of packets into a transmission
//	buffer for broadcasting onto either the operations or
//	programming track.
//

//
//	Define the data structure used to hold a single pending
//	DCC packet.  The fields defined are:
//
//	target		The new value for target upon setting up a new
//			bit stream.
//
//	preamble	The number of '1's to send as the lead-in preamble
//			to a DCC command.
//
//	postamble	The number of '1's to send at the end of a DCC
//			command.  This is normally 1, but for programming
//			command this can be used to create a pause in
//			command transmission (required to 'see' any
//			decoder reply).
//
//	duration	This is the new value for duration.
//
//	len		Length of the command in bytes.
//
//	command		This is the series of bytes which form the "byte"
//			version of the command to be sent.
//
//	next		Pointer to next packet to send (or NULL).
//
#define PENDING_PACKET struct pending_packet
PENDING_PACKET {
	int		target;
	byte		preamble,
			postamble,
			duration,
			len,
			command[ MAXIMUM_DCC_COMMAND ];
	PENDING_PACKET	*next;
};

//
//	Define the number of pending buffers which will be available.
//
//	For the moment, we will allocate the same number as there are
//	transmission buffers.  This should be ample as persistent commands
//	will not tie up a pending packet leaving the pending packet
//	buffers mostly free for transient commands which by their nature
//	only tie up pending packets for short periods of time.
//
#define PENDING_PACKETS	TRANSMISSION_BUFFERS

//
//	Define the the pending packet records and the head of the free
//	records list.
//
static PENDING_PACKET pending_dcc_packet[ PENDING_PACKETS ];
static PENDING_PACKET *free_pending_packets;

//
//	Copy a DCC command to a new location and append the parity data.
//	Returns length of data in the target location.
//
static byte copy_with_parity( byte *dest, byte *src, byte len ) {
	byte	p, i;

	ASSERT( len > 0 );

	p = 0;
	for( i = 0; i < len; i++ ) p ^= ( *dest++ = *src++ );
	*dest = p;
	return( len+1 );
}

//
//	Define standard routine to obtain and fill in a pending
//	packet record.
//
static bool create_pending_rec( PENDING_PACKET ***adrs, int target, byte duration, byte preamble, byte postamble, byte len, byte *cmd ) {
	PENDING_PACKET	*ptr, **tail;

	ASSERT( adrs != NULL );
	ASSERT( *adrs != NULL );
	ASSERT( len > 0 );
	ASSERT( len < MAXIMUM_DCC_COMMAND );
	ASSERT( cmd != NULL );
	
	//
	//	We work on being handed the address of the pointer to
	//	the tail of the list of pending records.  Just a simple
	//	triple indirection definition.
	//
	//	We return true if the record has been created and linked
	//	in correctly, false otherwise. 
	//
	if(( ptr = free_pending_packets ) == NULL ) return( false );
	free_pending_packets = ptr->next;
	//
	//	There is a spare record available so fill it in.
	//
	ptr->target = target;
	ptr->preamble = preamble;
	ptr->postamble = postamble;
	ptr->duration = duration;
	ptr->len = copy_with_parity( ptr->command, cmd, len );
	//
	//	Now link it in.
	//
	tail = *adrs;

	ASSERT( *tail == NULL );

	*tail = ptr;
	tail = &( ptr->next );
	*tail = NULL;
	*adrs = tail;
	//
	//	Done.
	//
	return( true );
}

//
//	Define a routine to release one (one=true) or all (one=false) pending packets in a list.
//
static PENDING_PACKET *release_pending_recs( PENDING_PACKET *head, bool one ) {
	PENDING_PACKET	*ptr;

	//
	//	We either release one record and return the address of the remaining
	//	records (one is true) or we release them all and return NULL (one is false).
	//
	while(( ptr = head ) != NULL ) {
		head = ptr->next;
		ptr->next = free_pending_packets;
		free_pending_packets = ptr;
		if( one ) break;
	}
	//
	//	Done.
	//
	return( head );
}

//
//	DCC Accessory Address conversion
//	--------------------------------
//
//	Define a routine which, given an accessory target address and sub-
//	address, returns a numerical value which is the external unified
//	equivalent value.
//

//
//	Define two routines which, given and external accessory number,
//	return the DCC accessory address and sub-address.
//
static int internal_acc_adrs( int target ) {

	ASSERT( target >= MIN_ACCESSORY_EXT_ADDRESS );
	ASSERT( target <= MAX_ACCESSORY_EXT_ADDRESS );

	return((( target - 1 ) >> 2 ) + 1 );
}
static int internal_acc_subadrs( int target ) {

	ASSERT( target >= MIN_ACCESSORY_EXT_ADDRESS );
	ASSERT( target <= MAX_ACCESSORY_EXT_ADDRESS );

	return(( target - 1 ) & 3 );
}

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
//	Define a number of cache records and bytes for bit storage in
//	each record.  We calculate FUNCTION_BIT_ARRAY based on the
//	MIN and MAX function numbers provided (the 7+ ensures correct
//	rounding in boundary cases).
//
//	We will base the function cache size on the maximum number of
//	DCC mobile decoders we can have active in parallel.
//
#define FUNCTION_CACHE_RECS	(MOBILE_TRANS_BUFFERS*2)
#define FUNCTION_BIT_ARRAY	((( 1 + MAX_FUNCTION_NUMBER - MIN_FUNCTION_NUMBER ) + 7 ) >> 3 )

//
//	The structure used to cache function values per decoder so that
//	the "block" function setting DCC packet can be used.
//
#define FUNCTION_CACHE struct func_cache
FUNCTION_CACHE {
	int		target;
	byte		bits[ FUNCTION_BIT_ARRAY ];
	FUNCTION_CACHE	*next,
			**prev;
};
static FUNCTION_CACHE	function_rec[ FUNCTION_CACHE_RECS ],
			*function_cache;
//
//	Function to initialise the cache records empty.
//
//	We will "pre-fill" the cache with empty records so that the code can
//	always assume that there are records in the cache, because there are.
//
static void init_function_cache( void ) {
	FUNCTION_CACHE	**tail, *ptr;
	
	tail = &function_cache;
	for( byte i = 0; i < FUNCTION_CACHE_RECS; i++ ) {
		//
		//	Note current record.
		//
		ptr = function_rec + i;
		//
		//	Empty the record.
		//
		ptr->target = 0;
		for( byte j = 0; j < FUNCTION_BIT_ARRAY; ptr->bits[ j++ ] = 0 );
		//
		//	Link in the record.
		//
		*tail = ptr;
		ptr->prev = tail;
		tail = &( ptr->next );
	}
	*tail = NULL;
}

//
//	Define the lookup and manage cache code.
//
static FUNCTION_CACHE *find_func_cache( int target ) {
	FUNCTION_CACHE	**adrs,
			*last,
			*ptr;

	ASSERT( target >= MINIMUM_DCC_ADDRESS );
	ASSERT( target <= MAXIMUM_DCC_ADDRESS );

	adrs = &function_cache;
	last = NULL;
	while(( ptr = *adrs ) != NULL ) {
		//
		//	Is this the one?
		//
		if( target == ptr->target ) {
			//
			//	Yes, so move to top of the list (if not already there)
			//	so that access to this record is as quick as possible
			//	for subsequent requests.
			//
			if( function_cache != ptr ) {
				//
				//	Detach from the list.
				//
				if(( *( ptr->prev ) = ptr->next )) ptr->next->prev = ptr->prev;
				//
				//	Add to head of list.
				//
				ptr->next = function_cache;
				function_cache->prev = &( ptr->next );
				function_cache = ptr;
				ptr->prev = &function_cache;
			}
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
	//	Replace with new target and empty function settings.
	//
	last->target = target;
	for( byte i = 0; i < FUNCTION_BIT_ARRAY; last->bits[ i++ ] = 0 );
	//
	//	Link onto head of cache.
	//
	last->next = function_cache;
	function_cache->prev = &( last->next );
	function_cache = last;
	last->prev = &function_cache;
	//
	//	Done.
	//
	return( last );
}

//
//	Routine applies a boolean value for a specified function
//	on a specified target number.
//
//
static bool update_function( int target, byte func, bool state ) {
	FUNCTION_CACHE	*ptr;
	byte		i, b; 

	ASSERT( func <= MAX_FUNCTION_NUMBER );

	ptr = find_func_cache( target );
	i = ( func - MIN_FUNCTION_NUMBER ) >> 3;
	b = 1 << (( func - MIN_FUNCTION_NUMBER ) & 7 );

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
static byte get_function( int target, byte func, byte val ) {
	FUNCTION_CACHE	*ptr;
	byte		i, b; 

	ASSERT( func <= MAX_FUNCTION_NUMBER );

	ptr = find_func_cache( target );
	i = ( func - MIN_FUNCTION_NUMBER ) >> 3;
	b = 1 << (( func - MIN_FUNCTION_NUMBER ) & 7 );

	if( ptr->bits[ i ] & b ) return( val );
	return( 0 );
}

//
//	Storage of Transmission Data
//	----------------------------
//
//	Output bit patterns are stored as an array of bytes
//	where alternate bytes indicate the number of "1"s or "0"s
//	to output (array index 0 represents an initial series of
//	"1"s).
//
//	While this method is less space efficient than simply
//	storing the bytes required, it has the benefit that the
//	inter-byte protocol "0"s and "1"s are easily captured, and
//	interrupt code required to generate a DCC packet has fewer
//	special case requirements and so will be faster.
//
//	This inevitably (and correctly) offloads processing work
//	from the interrupt routine to the non-time-critical general
//	IO code.
//

//
//	Define limits on the number of bit transitions which a
//	transmission buffer can contain.
//
//	The maximum size for a DCC command that this program can
//	process is defined as MAXIMUM_DCC_COMMAND, however this is
//	and extreme case, and so the bit transition array will be
//	based (for the moment) on a series of 4 bytes.
//
//	Worst case scenario for a DCC packet is a long sequence of
//	bytes of either 0xAA or 0x55 (individually alternating bits).
//
//	In this case the bit array would be filled thus:
//
//	Content					Ones	Zeros
//	-------					----	-----
//	Packets header of 'X' "1"s + 1 "0"	X	1
//	(X is selected preamble length)
//
//	Byte 0xAA + 1 "0"			1	1
//						1	1
//						1	1
//						1	2
//
//	Byte 0xAA + 1 "0"			1	1
//						1	1
//						1	1
//						1	2
//
//	Byte 0xAA + 1 "0"			1	1
//						1	1
//						1	1
//						1	2
//
//	Checksum 0xAA + 1 "1"			1	1
//						1	1
//						1	1
//						1	1
//
//	End of packet "1"			1
//	
//
//	In this situation a minimum 36 byte bit transition array would be
//	required for the data (remember additional space required for end
//	zero byte).
//
//	Statistically this would normally hold a larger DCC packet than
//	this.
//
#define BIT_TRANSITIONS		SELECT_SML( 36, 48, 64 )

//
//	Define maximum bit iterations per byte of the bit transition array.
//
//	It is possible to prove that this figure can never be reached as this
//	would imply a a series of 28 byte containing just 0 bits which (apart
//	from being an invalid DCC command, is over 4 times longer than the
//	longest DCC command this code will handle.
//
//	This value is applied inside verification assert statements.
//
#define MAXIMUM_BIT_ITERATIONS	255

//
//	Define the state information which is used to control the transmission
//	buffers.
//
//	The states are divided into a number of groups reflecting which section
//	of code monitors or controls it.  The concept here is that (like a game
//	of "ping pong") the responsibility for the buffers is handed back and
//	forth between the bits of code.  The intention here is to avoid requiring
//	locked out sections of code (using noInterrupts() and interrupts()) which
//	might impact the over all timing of the firmware.
//
//	In the following table:
//
//		State		The name of the state in this code.
//		Monitor		Which Code section operates in this state
//				(i.e. acts upon a buffer in that state).
//		Role		Briefly what is going when a buffer is moved
//				from its initial to result state).
//		Result		State into which the code moves the buffer.
//
//	Trying to capture state transitions here:
//
//	State		Monitor		Role			Result
//	-----		-------		----			------
//	TBS_RUN		ISR		Continuous packet	TBS_RUN
//					transmission
//					(duration is zero)
//
//	TBS_RUN		ISR		Limited packet		TBS_RUN
//					transmission
//					(--duration > 0 )
//
//	TBS_RUN		ISR		Limited packet		TBS_LOAD
//					transmission
//					(--duration == 0 )
//
//	TBS_LOAD	Manager		Pending pointer not	TBS_RUN
//					NULL, load next bit
//					sequence from record
//					and drop pending record
//
//	TBS_LOAD	Manager		Pending pointer is	TBS_EMPTY
//					NULL.
//
//	TBS_EMPTY	IO		Create a list of	TBS_LOAD
//					pending records and
//					attach to a buffer
//
//	TBS_RELOAD	ISR		Synchronised load	TBS_LOAD
//					initiated by IO code
//					on a TBS_RUN buffer
//
#define TBS_EMPTY	0
#define TBS_LOAD	1
#define TBS_RUN		2
#define TBS_RELOAD	3

//
//	Define a Transmission Buffer which contains the following
//	elements:
//
//	state		The current state of this buffer.  Primary method
//			synchronise activities between the interrupt driven
//			signal generator code and the iteratively called
//			management code.
//
//	Live Transmission fields:
//	-------------------------
//
//	target		The DCC ID being targetted by the buffer:
//			+ve -> mobile ID, -ve ->  negated External
//			Accessory ID, 0 Broadcast address or programming
//			track.
//
//	duration	Provides a mechanism between the main IO code
//			and the Interrupt driven DCC transmission code
//			for scheduling the disabling the buffer.
//
//			If set as zero the buffer will run continuously.
//
//			If set to a non-zero value, the content
//			of duration will be decreased each time the buffer
//			is sent.  When it reaches zero the buffer is
//			then disabled and made empty.
//
//	bits		The bit definition of a the DCC command string which
//			is to be broadcast as a series of 1/0 transitions.
//			Terminated with a zero byte.
//
//	Pending Transmission Fields:
//	----------------------------
//
//	pending		Address of the pending data record containing the next
//			command to load after this one is completed.  There is
//			an implication that if duration is 0 then this must
//			be NULL.
//
//	Confirmation reply data.
//	------------------------
//
//	reply_on_send	Define how the system should manage/generate a reply
//			to the host computer.  Set to false for no reply
//			required or true for reply at end of command.
//
//	contains	The (EOS terminated) reply string which should
//			form the reply sent (if requested).
//
//	Link to the next buffer, circular fashion:
//	------------------------------------------
//
//	next		The address of the next transmission buffer in
//			the loop
//
#define TRANS_BUFFER struct trans_buffer
TRANS_BUFFER {
	//
	//	The current state of this buffer.  Primary method used to
	//	synchronise activities between the interrupt driven signal
	//	generator code and the iteratively called management code.
	//
	byte		state;
	//
	//	Live Transmission fields:
	//	-------------------------
	//
	//
	//	The target ID of the packet.  Note the following usage:
	//
	//		target < 0	Accessory Decoder (negate to get ID)
	//		target == 0	Broadcast address
	//		target > 0	Mobile Decoder
	//
	int		target;
	//
	//	The duration countdown (if non-zero).  An initial value
	//	of zero indicates no countdown meaning the packet is transmitted
	//	indefinitely.
	//
	byte		duration;
	//
	//	The bit pattern to transmit.  This is only filled
	//	from the command field by the buffer management code
	//	so that it can be synchronised with operation of the
	//	interrupt routine.
	//
	//	Bit transitions are zero byte terminated, no length
	//	need be maintained.
	//
	byte		bits[ BIT_TRANSITIONS ];
	//
	//	Pending Transmission Fields:
	//	----------------------------
	//
	//	Address of the next pending DCC command to send, NULL
	//	if nothing to send after this bit pattern.
	//
	PENDING_PACKET	*pending;

	//
	//	Confirmation reply data.
	//	------------------------
	//
	//	When is a reply required and what does that reply contain?  This
	//	is applied only at the end of a series of pending records (i.e. when
	//	pending is NULL).
	//
	bool	reply_on_send;
	char	contains[ MAXIMUM_DCC_REPLY ];

	//
	//	Buffer linkage.
	//	---------------
	//
	//	Finally, the link to the next buffer
	//
	TRANS_BUFFER	*next;
};

//
//	Statistic collection variables
//	------------------------------
//
//	Variables conditionally defined when the statistics collection
//	has been compiled in.
//

static int	lcd_statistic_packets;



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
static bool backfill_int_to_text( char *buf, int v, byte len ) {
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
static bool backfill_byte_to_text( char *buf, byte v, byte len ) {

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


#if 0

//
//	Ignored as not used in the code at the moment.
//
		//
		//	Now front fill a value with unsigned byte, return number of
		//	character used or 0 on error.
		//
		static byte byte_to_text( char *buf, byte v, byte len ) {
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
#endif


//
//	Current monitoring code.
//	========================
//
//	With thanks to Nick Gammon and his post in the thread which can be
//	found at "http://www.gammon.com.au/forum/?id=11488&reply=5#reply5"
//	for those crucial first few steps that enabled this asynchronous ADC
//	implementation.
//

//
//	Global variables used to interact with the ADC interrupt service routine.
//
static volatile int	track_load_reading;
static volatile bool	reading_is_ready;

//
//	Macro generating code to restart the ADC process.  These could now
//	probably be subroutines as the earlier requirement for keeping
//	execution time to a minimum has been removed.
//
#define RESTART_ANALOGUE_READ()		track_load_reading=0;reading_is_ready=false;ADCSRA|=bit(ADSC)|bit(ADIE)
//
//	Macro to set the input pin the ADC will continuously read until
//	called to read another pin.
//
#define MONITOR_ANALOGUE_PIN(p)		ADMUX=bit(REFS0)|((p)&0x07);RESTART_ANALOGUE_READ()

//
//	Define the Interrupt Service Routine (ISR) which will
//	read the data and store it.  Restarting another ADC
//	conversion is done elsewhere.
//
ISR( ADC_vect ) {
	byte	low, high;

	//
	//	We have to read ADCL first; doing so locks both ADCL
	//	and ADCH until ADCH is read.  reading ADCL second would
	//	cause the results of each conversion to be discarded,
	//	as ADCL and ADCH would be locked when it completed.
	//
	low = ADCL;
	high = ADCH;
	//
	//	Store the reading away flagging that it is available.
	//
	track_load_reading = ( high << 8 ) | low;
	reading_is_ready = true;
}

//
//	The interrupt driven DCC packet transmission code.
//	==================================================
//

//
//	Define the Circular buffer.
//
static TRANS_BUFFER circular_buffer[ TRANSMISSION_BUFFERS ];

//
//	The following variables direct the actions of the interrupt
//	routine.
//

//
//	Define the pointer into the circular buffer
//	used (and updated by) the interrupt routine.
//
//	This always points to the buffer currently being transmitted.
//
static TRANS_BUFFER	*current;

//
//	Define variables to control the direction output
//
//	If we are not using direct port access then we define
//	a short array (with associated length) of the pin numbers
//	which need to be flipped.
//
//	output_pin		The individual pin numbers
//
//	output_phase		"in phase" or "anti-phase"
//
//	output_pins		Number of pins being controlled
//
static byte		output_pin[ SHIELD_OUTPUT_DRIVERS ];
static bool		output_phase[ SHIELD_OUTPUT_DRIVERS ];
static byte		output_pins;					/// JEFF we can remove this!

//
//	"side" flips between true and false and lets the routine know
//	which "side" of the signal was being generated.
//
static byte		side;

//
//	"remaining" The number of ticks before the next "side" transition.
//
//	"reload" is the value that should be reloaded into remaining if we are
//	only half way through generating a bit.
//
static byte		remaining,
			reload;

//
//	"one" is a boolean variable which indicates if we are currently
//	transmitting a series of ones (true) or zeros (false).
//
//	"left" is the number of ones or zeros which still need to be sent
//	before we start transmitting the next series of zeros or ones (or
//	the end of this bit transmission).
//
static byte		one,
			left;

//
//	This is the pointer the interrupt routine uses to collect the
//	bit stream data from inside the transmission buffer.  This can
//	point to data outside the bit buffer if the current buffer
//	contains no valid bit stream to transmit.
//
static byte		*bit_string;

//
//	The following array of bit transitions define the "DCC Idle Packet".
//
//	This packet contains the following bytes:
//
//		Address byte	0xff
//		Data byte	0x00
//		Parity byte	0xff
//
//	This is translated into the following bit stream:
//
//		1111...11110111111110000000000111111111
//
static byte dcc_idle_packet[] = {
	DCC_SHORT_PREAMBLE,	// 1s
	1,			// 0s
	8,			// 1s
	10,			// 0s
	9,			// 1s
	0
};

//
//	The following array does not describe a DCC packet, but a
//	filler of a single "1" which is required while working
//	with decoders in service mode.
//
static byte dcc_filler_data[] = {
	1,			// 1s
	0
};

//
//	The Interrupt Service Routine which generates the DCC signal.
//
ISR( HW_TIMERn_COMPA_vect ) {
	//
	//	The interrupt routine should be as short as possible, but
	//	in this case the necessity to drive forwards the output
	//	of the DCC signal is paramount.  So (while still time
	//	critical) the code has some work to achieve.
	//
	//	If "remaining" is greater than zero we are still counting down
	//	through half of a bit.  If it reaches zero it is time to
	//	flip the signal over.
	//
	if(!( --remaining )) {
		//
		//	Time is up for the current side.  Flip over and if
		//	a whole bit has been transmitted, the find the next
		//	bit to send.
		//
		//	We "flip" the output DCC signal now as this is the most
		//	time consistent position to do so.
		//

		//
		//	Code supporting the Arduino Motor Shield hardware where
		//	all pins need to individually flipped.
		//
		//	The data necessary to do the task is gathered into the two
		//	arrays output_pin[] (containing the actual pin numbers to
		//	change) and output_phase[] which details if the pin is in/out of
		//	phase.  output_pins gives the number of pins which are captured
		//	in these arrays.
		//
		{
			//
			//	We run through the array as fast as possible.
			//
			register byte *op, oc;
			register bool *ob;

			op = output_pin;
			oc = output_pins;
			ob = output_phase;
			//
			//	Replicated code is used to remove unnecessary computation
			//	from inside the loop to maximise speed through the pin
			//	adjustments.
			//
			//	Question: Can we remove the "?:" code and use the boolean
			//	value in the output_phase array directly?  Maybe, but not yet.
			//
			//	Use side to select broad logic choice..
			//
			if( side ) {
				while( oc-- ) digitalWrite( *op++, ( *ob++? HIGH: LOW ));
			}
			else {
				while( oc-- ) digitalWrite( *op++, ( *ob++? LOW: HIGH ));
			}
		}

		//
		//	Now undertake the logical flip and subsequent actions.
		//
		if(( side = !side )) {
			//
			//	Starting a new bit, is it more of the same?
			//
			if(!( --left )) {
				//
				//	No! It is now time to output a series
				//	of the alternate bits (assignment intentional).
				//
				if(( left = *bit_string++ )) {
					//
					//	More bits to send.
					//
					//	Select the correct tick count for the next
					//	next bit (again the assignment is intentional).
					//
					reload = ( one = !one )? TICKS_FOR_ONE: TICKS_FOR_ZERO;
				}
				else {
					//
					//	There are no more bits to transmit
					//	from this buffer, but before we move
					//	on we check the duration flag and act
					//	upon it.
					//
					//	If the current buffer is in RUN mode and duration
					//	is greater than 0 then we decrease duration and
					//	if zero, reset state to LOAD.  This will cause the
					//	buffer management code to check for any pending
					//	DCC commands.
					//
					if( current->duration && ( current->state == TBS_RUN )) {
						if(!( --current->duration )) current->state = TBS_LOAD;
					}
					//
					//	Move onto the next buffer.
					//
					current = current->next;

					//
					//	Count a successful packet transmission for LCD display
					//
					lcd_statistic_packets++;

					//
					//	Actions related to the current state of the new
					//	buffer (select bits to output and optional state
					//	change).
					//
					switch( current->state ) {
						case TBS_RUN: {
							//
							//	We just transmit the packet found in
							//	the bit data
							//
							bit_string = current->bits;
							break;
						}
						case TBS_RELOAD: {
							//
							//	We have been asked to drop this buffer
							//	so we output an idle packet while changing
							//	the state of buffer to LOAD so the manager
							//	can deal with it.
							//
							bit_string = dcc_idle_packet;
							current->state = TBS_LOAD;
							break;
						}
						case TBS_LOAD: {
							//
							//	This is a little tricky.  While we do not
							//	(and cannot) do anything with a buffer in
							//	load state, there is a requirement for the
							//	signal generator code NOT to output an idle
							//	packet if we are in the middle of a series
							//	of packets on the programming track.
							//
							//	To this end, if (and only if) the pending
							//	pointer is not NULL, then we will output
							//	the dcc filler data instead of the idle
							//	packet
							//
							bit_string = current->pending? dcc_filler_data: dcc_idle_packet;
							break;
						}
						default: {
							//
							//	If we find any other state we ignore the
							//	buffer and output an idle packet.
							//
							bit_string = dcc_idle_packet;
							break;
						}
					}
					//
					//	Initialise the remaining variables required to
					//	output the selected bit stream.
					//
					one = true;
					reload = TICKS_FOR_ONE;
					left = *bit_string++;
				}
			}
		}
		//
		//	Reload "remaining" with the next half bit
		//	tick count down from "reload".  If there has been
		//	a change of output bit "reload" will already have
		//	been modified appropriately.
		//
		remaining = reload;
	}
	//
	//	In ALL cases this routine needs to complete in less than TIMER_INTERRUPT_CYCLES
	//	(currently 232 for a 16 MHz machine).  This is (huge guestimation) approximately
	//	100 actual instructions (assuming most instructions take 1 cycle with some taking
	//	2 or 3).
	//
	//	The above code, on the "longest path" through the code (when moving
	//	between transmission buffers) I am estimating that this uses no more
	//	than 50 to 75% of this window.
	//
	//	This routine would be so much better written in assembler when deployed
	//	on an AVR micro-controller, however this C does work and produces the
	//	required output signal with no loss of accuracy.
	//	
}

//
//	DCC Packet to bit stream conversion routine.
//	--------------------------------------------
//

//
//	Define a routine which will convert a supplied series of bytes
//	into a DCC packet defined as a series of bit transitions (as used
//	in the transmission buffer).
//
//	This routine is not responsible for the meaning (valid or otherwise)
//	of the bytes themselves, but *is* required to construct the complete
//	DCC packet formation including the preamble and inter-byte bits.
//
//	The code assumes it is being placed into a buffer of BIT_TRANSITIONS
//	bytes (as per the transmission buffers).
//
//	Returns true on success, false otherwise.
//
static bool pack_command( byte *cmd, byte clen, byte preamble, byte postamble, byte *buf ) {
	byte	l, b, c, v, s;

	ASSERT( preamble >= DCC_SHORT_PREAMBLE );
	ASSERT( postamble >= 1 );

	//
	//	Start with a preamble of "1"s.
	//
	*buf++ = preamble;
	l = BIT_TRANSITIONS-1;

	//
	//	Prime pump with the end of header "0" bit.
	//
	b = 0;			// Looking for "0"s. Will
				// be value 0x80 when looking
				// for "1"s.
	c = 1;			// 1 zero found already.

	//
	//	Step through each of the source bytes one at
	//	a time..
	//
	while( clen-- ) {
		//
		//	Get this byte value.
		//
		v = *cmd++;
		//
		//	Count off the 8 bits in v
		//
		for( s = 0; s < 8; s++ ) {
			//
			//	take bits from from the MSB end
			//
			if(( v & 0x80 ) == b ) {
				//
				//	We simply increase the bit counter.
				//
				if( c == MAXIMUM_BIT_ITERATIONS ) return( false );
				c++;
			}
			else {
				//
				//	Time to flip to the other bit.
				//
				if(!( --l )) return( false );

				*buf++ = c;
				c = 1;
				b ^= 0x80;
			}
			//
			//	Shift v over for the next bit.
			//
			v <<= 1;
		}
		//
		//	Now add inter-byte bit "0", or end of packet
		//	bit "1" (clen will be 0 on the last byte).
		//	Remember we use the top bit in b to indicate
		//	which bit we are currently counting.
		//
		if(( clen? 0: 0x80 ) == b ) {
			//
			//	On the right bit (which ever bit that
			//	is) so we simply need to add to the
			//	current bit counter.
			//
			if( c == MAXIMUM_BIT_ITERATIONS ) return( false );
			c++;
		}
		else {
			//
			//	Need the other bit so save the count so
			//	far then flip to the other bit.
			//
			if(!( --l )) return( false );

			*buf++ = c;
			c = 1;
			b ^= 0x80;
		}
	}
	//
	//	Finally place the bit currently being counted, which
	//	must always be a "1" bit (end of packet marker).
	//

	ASSERT( b == 0x80 );
	
	if(!( --l )) return( false );

	//
	//	Here we have the post amble to append.  Rather than
	//	abort sending the command if adding the postamble
	//	exceeds the value of MAXIMUM_BIT_ITERATIONS, we will
	//	simply trim the value down to MAXIMUM_BIT_ITERATIONS.
	//
	if(( b = ( MAXIMUM_BIT_ITERATIONS - c )) < postamble ) postamble = b;
	c += postamble;
	
	*buf++ = c;
	//
	//	Mark end of the bit data.
	//
	if(!( --l )) return( false );

	*buf = 0;
	return( true );
}

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

#define REPLY_BUFFER struct reply_buffer
REPLY_BUFFER {
	char	*ptr;
	byte	left;
};

static bool _reply_in( REPLY_BUFFER *buf, char code ) {
	if( buf->left > 2 ) {
		*buf->ptr++ = PROT_IN_CHAR;
		*buf->ptr++ = code;
		buf->left -= 2;
		return( true );
	}
	return( false );
}

static bool _reply_out( REPLY_BUFFER *buf ) {
	if( buf->left > 2 ) {
		*buf->ptr++ = PROT_OUT_CHAR;
		*buf->ptr++ = NL;
		*buf->ptr = EOS;
		buf->left -= 2;
		return( true );
	}
	return( false );
}

static bool _reply_char( REPLY_BUFFER *buf, char c ) {
	if( buf->left > 1 ) {
		*buf->ptr++ = c;
		buf->left -= 1;
		return( true );
	}
	return( false );
}
static bool _reply_int( REPLY_BUFFER *buf, int v ) {
	char	r[ TEXT_BUFFER ];
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
		if( buf->left < 2 ) return( false );
		*buf->ptr++ = MINUS;
		buf->left -= 1;
	}
	if( buf->left <= c ) return( false );
	buf->left -= c;
	while( c ) *buf->ptr++ = '0' + r[ --c ];
	return( true );
}


static bool reply_1( char *buf, byte len, char code, int a1 ) {
	REPLY_BUFFER	brec;
	
	brec.ptr = buf;
	brec.left = len;
	
	return(
		_reply_in( &brec, code ) &&
		_reply_int( &brec, a1 ) &&
		_reply_out( &brec )
	);
}

static bool reply_2( char *buf, byte len, char code, int a1, int a2 ) {
	REPLY_BUFFER	brec;
	
	brec.ptr = buf;
	brec.left = len;
	
	return(
		_reply_in( &brec, code )	&&
		_reply_int( &brec, a1 )		&&
		_reply_char( &brec, SPACE )	&&
		_reply_int( &brec, a2 )		&&
		_reply_out( &brec )
	);
}

static bool reply_3( char *buf, byte len, char code, int a1, int a2, int a3 ) {
	REPLY_BUFFER	brec;
	
	brec.ptr = buf;
	brec.left = len;
	
	return(
		_reply_in( &brec, code )	&&
		_reply_int( &brec, a1 )		&&
		_reply_char( &brec, SPACE )	&&
		_reply_int( &brec, a2 )		&&
		_reply_char( &brec, SPACE )	&&
		_reply_int( &brec, a3 )		&&
		_reply_out( &brec )
	);
}

static bool reply_n( char *buf, byte len, char code, int n, int *a ) {
	REPLY_BUFFER	brec;
	
	brec.ptr = buf;
	brec.left = len;
	
	if( !_reply_in( &brec, code )) return( false );
	while( n-- ) {
		if( !_reply_int( &brec, *a++ )) return( false );
		if( n ) if( !_reply_char( &brec, SPACE )) return( false );
	}
	return( _reply_out( &brec ));
}

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

		char	buffer[ ERROR_OUTPUT_BUFFER ];
	
		//
		//	Build error report, and send it only if there
		//	is enough space.
		//
		if( reply_2( buffer, ERROR_OUTPUT_BUFFER, 'E', error, arg )) {
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
			if( console.space() >= strlen((const char *)buffer )) {
				FIRMWARE_OUTPUT( console.print( buffer ));
				errors.drop_error();
			}
		}
	}
}

//
//	Current/Load monitoring system
//	==============================
//

//
//	We are going to use an array of values compounded upon
//	each other to generate a series of average values which
//	represent averages over longer and longer periods (each
//	element of the array twice as long as the previous).
//
//	Define the size of the array.
//
#define COMPOUNDED_VALUES	10

//
//	Define the size of the "spike average" we will use to
//	identify a critical rise in power consumption signifying
//	a short circuit.  This is an index into the
//	compounded average table.
//
#define SPIKE_AVERAGE_VALUE	1

//
//	Define the size of the "short average" we will use to
//	identify a genuine rise in power consumption signifying
//	a confirmation return.  This is an index into the
//	compounded average table.
//
#define SHORT_AVERAGE_VALUE	2

//
//	Define a structure used to track one of the drivers
//	that the firmware is managing.  This contains the state
//	of that driver and the compounded averaging system.
//
//	compound_value:	The array of compound average values for the
//			specified driver.
//
//	status:		Enumeration representing the current status of
//			the driver.  The status values are:
//
//			DRIVER_ON_GRACE	Output is ON, but operating inside
//					the "power on grace period" where
//					overloading is ignored before being
//					moved to the DRIVER_ON state.
//			DRIVER_ON	Output is good, nothing to do.
//			DRIVER_FLIPPED	Output phase has been swapped,
//					recheck has been set.  If nothing
//					changes by recheck time then move
//					driver back to DRIVER_ON_GRACE.
//			DRIVER_BLOCKED	Output phase *wants* to be changed,
//					but another district has locked
//					exclusive access to do so.
//			DRIVER_OFF	Output is disabled and will be
//					retried at recheck time.
//			DRIVER_DISABLED	This driver is not currently being
//					used for the creation of a DCC
//					signal.
//
//	recheck:	If non-zero then this driver has been modified
//			in response to a power condition.  This is the
//			"future time" at which this needs to be reviewed.
//
#define DRIVER_STATUS enum driver_status
DRIVER_STATUS {
	DRIVER_ON,
	DRIVER_ON_GRACE,
	DRIVER_FLIPPED,
	DRIVER_BLOCKED,
	DRIVER_OFF,
	DRIVER_DISABLED
};
#define DRIVER_LOAD struct driver_load
DRIVER_LOAD {
	word		compound_value[ COMPOUNDED_VALUES ];
	DRIVER_STATUS	status;
	unsigned long	recheck;
};

//
//	Define the array of structures used to track each of the
//	drivers loads.
//
static DRIVER_LOAD	output_load[ SHIELD_OUTPUT_DRIVERS ];

//
//	This is the phase flipping code lock flag.  Normally
//	NULL, set to the address of a load record when a district
//	is trying a phase inversion to resolve a short situation.
//
static DRIVER_LOAD	*flip_lock = NULL;

//
//	Keep an index into the output_load array so that each of
//	the drivers can have its load assessed in sequence.
//
static byte		output_index;

//
//	This routine is called during setup() to ensure the
//	driver load code is initialised and the first ADC
//	is initialised.
//
static void init_driver_load( void ) {
	//
	//	Ensure all monitoring data is empty
	//
	for( byte d = 0; d < SHIELD_OUTPUT_DRIVERS; d++ ) {
		for( byte c = 0; c < COMPOUNDED_VALUES; c++ ) {
			output_load[ d ].compound_value[ c ] = 0;
		}
		output_load[ d ].status = DRIVER_DISABLED;
		output_load[ d ].recheck = 0;
	}
	//
	//	Start checking current on the first driver.
	//
	output_index = 0;
	MONITOR_ANALOGUE_PIN( progmem_read_byte( shield_output[ output_index ].analogue ));
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
//	Power selection and control code.
//	---------------------------------
//
//	Code to manage track power on and off.
//

static bool power_state = false;

//
//	Routine to turn ON the operating track.
//	Return true if this actually changed the
//	state of the power.
//
//
static void power_on_tracks( void ) {
	//
	//	Set up the output pin array for the main track.
	//
	output_pins = 0;
	for( byte i = 0; i < SHIELD_OUTPUT_DRIVERS; i++ ) {
		//
		//	Enable the specific operating track driver..
		//
		digitalWrite( progmem_read_byte( shield_output[ i ].enable ), HIGH );
		output_load[ i ].status = DRIVER_ON_GRACE;
		output_load[ i ].recheck = millis() + POWER_GRACE_PERIOD;
		//
		//	Add the direction pin to the output array..
		//
		output_pin[ output_pins ] = progmem_read_byte( shield_output[ i ].direction );
		//
		//	Kick off in the "normal" phase alignment.
		//
		output_phase[ output_pins ] = true;
		//
		//	..and then (after updating the arrays) increase the pin count.
		//
		output_pins++;
		//
		//	Clear load array.
		//
		for( byte j = 0; j < COMPOUNDED_VALUES; j++ ) {
			output_load[ i ].compound_value[ j ] = 0;
		}
		output_load[ i ].recheck = 0;
	}
	report_driver_status();
	power_state = true;
}

//
//	Routine to power OFF tracks, return true
//	if this actually changed the state of the
//	power.
//
static void power_off_tracks( void ) {
	//
	//	Mark the output pin array as empty.
	//
	output_pins = 0;

	for( byte i = 0; i < SHIELD_OUTPUT_DRIVERS; i++ ) {
		digitalWrite( progmem_read_byte( shield_output[ i ].enable ), LOW );
		for( byte j = 0; j < COMPOUNDED_VALUES; j++ ) {
			output_load[ i ].compound_value[ j ] = 0;
		}
		output_load[ i ].status = DRIVER_DISABLED;
		output_load[ i ].recheck = 0;
	}
	report_driver_status();
	power_state = false;
}

//
//	Buffer Control and Management Code.
//	-----------------------------------
//

//
//	The following variable is used by the "Management Processing" code which
//	continuously loops through the transmission buffers looking for buffers
//	which need some processing.
//
static TRANS_BUFFER	*manage;

//
//	This is the routine which controls (and synchronises with the interrupt routine)
//	the transition of buffers between various state.
//
static void management_service_routine( void ) {
	//
	//	This routine only handles the conversion of byte encoded DCC packets into
	//	bit encoded packets and handing the buffer off to the ISR for transmission.
	//
	//	Consequently we are only interested in buffers with a state of LOAD.
	//
	if( manage->state == TBS_LOAD ) {
		PENDING_PACKET	*pp;

		//
		//	Pending DCC packets to process? (assignment intentional)
		//
		if(( pp = manage->pending )) {
			//
			//	Our only task here is to convert the pending data into live
			//	data and set the state to RUN.
			//
			if( pack_command( pp->command, pp->len, pp->preamble, pp->postamble, manage->bits )) {
				//
				//	Good, set up the remainder of the live parameters.
				//
				manage->target = pp->target;
				manage->duration = pp->duration;
				//
				//	We set state now as this is the trigger for the
				//	interrupt routine to start processing the content of this
				//	buffer (so everything must be completed before hand).
				//
				//	We have done this in this order to prevent a situation
				//	where the buffer has state LOAD and pending == NULL, as
				//	this might (in a case of bad timing) cause the ISR to output
				//	an idle packet when we do not want it to.
				//
				manage->state = TBS_RUN;
				//
				//	Now we dispose of the one pending record we have used.
				//
				manage->pending = release_pending_recs( manage->pending, true );
				//
				//	Finally, if this had a "reply on send" confirmation and
				//	the command we have just lined up is the last one in the
				//	list, then send the confirmation now.
				//
				if( manage->reply_on_send &&( manage->pending == NULL )) {
					if( !FIRMWARE_OUTPUT( console.print( manage->contains ))) errors.log_error( COMMAND_REPORT_FAIL, manage->target );
					manage->reply_on_send = false;
				}
			}
			else {
				//
				//	Failed to complete as the bit translation failed.
				//
				errors.log_error( BIT_TRANS_OVERFLOW, manage->pending->target );
				//
				//	We push this buffer back to EMPTY, there is nothing
				//	else we can do with it.
				//
				manage->state = TBS_EMPTY;
				//
				//	Finally, we scrap all pending records.
				//
				manage->pending = release_pending_recs( manage->pending, false );
			}
		}
		else {
			//
			//	The pending field is empty.  Before marking the buffer as empty
			//	for re-use, we should check to see if a confirmation is required.
			//
			if( manage->reply_on_send && !FIRMWARE_OUTPUT( console.print( manage->contains ))) errors.log_error( COMMAND_REPORT_FAIL, manage->target );
			//
			//	Now mark empty.
			//
			manage->reply_on_send = false;
			manage->state = TBS_EMPTY;
		}
	}
	//
	//	Finally, before we finish, remember to move onto the next buffer in the
	//	circular queue.
	//
	manage = manage->next;
}

//
//	Initial buffer configuration routine and post-init
//	reconfiguration routines (for either main or programming
//	use).
//
static void link_buffer_chain( void ) {
	int	i;
	
	//
	//	All buffers flagged as empty.
	//
	for( i = 0; i < TRANSMISSION_BUFFERS; i++ ) {
		circular_buffer[ i ].state = TBS_EMPTY;
		circular_buffer[ i ].target = 0;
		circular_buffer[ i ].duration = 0;
		circular_buffer[ i ].bits[ 0 ] = 0;
		circular_buffer[ i ].pending = NULL;
	}
	//
	//	Link up *all* the buffers into a loop in numerical order.
	//
	//	If the programming track is not enabled, then this is the
	//	only time the circular buffer is formed, and must include
	//	all the buffers.
	//
	for( i = 0; i < TRANSMISSION_BUFFERS-1; i++ ) circular_buffer[ i ].next = circular_buffer + ( i + 1 );
	//
	//	point the tail to the head
	//
	circular_buffer[ TRANSMISSION_BUFFERS-1 ].next = circular_buffer;
}


//
//	Firmware initialisation routines
//	--------------------------------
//

//
//	Define the circular buffer and interrupt variable initialisation routine.
//
static void initialise_data_structures( void ) {
	int	i;

	//
	//	We start all buffer circularly linked.
	//
	link_buffer_chain();
	//
	//	Initialise the pending packets structures.
	//
	free_pending_packets = NULL;
	for( i = 0; i < PENDING_PACKETS; i++ ) {
		pending_dcc_packet[ i ].target = 0;
		pending_dcc_packet[ i ].duration = 0;
		pending_dcc_packet[ i ].len = 0;
		pending_dcc_packet[ i ].command[ 0 ] = EOS;
		pending_dcc_packet[ i ].next = free_pending_packets;
		free_pending_packets = &( pending_dcc_packet[ i ]);
	}
	//
	//	Now prime the transmission interrupt routine state variables.
	//
	current = circular_buffer;

	//
	//	Make sure the "pin out" data is empty as we are initially not
	//	driving current to any track.
	//
	output_pins = 0;

	side = true;
	remaining = 1;
	bit_string = dcc_idle_packet;
	one = true;
	reload = TICKS_FOR_ONE;
	left = *bit_string++;
	//
	//	Now prime the management code
	//
	manage = circular_buffer;
}

//
//	This is more useful now that multiple devices are present on the I2C bus.
//
void twi_error_callback( byte error ) {
	errors.log_error( I2C_COMMS_ERROR, error );
}

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
static bool display_status = true;

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
		for( byte f = MIN_FUNCTION_NUMBER; f <= MAX_FUNCTION_NUMBER; f++ ) {
			if( get_function( this_object->adrs, f, true )) {
				if( left ) {
					//
					//	left column.
					//
					(void)backfill_byte_to_text( buffer+1, f, 2 );
					left = false;
				}
				else {
					//
					//	right column.
					//
					(void)backfill_byte_to_text( buffer+3, f, 3 );
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
		(void)backfill_int_to_text( line+2, o->adrs, 5 );
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
			if( !backfill_byte_to_text( line+8, (byte)s, 2 )) {
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
			(void)backfill_int_to_text( line+2, -o->adrs, 5 );
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
	//	Initialise all of the H Bridge Drivers from the
	//	motor shield configuration table.
	//

	for( byte i = 0; i < SHIELD_OUTPUT_DRIVERS; i++ ) {
		byte	p;

		//
		//	Individually configure the direction pins.
		//
		p = progmem_read_byte( shield_output[ i ].direction );
		pinMode( p, OUTPUT );
		digitalWrite( p, LOW );
		p = progmem_read_byte( shield_output[ i ].enable );
		pinMode( p, OUTPUT );
		digitalWrite( p, LOW );
		p = progmem_read_byte( shield_output[ i ].brake );
		pinMode( p, OUTPUT );
		digitalWrite( p, LOW );
		p = progmem_read_byte( shield_output[ i ].load );
		pinMode( p, INPUT );
		(void)analogRead( p );
	}
	//
	//	Set up the data structures.
	//
	initialise_data_structures();
	init_function_cache();

	//
	//	Set up the Interrupt Service Routine
	//	------------------------------------
	//
	//	Disable interrupts.
	//
	noInterrupts();
	//
	//		Set Timer to default empty values.	
	//
	HW_TCCRnA = 0;	//	Set entire HW_TCCRnA register to 0
	HW_TCCRnB = 0;	//	Same for HW_TCCRnB
	HW_TCNTn  = 0;	//	Initialize counter value to 0
	//
	//		Set compare match register to
	//		generate the correct tick duration.
	//
	HW_OCRnA = TIMER_INTERRUPT_CYCLES;
	//
	//		Turn on CTC mode
	//
	HW_TCCRnA |= ( 1 << HW_WGMn1 );

#if TIMER_CLOCK_PRESCALER == 1

	//
	//		Set HW_CSn0 bit for no pre-scaler (factor == 1 )
	//
	HW_TCCRnB |= ( 1 << HW_CSn0 );
	
#else
#if TIMER_CLOCK_PRESCALER == 8

	//
	//		Set HW_CSn1 bit for pre-scaler factor 8
	//
	HW_TCCRnB |= ( 1 << HW_CSn1 );
#else

	//
	//	Pre-scaler value not supported.
	//
#error "Selected interrupt clock pre-scaler not supported"

#endif
#endif

	//
	//		Enable timer compare interrupt
	//
	HW_TIMSKn |= ( 1 << HW_OCIEnA );
  	//
	//	Enable interrupts.
	//
	interrupts();

	//
	//	Kick off the power monitor and management system.
	//
	init_driver_load();

	//
	//	Optional hardware initialisations
	//

	//
	//	LCD Display.
	//	------------
	//
	//	This might be a warm restart for the display,
	//	so reset everything to a clean slate.
	//

	//
	//	First enable the TWI interface:
	//
	//	0	No Slave address
	//	false	Do not accept general calls (broadcasts)
	//	true	Enable the I2C/TWI interrupt handler
	//	true	Use internal pull up resistors
	//
	twi_init( 0, false, true, true );
	
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
					if( !backfill_int_to_text( buffer+2, (int)( mul_div<word>( output_load[ line ].compound_value[ COMPOUNDED_VALUES-1 ], 100, AVERAGE_CURRENT_LIMIT )), LCD_DISPLAY_STATUS_WIDTH-3 )) {
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
			if( !backfill_byte_to_text( buffer+4, c, LCD_DISPLAY_STATUS_WIDTH-4 )) {
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
			if( !backfill_int_to_text( buffer+2, (int)( mul_div<word>( lcd_statistic_packets, 1000, LCD_UPDATE_INTERVAL )), LCD_DISPLAY_STATUS_WIDTH-3 )) {
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
//	Define a routine for finding an available buffer within
//	a specified range and return its address, or return NULL.
//
static TRANS_BUFFER *find_available_buffer( byte base, byte count, int target ) {
	byte		i;
	TRANS_BUFFER	*b;

	ASSERT( base < TRANSMISSION_BUFFERS );
	ASSERT(( count > 0 )&&( count <= TRANSMISSION_BUFFERS ));
	ASSERT(( base + count ) <= TRANSMISSION_BUFFERS );
	
	//
	//	Look for possible buffer *already* sending to this
	//	target.
	//
	b = circular_buffer + base;
	for( i = 0; i < count; i++ ) {
		if( b->target == target ) return( b );
		b++;
	}
	//
	//	Nothing found so far, look for an empty one.
	//
	b = circular_buffer + base;
	for( i = 0; i < count; i++ ) {
		if( b->state == TBS_EMPTY ) return( b );
		b++;
	}
	//
	//	No available buffer located.
	//
	return( NULL );
}

//
//	DCC composition routines
//	------------------------
//
//	The following routines are used to create individual byte oriented
//	DCC commands.
//

//
//	Create a speed and direction packet for a specified target.
//	Returns the number of bytes in the buffer.
//
static byte compose_motion_packet( byte *command, int adrs, int speed, int dir ) {
	byte	len;

	ASSERT( command != NULL );
	ASSERT(( adrs >= MINIMUM_DCC_ADDRESS )&&( adrs <= MAXIMUM_DCC_ADDRESS ));
	ASSERT(( speed == EMERGENCY_STOP )||(( speed >= MINIMUM_DCC_SPEED )&&( speed <= MAXIMUM_DCC_SPEED )));
	ASSERT(( dir == DCC_FORWARDS )||( dir == DCC_BACKWARDS ));

	if( adrs > MAXIMUM_SHORT_ADDRESS ) {
		command[ 0 ] = 0b11000000 | ( adrs >> 8 );
		command[ 1 ] = adrs & 0b11111111;
		len = 2;
	}
	else {
		command[ 0 ] = adrs;
		len = 1;
	}
	command[ len++ ] = 0b00111111;
	switch( speed ) {
		case 0: {
			command[ len++ ] = ( dir << 7 );
			break;
		}
		case EMERGENCY_STOP: {
			command[ len++ ] = ( dir << 7 ) | 1;
			break;
		}
		default: {
			command[ len++ ] = ( dir << 7 )|( speed + 1 );
			break;
		}
	}
	//
	//	Done.
	//
	return( len );
}

//
//	Create an accessory modification packet.  Return number of bytes
//	used by the command.
//
static byte compose_accessory_change( byte *command, int adrs, int subadrs, int state ) {

	ASSERT( command != NULL );
	ASSERT(( adrs >= MIN_ACCESSORY_ADDRESS )&&( adrs <= MAX_ACCESSORY_ADDRESS ));
	ASSERT(( subadrs >= MIN_ACCESSORY_SUBADRS )&&( subadrs <= MAX_ACCESSORY_SUBADRS ));
	ASSERT(( state == ACCESSORY_ON )||( state == ACCESSORY_OFF ));

	command[ 0 ] = 0b10000000 | ( adrs & 0b00111111 );
	command[ 1 ] = ((( adrs >> 2 ) & 0b01110000 ) | ( subadrs << 1 ) | state ) ^ 0b11111000;
	//
	//	Done.
	//
	return( 2 );
}

//
//	Create a Function set/reset command, return number of bytes used.
//
//	Until I can find a more focused mechanism for modifying the functions
//	this routine has to be this way.  I guess a smarter data driven approach
//	could be used to generate the same output, but at least this is clear.
//
static byte compose_function_change( byte *command, int adrs, int func, int state ) {

	ASSERT( command != NULL );
	ASSERT(( adrs >= MINIMUM_DCC_ADDRESS )&&( adrs <= MAXIMUM_DCC_ADDRESS ));
	ASSERT(( func >= MIN_FUNCTION_NUMBER )&&( func <= MAX_FUNCTION_NUMBER ));
	ASSERT(( state == FUNCTION_ON )||( state == FUNCTION_OFF ));

	if( update_function( adrs, func, ( state == FUNCTION_ON ))) {
		byte	len;

		//
		//	Function has changed value, update the corresponding decoder.
		//
		if( adrs > MAXIMUM_SHORT_ADDRESS ) {
			command[ 0 ] = 0b11000000 | ( adrs >> 8 );
			command[ 1 ] = adrs & 0b11111111;
			len = 2;
		}
		else {
			command[ 0 ] = adrs;
			len = 1;
		}
		//
		//	We have to build up the packets depending on the
		//	function number to modify.
		//
		if( func <= 4 ) {
			//
			//	F0-F4		100[F0][F4][F3][F2][F1]
			//
			command[ len++ ] =	0x80	| get_function( adrs, 0, 0x10 )
							| get_function( adrs, 1, 0x01 )
							| get_function( adrs, 2, 0x02 )
							| get_function( adrs, 3, 0x04 )
							| get_function( adrs, 4, 0x08 );
		}
		else if( func <= 8 ) {
			//
			//	F5-F8		1011[F8][F7][F6][F5]
			//
			command[ len++ ] =	0xb0	| get_function( adrs, 5, 0x01 )
							| get_function( adrs, 6, 0x02 )
							| get_function( adrs, 7, 0x04 )
							| get_function( adrs, 8, 0x08 );
		}
		else if( func <= 12 ) {
			//
			//	F9-F12		1010[F12][F11][F10][F9]
			//
			command[ len++ ] =	0xa0	| get_function( adrs, 9, 0x01 )
							| get_function( adrs, 10, 0x02 )
							| get_function( adrs, 11, 0x04 )
							| get_function( adrs, 12, 0x08 );
		}
		else if( func <= 20 ) {
			//
			//	F13-F20		11011110, [F20][F19][F18][F17][F16][F15][F14][F13]
			//
			command[ len++ ] =	0xde;
			command[ len++ ] =		  get_function( adrs, 13, 0x01 )
							| get_function( adrs, 14, 0x02 )
							| get_function( adrs, 15, 0x04 )
							| get_function( adrs, 16, 0x08 )
							| get_function( adrs, 17, 0x10 )
							| get_function( adrs, 18, 0x20 )
							| get_function( adrs, 19, 0x40 )
							| get_function( adrs, 20, 0x80 );
		}
		else {
			//
			//	F21-F28		11011111, [F28][F27][F26][F25][F24][F23][F22][F21]
			//
			command[ len++ ] =	0xdf;
			command[ len++ ] =		  get_function( adrs, 21, 0x01 )
							| get_function( adrs, 22, 0x02 )
							| get_function( adrs, 23, 0x04 )
							| get_function( adrs, 24, 0x08 )
							| get_function( adrs, 25, 0x10 )
							| get_function( adrs, 26, 0x20 )
							| get_function( adrs, 27, 0x40 )
							| get_function( adrs, 28, 0x80 );
		}
		return( len );
	}
	//
	//	We have to "do" something..
	//	.. so we substitute in an idle packet.
	//
	command[ 0 ] = 0xff;
	command[ 1 ] = 0x00;
	return( 2 );
}

//
//	Create one part of the bulk function setting command.  Which part
//	is controlled by the "state" variable.  For creating the series of
//	function setting commands state with start at 0 and progress through
//	small positive integers.  To mark the end of the commands a length of
//	0 is returned
//
static byte compose_function_block( byte *command, int *state, int adrs, int *fn, int count ) {
	byte	len;

	ASSERT( command != NULL );
	ASSERT( state != NULL );
	ASSERT( *state >= 0  );
	ASSERT(( adrs >= MINIMUM_DCC_ADDRESS )&&( adrs <= MAXIMUM_DCC_ADDRESS ));
	ASSERT( fn != NULL );
	ASSERT( count >= 0 );
	ASSERT(( count * 8 ) > MAX_FUNCTION_NUMBER );
	
	//
	//	Function has changed value, update the corresponding decoder.
	//
	if( adrs > MAXIMUM_SHORT_ADDRESS ) {
		command[ 0 ] = 0b11000000 | ( adrs >> 8 );
		command[ 1 ] = adrs & 0b11111111;
		len = 2;
	}
	else {
		command[ 0 ] = adrs;
		len = 1;
	}
	//
	//	Now the value of the state variable tells us which DCC function
	//	setting command we need to create (which we also auto increment
	//	in preparationn for the next call).
	//
	switch( (*state)++ ) {
		//
		//	I know the bit manipulation code in the following is
		//	really inefficient, but it is clear and easy to see that
		//	it's correct.  For the time being this is more important
		//	than fast code which is wrong.
		//
		case 0: {
			//
			//	F0-F4		100[F0][F4][F3][F2][F1]
			//
			command[ len++ ] =	0x80	| (( fn[0] & 0x01 )? 0x10: 0 )
							| (( fn[0] & 0x02 )? 0x01: 0 )
							| (( fn[0] & 0x04 )? 0x02: 0 )
							| (( fn[0] & 0x08 )? 0x04: 0 )
							| (( fn[0] & 0x10 )? 0x08: 0 );
			break;
		}
		case 1: {
			//
			//	F5-F8		1011[F8][F7][F6][F5]
			//
			command[ len++ ] =	0xb0	| (( fn[0] & 0x20 )? 0x01: 0 )
							| (( fn[0] & 0x40 )? 0x02: 0 )
							| (( fn[0] & 0x80 )? 0x04: 0 )
							| (( fn[1] & 0x01 )? 0x08: 0 );
			break;
		}
		case 2: {
			//
			//	F9-F12		1010[F12][F11][F10][F9]
			//
			command[ len++ ] =	0xa0	| (( fn[1] & 0x02 )? 0x01: 0 )
							| (( fn[1] & 0x04 )? 0x02: 0 )
							| (( fn[1] & 0x08 )? 0x04: 0 )
							| (( fn[1] & 0x10 )? 0x08: 0 );
			break;
		}
		case 3: {
			//
			//	F13-F20		11011110, [F20][F19][F18][F17][F16][F15][F14][F13]
			//
			command[ len++ ] =	0xde;
			command[ len++ ] =		  (( fn[1] & 0x20 )? 0x01: 0 )
							| (( fn[1] & 0x40 )? 0x02: 0 )
							| (( fn[1] & 0x80 )? 0x04: 0 )
							| (( fn[2] & 0x01 )? 0x08: 0 )
							| (( fn[2] & 0x02 )? 0x10: 0 )
							| (( fn[2] & 0x04 )? 0x20: 0 )
							| (( fn[2] & 0x08 )? 0x40: 0 )
							| (( fn[2] & 0x10 )? 0x80: 0 );
			break;
		}
		case 4: {
			//
			//	F21-F28		11011111, [F28][F27][F26][F25][F24][F23][F22][F21]
			//
			command[ len++ ] =	0xdf;
			command[ len++ ] =		  (( fn[2] & 0x20 )? 0x01: 0 )
							| (( fn[2] & 0x40 )? 0x02: 0 )
							| (( fn[2] & 0x80 )? 0x04: 0 )
							| (( fn[3] & 0x01 )? 0x08: 0 )
							| (( fn[3] & 0x02 )? 0x10: 0 )
							| (( fn[3] & 0x04 )? 0x20: 0 )
							| (( fn[3] & 0x08 )? 0x40: 0 )
							| (( fn[3] & 0x10 )? 0x80: 0 );
			break;
		}
		default: {
			return( 0 );
		}
	}
	//
	//	Done!
	//
	return( len );
}

//
//	DCC Generator Command Summary
//	=============================
//
//	For the moment the following commands are described in outline
//	only; details to be provided.
//
//	Mobile decoder set speed and direction
//	--------------------------------------
//
//	[M ADRS SPEED DIR] -> [M ADRS SPEED DIR]
//
//		ADRS:	The short (1-127) or long (128-10239) address of the engine decoder
//		SPEED:	Throttle speed from 0-126, or -1 for emergency stop
//		DIR:	1=Forward, 0=Reverse
//
//	Accessory decoder set state
//	---------------------------
//
//	[A ADRS STATE] -> [A ADRS STATE]
//
//		ADRS:	The combined address of the decoder (1-2048)
//		STATE:	1=on (set), 0=off (clear)
//
//	Mobile decoder set function state
//	---------------------------------
//
//	[F ADRS FUNC VALUE] -> [F ADRS FUNC STATE]
//
//		ADRS:	The short (1-127) or long (128-10239) address of the engine decoder
//		FUNC:	The function number to be modified (0-21)
//		VALUE:	1=Enable, 0=Disable
//		STATE:	1=Confirmed, 0=Failed
//	
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
//	[W ADRS SPEED DIR FNA FNB FNC FND] -> [W ADRS SPEED DIR FNA FNB FNC FND]
//
//		ADRS:	The short (1-127) or long (128-10239) address of the engine decoder
//		SPEED:	Throttle speed from 0-126, or -1 for emergency stop
//		DIR:	1=Forward, 0=Reverse
//		FNA:	Bit mask (in decimal) for Functions 0 through 7
//		FNB:	... Functions 8 through 15
//		FNC:	... Functions 16 through 23
//		FND:	... Functions 24 through 28 (bit positions for 29 through 31 ignored)
//
//	Enable/Disable Power to track
//	-----------------------------
//
//	[P STATE] -> [P STATE]
//
//		STATE: 1=On, 0=Off
//
//	Set CV value (Programming track)
//	--------------------------------
//
//	[S CV VALUE] -> [S CV VALUE STATE]
//
//		CV:	Number of CV to set (1-1024)
//		VALUE:	8 bit value to apply (0-255)
//		STATE:	1=Confirmed, 0=Failed
//
//	Set CV bit value (Programming track)
//	------------------------------------
//
//	Set the specified CV bit with the supplied
//	value.
//
//	[U CV BIT VALUE] -> [U CV BIT VALUE STATE]
//
//		CV:	Number of CV to set (1-1024)
//		BIT:	Bit number (0 LSB - 7 MSB)
//		VALUE:	0 or 1
//		STATE:	1=Confirmed, 0=Failed
//
//
//	Verify CV value (Programming track)
//	-----------------------------------
//
//	[V CV VALUE] -> [V CV VALUE STATE]
//
//		CV:	Number of CV to set (1-1024)
//		VALUE:	8 bit value to apply (0-255)
//		STATE:	1=Confirmed, 0=Failed
//
//	Verify CV bit value (Programming track)
//	---------------------------------------
//
//	Compare the specified CV bit with the supplied
//	value, if they are the same, return 1, otherwise
//	(or in the case of failure) return 0.
//
//	[R CV BIT VALUE] -> [R CV BIT VALUE STATE]
//
//		CV:	Number of CV to set (1-1024)
//		BIT:	Bit number (0 LSB - 7 MSB)
//		VALUE:	0 or 1
//		STATE:	1=Confirmed, 0=Failed
//
//	Accessing EEPROM configurable constants
//	---------------------------------------
//
//		[Q] -> [Q N]			Return number of tunable constants
//		[Q C] ->[Q C V NAME]		Access a specific constant C (range 0..N-1)
//		[Q C V V] -> [Q C V NAME]	Set a specific constant C to value V,
//						second V is to prevent accidental
//						update.
//		[Q -1 -1] -> [Q -1 -1]		Reset all constants to default.
//
//
//	Asynchronous data returned from the firmware
//	============================================
//
//	Change in Power state:
//
//		-> [P STATE]
//
//	Current power consumption of the system:
//
//		-> [L LOAD]
//
//		LOAD: Figure between 0 and 1023
//
//	Report status of individual districts
//
//		[D a b ...]
//
//		Reported numbers (a, b, c ...) reflect
//		the individual districts A, B C etc (independent
//		of the role of the district).  The values
//		provided follow the following table:
//
//			0	Disabled
//			1	Enabled
//			2	Phase Flipped
//			3	Overloaded
//
//	Error detected by the firmware
//
//		-> [E ERR ARG]
//
//		ERR:	Error number giving nature of problem
//		ARG:	Additional information data, nature
//			dependant on the error number.
//

//
//	DCC command interpretation routines.
//	------------------------------------
//
//	These routines are used to interpret both the USB commands
//	recieved and the local user interface commands.  This is done
//	to ensure that all DCC actions pass through the same set of
//	gate and error checking to avoid errors reaching the trains.
//


//
//	Simple compiler time check so we do not need to do pointless
//	check in the run time code.
//
#if MAXIMUM_DCC_COMMAND < 5
#error "MAXIMUM_DCC_COMMAND too small."
#endif

//
//	Interpret command to modify mobile decoder speed/direction.
//
static void process_mobile_command( char cmd, int *arg, int args ) {
	PENDING_PACKET	**tail;
	TRANS_BUFFER	*buf;
	int		target,
			speed,
			dir;
	//
	//	Where we construct the DCC packet data.
	//
	byte	command[ MAXIMUM_DCC_COMMAND ];

	//
	//	Mobile decoder set speed and direction
	//	--------------------------------------
	//
	//	[M ADRS SPEED DIR] -> [M ADRS SPEED DIR]
	//
	//	0	ADRS:	The short (1-127) or long (128-10239) address of the engine decoder
	//	1	SPEED:	Throttle speed from 0-126, or -1 for emergency stop
	//	2	DIR:	1=Forward, 0=Reverse
	//
	if( args != 3 ) {
		errors.log_error( INVALID_ARGUMENT_COUNT, cmd );
		return;
	}
	//
	//	Save command arguments.
	//
	target = arg[ 0 ];
	speed = arg[ 1 ];
	dir = arg[ 2 ];
	//
	//	Verify ranges
	//
	if(( target < MINIMUM_DCC_ADDRESS )||( target > MAXIMUM_DCC_ADDRESS )) {
		errors.log_error( INVALID_ADDRESS, target );
		return;
	}
	if((( speed < MINIMUM_DCC_SPEED )||( speed > MAXIMUM_DCC_SPEED ))&&( speed != EMERGENCY_STOP )) {
		errors.log_error( INVALID_SPEED, speed );
		return;
	}
	if(( dir != DCC_FORWARDS )&&( dir != DCC_BACKWARDS )) {
		errors.log_error( INVALID_DIRECTION, dir );
		return;
	}
	//
	//	Find a destination buffer
	//
	if(( buf = find_available_buffer( MOBILE_BASE_BUFFER, MOBILE_TRANS_BUFFERS, target )) == NULL ) {
		//
		//	No available buffers
		//
		errors.log_error( TRANSMISSION_BUSY, cmd );
		return;
	}
	buf->pending = release_pending_recs( buf->pending, false );
	tail = &( buf->pending );
	//
	//	Now create and append the command to the pending list.
	//
	if( !create_pending_rec( &tail, target, ((( speed == EMERGENCY_STOP )||( speed == MINIMUM_DCC_SPEED ))? TRANSIENT_COMMAND_REPEATS: 0 ), DCC_SHORT_PREAMBLE, 1, compose_motion_packet( command, target, speed, dir ), command )) {
		//
		//	Report that no pending record has been created.
		//
		errors.log_error( COMMAND_QUEUE_FAILED, cmd );
		return;
	}

	//
	//	Construct the reply to send when we get send
	//	confirmation and pass to the manager code to
	//	insert the new packet into the transmission
	//	process.
	//
	buf->reply_on_send = reply_3( buf->contains, MAXIMUM_DCC_REPLY, 'M', target, speed, dir );
	buf->state = ( buf->state == TBS_EMPTY )? TBS_LOAD: TBS_RELOAD;
}

static void process_power_command( char cmd, int *arg, int args ) {
	char	reply[ 8 ];

	//	
	//	Enable/Disable Power to track
	//	-----------------------------
	//
	//	[P STATE] -> [P STATE]
	//
	//		STATE: 0=Off, 1=Main, 2=Prog
	//
	if( args != 1 ) {
		errors.log_error( INVALID_ARGUMENT_COUNT, cmd );
		return;
	}
	switch( arg[ 0 ]) {
		case 0: {	// Power track off
			power_off_tracks();
			if(!( reply_1( reply, 8, 'P', 0 ) && FIRMWARE_OUTPUT( console.print( reply )))) errors.log_error( COMMAND_REPORT_FAIL, cmd );
			return;
		}
		case 1: {
			//
			//	Power Main Track on
			//
			power_on_tracks();
			if(!( reply_1( reply, 8, 'P', 1 ) && FIRMWARE_OUTPUT( console.print( reply )))) errors.log_error( COMMAND_REPORT_FAIL, cmd );
			return;
		}
		default: {
			errors.log_error( INVALID_STATE, cmd );
			return;
		}
	}
}

static void process_accessory_command( char cmd, int *arg, int args ) {
	PENDING_PACKET	**tail;
	TRANS_BUFFER	*buf;
	int		target,
			adrs,
			subadrs,
			state;

	//
	//	Where we construct the DCC packet data.
	//
	byte	command[ MAXIMUM_DCC_COMMAND ];

	//
	//	Accessory decoder set state
	//	---------------------------
	//
	//	[A ADRS STATE] -> [A ADRS STATE]
	//
	//		ADRS:	The combined address of the decoder (1-2048)
	//		STATE:	1=on (set), 0=off (clear)
	//
	if( args != 2 ) {
		errors.log_error( INVALID_ARGUMENT_COUNT, cmd );
		return;
	}
	//
	//	Save arguments.
	//
	target = arg[ 0 ];
	state = arg[ 1 ];
	//
	//	Verify ranges
	//
	if(( target < MIN_ACCESSORY_EXT_ADDRESS )||( target > MAX_ACCESSORY_EXT_ADDRESS )) {
		errors.log_error( INVALID_ADDRESS, target );
		return;
	}
	if(( state != ACCESSORY_ON )&&( state != ACCESSORY_OFF )) {
		errors.log_error( INVALID_STATE, state );
		return;
	}
	//
	//	Convert to internal address and sub-address values and
	//	remember to invert the external target number since we
	//	use negative numbers to represent accessories internally.
	//
	adrs = internal_acc_adrs( target );
	subadrs = internal_acc_subadrs( target );
	target = -target;
	//
	//	Find a destination buffer
	//
	if(( buf = find_available_buffer( ACCESSORY_BASE_BUFFER, ACCESSORY_TRANS_BUFFERS, target )) == NULL ) {
		//
		//	No available buffers
		//
		errors.log_error( TRANSMISSION_BUSY, cmd );
		return;
	}
	buf->pending = release_pending_recs( buf->pending, false );
	tail = &( buf->pending );
	//
	//	Now create and append the command to the pending list.
	//
	if( !create_pending_rec( &tail, target, TRANSIENT_COMMAND_REPEATS, DCC_SHORT_PREAMBLE, 1, compose_accessory_change( command, adrs, subadrs, state ), command )) {
		//
		//	Report that no pending has been record created.
		//
		errors.log_error( COMMAND_QUEUE_FAILED, cmd );
		return;
	}

	//
	//	Construct the future reply and set the state (un-negate
	//	target value).
	//
	buf->reply_on_send = reply_2( buf->contains, MAXIMUM_DCC_REPLY, 'A', -target, state );
	buf->state = ( buf->state == TBS_EMPTY )? TBS_LOAD: TBS_RELOAD;
}

static void process_function_command( char cmd, int *arg, int args ) {
	PENDING_PACKET	**tail;
	TRANS_BUFFER	*buf;
	int		target,
			func,
			state;

	//
	//	Where we construct the DCC packet data.
	//
	byte	command[ MAXIMUM_DCC_COMMAND ];

	//
	//	Mobile decoder set function state
	//	---------------------------------
	//
	//	[F ADRS FUNC STATE] -> [F ADRS FUNC STATE]
	//
	//		ADRS:	The short (1-127) or long (128-10239) address of the engine decoder
	//		FUNC:	The function number to be modified (0-21)
	//		STATE:	1=Enable, 0=Disable, 2=Toggle
	//
	//	Encoding a new "state": 2.  This is the act of turning
	//	on a function then almost immediately turning it off
	//	again (as a mnemonic this is, in binary, a 1 followed
	//	by a 0)
	//	
	if( args != 3 ) {
		errors.log_error( INVALID_ARGUMENT_COUNT, cmd );
		return;
	}
	//
	//	Gather arguments
	//
	target = arg[ 0 ];
	func = arg[ 1 ];
	state = arg[ 2 ];
	//
	//	Verify ranges
	//
	if(( target < MINIMUM_DCC_ADDRESS )||( target > MAXIMUM_DCC_ADDRESS )) {
		errors.log_error( INVALID_ADDRESS, target);
		return;
	}
	if(( func < MIN_FUNCTION_NUMBER )||( func > MAX_FUNCTION_NUMBER )) {
		errors.log_error( INVALID_FUNC_NUMBER, func );
		return;
	}
	if(( state != FUNCTION_ON )&&( state != FUNCTION_OFF )&&( state != FUNCTION_TOGGLE )) {
		errors.log_error( INVALID_STATE, state );
		return;
	}
	//
	//	Find a destination buffer
	//
	if(( buf = find_available_buffer( ACCESSORY_BASE_BUFFER, ACCESSORY_TRANS_BUFFERS, target )) == NULL ) {
		//
		//	No available buffers
		//
		errors.log_error( TRANSMISSION_BUSY, cmd );
		return;
	}
	buf->pending = release_pending_recs( buf->pending, false );
	tail = &( buf->pending );
	if( state == FUNCTION_TOGGLE ) {
		bool		ok;
		
		//
		//	Create a pair of DCC commands to turn the function
		//	on then off:- the toggle option.
		//
		ok = create_pending_rec( &tail, target, TRANSIENT_COMMAND_REPEATS, DCC_SHORT_PREAMBLE, 1, compose_function_change( command, target, func, FUNCTION_ON ), command );
		ok &= create_pending_rec( &tail, target, TRANSIENT_COMMAND_REPEATS, DCC_SHORT_PREAMBLE, 1, compose_function_change( command, target, func, FUNCTION_OFF ), command );
		if( !ok ) {
			//
			//	Report that no pending has been record created.
			//
			buf->pending = release_pending_recs( buf->pending, false );
			errors.log_error( COMMAND_QUEUE_FAILED, cmd );
			return;
		}
	}
	else {
		//
		//	Now create and append the command to the pending list.
		//
		if( !create_pending_rec( &tail, target, TRANSIENT_COMMAND_REPEATS, DCC_SHORT_PREAMBLE, 1, compose_function_change( command, target, func, state ), command )) {
			//
			//	Report that no pending has been record created.
			//
			errors.log_error( COMMAND_QUEUE_FAILED, cmd );
			return;
		}
	}

	//
	//	Prepare the future reply and set new state.
	//
	if( state == FUNCTION_TOGGLE ) {
		buf->reply_on_send = reply_3( buf->contains, MAXIMUM_DCC_REPLY, 'F', target, func, FUNCTION_OFF );
	}
	else {
		buf->reply_on_send = reply_3( buf->contains, MAXIMUM_DCC_REPLY, 'F', target, func, state );
	}
	buf->state = ( buf->state == TBS_EMPTY )? TBS_LOAD: TBS_RELOAD;
}

static void process_state_command( char cmd, int *arg, int args ) {
	//
	//	[W ADRS SPEED DIR FNA FNB FNC FND] -> [W ADRS SPEED DIR]
	//
	//	0	ADRS:	The short (1-127) or long (128-10239) address of the engine decoder
	//	1	SPEED:	Throttle speed from 0-126, or -1 for emergency stop
	//	2	DIR:	1=Forward, 0=Reverse
	//	3	FNA:	Bit mask (in decimal) for Functions 0 through 7
	//	4	FNB:	... Functions 8 through 15
	//	5	FNC:	... Functions 16 through 23
	//	6	FND:	... Functions 24 through 28 (bit positions for 29 through 31 ignored)
	//
	//	Define how many function bit blocks there will be (see above).
	//
	const int bit_blocks = 4;
	
	PENDING_PACKET	**tail;
	TRANS_BUFFER	*buf;
	int		target,
			speed,
			dir,
			fn[ bit_blocks ],
			i;
	byte		l;
			
	//
	//	Where we construct the DCC packet data.
	//
	byte	command[ MAXIMUM_DCC_COMMAND ];

	//
	//	Verify we have all the arguments required.
	//
	if( args != ( 3 + bit_blocks )) {
		errors.log_error( INVALID_ARGUMENT_COUNT, cmd );
		return;
	}
	//
	//	Save command arguments.
	//
	target = arg[ 0 ];
	speed = arg[ 1 ];
	dir = arg[ 2 ];
	for( i = 0; i < bit_blocks; i++ ) fn[ i ] = arg[ 3 + i ];
	
	//
	//	Verify ranges
	//
	if(( target < MINIMUM_DCC_ADDRESS )||( target > MAXIMUM_DCC_ADDRESS )) {
		errors.log_error( INVALID_ADDRESS, target );
		return;
	}
	if(( speed < MINIMUM_DCC_SPEED )||( speed > MAXIMUM_DCC_SPEED )) {
		errors.log_error( INVALID_SPEED, speed );
		return;
	}
	if(( dir != DCC_FORWARDS )&&( dir != DCC_BACKWARDS )) {
		errors.log_error( INVALID_DIRECTION, dir );
		return;
	}
	for( i = 0; i < bit_blocks; i++ ) {
		if(( fn[ i ] < 0 )||( fn[ i ] > 255 )) {
			errors.log_error( INVALID_BIT_MASK, fn[ i ]);
			return;
		}
	}
	
	//
	//	Find a destination buffer in the ACCESSORY part of the table.
	//
	if(( buf = find_available_buffer( ACCESSORY_BASE_BUFFER, ACCESSORY_TRANS_BUFFERS, target )) == NULL ) {
		//
		//	No available buffers
		//
		errors.log_error( TRANSMISSION_BUSY, cmd );
		return;
	}
	//
	//	Clear any pending commands.
	//
	buf->pending = release_pending_recs( buf->pending, false );
	tail = &( buf->pending );
	//
	//	create the function setting commands through repeatedly calling
	//	the compose_function_block() function.
	//
	i = 0;	// this is the state variable required by compose_function_block()
	//
	//	Create a DCC command, and if it is more than 0 bytes long
	//	add it to the pending set of commands and go round again.
	//
	while(( l = compose_function_block( command, &i, target, fn, bit_blocks ))) {
		//
		//	... made one, so append it ...
		//
		if( !create_pending_rec( &tail, target, TRANSIENT_COMMAND_REPEATS, DCC_SHORT_PREAMBLE, 1, l, command )) {
			//
			//	Report that no pending record has been created.
			//
			buf->pending = release_pending_recs( buf->pending, false );
			errors.log_error( COMMAND_QUEUE_FAILED, cmd );
			return;
		}
	}

	//
	//	Now create and append the motion command to the pending list, but we
	//	treat it like a transient (ie a non-permanent command) as this IS NOT
	//	heading into the mobile part of the transmission table.
	//
	if( !create_pending_rec( &tail, target, TRANSIENT_COMMAND_REPEATS, DCC_SHORT_PREAMBLE, 1, compose_motion_packet( command, target, speed, dir ), command )) {
		//
		//	Report that no pending record has been created.
		//
		buf->pending = release_pending_recs( buf->pending, false );
		errors.log_error( COMMAND_QUEUE_FAILED, cmd );
		return;
	}

	//
	//	Construct the reply to send when we get send
	//	confirmation and pass to the manager code to
	//	insert the new packet into the transmission
	//	process.
	//
	buf->reply_on_send = reply_3( buf->contains, MAXIMUM_DCC_REPLY, 'W', target, speed, dir );
	buf->state = ( buf->state == TBS_EMPTY )? TBS_LOAD: TBS_RELOAD;
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
			case 'P': {
				process_power_command( cmd, arg, args );
				break;
			}
			
			//
			//	Cab/Mobile decoder commands
			//	---------------------------
			//
			case 'M': {
				process_mobile_command( cmd, arg, args );
				break;
			}

			//
			//	Accessory Commands
			//	------------------
			//
			case 'A': {
				process_accessory_command( cmd, arg, args );
				break;
			}

			//
			//	Mobile decoder functions
			//	------------------------
			//
			case 'F': {
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
			case 'W': {
				process_state_command( cmd, arg, args );
				break;
			}
			//
			//	EEPROM configurable constants
			//
			case 'Q': {
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

	if( IS_KEYPAD_LETTER( key )) {
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
	
	if( !IS_KEYPAD_NUMBER( key )) return;
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
			if( a <= MAXIMUM_DCC_ADDRESS ) this_object->adrs = a;
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
		if(( s = this_object->state ) == 0 ) {
			//
			//	A zero state is ok, it means forwrds
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
		if(( t < MINIMUM_DCC_SPEED )||( t > MAXIMUM_DCC_SPEED )) t = ( change < 0 )? MINIMUM_DCC_SPEED: MAXIMUM_DCC_SPEED;
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
		if( rotary_average ) {
			user_rotary_movement( rotary_average );
			rotary_average = 0;
		}
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
