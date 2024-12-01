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
#include "Task.h"
#include "Critical.h"
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
//	Define the DCC Generator class.
//
class DCC : public Task {
	//
	//	READ ME
	//	=======
	//
	//	Contrary to the C++ design ethic all the variables
	//	defined and used in this class will be statically
	//	declared.
	//
	//	This is for two reasons:
	//
	//	o	There should only ever be one dcc_generator
	//		in any firmware implementation.
	//
	//	o	The C++ object pointer (effectively the "this"
	//		pointer) directly impacts performance and accessing
	//		any variable inside the object has to be redirected
	//		(through "this") to the location of the variable.
	//
	//		This has a meaningful impact on the performance
	//		of the code, especially in the Interrupt Response
	//		Routine (ISR) which is super time critical.
	//

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
	//	DCC protocol constants
	//	----------------------
	//
	static const int	broadcast_address	= 0;
	static const int	minimum_address		= 1;
	static const int	maximum_short_address	= 127;
	static const int	maximum_address		= 10239;
	//
	static const int	emergency_stop		= -1;
	static const int	minimum_speed		= 0;
	static const int	maximum_speed		= 126;
	//
	static const int	direction_backwards	= 0;
	static const int	direction_forwards	= 1;
	//
	//	Internal DCC Accessory addresses.  The address structure
	//	as defined in the DCC protocol.
	//
	static const int	minimum_acc_address	= 0;
	static const int	maximum_acc_address	= 511;
	static const int	minimum_sub_address	= 0;
	static const int	maximum_sub_address	= 3;
	//
	//	External combined DCC accessory address.  The address range
	//	as commonly used by external software systems.
	//
	static const int	minimum_ext_address	= 1;
	static const int	maximum_ext_address	= 2044;
	//
	static const int	accessory_off		= 0;
	static const int	accessory_on		= 1;
	//
	//	The DCC standard specifies CV values between 1 and 1024,
	//	but the actual "on wire" protocol utilised the values 0
	//	to 1023.
	//
	static const int	minimum_cv_address	= 1;
	static const int	maximum_cv_address	= 1024;
	//
	//	Function numbers within a decoder
	//
	static const int	minimum_func_number	= 0;
	static const int	maximum_func_number	= 28;
	//
	static const int	function_off		= 0;
	static const int	function_on		= 1;
	static const int	function_toggle		= 2;
	
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
	static byte DCC::idle_packet[];

	//
	//	The following array does not describe a DCC packet, but a
	//	filler of a single "1" which is required while working
	//	with decoders in service mode.
	//
	static byte DCC::filler_data[];


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
		//		target < 0	Accessory Decoder (negate to get ID)
		//		target == 0	Broadcast address (mobile decoder only)
		//		target > 0	Mobile Decoder
		//
		int		target;
		//
		//	This flag indicates if this DCC packet is a "re-writable"
		//	command.  Specifically this applies *only* to speed and direction
		//	commands which are frequently dynamically updated.
		//
		//	For any target there should (must!) only be one rewritable
		//	command in place at any time.
		//
		bool		rewritable;
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
	//	This is the task control flag, set true when the
	//	ISR needs the Management routine to perform some
	//	table actions.
	//
	//	"_outstanding" is a count of how many records the
	//	ISR needs the management routine to process.  It
	//	is volatile as it is modified by both this routine
	//	and the interrupt routine.
	//
	volatile bool		_call_manager;
	volatile byte		_outstanding;
	
	//
	//	Private support routines.
	//	-------------------------
	//

	//
	//	Copy a DCC command to a new location and append the parity data.
	//	Returns length of data in the target location.
	//
	byte copy_with_parity( byte *dest, byte *src, byte len ) {
		byte	p, i;

		ASSERT( len > 0 );

		p = 0;
		for( i = 0; i < len; i++ ) p ^= ( *dest++ = *src++ );
		*dest = p;
		return( len+1 );
	}

	//
	//	Define a routine to release one (one=true) or all (one=false) pending packets in a list.
	//
	pending_packet *release_pending_recs( pending_packet *head, bool one ) {
		pending_packet	*ptr;

		//
		//	We either release one record and return the address of the remaining
		//	records (when one is true) or we release them all and return NULL
		//	(when one is false).
		//
		while(( ptr = head ) != NIL( pending_packet )) {
			head = ptr->next;
			ptr->next = _free_packets;
			_free_packets = ptr;
			if( one ) break;
		}
		//
		//	Done.
		//
		return( head );
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
	bool pack_command( byte *cmd, byte clen, byte preamble, byte postamble, byte *buf ) {
		byte	l, b, c, v, s;

		ASSERT( preamble >= short_preamble );
		ASSERT( postamble >= 1 );

		//
		//	Start with a preamble of "1"s.
		//
		*buf++ = preamble;
		l = bit_transitions-1;

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
					if( c == maximum_bit_iterations ) return( false );
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
				if( c == maximum_bit_iterations ) return( false );
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
		if(( b = ( maximum_bit_iterations - c )) < postamble ) postamble = b;
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
	byte compose_motion_packet( byte *command, int adrs, int speed, int dir ) {
		byte	len;

		ASSERT( command != NIL( byte ));
		ASSERT(( adrs >= DCC::minimum_address )&&( adrs <= DCC::maximum_address ));
		ASSERT(( speed == DCC::emergency_stop )||(( speed >= DCC::minimum_speed )&&( speed <= DCC::maximum_speed )));
		ASSERT(( dir == DCC_FORWARDS )||( dir == DCC_BACKWARDS ));

		if( adrs > DCC::maximum_short_address ) {
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
			case DCC::emergency_stop: {
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
	byte compose_accessory_change( byte *command, int adrs, int subadrs, int state ) {

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
	byte compose_function_change( byte *command, int adrs, int func, int state ) {

		ASSERT( command != NULL );
		ASSERT(( adrs >= DCC::minimum_address )&&( adrs <= DCC::maximum_address ));
		ASSERT(( func >= DCC::minimum_func_number )&&( func <= DCC::maximum_func_number ));
		ASSERT(( state == FUNCTION_ON )||( state == FUNCTION_OFF ));

		if( update_function( adrs, func, ( state == FUNCTION_ON ))) {
			byte	len;

			//
			//	Function has changed value, update the corresponding decoder.
			//
			if( adrs > DCC::maximum_short_address ) {
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
	//	function setting commands state with start at 0 and repeat calling the
	//	routine until the routine returns zero (which marks the end of the commands).
	//
	byte compose_function_block( byte *command, int *state, int adrs, int *fn, int count ) {
		byte	len;

		ASSERT( command != NULL );
		ASSERT( state != NULL );
		ASSERT( *state >= 0  );
		ASSERT(( adrs >= DCC::minimum_address )&&( adrs <= DCC::maximum_address ));
		ASSERT( fn != NULL );
		ASSERT( count >= 0 );
		ASSERT(( count * 8 ) > DCC::maximum_func_number );
		
		//
		//	Function has changed value, update the corresponding decoder.
		//
		if( adrs > DCC::maximum_short_address ) {
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
		//	in preparation for the next call).
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



public:
	//
	//	Constructor
	//
	DCC( void ) {
		//
		//	Reset everything to the start state.
		//
		_call_manager = false;
		_outstanding = 0;
		_remaining = 1;
		side = false;
		one = false;
		_bit_string = idle_packet;
		
		//
		//	Mark all buffers as unused and empty.
		//
		for( byte i = 0; i < transmission_buffers; i++ ) {
			//
			//	Clear out the record.
			//
			_circular_buffer[ i ].state = state_empty;
			_circular_buffer[ i ].pending == NIL( pending_packet );
		}
		
		//
		//	Link all buffers into a circle for symmetry
		//	in the code when walking through the buffers.
		//
		for( byte i = 0; i < transmission_buffers-1; i++ ) {
			//
			//	Next pointer links forwards.
			//
			_circular_buffer[ i ].next = &( _circular_buffer[ i+1 ]);
		}
		//
		//	Last links back to first.
		//
		_circular_buffer[ transmission_buffers-1 ].next = &( _circular_buffer[ 0 ]);
		
		//
		//	Set up all the free pending packets, just a simple
		//	linked list.
		//
		_free_packets = NIL( pending_packet );
		for( byte i = 0; i < pending_packets; i++ ) {
			_free_packet[ i ].next = _free_packets;
			_free_packets = &( _free_packet[ i ]);
		}
		
		//
		//	Finally set up the ISR and managers pointers
		//	into the circular buffer area.
		//
		_current = _manage = _circular_buffer;
	}

	//
	//	Call this routine to get the object going.  After
	//	this call is completed the dcc driver object *will*
	//	start getting calls to toggle the direction pins to
	//	generate the output signal.
	//
	void start( void ) {
		Critical code;
		
		//
		//	Set up the DCC signal iimer.
		//
		//
		//		Set Timer to default empty values.
		//
		DCC_TCCRnA = 0;		// Set entire DCC_TCCRnA register to 0
		DCC_TCCRnB = 0;		// Same for DCC_TCCRnB
		DCC_TCNTn  = 0;		// Initialise counter value to 0
		//
		//		Set compare match register to
		//		generate the correct tick duration.
		//
		DCC_OCRnA = timer_interrupt_cycles;
		//
		//		Turn on CTC mode
		//
		DCC_TCCRnA |= ( 1 << DCC_WGMn1 );

		//
		//	Assign the pre scaler value required.
		//
		switch( timer_clock_prescaler ) {
			case 1: {
				//
				//		Set DCC_CSn0 bit for no pre-scaler (factor == 1 )
				//
				DCC_TCCRnB |= ( 1 << DCC_CSn0 );
				break;
			}
			case 8: {
				//
				//		Set DCC_CSn1 bit for pre-scaler factor 8
				//
				DCC_TCCRnB |= ( 1 << DCC_CSn1 );
				break;
			}
			default: {
				//
				//	Not coded for!
				//
				ABORT();
			}
		}
		//
		//		Enable timer compare interrupt
		//
		DCC_TIMSKn |= ( 1 << DCC_OCIEnA );
 	}

	//
	//	Request an empty buffer to be set aside for building
	//	a new transmission activity.
	//
	trans_buffer *acquire_buffer( char *reply, reply_time when, int target = broadcast_address, bool rewritable = false ) {
		trans_buffer	*look,
				*found;

		//
		//	The arguments need to be consistent.
		//
		ASSERT(	  (( when != reply_none )&&( reply != NIL( char )))
			||(( when == reply_none )&&( reply == NIL( char ))));

		//
		//	Look for a transmission record we can use to
		//	build a new DCC packet sequence in.
		//
		//	In the case that target is broadcast_address then
		//	we are looking for an empty address.  When target
		//	is a valid target address we are looking for the
		//	record already sending to that address before using
		//	an empty record.
		//
		look = _manage;			// We start where the manager is.
		found = NIL( trans_buffer );	// Not found yet.
		//
		//	If we want a re-writable record, try this scan first.
		//
		if( rewritable ) {
			for( byte i = 0; i < transmission_buffers; i++ ) {
				if( look->rewritable &&( look->state == state_run )&&( look->target == target )) {
					//
					//	Found a re-writable buffer in the right mode, to the right target.
					//
					found = look;
					break;
				}
				look = look->next;
			}
		}
		//
		//	If we didn't want a re-writable record, or simply didn't
		//	find one, then try this scan.
		//
		if( found == NIL( trans_buffer )) {
			for( byte i = 0; i < transmission_buffers; i++ ) {
				if( look->state == state_empty ) {
					found = look;
					break;
				}
				look = look->next;
			}
		}
		
		//
		//	If found is empty then we have failed, return
		//	an empty pointer
		//
		if( found == NIL( trans_buffer )) return( NIL( trans_buffer ));

		//
		//	Clear any pending records that may be attached, they are
		//	not wanted.
		//
		found->pending = release_pending_recs( constructing->pending, false );

		//
		//	Fill in the generic details of this transmission
		//
		found->rewritable = rewritable;
		if(( found->reply_when = when ) != reply_none ) strncpy( found->reply, reply, maximum_output );
		
		//
		//	Ready to add dcc packets to the record.
		//
		//	Note that, at this point in time, the transmission record is
		//	in one of three states:
		//
		//	State		Rewritable	Reason
		//	-----		----------	------
		//	build		false		A new "one off" series of commands
		//	build		true		A speed command for a new target
		//	run		true		A new speed command for an existing target
		//
		return( found );
	}

	//
	//	This is the routine which adds a pending DCC packet to the
	//	pending queue on the buffer being constructed.  Returns
	//	true if a new pending record has been created or false
	//	if this was not possible.
	//
	bool extend_buffer( trans_buffer *rec, int target, byte duration, byte preamble, byte postamble, byte *cmd, byte len ) {
		pending_packet	*ptr;

		ASSERT( rec != NIL( trans_buffer ));
		ASSERT( rec->state == state_empty );
		ASSERT( cmd != NIL( byte ));
		ASSERT( len > 0 );
		ASSERT( len < maximum_command );
		
		//
		//	Look for an empty pending record. 
		//
		if(( ptr = _free_packets ) == NIL( pending_packet )) return( false );
		_free_packets = ptr->next;
		
		//
		//	There is a spare record available so fill it in.
		//
		ptr->target = target;
		ptr->preamble = preamble;
		ptr->postamble = postamble;
		ptr->duration = duration;
		ptr->len = copy_with_parity( ptr->command, cmd, len );
		
		//
		//	We "pre-pend" this pending record to the front
		//	of the list (as this is quick) and allow the buffer
		//	complete routine to reverse the order as the buffer
		//	is activated.
		//
		ptr->next = rec->pending;
		rec->pending = ptr;

		//
		//	Done.
		//
		return( true );
	}

	//
	//	Complete a buffer and prepare it for transmission.
	//
	bool complete_buffer( trans_buffer *rec ) {
		pending_packet	*ptr;

		ASSERT( rec != NIL( trans_buffer ));
		ASSERT( rec->state == state_empty );

		//
		//	We have to move to buffer to the LOAD state
		//	and increment the _outstanding count so that
		//	the management routine can handle the transition
		//	to operating buffer.
		//
		if(( ptr = rec->pending ) == NIL( pending_packet )) return( false );
		
		//
		//	Reverse the pending records list to correct the
		//	order in which they are "pre-pended" onto the
		//	pending list inside the "extend_buffer()" routine.
		//
		rec->pending = NIL( pending_packet );
		while( ptr != NIL( pending_packet )) {
			pending_packet *t;
			
			t = ptr;
			ptr = t->next;
			t->next = rec->pending;
			rec->pending = t;
		}
		//
		//	Release the record into action, carefully.
		//
		{
			Critical code;

			constructing->state = constructing->rewritable? state_reload :state_load;
			_outstanding++;
		}
		//
		//	Report success.
		//
		return( true );
	}

	//
	//	This routine is the "manager" of the DCC packets being
	//	transmitted.  This is called asynchronously from the
	//	clock_pulse() routine by setting the "_call_manager" flag
	//	after incrementing the "_outstanding" counter.
	//
	//	The task manager, seeing the "_call_manager" flag set, will
	//	initiate the call to this routine.
	//
	//	To avoid a blocking the system this routine will only
	//	perform management activities on a single buffer per call,
	//	and will reset the "_call_manager" flag at the end if there
	//	remains more work to complete.
	//
	
	virtual bool process( void ) {
		pending_packet	*pp;

		//
		//	This routine only handles the conversion of byte encoded DCC packets into
		//	bit encoded packets and handing the buffer off to the ISR for transmission.
		//
		//	Consequently we are only interested in buffers set to "state_load".
		//
		
		//
		//	At least one record must be in "state_load" awaiting
		//	some service activity for this routine to be called.
		//
		ASSERT( _outstanding > 0 );
		
		//
		//	Find that buffer by moving through the circular
		//	list until we tumble over it.
		//
		//	Beware!		This is a real candidate
		//			for getting caught in a
		//			permanent spin-loop.
		//
		while( _manage->state != state_load ) _manage = _manage->next;
		
		//
		//	Pending DCC packets to process? (assignment intentional)
		//
		if(( pp = _manage->pending )) {
			//
			//	Our tasks here are to convert the pending data into live
			//	data, set the state to RUN and decrement the _outstanding
			//	count.
			//
			if( pack_command( pp->command, pp->len, pp->preamble, pp->postamble, _manage->bits )) {
				//
				//	Good, set up the remainder of the live parameters.
				//
				_manage->target = pp->target;
				_manage->duration = pp->duration;
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
				{
					Critical code;
					
					_manage->state = state_run;
					_outstanding--;
				}
				//
				//	Now we dispose of the one pending record we have used.
				//
				_manage->pending = release_pending_recs( _manage->pending, true );
				//
				//	Finally, if this had a "reply on send" confirmation and
				//	the command we have just lined up is the last one in the
				//	list, then send the confirmation now.
				//
				if(( _manage->when == reply_at_start )&&( _manage->pending == NIL( pending_packet ))) {
					//
					//	We have just loaded the last pending record, so this
					//	is the one for which the reply is appropiate.
					//
					if( !console.print( _manage->contains )) errors.log_error( COMMAND_REPORT_FAIL, _manage->target );
				}
			}
			else {
				//
				//	Failed to complete as the bit translation failed.
				//
				errors.log_error( BIT_TRANS_OVERFLOW, _manage->pending->target );
				//
				//	We push this buffer back to EMPTY, there is nothing
				//	else we can do with it.
				//
				_manage->state = state_empty;
				//
				//	Finally, we scrap all pending records.
				//
				_manage->pending = release_pending_recs( _manage->pending, false );
			}
		}
		else {
			//
			//	The pending field is empty.  Before marking the buffer as empty
			//	for re-use, we should check to see if a confirmation is required.
			//
			if( _manage->when == reply_at_end ) {
				if( !console.print( _manage->contains )) {
					errors.log_error( COMMAND_REPORT_FAIL, _manage->target );
				}
			}
			//
			//	Now mark empty.
			//
			_manage->when = reply_none;
			_manage->state = state_empty;
		}

		//
		//	Finally, before we finish, remember to move onto the next buffer in the
		//	circular queue.  This will prevent the management routine getting "fixed"
		//	on a single buffer (even if this is *really* unlikely).
		//
		_manage = _manage->next;
		
		//
		//	Reset the flag if there are more records outstanding.  Note we *must not*
		//	do a test like this: "_call_manager = ( _outstanding > 0 )" as this runs
		//	the risk of resetting "_call_manager" to false when the ISR may have set it
		//	to true.
		//
		if( _outstanding ) _call_manager = true;		// Much safer.
	}

	//
	//	Define the Interrupt Service Routine, where we do the work.  This is
	//	a static routine as it shuold only ever access the static variables
	//	defined in the class.
	//
	void clock_pulse( void ) {
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
		if(!( --_remaining )) {
			//
			//	Time is up for the current side.  Flip over and if
			//	a whole bit has been transmitted, then find the next
			//	bit to send.
			//
			//	We "flip" the output DCC signal now as this is the most
			//	time consistent position to do so.
			//
			dcc_driver.toggle();

			//
			//	Now undertake the logical flip and subsequent actions.
			//
			//	The variable "side" tells us which half of a transmitted
			//	bit we have *just* started (through the flip above).
			//
			//	True	This is the front half of a new bit.
			//
			//	False	This is the tail half of an old bit.
			//
			if(( side = !side )) {
				//
				//	Starting a new bit, is it more of the same?
				//
				if(!( --left )) {
					//
					//	"left" counts down the number bits (either 1s
					//	or 0s) which we are outputting in series.
					//
					//	Getting to zero means now time to output a series
					//	of the alternate bits.  We extract the number of
					//	bits to output from the assignment below.
					//
					if(( left = *_bit_string++ )) {
						//
						//	if left > 0 then there are more bits to send.
						//
						//	Select the correct tick count for the next
						//	next bit (again the assignment is intentional).
						//
						_reload = ( _one = !_one )? ticks_for_one: ticks_for_zero;
					}
					else {
						//
						//	left is zero - this set of bit transitions
						//	has been completed.
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
						if( _current->duration && ( _current->state == state_run )) {
							if(!( --_current->duration )) {
								_current->state = state_load;
								_outstanding++;
								_call_manager = true;
							}
						}

						//
						//	Move onto the next buffer.
						//
						_current = _current->next;

						//
						//	Actions related to the current state of the new
						//	buffer (select bits to output and optional state
						//	change).
						//
						switch( _current->state ) {
							case state_run: {
								//
								//	We just transmit the packet found in
								//	the bit data
								//
								_bit_string = _current->bits;
								break;
							}
							case state_reload: {
								//
								//	We have been asked to drop this buffer
								//	so we output an idle packet while changing
								//	the state of buffer to LOAD so the manager
								//	can deal with it.
								//
								_bit_string = idle_packet;
								_current->state = state_load;
								_outstanding++;
								_call_manager = true;
								break;
							}
							case state_load: {
								//
								//	This is a little tricky.  While we do not
								//	(and cannot) do anything with a buffer in
								//	load state, there is a requirement for the
								//	signal generator code NOT to output an idle
								//	packet if we are in the middle of a series
								//	of packets on the programming track.
								//
								//	This code *cannot* tell if it is programming
								//	or just simply running trains.  The difference
								//	is essentially that the main running track has
								//	many transmission buffers, but the programming
								//	track has only a single transmission buffer.
								//
								//	Therefore, when a sequence or programming commands
								//	are sent to a programming track this code fills
								//	in the gaps between them with "1"s so that semantics
								//	of the programming track remain consistent.  
								//
								_bit_string = _current->pending? filler_data: idle_packet;
								break;
							}
							default: {
								//
								//	If we find any other state we ignore the
								//	buffer and output an idle packet.
								//
								_bit_string = idle_packet;
								break;
							}
						}
						//
						//	Initialise the remaining variables required to
						//	output the selected bit stream.
						//
						_one = true;
						_reload = ticks_for_one;
						_left = *_bit_string++;
					}
				}
			}
			//
			//	Reload "_remaining" with the next half bit
			//	tick count down from "reload".  If there has been
			//	a change of output bit "reload" will already have
			//	been modified appropriately.
			//
			_remaining = _reload;
		}
		//
		//	The following comments are primarily applicable to the AVR MCUs at 16MHz.
		//
		//	In ALL cases this routine needs to complete in less than timer_interrupt_cycles
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

		ASSERT( target >= minimum_ext_address );
		ASSERT( target <= maximum_ext_address );

		return((( target - 1 ) >> 2 ) + 1 );
	}
	static int internal_acc_subadrs( int target ) {

		ASSERT( target >= minimum_ext_address );
		ASSERT( target <= maximum_ext_address );

		return(( target - 1 ) & 3 );
	}

	//
	//	Send a Speed and Direction command to the specified
	//	mobile decoder target address.
	//
	bool mobile_command( word target, byte speed, byte direction ) {
		//
		//	Where we construct the DCC packet data and its
		//	associated reply.
		//
		trans_buffer		*buf;
		byte			command[ maximum_command ];
		Buffer<maximum_output>	reply;

		//
		//	Form the reply from the arguments.
		//
		if( !reply.format( Protocol::mobile, target, speed, direction )) return( false );  JEFF
		
		//
		//	Find a transmission buffer
		//
		if(( buf = acquire_buffer( reply.buffer, reply_at_start, target, true )) == NIL( trans_buffer )) return( false );
		//
		//	Now create and append the command to the pending list.
		//
		if( !create_pending_rec( &tail, target, ((( speed == DCC::emergency_stop )||( speed == DCC::minimum_speed ))? TRANSIENT_COMMAND_REPEATS: 0 ), DCC_SHORT_PREAMBLE, 1, compose_motion_packet( command, target, speed, dir ), command )) {
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
		if(( buf->reply_on_send = reply.format( 'M', target, speed, dir ))) reply.copy( buf->contains, DCC:maximum_output );
		buf->state = ( buf->state == state_empty )? state_load: state_reload;
	}

};

//
//	Declare the DCC Generator.
//
extern DCC dcc_generator;

#endif

//
//	EOF
//
