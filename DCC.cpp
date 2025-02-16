//
//	DCC.cpp
//	=======
//
//	The implementation of the DCC generator
//

#include "DCC.h"
#include "Task.h"
#include "Console.h"
#include "Clock.h"
#include "Stats.h"

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
	DCC::short_preamble,	// 1s
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
//	Private support routines.
//	-------------------------
//

//
//	Copy a DCC command to a new location and append the parity data.
//	Returns length of data in the target location.
//
byte DCC::copy_with_parity( byte *dest, byte *src, byte len ) {
	
	STACK_TRACE( "byte DCC::copy_with_parity( byte *dest, byte *src, byte len )" );
	
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
DCC::pending_packet *DCC::release_pending_recs( DCC::pending_packet *head, bool one ) {
	
	STACK_TRACE( "DCC::pending_packet *DCC::release_pending_recs( DCC::pending_packet *head, bool one )" );
	
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
bool DCC::pack_command( byte *cmd, byte clen, byte preamble, byte postamble, byte *buf ) {
	
	STACK_TRACE( "bool DCC::pack_command( byte *cmd, byte clen, byte preamble, byte postamble, byte *buf )" );
	
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
byte DCC::compose_motion_packet( byte *command, word adrs, byte speed, byte dir ) {
	
	STACK_TRACE( "byte DCC::compose_motion_packet( byte *command, word adrs, byte speed, byte dir )" );
	
	byte	len;

	ASSERT( command != NIL( byte ));
	ASSERT( DCC_Constant::valid_mobile_target( adrs ));
	ASSERT( DCC_Constant::valid_mobile_speed( speed ));
	ASSERT( DCC_Constant::valid_mobile_direction( dir ));

	if( adrs > DCC_Constant::maximum_short_address ) {
		command[ 0 ] = 0b11000000 | ( adrs >> 8 );
		command[ 1 ] = adrs & 0b11111111;
		len = 2;
	}
	else {
		command[ 0 ] = adrs;
		len = 1;
	}
	//
	//	NMRA Document "S-9.2.1"
	//	Advanced Operations Instruction (001)
	//	format: 001CCCCC 0 DDDDDDDD
	//	128 set Speed/Dir: CCCCC = 0b11111
	//	D7: Direction (1=forwards, 0=backwards)
	//	D6-0: Speed, 0: stop, 1: Emergency Stop, 2-127: velocity.
	//				
	command[ len++ ] = 0b00111111;
	command[ len++ ] = ( dir << 7 )| speed;
	//
	//	Done.
	//
	return( len );
}

//
//	Create an accessory modification packet.  Return number of bytes
//	used by the command.
//
byte DCC::compose_accessory_change( byte *command, word adrs, byte subadrs, byte state ) {
	
	STACK_TRACE( "byte DCC::compose_accessory_change( byte *command, word adrs, byte subadrs, byte state )" );
	
	ASSERT( command != NIL( byte ));
	ASSERT( DCC_Constant::valid_accessory_address( adrs ));
	ASSERT( DCC_Constant::valid_accessory_sub_address( subadrs ));
	ASSERT( DCC_Constant::valid_accessory_state( state ));

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
byte DCC::compose_function_change( byte *command, word adrs, byte func, bool on ) {
	
	STACK_TRACE( "byte DCC::compose_function_change( byte *command, word adrs, byte func, bool on )" );
	
	ASSERT( command != NULL );
	ASSERT( DCC_Constant::valid_mobile_target( adrs ));
	ASSERT( DCC_Constant::valid_function_number( func ));

	if( function_cache.update( adrs, func, on )) {
		byte	len;

		//
		//	Function has changed value, update the corresponding decoder.
		//
		if( adrs > DCC_Constant::maximum_short_address ) {
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
			command[ len++ ] =	0x80	| function_cache.get( adrs, 0, 0x10 )
							| function_cache.get( adrs, 1, 0x01 )
							| function_cache.get( adrs, 2, 0x02 )
							| function_cache.get( adrs, 3, 0x04 )
							| function_cache.get( adrs, 4, 0x08 );
		}
		else if( func <= 8 ) {
			//
			//	F5-F8		1011[F8][F7][F6][F5]
			//
			command[ len++ ] =	0xb0	| function_cache.get( adrs, 5, 0x01 )
							| function_cache.get( adrs, 6, 0x02 )
							| function_cache.get( adrs, 7, 0x04 )
							| function_cache.get( adrs, 8, 0x08 );
		}
		else if( func <= 12 ) {
			//
			//	F9-F12		1010[F12][F11][F10][F9]
			//
			command[ len++ ] =	0xa0	| function_cache.get( adrs, 9, 0x01 )
							| function_cache.get( adrs, 10, 0x02 )
							| function_cache.get( adrs, 11, 0x04 )
							| function_cache.get( adrs, 12, 0x08 );
		}
		else if( func <= 20 ) {
			//
			//	F13-F20		11011110, [F20][F19][F18][F17][F16][F15][F14][F13]
			//
			command[ len++ ] =	0xde;
			command[ len++ ] =		  function_cache.get( adrs, 13, 0x01 )
							| function_cache.get( adrs, 14, 0x02 )
							| function_cache.get( adrs, 15, 0x04 )
							| function_cache.get( adrs, 16, 0x08 )
							| function_cache.get( adrs, 17, 0x10 )
							| function_cache.get( adrs, 18, 0x20 )
							| function_cache.get( adrs, 19, 0x40 )
							| function_cache.get( adrs, 20, 0x80 );
		}
		else {
			//
			//	F21-F28		11011111, [F28][F27][F26][F25][F24][F23][F22][F21]
			//
			command[ len++ ] =	0xdf;
			command[ len++ ] =		  function_cache.get( adrs, 21, 0x01 )
							| function_cache.get( adrs, 22, 0x02 )
							| function_cache.get( adrs, 23, 0x04 )
							| function_cache.get( adrs, 24, 0x08 )
							| function_cache.get( adrs, 25, 0x10 )
							| function_cache.get( adrs, 26, 0x20 )
							| function_cache.get( adrs, 27, 0x40 )
							| function_cache.get( adrs, 28, 0x80 );
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
//	is set by the range argument.
//
byte DCC::compose_function_block( byte *command, word adrs, byte *state, byte fn[ DCC_Constant::bit_map_array ]) {
	
	STACK_TRACE( "byte DCC::compose_function_block( byte *command, word adrs, byte *state, byte fn[ DCC_Constant::bit_map_array ])" );
	
	byte	len;

	ASSERT( DCC_Constant::valid_mobile_target( adrs ));
	ASSERT( command != NIL( byte ));
	ASSERT( state != NIL( byte ));
	
	//
	//	Function has changed value, update the corresponding decoder.
	//
	if( adrs > DCC_Constant::maximum_short_address ) {
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

//
//	Request an empty buffer to be set aside for building
//	a new transmission activity.
//
DCC::trans_buffer *DCC::acquire_buffer( word target, bool mobile, bool overwrite ) {
	
	STACK_TRACE( "DCC::trans_buffer *DCC::acquire_buffer( word target, bool mobile, bool overwrite )" );

	TRACE_DCC( console.print( F( "DCC find " )));
	TRACE_DCC( console.print( target ));
	TRACE_DCC( console.print( F( " mobile " )));
	TRACE_DCC( console.print( mobile ));
	TRACE_DCC( console.print( F( " overwrite " )));
	TRACE_DCC( console.println( overwrite ));

	//
	//	For the moment the DCC generation code cannot transmit
	//	to the broadcast address as this also implies that all
	//	the targeted transmissions are cancelled and removed.
	//
	//	This is code which is not yet present in this module.
	//
	ASSERT( target != DCC_Constant::broadcast_address );

	//
	//	Overwrite can only be used with mobile transmissions
	//	which should be obvious as only mobile speed/dir
	//	transmissions can be persistent.
	//
	ASSERT( mobile ||( !mobile && !overwrite ));
	
	trans_buffer	*look,
			*found;

	//
	//	Look for a transmission record we can use to
	//	build a new DCC packet sequence in.
	//
	//	When target is a valid target address we are looking
	//	for the record already sending to that address before
	//	using an empty record.
	//
	look = _manage;			// We start where the manager is.
	found = NIL( trans_buffer );	// Not found yet.
	//
	//	If we want a re-writable record, try this scan first.
	//
	if( overwrite ) {
		for( byte i = 0; i < transmission_buffers; i++ ) {
			if(( look->mobile == mobile )&&( look->target == target )&&( look->state == state_run )&&( look->duration == 0 )) {
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
	//	Clear any pending records that may be attached.
	//
	found->pending = release_pending_recs( found->pending, false );

	//
	//	Remember the target address used for this record.
	//	This is required during following record searches
	//	but has no direct impact on the actual address
	//	targeted as this is encoded in the bit patterns.
	//
	found->target = target;
	found->mobile = mobile;

	//
	//	Ready to add dcc packets to the record.
	//
	//	Note that, at this point in time, the transmission record is
	//	in one of three states:
	//
	//	State		Rewritable	Reason
	//	-----		----------	------
	//	empty		false		A new "one off" series of commands
	//	empty		true		A speed command for a new target
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
bool DCC::extend_buffer( DCC::trans_buffer *rec, byte duration, byte preamble, byte postamble, byte *cmd, byte len ) {
	
	STACK_TRACE( "bool DCC::extend_buffer( DCC::trans_buffer *rec, byte duration, byte preamble, byte postamble, byte *cmd, byte len )" );
	
	pending_packet	*ptr;

	ASSERT( rec != NIL( trans_buffer ));
	ASSERT(( rec->state == state_empty )||( rec->state == state_run ));
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
bool DCC::complete_buffer( DCC::trans_buffer *rec ) {
	
	STACK_TRACE( "bool DCC::complete_buffer( DCC::trans_buffer *rec )" );
	
	pending_packet	*ptr;
	byte		persistent;

	ASSERT( rec != NIL( trans_buffer ));
	ASSERT(( rec->state == state_empty )||( rec->state == state_run ));

	//
	//	As a point here, if we are "completing" a
	//	transmission record with an empty pending
	//	list then there is nothing to .. complete.
	//
	//	We return false because this is a error, just
	//	not a fatal one.
	//
	if(( ptr = rec->pending ) == NIL( pending_packet )) return( false );
	
	//
	//	Reverse the pending records list to correct the
	//	order in which they are "pre-pended" onto the
	//	pending list inside the "extend_buffer()" routine.
	//
	rec->pending = NIL( pending_packet );
	persistent = 0;
	while( ptr != NIL( pending_packet )) {
		pending_packet *t;

		//
		//	Push onto the front of the list.
		//
		t = ptr;
		ptr = t->next;
		t->next = rec->pending;
		rec->pending = t;
		//
		//	Increment the persistent count, if persistent.
		//
		if( t->duration == 0 ) persistent++;
	}

	//
	//	This is a "no-brain-er" really, but wise to check.
	//	it has to be zero or one.
	//
	ASSERT( persistent <= 1 );
	
	//
	//	Now verify that there is sufficient persistent
	//	capacity remaining to support this new command.
	//	Need to consider that the "current" active
	//	transmission might already be a persistent one.
	//
	if(( rec->state == state_run )&&( rec->duration == 0 )) {
		//
		//	As this is already persistent we only need to
		//	modify the persistence count if it is to be
		//	replaced by one which is not.  In which case
		//	we can increase the persistence availability.
		//
		if( !persistent ) _persistent += 1;
	}
	else {
		//
		//	So we are not modifying a persistent transmission
		//	then we only need to ensure that there remains
		//	enough capacity for any new persistent transmission.
		//
		if( persistent > _persistent ) {
			//
			//	Failed! -- unwind the new records.
			//
			rec->pending = release_pending_recs( rec->pending, false );
			return( false );
		}
		//
		//	Adjust persistence count down.
		//
		_persistent -= persistent;
	}

	//
	//	Release the record into action by setting the
	//	state of the record to one which the ISR will
	//	respond to.
	//
	if( rec->state == state_run ) {
		//
		//	We need to get the ISR to convert
		//	the record from its current state
		//	to state_load so we get it to
		//	state_reload (this is safe way to
		//	do this).
		//
		rec->state = state_reload;
	}
	else {
		//
		//	We can directly pass this to the manager
		//	to handle, so set this to state load
		//	*and* release() the _manager (also safe).
		//
		rec->state = state_load;
		_manager.release();
		
		//
		//	Reduce free buffers count.
		//
		
		ASSERT( _free_buffers > 0 );
		
		_free_buffers--;
	}
	//
	//	Report success.
	//
	return( true );
}

//
//	Cancel construction of a new transmission buffer.
//
void DCC::cancel_buffer( trans_buffer *rec ) {
	
	STACK_TRACE( "void DCC::cancel_buffer( trans_buffer *rec )" );
	
	//
	//	Sanity check - we should only be cancelling
	//	a running or empty transmission record.
	//
	ASSERT( rec != NIL( trans_buffer ));
	ASSERT(( rec->state == state_run )||( rec->state == state_empty ));

	//
	//	How we cancel a buffer could depend on the
	//	state that it is currently in.
	//
	//	We *could* be updating a running buffer - in
	//	which case we clear the pending list and leave
	//	it alone.
	//
	//	If we are cancelling an empty buffer then we
	//	*also* clear the pending list and leave it alone.
	//
	//	While the actual actions are the same they must
	//	not be see as identical as one transmission record
	//	is live and working and the other is not.

	//
	//	Clear any pending records so they are available
	//	for other commands.
	//
	rec->pending = release_pending_recs( rec->pending, false );
}


//
//	Constructor
//
DCC::DCC( void ) {
	//
	//	Reset everything to the start state.
	//
	_side = true;
	_one = true;
	_bit_string = idle_packet;
	
	//
	//	Mark all buffers as unused and empty.
	//
	for( byte i = 0; i < transmission_buffers; i++ ) {
		//
		//	Clear out the record.
		//
		_circular_buffer[ i ].state = state_empty;
		_circular_buffer[ i ].pending = NIL( pending_packet );
	}
	_persistent = max_persistent_buffers;
	_free_buffers = transmission_buffers;
	_packets_sent = 0;
#if defined( ENABLE_DCC_SYNCHRONISATION )
	_irq_sync = 0;
#endif
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
//	clock out is a pin specifically used export the internal
//	DCC clock from the ISR routine to enable basic operational
//	state verification (ie, is there even a clock working,
//	and what speed is it?).
//
void DCC::initialise( void ) {
	
	STACK_TRACE( "void DCC::initialise( void )" );

	TRACE_DCC( console.print( F( "DCC manager flag " )));
	TRACE_DCC( console.println( _manager.identity()));

	//
	//	link this object into the task manager so
	//	that the management actions can be called
	//	when required.
	//
	if( !task_manager.add_task( this, &_manager, management_process )) ABORT( TASK_MANAGER_QUEUE_FULL );

#ifdef ENABLE_DCC_SYNCHRONISATION
	//
	//	Link this object into the clock manager to
	//	generate period IRQ synchonrisation recalibrations.
	//
	if( !task_manager.add_task( this, &_recalibrate, recalibrate_process )) ABORT( TASK_MANAGER_QUEUE_FULL );
	if( !event_timer.delay_event( MSECS( dcc_recalibration_period ), &_recalibrate, true )) ABORT( EVENT_TIMER_QUEUE_FULL );
#endif

	//
	//	Now kick of the time critical code that
	//	generates the DCC signal itself.
	//
	{
		Critical code;
		
		//
		//	Set up the DCC signal timer.
		//
		//
		//		Set Timer to default empty values.
		//
		DCC_TCCRnA = 0;		// Set entire DCC_TCCRnA register to 0
		DCC_TCCRnB = 0;		// Same for DCC_TCCRnB
		DCC_TCNTn  = 0;		// Initialise counter value to 0
		//
		//		Set compare match register with an
		//		initial value just to kick off the whole
		//		process.  This will be modified every
		//		alternate interrupt to reflect outputing
		//		1's or 0's as required.
		//
		DCC_COMPARE_REGISTER = timer_digit_1_cycles;
		//
		//		Set up the output parameters to match an
		//		idle packet so the interrupt routine has
		//		something sensible to work on.
		//
		_bit_string = idle_packet;
		_left = *_bit_string++;
		_side = true;
		_one = true;
		//
		//		Turn on CTC mode
		//
		DCC_TCCRnA |= bit( DCC_WGMn1 );

		//
		//	Assign the pre scaler value required.
		//
		switch( timer_clock_prescaler ) {
			case 1: {
				//
				//		Set DCC_CSn0 bit for no pre-scaler (factor == 1 )
				//
				DCC_TCCRnB |= bit( DCC_CSn0 );
				break;
			}
			case 8: {
				//
				//		Set DCC_CSn1 bit for pre-scaler factor 8
				//
				DCC_TCCRnB |= bit( DCC_CSn1 );
				break;
			}
			default: {
				//
				//	Not coded for!
				//
				ABORT( PROGRAMMER_ERROR_ABORT );
			}
		}
		//
		//		Enable timer compare interrupt
		//
		DCC_TIMSKn |= ( 1 << DCC_OCIEnA );
	}
}

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
void DCC::process( byte handle ) {
	
	STACK_TRACE( "void DCC::process( void )" );

	//
	//	Is this a managment request.
	//
	if( handle == management_process ) {

		TRACE_DCC( console.println( F( "DCC management" )));

		pending_packet	*pp;

		//
		//	This routine only handles the conversion of byte encoded DCC packets into
		//	bit encoded packets and handing the buffer off to the ISR for transmission.
		//
		//	Consequently we are only interested in buffers set to "state_load".
		//
		//	We should not be called up, ever, unless there is at least one buffer set
		//	to "state_load".
		//
		
		//
		//	Find that buffer by moving through the circular
		//	list until we tumble over it.
		//
		//	JEFF		This is a real candidate
		//			for getting caught in a
		//			permanent spin-loop.
		//
		//	Might need to consider some active counting code here to
		//	catch situations where this goes wrong.
		//
		while( _manage->state != state_load ) _manage = _manage->next;
		
		//
		//	Pending DCC packets to process? (assignment intentional)
		//
		if(( pp = _manage->pending )) {
			//
			//	Our tasks here are to convert the pending data into live
			//	data, then set the state to RUN.
			//
			if( pack_command( pp->command, pp->len, pp->preamble, pp->postamble, _manage->bits )) {
				//
				//	Good, set up the remainder of the live parameters.
				//
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
				_manage->state = state_run;
				
				//
				//	Now we dispose of the one pending record we have used.
				//
				_manage->pending = release_pending_recs( _manage->pending, true );
				
				//
				//	Finally, if this had a "reply at start" confirmation and
				//	the command we have just lined up is the last one in the
				//	list, then send the confirmation now.
				//
				if(( _manage->reply_when == reply_at_start )&&( _manage->pending == NIL( pending_packet ))) {
					//
					//	We have just loaded the last pending record, so this
					//	is the one for which the reply is appropriate.
					//
					if( !console.print( _manage->reply )) {
						errors.log_error( COMMAND_REPORT_FAIL, _manage->target );
					}

					//
					//	Clear reply flag
					//
					_manage->reply_when = reply_none;
				}
			}
			else {
				//
				//	Failed to complete as the bit translation failed.
				//
				errors.log_error( TRANSMISSION_BIT_OVERFLOW, _manage->target );
				
				//
				//	We push this buffer back to EMPTY, there is nothing
				//	else we can do with it.
				//
				_manage->state = state_empty;
				
				//
				//	Finally, we scrap all pending records.
				//
				_manage->pending = release_pending_recs( _manage->pending, false );
				
				//
				//	Note a free buffer available.
				//
				_free_buffers++;
			}
		}
		else {
			//
			//	The pending field is empty.  Before marking the buffer as empty
			//	for re-use, we should check to see if a confirmation is required.
			//
			if( _manage->reply_when == reply_at_end ) {
				if( !console.print( _manage->reply )) {
					errors.log_error( COMMAND_REPORT_FAIL, _manage->target );
				}
				//
				//	Clear reply flag
				//
				_manage->reply_when = reply_none;
			}
			//
			//	Now mark empty.
			//
			_manage->state = state_empty;

			//
			//	Note a free buffer available.
			//
			_free_buffers++;
		}

		//
		//	Finally, before we finish, remember to move onto the next buffer in the
		//	circular queue.  This will prevent the management routine getting "fixed"
		//	on a single buffer (even if this is *really* unlikely).
		//
		_manage = _manage->next;
		//
		//	Done.
		//
		return;
	}

#ifdef ENABLE_DCC_SYNCHRONISATION
	//
	//	Is this a recalibration request.
	//
	if( handle == recalibrate_process ) {
		
		TRACE_DCC( console.println( F( "DCC recalibration" )));
		
		if( _delay.last() < ( _irq_sync >> 1 )) _irq_sync -= 1;
		
		return;
	}
#endif
	
	//
	//	Done.
	//
}

//
//	Define the Interrupt Service Routine, where we do the work.  This is
//	a static routine as it shuold only ever access the static variables
//	defined in the class.
//
void DCC::clock_pulse( void ) {

#if defined( ENABLE_DCC_DELAY_REPORT ) || defined( ENABLE_DCC_SYNCHRONISATION )
	byte	delayed;
	
	//
	//	Gather the DCC delay experienced, if compiled in.
	//
	//	We gather the time here, but do not do anything until
	//	after the "fixed time" output signal toggle has taken
	//	place.
	//
	delayed = DCC_COUNTER_REGISTER;
#endif

	//
	//	Stack trace placed here to avoid delaying the collection
	//	of the DCC counter.
	//
	STACK_TRACE( "void DCC::clock_pulse( void )" );


#ifdef ENABLE_DCC_SYNCHRONISATION
	//
	//	Now we wait for the target delay as calculated by the
	//	previous interrupt calls (if it hasn't already passed
	//	us by).
	//
	//	This does waste time on the CPU, but by doing so attempts
	//	to more accurately in time run the following line of code
	//	and create a more time accurate output signal.
	//
	//	There a risk that this will create an infinite loop
	//	if the value of _irq_sync ever equals or exceeds the
	//	value in the comparison register!  A hard ceiling
	//	needs to be set for the delay sync value.
	//
	while( DCC_COUNTER_REGISTER < _irq_sync );
#endif

	//
	//	We "flip" the output DCC signal now as this is the most
	//	time consistent position to do so (being  the first
	//	action of the interrrupt routine).
	//
	dcc_driver.toggle();

#ifdef ENABLE_DCC_SYNCHRONISATION
	//
	//	Now that the time critical element of the ISR code has
	//	been met, we can complete the delay calculation in
	//	readyness for interrupts.
	//
	if(( delayed = _delay.add( delayed )) > _irq_sync ) {
		if( delayed < max_sync ){
			_irq_sync = delayed;
		}
		else {
			_irq_sync = max_sync;
		}
	}
#else
#ifdef ENABLE_DCC_DELAY_REPORT
	//
	//	Just gathering the DCC delay stats - if compiled in.
	//
	_delay.add( DCC_COUNTER_REGISTER );
#endif
#endif

	//
	//	Now undertake the logical flip and subsequent actions.
	//
	//	The variable "side" tells us which half of a transmitted
	//	bit we have *just* started (through the flip above).
	//
	//		True	This is the front half of a new bit.
	//
	//		False	This is the tail half of an old bit.
	//
	//	If it's true (start of new bit) then there *might* be
	//	stuff for us to do: Change bit value (0<->1) or even
	//	start new packet.
	//
	if(( _side = !_side )) {
		//
		//	Starting a new bit, is it more of the same?
		//
		if(!( --_left )) {
			//
			//	"left" counts down the number of similar
			//	bits (either 1s or 0s) which we are
			//	outputing in series.
			//
			//	Getting to zero means it is now time to output a series
			//	of the alternate bits.  We extract the number of
			//	bits to output from the assignment below.
			//
			if(( _left = *_bit_string++ )) {
				//
				//	If left > 0 then there are more bits to send.
				//
				//	Select the correct tick count for the next
				//	next bit (again the assignment is intentional).
				//
				DCC_COMPARE_REGISTER = ( _one = !_one )? timer_digit_1_cycles: timer_digit_0_cycles;
			}
			else {
				//
				//	_left is zero so this set of bit transitions
				//	has been completed; we have sent a packet!
				//
				_packets_sent++;
				
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
						_manager.release();
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
						_manager.release();
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
				DCC_COMPARE_REGISTER = timer_digit_1_cycles;
				_left = *_bit_string++;
			}
		}
	}
}

//
//	Send a Speed and Direction command to the specified
//	mobile decoder target address.
//
bool DCC::mobile_command( word target, byte speed, byte direction, Buffer_API *reply ) {
	
	STACK_TRACE( "bool DCC::mobile_command( word target, byte speed, byte direction, Buffer_API *reply )" );

	//
	//	Where we construct the DCC packet data.
	//
	trans_buffer			*buf;
	byte				command[ maximum_command ];

	TRACE_DCC( console.print( F( "DCC mobile " )));
	TRACE_DCC( console.print( target ));
	TRACE_DCC( console.print( F( " speed " )));
	TRACE_DCC( console.print( speed ));
	TRACE_DCC( console.print( F( " dir " )));
	TRACE_DCC( console.println( direction ));

	//
	//	Sanity checks.
	//
	ASSERT( DCC_Constant::valid_mobile_target( target ));
	ASSERT( DCC_Constant::valid_mobile_speed( speed ));
	ASSERT( DCC_Constant::valid_mobile_direction( direction ));

	//
	//	Find a transmission buffer to suit.
	//
	if(!( buf = acquire_buffer( target, true, true ))) {
		errors.log_error( TRANSMISSION_TABLE_FULL, Protocol::mobile );
		return( false );
	}

	//
	//	Now create and append the command to the pending list.
	//
	if( !extend_buffer( buf, ( DCC_Constant::stationary_speed( speed )? TRANSIENT_COMMAND_REPEATS: 0 ), short_preamble, 1, command, compose_motion_packet( command, target, speed, direction ))) {

		TRACE_DCC( console.println( F( "DCC extend failed" )));

		cancel_buffer( buf );
		errors.log_error( TRANSMISSION_PENDING_FULL, Protocol::mobile );
		return( false );
	}

	//
	//	Save the reply to send when we to send it.
	//
	if( reply ) {
		reply->copy( buf->reply, maximum_output );
		buf->reply_when = reply_at_start;
	}
	else {
		buf->reply_when = reply_none;
	}
	
	//
	//	Finalise the record and kick it off.
	//
	return( complete_buffer( buf ));
}

//
//	Accessory Command
//
bool DCC::accessory_command( word target, byte state, Buffer_API *reply ) {
	
	STACK_TRACE( "bool DCC::accessory_command( word target, byte state, Buffer_API *reply )" );
	
	//
	//	Where we construct the DCC packet data and its
	//	associated reply.
	//
	trans_buffer			*buf;
	byte				command[ maximum_command ];
	
	word				pri_adrs;
	byte				sub_adrs;

	//
	//	Sanity Checks.
	//
	ASSERT( DCC_Constant::valid_accessory_ext_address( target ));
	ASSERT( DCC_Constant::valid_accessory_state( state ));

	//
	//	Break accessory address into primary and sub address
	//
	pri_adrs = DCC_Constant::internal_acc_adrs( target );
	sub_adrs = DCC_Constant::internal_acc_subadrs( target );

	//
	//	Find an unassigned empty transmission buffer.
	//
	if(!( buf = acquire_buffer( target, false, false ))) {
		errors.log_error( TRANSMISSION_TABLE_FULL, Protocol::accessory );
		return( false );
	}
	
	//
	//	Now create and append the command to the pending list.
	//
	if( !extend_buffer( buf, TRANSIENT_COMMAND_REPEATS, short_preamble, 1, command, compose_accessory_change( command, pri_adrs, sub_adrs, state ))) {
		cancel_buffer( buf );
		errors.log_error( TRANSMISSION_PENDING_FULL, Protocol::accessory );
		return( false );
	}

	//
	//	Save the reply to send when we to send it.
	//
	if( reply ) {
		reply->copy( buf->reply, maximum_output );
		buf->reply_when = reply_at_end;
	}
	else {
		buf->reply_when = reply_none;
	}

	//
	//	Finalise the record and kick it off.
	//
	return( complete_buffer( buf ));
}

bool DCC::function_command( word target, byte func, byte state, Buffer_API *reply ) {
	
	STACK_TRACE( "bool DCC::function_command( word target, byte func, byte state, Buffer_API *reply )" );
	
	//
	//	Where we construct the DCC packet data and its
	//	associated reply.
	//
	trans_buffer			*buf;
	byte				command[ maximum_command ];

	//
	//	Sanity Checks.
	//
	ASSERT( DCC_Constant::valid_mobile_target( target ));
	ASSERT( DCC_Constant::valid_function_number( func ));
	ASSERT( DCC_Constant::valid_function_state( state ));

	//
	//	Find an unassigned empty transmission buffer.
	//
	if(!( buf = acquire_buffer( target, true, false ))) {
		errors.log_error( TRANSMISSION_TABLE_FULL, Protocol::function );
		return( false );
	}
	
	//
	//	Now create and append the command(s) to the pending list.
	//
	if( state == DCC_Constant::function_toggle ) {
		if( !extend_buffer( buf, TRANSIENT_COMMAND_REPEATS, short_preamble, 1, command, compose_function_change( command, target, func, true ))) {
			cancel_buffer( buf );
			errors.log_error( TRANSMISSION_PENDING_FULL, Protocol::function );
			return( false );
		}
		if( !extend_buffer( buf, TRANSIENT_COMMAND_REPEATS, short_preamble, 1, command, compose_function_change( command, target, func, false ))) {
			cancel_buffer( buf );
			errors.log_error( TRANSMISSION_PENDING_FULL, Protocol::function );
			return( false );
		}
	}
	else {
		if( !extend_buffer( buf, TRANSIENT_COMMAND_REPEATS, short_preamble, 1, command, compose_function_change( command, target, func, ( state == DCC_Constant::function_on )))) {
			cancel_buffer( buf );
			errors.log_error( TRANSMISSION_PENDING_FULL, Protocol::function );
			return( false );
		}
	}
	
	//
	//	Save the reply to send when we to send it.
	//
	if( reply ) {
		reply->copy( buf->reply, maximum_output );
		buf->reply_when = reply_at_end;
	}
	else {
		buf->reply_when = reply_none;
	}
	
	//
	//	Finalise the record and kick it off.
	//
	return( complete_buffer( buf ));
}

bool DCC::state_command( word target, byte speed, byte dir, byte fn[ DCC_Constant::bit_map_array ], Buffer_API *reply ) {
	
	STACK_TRACE( "bool DCC::state_command( word target, byte speed, byte dir, byte fn[ DCC_Constant::bit_map_array ], Buffer_API *reply )" );
	
	//
	//	Where we construct the DCC packet data and its
	//	associated reply.
	//
	trans_buffer			*buf;
	byte				command[ maximum_command ];

	//
	//	The "state" and "length" variables used in the
	//	function mapping code.
	//
	byte				s, l;

	//
	//	Sanity checks - the function bits cannot be checked.
	//
	ASSERT( DCC_Constant::valid_mobile_target( target ));
	ASSERT( DCC_Constant::valid_mobile_speed( speed ));
	ASSERT( DCC_Constant::valid_mobile_direction( dir ));

	//
	//	Find a transmission buffer to suit.
	//
	if(!( buf = acquire_buffer( target, true, true ))) {
		errors.log_error( TRANSMISSION_TABLE_FULL, Protocol::rewrite_state );
		return( false );
	}

	//
	//	Run though the full set of function states.
	//
	s = 0;
	while(( l = compose_function_block( command, target, &s, fn )) > 0 ) {
		//
		//	Add this command to the pending list
		//
		if( !extend_buffer( buf, TRANSIENT_COMMAND_REPEATS, short_preamble, 1, command, l )) {
			cancel_buffer( buf );
			errors.log_error( TRANSMISSION_PENDING_FULL, Protocol::rewrite_state );
			return( false );
		}
	}
	
	//
	//	Now create and append the speed+direction command.
	//
	if( !extend_buffer( buf, ( DCC_Constant::stationary_speed( speed )? TRANSIENT_COMMAND_REPEATS: 0 ), short_preamble, 1, command, compose_motion_packet( command, target, speed, dir ))) {
		cancel_buffer( buf );
		errors.log_error( TRANSMISSION_PENDING_FULL, Protocol::rewrite_state );
		return( false );
	}

	//
	//	Save the reply to send when we to send it.
	//
	if( reply ) {
		reply->copy( buf->reply, maximum_output );
		buf->reply_when = reply_at_start;
	}
	else {
		buf->reply_when = reply_none;
	}
	//
	//	Finalise the record and kick it off.
	//
	return( complete_buffer( buf ));
}

//
//	Routines used to access statistical analysis
//
byte DCC::free_buffers( void ) {
	
	STACK_TRACE( "byte DCC::free_buffers( void )" );
	
	return( _free_buffers );
}

word DCC::packets_sent( void ) {
	
	STACK_TRACE( "word DCC::packets_sent( void )" );
	
	Critical	code;
	word		sent;

	sent = _packets_sent;
	_packets_sent = 0;

	TRACE_DCC( console.print( F( "DCC pkts " )));
	TRACE_DCC( console.println( sent ));

	return( sent );
}

#if defined( ENABLE_DCC_DELAY_REPORT ) || defined( ENABLE_DCC_SYNCHRONISATION )
byte DCC::irq_delay( void ) {
	return( _delay.last());
}
#endif


#if defined( ENABLE_DCC_SYNCHRONISATION )
byte DCC::irq_sync( void ) {
	return( _irq_sync );
}
#endif


//
//	Declare the DCC Generator.
//
DCC dcc_generator;

//
//	And its interface to the timer hardware.
//
ISR( DCC_TIMERn_COMPA_vect ) {
	COUNT_INTERRUPT;
	dcc_generator.clock_pulse();
}

//
//	EOF
//
