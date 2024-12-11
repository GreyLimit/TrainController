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
#include "Protocol.h"
#include "Driver.h"
#include "Task_Entry.h"
#include "Critical.h"
#include "Driver.h"
#include "DCC_Constant.h"
#include "Function.h"
#include "Buffer.h"
#include "Constants.h"

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
//	Define the DCC Generator class.
//
class DCC : public Task_Entry {
public:
	//
	//	Timing definitions
	//	------------------
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
	static const byte	timer_interrupt_cycles = 232;
	static const byte	timer_clock_prescaler = 1;

#elif F_CPU == 20000000
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
	static const byte	timer_interrupt_cycles = 36;
	static const byte	timer_clock_prescaler = 8;

#else
	//
	//	The target MCU clock frequency has not been accounted for.
	//
#error "DCC Clock speed calculation needs to be calculate for this clock rate."

	//
	//	The calculations outlined above need to be carried out and appropriate
	//	results captured in the definitions of timer_interrupt_cycles and
	//	timer_clock_prescaler values.
	//
#endif

	//
	//	The following two macros return the number of interrupt
	//	ticks (in the light of the above calculations ) which
	//	are required to generate half of a "1" or half of a "0"
	//	bit in the DCC signal.
	//
	static const byte	ticks_for_one = 4;
	static const byte	ticks_for_zero = 7;


	//
	//	DCC Constant Definitions
	//	------------------------
	//

	//
	//	Define the maximum number of bytes in a DCC packet that we
	//	can handle.
	//
	//	Note:	This is different from both:
	//
	//	o	The number of bit transitions which the system will
	//		support.
	//
	//	o	The number of characters used to communicate a DCC
	//		command to/from the firmware in textual form.
	//
	static const byte	maximum_command		= 6;

	//
	//	Define the maximum number of bytes that can be accepted as
	//	a complete input command.
	//
	static const byte	maximum_input		= 32;

	//
	//	Define a maximum number of characters that are required to
	//	formulate the host reply to a command being received successfully.
	//
	static const byte	maximum_output		= 16;

	//
	//	Define the number of buffers set aside for storing DCC packets
	//	before transmission and the number of packets which can be 
	//	transmitted aat any given time.
	//
#ifdef PENDING_PACKETS
	static const byte	pending_packets		= PENDING_PACKETS;
#else
	static const byte	pending_packets		= SELECT_SML(8,16,32);
#endif
#ifdef TRANSMISSION_BUFFERS
	static const byte	transmission_buffers	= TRANSMISSION_BUFFERS;
#else
	static const byte	transmission_buffers	= SELECT_SML(8,16,32);
#endif

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
	//	The result of this analysis is that 8 bytes are required
	//	for each data byte in the packet, 8 for the checksum, 2
	//	for the pre-amble and 1 for the post-amble.
	//
#ifdef BIT_TRANSITIONS
	static const byte	bit_transitions = BIT_TRANSITIONS;
#else
	static constexpr byte	bit_transitions = ( maximum_command + 1 ) * 8 + 3;
#endif

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
	static const byte	maximum_bit_iterations	= 255;

	//
	//	Define the number of "1"s transmitted by the firmware
	//	forming the "preamble" for the DCC packet itself.
	//
	//	The DCC standard specifies a minimum of 14 for normal
	//	commands and 20 for programming commands.
	//
	//	These are the short and long preambles.
	//
	static const byte	short_preamble		= 14;
	static const byte	long_preamble		= 20;

	//
	//	Define an enumeration that captures the possible times when
	//	a command reply is desired.
	//
	//	reply_none	No reply from the system is expected.
	//
	//	reply_at_start	A reply is expected as the *last* DCC packet
	//			is loaded and transmission begins.
	//
	//	reply_at_end	A reply is expected only when the transmission
	//			of the DCC packet has been completed and the
	//			buffer is released.
	//
	enum reply_time {
		reply_none	= 0,
		reply_at_start,
		reply_at_end
	};

private:
	//
	//	Pending Transmissions
	//	---------------------
	//
	//	Define the data structure used to hold a single pending
	//	DCC packet.  The fields defined are:
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
		byte		preamble,
				postamble,
				duration,
				len,
				command[ maximum_command ];
		pending_packet	*next;
	};
	
	//
	//	Define the the pending packet records and the head of the free
	//	records list.
	//
	pending_packet		_free_packet[ pending_packets ];
	pending_packet		*_free_packets;

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
	static byte idle_packet[];

	//
	//	The following array does not describe a DCC packet, but a
	//	filler of a single "1" which is required while working
	//	with decoders in service mode.
	//
	static byte filler_data[];


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
	//	state_reload	ISR		Synchronised load	state_load
	//					initiated by IO code
	//					on a state_run buffer
	//
	//	state_load	Manager		Pending pointer not	state_run
	//					NULL, load next bit
	//					sequence from record
	//					and drop pending record
	//
	//	state_load	Manager		Pending pointer is	state_empty
	//					NULL.
	//
	//	state_empty	System		Create a list of	state_load
	//					pending records and
	//					attach to a new buffer
	//
	//	state_run	System		Create a list of	state_reload
	//					pending records and
	//					attach to a running
	//					buffer (how engines
	//					change speed).
	//
	enum buffer_state : byte {		// One line summary of meaning.
		state_empty	= 0,		// Record empty available for use.
		state_load,			// Current transmission completed, load next pending record.
		state_run,			// Current transmission in progress.
		state_reload			// Terminate current transmission (ISR to flip to state_load)
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
	//	reply_when	Define how the system should manage/generate a reply
	//			to the host computer.
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
		//		target == 0	Transient command (self deleting)
		//		target > 0	Mobile Decoder (possibly permanent)
		//
		word		target;
		//
		//	The duration countdown (if non-zero).  An initial value
		//	of zero indicates no countdown meaning the packet is transmitted
		//	indefinitely.
		//
		byte		duration;
		//
		//	The bit pattern to transmit.  This is only filled in
		//	from the command field by the buffer management code
		//	so that it can be synchronised with operation of the
		//	interrupt routine.
		//
		//	Bit transitions are zero byte terminated, no length
		//	need be maintained.
		//
		byte		bits[ bit_transitions ];

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
		//	is applied only at the end of a series of pending records (i.e.
		//	when pending is NULL).
		//
		reply_time	reply_when;
		char		reply[ maximum_output ];

		//
		//	Buffer linkage.
		//	---------------
		//
		//	Finally, the link to the next buffer
		//
		trans_buffer	*next;
	};

	//
	//	Define the transmission buffers to be used, and the
	//	pointers into it for various purposes.
	//
	trans_buffer		_circular_buffer[ transmission_buffers ],
				*_current,	// Current buffer being transmitted.
				*_manage;	// Next buffer to be managed.

	//
	//	Signal generation variables, not volatile as they are
	//	only modified by the interrupt routine.
	//
	byte			_remaining,
				_left,
				_reload,
				*_bit_string;
	bool			_side,
				_one;

	//
	//	For statistical purposes the following variable are
	//	maintained
	//
	byte			_free_buffers;
	word			_packets_sent;

	//
	//	Declare the link back to the "Manager" task.  The
	//	ISR will "release()" _manager for every record in
	//	the transmission buffers that needs management
	//	attention.  The Manager will the "claim()" each one
	//	as it is processed.
	//
	//	This system creates a simple and effective asynchronous
	//	supplier/consumer between the two systems.
	//
	Signal			_manager;
	
	//
	//	Private support routines.
	//	-------------------------
	//

	//
	//	Copy a DCC command to a new location and append the parity data.
	//	Returns length of data in the target location.
	//
	byte copy_with_parity( byte *dest, byte *src, byte len );

	//
	//	Define a routine to release one (one=true) or all (one=false) pending packets in a list.
	//
	pending_packet *release_pending_recs( pending_packet *head, bool one );

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
	bool pack_command( byte *cmd, byte clen, byte preamble, byte postamble, byte *buf );

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
	byte compose_motion_packet( byte *command, word adrs, byte speed, byte dir );

	//
	//	Create an accessory modification packet.  Return number of bytes
	//	used by the command.
	//
	byte compose_accessory_change( byte *command, word adrs, byte subadrs, byte state );

	//
	//	Create a Function set/reset command, return number of bytes used.
	//
	//	Until I can find a more focused mechanism for modifying the functions
	//	this routine has to be this way.  I guess a smarter data driven approach
	//	could be used to generate the same output, but at least this is clear.
	//
	byte compose_function_change( byte *command, word adrs, byte func, bool on );

	//
	//	Create one part of the bulk function setting command.  Which part
	//	is set by the range argument.
	//
	byte compose_function_block( byte *command, word adrs, byte *state, byte fn[ DCC_Constant::bit_map_array ]);

	//
	//	Request an empty buffer to be set aside for building
	//	a new transmission activity.
	//
	//	Default target of zero is the "any free record" option.
	//
	//	Note:	target > 0	Mobile Decoder
	//		target < 0	Accessory Decoder
	//
	trans_buffer *acquire_buffer( word target = DCC_Constant::broadcast_address, bool overwrite = false );

	//
	//	This is the routine which adds a pending DCC packet to the
	//	pending queue on the buffer being constructed.  Returns
	//	true if a new pending record has been created or false
	//	if this was not possible.
	//
	bool extend_buffer( trans_buffer *rec, byte duration, byte preamble, byte postamble, byte *cmd, byte len );

	//
	//	Complete a buffer and prepare it for transmission.
	//
	bool complete_buffer( trans_buffer *rec );

	//
	//	Cancel construction of a new transmission buffer.
	//
	void cancel_buffer( trans_buffer *rec );



public:
	//
	//	Constructor
	//
	DCC( void );

	//
	//	Call this routine to get the object going.  After
	//	this call is completed the dcc driver object *will*
	//	start getting calls to toggle the direction pins to
	//	generate the output signal.
	//
	void initialise( void );

	//
	//	This routine is the "manager" of the DCC packets being
	//	transmitted.  This is called asynchronously from the
	//	clock_pulse() routine by releasing a resource on the
	//	"_manager" signal.
	//
	//	The task manager, seeing a resource become available, will
	//	initiate the call to this routine.
	//
	//	The Signal object manages all aspects of of concurrency
	//	where the ISR might release multiple transmission records
	//	before the manager routine is scheduled.
	//
	virtual void process( void );

	//
	//	Define the Interrupt Service Routine, where we do the work.  This is
	//	a static routine as it shuold only ever access the static variables
	//	defined in the class.
	//
	void clock_pulse( void );

	//
	//	API for sending commands out through DCC.  *ALL* paramters
	//	are the DCC specification values.  Note especially speeds.
	//
	bool mobile_command( word target, byte speed, byte direction );
	bool accessory_command( word target, byte state );
	bool function_command( word target, byte func, byte state );
	bool state_command( word target, byte speed, byte dir, byte fn[ DCC_Constant::bit_map_array ]);

	//
	//	Routines used to access statistical analysis
	//
	byte free_buffers( void );
	word packets_sent( void );
};

//
//	Declare the DCC Generator.
//
extern DCC dcc_generator;

#endif

//
//	EOF
//
