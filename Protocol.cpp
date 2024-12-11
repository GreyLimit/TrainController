//
//	Protocol.cpp
//	============
//
//	Computer IO Protocol
//

#include "Protocol.h"
#include "Signal.h"
#include "Task.h"
#include "Errors.h"
#include "Code_Assurance.h"

//
//	Set up ready to be initialised.
//
Protocol::Protocol( void ) {
	_inside = false;
	_valid = true;
	_len = 0;
}

//
//	Define simple "in string" number parsing routine.  This
//	effectively does the bulk of the syntax parsing for the
//	commands so it a little more fiddly than strictly
//	necessary.
//
//	Return the address of the next unparsed character, this
//	be EOS if at the end of the string.
//
char *Protocol::parse_number( char *buf, bool *found, int *value ) {
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
//	Parse an input buffer.
//
void Protocol::parse_buffer( UNUSED( char *buf ), UNUSED( int len )) {
}


void Protocol::initialise( void ) {
	//
	//	All we do, really, is link ourselves into the console
	//	stream.  Now the 'process()' routine is called every
	//	time there is data to be processed.
	//
	task_manager.add_task( this, console_control());
}

//
//	The task entry point.
//
void Protocol::process( void ) {
	char	data;

	switch(( data = console.read())) {
		//
		//	The protocol wrappers
		//
		case lead_in: {
			//
			//	A new packet starts here.  If we were already
			//	inside one then we throw it away.
			//
			if( _inside ) errors.log_error( DCC_COMMAND_TRUNCATED, _len );
			//
			//	Set up for a new command to arrive.
			//
			_len = 0;
			_inside = true;
			_valid = true;
			break;
		}
		case lead_out: {
			//
			//	The end of a protocol packet - process it.
			//
			//	Sanity checks.
			//
			if( !_inside ) {
				errors.log_error( DCC_PROTOCOL_ERROR, 0 );
				break;
			}
			if( _len == 0 ) {
				errors.log_error( DCC_COMMAND_EMPTY, 0 );
				break;
			}

			//
			//	Parse buffer (if there was no error)  and
			//	reset for next command.
			//
			if( _valid ) {
				ASSERT( _len < buffer_size );
				_buffer[ _len ] = EOS;
				parse_buffer( _buffer, _len );
			}
			_len = 0;
			_inside = false;
			_valid = true;
			break;
		}
		default: {
			//
			//	Some for a packet or just "other stuff" moving
			//	on past...
			//
			if( _inside ) {
				if( _len < buffer_size-1 ) {
					_buffer[ _len++ ] = data;
				}
				else {
					if( _valid ) {
						_valid = false;
						errors.log_error( DCC_COMMAND_TRUNCATED, _len );
					}
				}
			}
			break;
		}
	}
}


#if 0

JEFF write stuff here now!

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
								errors.log_error( INVALID_DCC_COMMAND, 0 );
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


#endif


//
//	Here is the protocol engine.
//
Protocol protocol;


//
//	EOF
//
