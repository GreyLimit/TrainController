//
//	Configuration.h
//	===============
//
//	Settings pertinent to the configuration of the whole firmware
//	package.
//
//	This file should contain only defintions that configure the
//	*compilation* of the firmware directing the firmware to suit
//	specific hardware configurations (hense the name).
//


#ifndef _CONFIGURATION_H_
#define _CONFIGURATION_H_

//
//	Arduino Uno R3 Pin Allocations
//	==============================
//
//	Logical	Physical	device		Role
//	-------	--------	------		----
//	D0/RX	PD0		Serial		UART Rx
//	D1/TX	PD1		Serial		UART Tx
//	D2	PD2		
//	D3	PD3		Motor Shield	SHIELD_DRIVER_A_ENABLE
//	D4	PD4		
//	D5	PD5		Rotary Control	Button
//	D6	PD6		Rotary Control	Signal A
//	D7	PD7		Rotary Control	Signal A
//	D8	PB0		Motor Shield	SHIELD_DRIVER_B_BRAKE
//	D9	PB1		Motor Shield	SHIELD_DRIVER_A_BRAKE
//	D10	PB2
//	D11	PB3		Motor Shield	SHIELD_DRIVER_B_ENABLE
//	D12	PB4		Motor Shield	SHIELD_DRIVER_A_DIRECTION
//	D13	PB5		Motor Shield	SHIELD_DRIVER_B_DIRECTION
//	D14/A0	PC0		Motor Shield	SHIELD_DRIVER_A_LOAD
//	D15/A1	PC1		Motor Shield	SHIELD_DRIVER_B_LOAD
//	D16/A2	PC2
//	D17/A3	PC3
//	D18/A4	PC4				I2C SCL
//	D19/A5	PC5				I2C SDA
//
//	I2C
//	===
//
//	Address		Interface		Device		Note
//	-------		---------		------		----
//	0x27		I2C to parallel		LCD		This is the default address on PCF8475T cards
//	0x28		I2C to Parallel		Keypad		Interface will need address physically setting
//
//
//	Arduino Mega2560 Pin Allocations
//	================================
//
//	There are (currently) identical to the UNO R3 allocations
//	in order to simplify the firmware.
//

//
//	The I2C bus frequency
//	=====================
//
//	Defined to be the frequency divided by 10K then used as a lookup
//	into a table in the TWI code.
//
//	See TWI_IO.[ch]
//
//	This has been set for 100 KBits/s (10), but can be slowed down
//	to determine if there is a timing issue causing communications
//	issues with the LCD.
//
#define TWI_FREQ		10

//
//	ROTARY CONTROLLER
//	=================
//
//	Definition allowing for "teaking" of the pin allocations in
//	the event that the hardware installation requires the reversal
//	of some connections.
//
//	Define REVERSE_ROTARY if a clockwise motion reduces the speed.
//	this is cause by the rotary pin A and pin B being reversed.
//	Simply swappping these about solves this problem.
//
#define REVERSE_ROTARY 1

//
//	Rotary Control PIN allocations
//
#define ROTARY_BUTTON		5

#if defined( REVERSE_ROTARY )

#define ROTARY_A		6
#define ROTARY_B		7

#else

#define ROTARY_A		7
#define ROTARY_B		6

#endif

//
//	4x4 KEYPAD
//	==========
//
//	Define MIRROR_KEYPAD if the keypad returnes the wrong keys.  This
//	is cause by the parallel IO expander being installed in the
//	opposite direction from the anticipated position (ie underneather
//	the keypad PCB).
//
#define MIRROR_KEYPAD 1

//
//	Default I2C address for the PCF8574 Remote 8-Bit I/O Expander.
//
//	This is based on the initial "fixed" binary address
//	of:
//		0b0100000 (0x20) or 32 in decimal.
//
//	However the board supports the hard specification of the
//	bottom 3 bits of the 7-bit address using jupper pins.
//	The jumpers allow these bits (A0, A1 and A2) to be explicitly
//	set giving a final address range of 0x20 to 0x27.
//
//	The keypad address as actiually configured:
//
#define KEYPAD_ADDRESS		0x21


//
//	20x4 LCD
//	========
//
//	The following definitions specify the key hardware character-
//	istics of the LCD display being used.
//
//	LCD_DISPLAY_ROWS	Number of rows the display has.
//	LCD_DISPLAY_COLS	Number of columns available per row.
//	LCD_DISPLAY_ADRS	The I2C address of the display
//
//	The display is assumed to be attached using a LCD specific
//	PCF8575 I2C to Parallel adaptor.  The following default
//	definitions apply to a generic 20x4 display.
//

#define LCD_DISPLAY_ROWS	4
#define LCD_DISPLAY_COLS	20
#define LCD_DISPLAY_ADRS	0x27

//
//	Define the total number of characters on the LCD - the
//	size of the buffer space required for it.
//
#define LCD_FRAME_BUFFER	(LCD_DISPLAY_COLS*LCD_DISPLAY_ROWS)

//
//	Define the macro _LCD_USE_READ_BUSY_READY_ to cause the LCD
//	code to used the "busy ready" status flag from the LCD to time
//	the data and instruction commands to the LCD.
//
//	Note:	At the moment (V0.1.7) this code is written (in the
//		form of "micro code" in the LCD_TWI_IO module), but the
//		data read back from the LCD does not seem functional
//		and the earlier, and simpler, timed delay approach
//		works reliably.
//
//#define _LCD_USE_READ_BUSY_READY_

//
//	Outline description of the  LCD display
//	---------------------------------------
//
//	The display will need to provide information in three "zones":
//
//	o	DCC Generator status
//	o	Current Menu View
//	o	Current controlled objects
//
//	The software will, for the time being, assume that the display
//	is always a 20x4 LCD display.  Smaller displays will make the
//	information shown too crypitic, or simply unavailable.
//
//	The display options might be modified at a future time with the
//	consideration of either a bitmapped LCD or colour OLED.
//
//	A drawing of the target output display:
//
//	 0....0....1....1....2
//	 0    5    0    5    0
//	+--------------------+	The MENU area of the display.
//	|MMMM                |
//	|MMMM                |
//	|MMMM                |
//	|MMMM                |
//	+--------------------+
//
//	 0....0....1....1....2
//	 0    5    0    5    0
//	+--------------------+	The PAGE area of the display.
//	|    PPPPPPPPPP      |
//	|    PPPPPPPPPP      |
//	|    PPPPPPPPPP      |
//	|    PPPPPPPPPP      |
//	+--------------------+
//
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
//	The following definitions define some parameters which
//	shape the output ot the LCD.  The fixed values here should
//	not be modified unless the appropriate code is adjusted
//	accordingly.
//
#define LCD_DISPLAY_MENU_COLUMN		0
#define LCD_DISPLAY_MENU_WIDTH		4
//
#define LCD_DISPLAY_PAGE_COLUMN		4
#define LCD_DISPLAY_PAGE_WIDTH		10
//
#define LCD_DISPLAY_STATUS_COLUMN	14
#define LCD_DISPLAY_STATUS_WIDTH	6


#endif

//
//	EOF
//
