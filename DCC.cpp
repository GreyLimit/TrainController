//
//	DCC.cpp
//	=======
//
//	The implementation of the DCC generator
//

#include "DCC.h"

//
//	Declare the actual object itself.
//

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
byte DCC::idle_packet[] = {
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
byte DCC::filler_data[] = {
	1,			// 1s
	0
};


//
//	Here we define the "object variable" which we have chosen to
//	make static for the sake of performance.
//

//
//	Define the transmission buffers to be used.
//
static DCC::trans_buffer	DCC::circular_buffer[ TRANSMISSION_BUFFERS ],
				*DCC::active;

//
//	Signal generation variables.
//
static DCC::trans_buffer	*DCC::_current,
				*DCC::_manage;
static byte			DCC::_remaining,
				DCC::_left,
				DCC::_reload,
				*DCC::_bit_string;
static bool			DCC::_side,
				DCC::_one;

//
//	This is the task control flag, set true when the
//	ISR needs the Management routine to perform some
//	table actions.
//
static bool			DCC::_call_manager;




//
//	Declare the DCC Generator.
//
DCC dcc_generator;

//
//	And its interface to the timer hardware.
//
ISR( DCC_TIMERn_COMPA_vect ) {
	dcc_generator.clock_pulse();
}

//
//	EOF
//
