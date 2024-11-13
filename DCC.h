//
//	DCC.h
//	=====
//
//	Define the engine that generates a DCC signal through a
//	virtual class interface.
//

#ifndef _DCC_H_
#define _DCC_H_

//
//	Bring in the Driver class as we require this inside the DCC
//	Generator class.
//
#include "Configuration.h"
#include "Environment.h"
#include "Driver.h"

//
//	Hardware specific DCC configuration
//	-----------------------------------
//
//	Define the timer device we will be using for this module.
//
#if defined( __AVR_ATmega328__ )| defined( __AVR_ATmega328P__ )| defined( __AVR_ATmega328PB__ )

//
//	Standard Nano or Uno R3 configuration
//
#define DCC_TCCRnA		TCCR2A
#define DCC_TCCRnB		TCCR2B
#define DCC_TIMERn_COMPA_vect	TIMER2_COMPA_vect
#define DCC_TCNTn		TCNT2
#define DCC_OCRnA		OCR2A
#define DCC_WGMn1		WGM21
#define DCC_CSn0		CS20
#define DCC_CSn1		CS21
#define DCC_TIMSKn		TIMSK2
#define DCC_OCIEnA		OCIE2A


#elif defined( __AVR_ATmega2560__ )
//
//	Standard Mega 2560 configuration
//
#define DCC_TCCRnA		TCCR2A
#define DCC_TCCRnB		TCCR2B
#define DCC_TIMERn_COMPA_vect	TIMER2_COMPA_vect
#define DCC_TCNTn		TCNT2
#define DCC_OCRnA		OCR2A
#define DCC_WGMn1		WGM21
#define DCC_CSn0		CS20
#define DCC_CSn1		CS21
#define DCC_TIMSKn		TIMSK2
#define DCC_OCIEnA		OCIE2A

#else
//
//	Firmware has not been configured for this board.
//
#error "DCC has not been configured for this board"

#endif

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
//	Define the number of buffers set aside for transmission of DCC
//	commands.
//
#ifndef TRANSMISSION_BUFFERS
#define TRANSMISSION_BUFFERS	SELECT_SML(8,16,32)
#endif

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
#error "DCC Clock speed calculation needs to be calculate for this clock rate."

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
//	Define the DCC Generator class.
//
class DCC {
private:
	//
	//	Remember our link to the output driver object.
	//
	Driver		*_districts;

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
	struct pending_packet {
		int		target;
		byte		preamble,
				postamble,
				duration,
				len,
				command[ MAXIMUM_DCC_COMMAND ];
		pending_packet	*next;
	};


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
	//	state_run	ISR		Continuous packet	state_run
	//					transmission
	//					(duration is zero)
	//
	//	state_run	ISR		Limited packet		state_run
	//					transmission
	//					(--duration > 0 )
	//
	//	state_run	ISR		Limited packet		state_load
	//					transmission
	//					(--duration == 0 )
	//
	//	state_load	Manager		Pending pointer not	state_run
	//					NULL, load next bit
	//					sequence from record
	//					and drop pending record
	//
	//	state_load	Manager		Pending pointer is	state_empty
	//					NULL.
	//
	//	state_empty	IO		Create a list of	state_load
	//					pending records and
	//					attach to a buffer
	//
	//	state_reload	ISR		Synchronised load	state_load
	//					initiated by IO code
	//					on a state_run buffer
	//
	enum buffer_state {
		state_empty	= 0,
		state_load,
		state_run,
		state_reload
	};

	//
	//	Define a Transmission Buffer which contains the following
	//	elements:
	//
	//	state		The current state of this buffer.  Primary method
	//			synchronise activities between the interrupt driven
	//			signal generator code and the table management code.
	//
	//	Live Transmission fields:
	//	-------------------------
	//
	//	target		The DCC ID being targeted by the buffer:
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
	struct trans_buffer {
		//
		//	The current state of this buffer.
		//
		buffer_state	state;
		//
		//	Live Transmission fields:
		//	-------------------------
		//
		//
		//	The target ID of the packet.  Note the following usage:
		//
		//		target < 0	Accessory Decoder (negate to get ID)
		//		target == 0	Broadcast address (mobile decoder only)
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
		pending_packet	*pending;

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
		trans_buffer	*next;
	};

	//
	//	Define the transmission buffers to be used.
	//
	trans_buffer	_circular_buffer[ TRANSMISSION_BUFFERS ],
			*_active;

	//
	//	This is the task control flag, set true when the
	//	ISR needs the Management routine to perform some
	//	table actions.
	//
	bool	_flag;

public:
	//
	//	Constructor
	//
	DCC( void ) {
		_flag = false;
		JEFF
	}

	//
	//	Call this routine the get the object going.
	//
	void start( void ) {
		//
		//	Set up the Timer and kick off task controlled
		//	management services.
		//
		JEFF
	}

	//
	//	Define the Interrupt Service Routine, where we do the work.
	//
	void isr( void );
};

//
//	Declare the DCC Generator.
//
extern DCC dcc_generator;

#endif

//
//	EOF
//
