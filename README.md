# Arduino Train Controller V0.2.0

## Summary

A branch from the Ardunio DCC Generator with the intent of creating a free standing mechanism that will allow DCC operation of small layouts without significant expenditure.

## Version 0.2.0

Version raised to 0.2.0 to reflect status with firmware exhibiting no known bugs and, within the initial parameters, being fully functional.

The subsequent version 0.2.X will be associated with development for support of PoM actions (Program On Main) that will facilitate *most* of the actions which a true programming track permits.  However, this firmware will still not support a dedicated "Programming Track".

## Version 0.1.7

Debugging the LCD driver code; specifically the "screen buffer" code where a very specific "edge case" literally put data across the screen.

The bug was finally isolated to a piece of code four years old that had never shown errors before.  Specifically if the last character displayed as at the edge of the LCD and the next character displayed was to over write the previous character then the LCD was not told to re-place the cursor to the right place.  The effect of this was that the "over written" characters walked through the LCD display nose-to-tail overwriting other parts of the display.

Also found bugs with the status display and object display code where I had used, in error, direct output methods rather than the through buffer methods for displaying data.  This, confusingly, created almost exactly the same effect on screen as the above error and so was not noticed as an error until the above bug was fixed.

## Version 0.1.6

Redesign display layout to allow for optional "per object" data to be displayed.  The left hand side of display (the status area) has been removed from permanent display, the other areas moved left and a new area on the right will be an area to display data about the select object or the optional system status which was on the left.

Still debugging display "artifacts" that seem to be "overflowing" from the right hand side of the screen to thge left hand side.  This would seem to be an issue with the "set cursor position" LCD command not always working.  Primary cause of the issue initially seems to be timing.  The artifacts  relate to the "repositioning" of the output cursor.  This is a command which "takes time" and does seem to always work.

## Version 0.1.5

Consolidate the firmware configuration aspects of the source code - creation of the "Configuration.h" file.

Revise the design of the menu presentation - reduce width to create a wider area for the controlled objects display.

Debug display "artifacts" that seem to be "overflowing" from the objects area into the status area.

## Version 0.1.4

Another sub-sub-version (before going to V0_2) specifically to address the "status" component of the LCD.  With the controller still having two districts (but no longer having a district display) I will adjust the Status display to lose the "uptime" line and replace the "Load" line with two Load/Status entries for the two districts A and B.

## Version 0.1.3

Complete the implementation the of action code behind the user interface.  This will become the first candidate for promotion to V0.2, but not yet.

The UI still contains 'features' which need to be smoothed out - the throttle control is a little, lumpy.

The general layout of the display is also a little clunky, and requires some more finessing.

## Version 0.1.2

Complete design and implement the user interface.

## Version 0.1.1

Keypad and Rotary control support coded and operational.  The DCC computer interface via USB/Serial (using the same syntax as the Arduino Generator) has been kept - there has been no attempt to restore the DCC++ syntax, yet.

The focus of this version is to begin a credible human interface to the firmware view the rotary control, 4x4 matrix keypad and 20x4 LCD display.

## Version 0.1

Train Controller initial version as, effectively, a fork from the Arduino Generator software.

The functional content of the firmware will be reduced with the aim of making space for a HCI to be developed and implemented while still keeping the operating memory footprint within the 2 KByte boundary of a basic Arduino based on the AVR Mega 328 MCU.

## Design Decisions

This initial version of the Train Controller takes as its base
the latest (as of July 2024) version of the Arduino DCC Generator
firmware and make the following changes:

Simplification:

  *	The options for different forms of support hardware are reduced to just the Arduino Uno (and by inference the Nano) and the Arduino Mega2560.
  *	The "DCC Board" is now *only* the motor driver shield. The bespoke 6 district control board has been dropped.
  *	Only a single district is supported (this restriction has not yet been enforced) , though this will naturally support multiple engines (as per normal DCC functionality).
  *	DCC programming track support to be removed.

Extensions:

  *	A "human interface" will be incorporated into the firmware to enable the operator to interact with the device directly:

		A rotary knob with turn for speed and click for direction control

		A joystick up/down/left/right/click for menu

		A keypad for numerical data entry

  *	Restore the DCC++ serial interface (simplified) to optionally restore the interface to JMRI running on a USB attached PC

The above changes will not all happen in Version 0.1.



## Arduino Generator V1.5

### Version 1.5

Discovery of incorrect Accessory Logical Address algorithm which produced the wrong Access Address (out by 1).  This is a result of the previous algorithm taking accessory address 0 as the first valid address and everyone else (ECU) using address 1 as the first valid address.  This is ignoring the sub-addresses 0 through to 3.

### Version 1.4
Development of a additional interface model to the DCC Generator to facilitate setting "logical" decoder parameters while the Generator is operating against the programming track.  This will make programming of chips possible (and simple) through a direct conversion with the Generator firmware, not requiring the mandatory use of additional software on the host computer.

### Version 1.3.4

This version sees the introduction of the "State Restore" command.  This is a command specifically created to facilitate the ability to completely overwrite the "state" of a mobile decoder.  This single command allows both the speed and direction *and all* of the function states to be reset in a single (hopefully nearly atomic) action.  See the description of the command for details and caveats.

### Versions 1.3.x

These versions have seen the replacement of the Arduino Serial library with my own USART class and a further development of the code associated with the detection of confirmation signals when programming decoders on the programming track.

Commands operating upon the programming track have been "better" constructed as a result of a note read indicating that decoders should only act on a command if they receive TWO sequential and intact copies of it.  To this end when such a command is initiated the transmission buffer has 2 copies of the command placed into it for this purpose.

### Communication Protocol

The advent of V1.4 has brought about the inclusion of a second "protocol" over the USB/Serial interface.  The original protocol based on a pair of brackets ('[' and ']') containing a single letter and a series of decimal values has been expanded to include another protocol base on braces ('{' and '}') containing a command statement either requesting a logical value, or setting a logical value.

#### Native Arduino Generator Mode
The USB connection to the host computer is 8-bit serial, no parity at 38400 baud.

```
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
//	of the command should ensure that either *all* of these commands
//	are transmitted or *none* of them are.  While this does not
//	guarantee that the target decoder gets all of the updates
//	it does increase the likelihood that a complete update is
//	successful.
//
//	[W ADRS SPEED DIR FNA FNB FNC FND] -> [W ADRS SPEED DIR]
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
//		STATE: 0=Off, 1=Operations Track ON, 2= Programming Track ON. 
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
//		[Q] -> [Q N]			Return number of tune-able constants
//		[Q C] -> [Q C V NAME]		Access a specific constant C (range 0..N-1)
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
//		-> [D a b ...]
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
```

```
//
//	DCC Generator Variable Commands
//	===============================
//
//	Request list of decoder parameters directly encoded in this
//	version of the firmware:
//
//		{L}
//
//	Get a specific named decoder parameter value:
//
//		{G variable} -> {G variable value}
//
//	Set a specific named decoder parameter value:
//
//		{S variable value} -> {S variable value}
//
//		{S variable index value} -> {S variable index value}
//
//	Errors from configuration setting returned through the same
//	mechanism as thhe "primary" command structure.
//
//		-> [Enn aaaa]
//
//			nn	Error number
//			aaaa	Argument relating to the error
//
```
