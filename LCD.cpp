//
//	LCD - Arduino library to control an LCD via the TWI_IO Library
//
//	Copyright(C) 2021 Jeff Penfold <jeff.penfold@googlemail.com>
//
//	This program is free software : you can redistribute it and /or modify
//	it under the terms of the GNU General Public License as published by
//	the Free Software Foundation, either version 3 of the License, or
//	(at your option) any later version.
//
//	This program is distributed in the hope that it will be useful,
//	but WITHOUT ANY WARRANTY; without even the implied warranty of
//	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.See the
//	GNU General Public License for more details.
//
//	You should have received a copy of the GNU General Public License
//	along with this program.  If not, see < https://www.gnu.org/licenses/>.
//

//
//	Environment for this module
//
#include "LCD.h"
#include "Trace.h"
#include "Critical.h"
#include "TWI.h"
#include "Errors.h"
#include "Task.h"
#include "Clock.h"


//
//	The LCD "programs" for sending data to the
//	display via the TWI IO module.
//

//
//	Define two "system" programs which are used to handle situations
//	that arise during the LCDs operation.
//
static const LCD::mc_state LCD::mc_idle_program[] PROGMEM = {
	mc_idle
};
static const LCD::mc_state LCD::mc_reset_program[] PROGMEM = {
	mc_reset, mc_transmit_buffer, mc_wait_on_done, mc_set_delay_40000us, mc_delay_wait,
	mc_idle
};

//
//	The init programs are used only during the initial power on
//	sequence and have to use timed delays to complete their
//	operating sequence.
//
static const LCD::mc_state LCD::mc_init_long_delay[] PROGMEM = {
	mc_inst_high_enable, mc_transmit_buffer, mc_wait_on_done, mc_set_delay_10us, mc_delay_wait,
	mc_inst_high_disable, mc_transmit_buffer, mc_wait_on_done, mc_set_delay_4200us, mc_delay_wait,
	mc_finish_up, mc_idle
};
static const LCD::mc_state LCD::mc_init_medium_delay[] PROGMEM = {
	mc_inst_high_enable, mc_transmit_buffer, mc_wait_on_done, mc_set_delay_10us, mc_delay_wait,
	mc_inst_high_disable, mc_transmit_buffer, mc_wait_on_done, mc_set_delay_150us, mc_delay_wait,
	mc_finish_up, mc_idle
};
static const LCD::mc_state LCD::mc_init_short_delay[] PROGMEM = {
	mc_inst_high_enable, mc_transmit_buffer, mc_wait_on_done, mc_set_delay_10us, mc_delay_wait,
	mc_inst_high_disable, mc_transmit_buffer, mc_wait_on_done, mc_set_delay_37us, mc_delay_wait,
	mc_finish_up, mc_idle
};

//
//	The Instruciton and Data programs now use the "Busy Flag"
//	to determine if the LCD is ready to accept the next command.
//
static const LCD::mc_state LCD::mc_send_inst[] PROGMEM = {
#if _LCD_USE_READ_BUSY_READY_
	mc_begin_wait,
	mc_status_enable, mc_transmit_buffer, mc_wait_on_done, mc_set_delay_10us, mc_delay_wait,
	mc_status_disable, mc_transmit_buffer, mc_wait_on_done, mc_set_delay_37us, mc_delay_wait,
	mc_read_buffer, mc_wait_on_done, mc_store_high_data,
	mc_status_enable, mc_transmit_buffer, mc_wait_on_done, mc_set_delay_10us, mc_delay_wait,
	mc_status_disable, mc_transmit_buffer, mc_wait_on_done, mc_set_delay_37us, mc_delay_wait,
	mc_read_buffer, mc_wait_on_done, mc_store_low_data,
	mc_wait_loop,
	mc_inst_high_enable, mc_transmit_buffer, mc_wait_on_done, mc_set_delay_10us, mc_delay_wait,
	mc_inst_high_disable, mc_transmit_buffer, mc_wait_on_done, mc_set_delay_37us, mc_delay_wait,
	mc_inst_low_enable, mc_transmit_buffer, mc_wait_on_done, mc_set_delay_10us, mc_delay_wait,
	mc_inst_low_disable, mc_transmit_buffer, mc_wait_on_done, mc_set_delay_37us, mc_delay_wait,
	mc_finish_up, mc_idle
#else
	mc_inst_high_enable, mc_transmit_buffer, mc_wait_on_done, mc_set_delay_10us, mc_delay_wait,
	mc_inst_high_disable, mc_transmit_buffer, mc_wait_on_done, mc_set_delay_37us, mc_delay_wait,
	mc_inst_low_enable, mc_transmit_buffer, mc_wait_on_done, mc_set_delay_10us, mc_delay_wait,
	mc_inst_low_disable, mc_transmit_buffer, mc_wait_on_done, mc_set_delay_1600us, mc_delay_wait,
	mc_finish_up, mc_idle
#endif
};
static const LCD::mc_state LCD::mc_send_data[] PROGMEM = {
#if _LCD_USE_READ_BUSY_READY_
	mc_begin_wait,
	mc_status_enable, mc_transmit_buffer, mc_wait_on_done, mc_set_delay_10us, mc_delay_wait,
	mc_status_disable, mc_transmit_buffer, mc_wait_on_done, mc_set_delay_37us, mc_delay_wait,
	mc_read_buffer, mc_wait_on_done, mc_store_high_data,
	mc_status_enable, mc_transmit_buffer, mc_wait_on_done, mc_set_delay_10us, mc_delay_wait,
	mc_status_disable, mc_transmit_buffer, mc_wait_on_done, mc_set_delay_37us, mc_delay_wait,
	mc_read_buffer, mc_wait_on_done, mc_store_low_data,
	mc_wait_loop,
	mc_data_high_enable, mc_transmit_buffer, mc_wait_on_done, mc_set_delay_10us, mc_delay_wait,
	mc_data_high_disable, mc_transmit_buffer, mc_wait_on_done, mc_set_delay_37us, mc_delay_wait,
	mc_data_low_enable, mc_transmit_buffer, mc_wait_on_done, mc_set_delay_10us, mc_delay_wait,
	mc_data_low_disable, mc_transmit_buffer, mc_wait_on_done, mc_set_delay_37us, mc_delay_wait,
	mc_finish_up, mc_idle
#else
	mc_data_high_enable, mc_transmit_buffer, mc_wait_on_done, mc_set_delay_10us, mc_delay_wait,
	mc_data_high_disable, mc_transmit_buffer, mc_wait_on_done, mc_set_delay_37us, mc_delay_wait,
	mc_data_low_enable, mc_transmit_buffer, mc_wait_on_done, mc_set_delay_10us, mc_delay_wait,
	mc_data_low_disable, mc_transmit_buffer, mc_wait_on_done, mc_set_delay_37us, mc_delay_wait,
	mc_finish_up, mc_idle
#endif
};


//
//	bool queue_transfer( const mc_state *program, byte value )
//	-----------------------------------------------------
//
//	Routine used to add another action to the queue, return true
//	if the action has been queued, false otherwise.
//
bool LCD::queue_transfer( const mc_state *program, byte value, Signal *flag ) {
	
	if( _queue_len >= max_pending ) return( false );
	
	_queue[ _queue_in ].value = value;
	_queue[ _queue_in ].program = program;
	_queue[ _queue_in ].flag = flag;
	
	if( ++_queue_in >= max_pending ) _queue_in = 0;

	//
	//	Finally increase the number of pending records
	//	and if this results in a queue of 1 entry then
	//	we need to release the control flag to restart
	//	the FSM.
	//
	if(( _queue_len += 1 ) == 1 ) _flag.release();
	
	return( true );
}

//
//	bool queue_transfer_wait( const byte *program, byte value )
//	---------------------------------------------------------
//
//	Routine used to add another action to the queue but waits until the
//	action has been successfully added to the queue.
//
//	This being done, the routine will then wait until the action has
//	been completed.
//
//	This will block the execution of the firmware at this point.
//
void LCD::queue_transfer_wait( const LCD::mc_state *program, byte value ) {
	Signal	wait;
	
	while( !queue_transfer( program, value, &wait )) task_manager.pole_task();
	while( !wait.acquire()) task_manager.pole_task();
}

//
//	Constructor!
//	============
//
LCD::LCD( void ) {
	//
	//	No details of the LCD, yet.
	//
	_adrs = 0;
	_rows = 0;
	_cols = 0;
	//
	//	Empty the pending queue in preparation for the
	//	initial initialisation code to execute.
	//
	_queue_len = 0;
	_queue_in = 0;
	_queue_out = 0;
	//
	//	Set the back light off and the state variables
	//	as empty.
	//
	_back_light = 0;
	_display_state = 0;
	_entry_state = 0;
	//
	//	Initialise the program machine which controls
	//	data transmission to the LCD
	//
	_fsm_instruction = mc_idle_program;
	_fsm_loop = NIL( mc_state );
	_fsm_data_byte = 0;
	_fsm_buffer = 0;
}

void LCD::initialise( byte adrs, byte rows, byte cols ) {
	//
	//	Save the physical details of the display
	//
	_adrs = adrs;
	_rows = rows;
	_cols = cols;
	//
	//	Attach this object to the task manager
	//
	task_manager.add_task( this, &_flag );
	//
	//	Clear i2c adapter, then wait more than 40ms after powerOn.
	//
	queue_transfer_wait( mc_reset_program, 0b00000000 );
	//
	//	See HD44780U datasheet "Initializing by Instruction" Figure 24 (4-Bit Interface)
	//
	//	Now set up the LCD
	//
	//	Function set flags
	//
	//	DL
	//	--
	//	0	4-bit data IO
	//	1	8-bit data IO
	//
	//	4-bit "Function Set" opcode is
	//
	//		0-0-1-DL
	//
	//	N F	Lines	Font
	//	- -	-----	----
	//	0 0	1	5x8
	//	0 1	1	5x10
	//	1 0	2	5x8
	//
	//	8-bit "Function Set" opcode is
	//
	//		0-0-1-DL-N-F-X-X
	//
	//	These are all written to the Instruction Register
	//	of the LCD (RS = 0, R/W = 0)
	//	
	queue_transfer_wait( mc_init_long_delay, 0b00110000 );	// Function Set + 8 bit mode (As a 4 bit instruction)
	queue_transfer_wait( mc_init_medium_delay, 0b00110000 );	// Function Set + 8 bit mode (As a 4 bit instruction)
	queue_transfer_wait( mc_init_short_delay, 0b00110000 );	// Function Set + 8 bit mode (As a 4 bit instruction)
	queue_transfer_wait( mc_init_short_delay, 0b00100000 );	// Function Set + 4 bit mode (As a 4 bit instruction)
	if( _rows == 1 ) {
		//
		//	1 line displays get initialised here
		//
		queue_transfer_wait( mc_send_inst, 0b00100000 );	// Function Set + 4 bit mode + 1 line + 5x8 font (As an 8 bit instruction)
	}
	else {
		//
		//	2 and 4 line displays here (as 4 line displays
		//	are doubled up 2 line displays).
		//
		queue_transfer_wait( mc_send_inst, 0b00101000 );	// Function Set + 4 bit mode + 2 lines + 5x8 font (As an 8 bit instruction)
	}
	//
	//	And pause..
	//
	event_timer.inline_delay( MSECS( 1000 ));
	//
	//	Now some ordinary tidy up steps.
	//
	display( true );
	clear();
	leftToRight( true );
	cursor( false );
	blink( false );
	backlight( true );
}

//
//	This routine called each time round loop to support going
//	LCD processes.
//
void LCD::process( void ) {
	//
	//	Everything done in this routine is under the control
	//	of the current LCD program.  This is, effectively,
	//	where all the actual work gets done.
	//
main_loop:
	switch( progmem_read_byte_at( _fsm_instruction )) {
		case mc_idle: {			// machine at idle
			//
			//	If we are here and there is something in the queue
			//	we load up the necessary components and run the
			//	program.
			//
			if( _queue_len ) {
				//
				//	Load the _fsm_ variables with the required
				//	data to run this job and remove the job from
				//	the queue.
				//
				_fsm_data_byte = _queue[ _queue_out ].value;
				_fsm_instruction = _queue[ _queue_out ].program;
				_fsm_flag = _queue[ _queue_out ].flag;

				if(( _queue_out += 1 ) >= max_pending ) _queue_out = 0;
				_queue_len--;

				goto main_loop;
			}
			//
			//	Leaving here means that the control flag is "empty"
			//	and the FSM will only restart once a new pending record
			//	has been added (and so release the control flag).
			//
			break;
		};
		case mc_reset: {		// Resetting the device by sending
			_fsm_buffer = 0;	// an empty byte.
			_fsm_instruction++;
			goto main_loop;
		}
		case mc_inst_high_enable: {	// send high nybble with E=1 as inst
			//
			//	RS = 0	(Select IR)
			//	R/W = 0 (Write)
			//	E = 1	(Enable)
			//
			_fsm_buffer = high_nybble( _fsm_data_byte ) | _back_light | bit( enable );
			_fsm_instruction++;
			goto main_loop;
		};
		case mc_inst_high_disable: {	// send high nybble with E=0 as inst
			//
			//	RS = 0	(Select IR)
			//	R/W = 0 (Write)
			//	E = 0	(Disable)
			//
			_fsm_buffer = high_nybble( _fsm_data_byte ) | _back_light;
			_fsm_instruction++;
			goto main_loop;
		};
		case mc_inst_low_enable: {	// send low nybble with E=1 as inst
			//
			//	RS = 0	(Select IR)
			//	R/W = 0 (Write)
			//	E = 1	(Enable)
			//
			_fsm_buffer = low_nybble( _fsm_data_byte ) | _back_light | bit( enable );
			_fsm_instruction++;
			goto main_loop;
		};
		case mc_inst_low_disable: {	// send low nybble with E=0 as inst
			//
			//	RS = 0	(Select IR)
			//	R/W = 0 (Write)
			//	E = 0	(Disable)
			//
			_fsm_buffer = low_nybble( _fsm_data_byte ) | _back_light;
			_fsm_instruction++;
			goto main_loop;
		};
		case mc_data_high_enable: {	// send high nybble with E=1 as data
			//
			//	RS = 1	(Select DR)
			//	R/W = 0 (Write)
			//	E = 1	(Enable)
			//
			_fsm_buffer = high_nybble( _fsm_data_byte ) | _back_light | bit( register_select ) | bit( enable );
			_fsm_instruction++;
			goto main_loop;
		};
		case mc_data_high_disable: {	// send high nybble with E=0 as data
			//
			//	RS = 1	(Select DR)
			//	R/W = 0 (Write)
			//	E = 0	(Disable)
			//
			_fsm_buffer = high_nybble( _fsm_data_byte ) | _back_light | bit( register_select );
			_fsm_instruction++;
			goto main_loop;
		};
		case mc_data_low_enable: {	// send low nybble with E=1 as data
			//
			//	RS = 1	(Select DR)
			//	R/W = 0 (Write)
			//	E = 1	(Enable)
			//
			_fsm_buffer = low_nybble( _fsm_data_byte ) | _back_light | bit( register_select ) | bit( enable );
			_fsm_instruction++;
			goto main_loop;
		};
		case mc_data_low_disable: {	// send low nybble with E=0 as data
			//
			//	RS = 1	(Select DR)
			//	R/W = 0 (Write)
			//	E = 0	(Disable)
			//
			_fsm_buffer = low_nybble( _fsm_data_byte ) | _back_light | bit( register_select );
			_fsm_instruction++;
			goto main_loop;
		};
		case mc_status_enable: {	// send status request with E=1 as inst
			//
			//	RS = 0	(Select IR)
			//	R/W = 1 (Read)
			//	E = 1	(Enable)
			//
			_fsm_buffer = 0xf0 | _back_light | bit( read_write ) | bit( enable );
			_fsm_instruction++;
			goto main_loop;
		};
		case mc_status_disable: {	// send status request with E=0 as inst
			//
			//	RS = 0	(Select IR)
			//	R/W = 1 (Read)
			//	E = 0	(Disable)
			//
			_fsm_buffer = 0xf0 | _back_light | bit( read_write );
			_fsm_instruction++;
			goto main_loop;
		};
		case mc_read_buffer: {		// read the interface into the buffer
			//
			//	We read and read again until this returns true then
			//	move to the next instruction
			//
			if( twi.receive_byte( _adrs, &_fsm_buffer, &_flag, &_error )) {
				//
				//	Sent the TWI command; we will be woken up when the
				//	command is completed.
				//
				_fsm_instruction++;
			}
			else {
				//
				//	Failed to submit the command, so we delay this
				//	code and try again in a short while.
				//
				if( !event_timer.delay_event( processing_delay, &_flag, false )) {
					//
					//	Failing to schedule an actual timed delay, we log an error
					//	and then effect a 0 second delay by self-scheduling
					//	the task manager to call us again.
					//
					errors.log_error( EVENT_TIMER_QUEUE_FULL, LCD_PROCESSING_DELAY );
					_flag.release();
				}
			}
			//
			//	If all cases, when we get here we are expecting the flag signal
			//	to initiate this code to restarted.
			//
			break;
		}
		case mc_store_high_data: {	// place buffer into data high bits
			_fsm_data_byte = ( _fsm_buffer & 0xf0 )|( _fsm_data_byte & 0x0f );
			_fsm_instruction++;
			goto main_loop;
		}
		case mc_store_low_data: {	// place buffer into data high bits
			_fsm_data_byte = (( _fsm_buffer & 0xf0 ) >> 4 )|( _fsm_data_byte & 0xf0 );
			_fsm_instruction++;
			goto main_loop;
		}
		case mc_transmit_buffer: {	// send the content of the buffer
			//
			//	We send and send again until this returns true then
			//	move to the next instruction
			//
			if( twi.send_data( _adrs, &_fsm_buffer, 1, &_flag, &_error )) {
				//
				//	Sent the TWI command; we will be woken up when the
				//	command is completed.
				//
				_fsm_instruction++;
			}
			else {
				//
				//	Failed to submit the command, so we delay this
				//	code and try again in a short while.
				//
				if( !event_timer.delay_event( processing_delay, &_flag, false )) {
					//
					//	Failing to schedule an actual timed delay, we log an error
					//	and then effect a 0 second delay by self-scheduling
					//	the task manager to call us again.
					//
					errors.log_error( EVENT_TIMER_QUEUE_FULL, LCD_PROCESSING_DELAY );
					_flag.release();
				}
			}
			//
			//	If all cases, when we get here we are expecting the flag signal
			//	to initiate this code to restarted.
			//
			break;
		}
		case mc_wait_on_done: {		// Wait for the TWI action to complete
			//
			//	Transmission completed
			//
			if( _error == TWI::error_none ) {
				//
				//	Successfully
				//
				_fsm_instruction++;
			}
			else {
				//
				//	Unsuccessfully
				//
				//	What do we do?  We are (probably)
				//	in the middle of a series of a series
				//	of data transfers such that "losing"
				//	one will break the activity.
				//
				//	Best option is to drop all pending
				//	activities and execute a reset.
				//
				//	However - we need to remember that
				//	a series of other tasks might be
				//	waiting for flags to be released.
				//
				//	So, we have to handle this directly
				//	or the firmware *will* lock up.
				//
				//	Did the "in flight" action have a signal?
				//
				if( _fsm_flag ) _fsm_flag->release();
				//
				//	Unroll all the pending actions checking
				//	the flag pointers.
				//
				while( _queue_len ) {
					if(( _fsm_flag = _queue[ _queue_out ].flag )) _fsm_flag->release();
					if(( _queue_out += 1 ) >= max_pending ) _queue_out = 0;
					_queue_len--;
				}
				//
				//	Reset everything to "initial values"
				//
				_queue_len = 0;
				_queue_in = 0;
				_queue_out = 0;
				//
				//	Now we execute the "reset program" to try
				//	restore control of the LCD.
				//
				_fsm_instruction = mc_reset_program;
				_fsm_loop = NIL( mc_state );
				_fsm_flag = NIL( Signal );
			}
			goto main_loop;
		}
		case mc_begin_wait: {		// Note the top of the wait loop
			//
			//	Just need to remember the instruction pointer
			//	after this one (no need to re-run this code
			//	for each loop.
			//
			_fsm_loop = ++_fsm_instruction;
			goto main_loop;
		}
		case mc_wait_loop: {		// If not ready loop again
			//
			//	The Busy Flag is the top bit of the data recovered from
			//	the LCD.
			//
			if( _fsm_data_byte & 0x80 ) {
				_fsm_instruction++;
			}
			else {
				_fsm_instruction = _fsm_loop;
			}
			goto main_loop;
		}
		case mc_finish_up: {
			//
			//	This is the final step in the execution of a command
			//	and is, essentially, here to signal to the owner of
			//	the action that it is complete (one way or the other).
			//
			if( _fsm_flag ) _fsm_flag->release();
			_fsm_instruction++;
			goto main_loop;
		}
		case mc_set_delay_40000us: {	// Set delay countdown to 40000us
			if( !event_timer.delay_event( MSECS( 40 ), &_flag, false )) {
				//
				//	Failing to schedule an actual timed delay, we log
				//	an error and then effect a 0 second delay by self
				//	scheduling the task manager to call us again.
				//
				errors.log_error( EVENT_TIMER_QUEUE_FULL, 40000 );
				_flag.release();
			}
			_fsm_instruction++;
			break;
		}
		case mc_set_delay_4200us: {	// 4200us
			if( !event_timer.delay_event( USECS( 4200 ), &_flag, false )) {
				//
				//	Failing to schedule an actual timed delay, we log an error
				//	and then effect a 0 second delay by self-scheduling
				//	the task manager to call us again.
				//
				errors.log_error( EVENT_TIMER_QUEUE_FULL, 4 );
				_flag.release();
			}
			_fsm_instruction++;
			break;
		}
		case mc_set_delay_1600us: {	// 1600us
			if( !event_timer.delay_event( USECS( 1600 ), &_flag, false )) {
				//
				//	Failing to schedule an actual timed delay, we log an error
				//	and then effect a 0 second delay by self-scheduling
				//	the task manager to call us again.
				//
				errors.log_error( EVENT_TIMER_QUEUE_FULL, 2 );
				_flag.release();
			}
			_fsm_instruction++;
			break;
		}
		case mc_set_delay_150us: {	// 150us
			if( !event_timer.delay_event( USECS( 150 ), &_flag, false )) {
				//
				//	Failing to schedule an actual timed delay, we log an error
				//	and then effect a 0 second delay by self-scheduling
				//	the task manager to call us again.
				//
				errors.log_error( EVENT_TIMER_QUEUE_FULL, 0 );
				_flag.release();
			}
			_fsm_instruction++;
			break;
		}
		case mc_set_delay_41us: {	// 41us
			if( !event_timer.delay_event( USECS( 41 ), &_flag, false )) {
				//
				//	Failing to schedule an actual timed delay, we log an error
				//	and then effect a 0 second delay by self-scheduling
				//	the task manager to call us again.
				//
				errors.log_error( EVENT_TIMER_QUEUE_FULL, 0 );
				_flag.release();
			}
			_fsm_instruction++;
			break;
		}
		case mc_set_delay_37us: {	// 37us
			if( !event_timer.delay_event( USECS( 37 ), &_flag, false )) {
				//
				//	Failing to schedule an actual timed delay, we log an error
				//	and then effect a 0 second delay by self-scheduling
				//	the task manager to call us again.
				//
				errors.log_error( EVENT_TIMER_QUEUE_FULL, 0 );
				_flag.release();
			}
			_fsm_instruction++;
			break;
		}
		case mc_set_delay_10us: {	// 10us
			if( !event_timer.delay_event( USECS( 10 ), &_flag, false )) {
				//
				//	Failing to schedule an actual timed delay, we log an error
				//	and then effect a 0 second delay by self-scheduling
				//	the task manager to call us again.
				//
				errors.log_error( EVENT_TIMER_QUEUE_FULL, 0 );
				_flag.release();
			}
			_fsm_instruction++;
			break;
		}
		case mc_delay_wait: {	// Wait until the delay period has expired
			_fsm_instruction++;
			goto main_loop;
		}
		default: {
			//
			//	This should not happen, but if it does then
			//	there is a real problem.  All I can do is
			//	log an error (if possible) and reset the program.
			//	Error logging not implement yet.
			//
			_fsm_instruction = mc_idle_program;
			goto main_loop;
		}
	}
}

//
//	Functional Routines
//	===================
//
//	These return true if the command/action was
//	successfully queued, false otherwise.
//
bool LCD::backlight( bool on, Signal *flag ) {
	//
	//	The back light is explicitly controlled by
	//	one of the pins on the LCDs primary hardware
	//	interface.  The _back_light variable is
	//	used as part of the mask when writing to
	//	the LCD so the LCD is controlled simply by
	//	setting (or resetting) this specific bit.
	//
	bitWrite( _back_light, LED_backlight, on );
	//
	//	The change in backlight status will be
	//	implemented on any following command to
	//	the LCD.  If a signal has been included
	//	set it.
	//
	if( flag ) flag->release();
	return( true );
}

bool LCD::clear( Signal *flag ) {

	TRACE_LCD( console.write( 'C' ));
	
	return( queue_transfer( mc_send_inst, clear_screen, flag ));
}

bool LCD::home( Signal *flag ) {

	TRACE_LCD( console.write( 'H' ));
	
	return( queue_transfer( mc_send_inst, home_screen,flag ));
}

bool LCD::leftToRight( bool l2r, Signal *flag ) {
	//
	//	Note that this facility does not apply consistently
	//	to "wrapping" between lines.  If you write off the end
	//	of line 0 it rolls into line 2.  Like wise rolling off
	//	line 1 rolls into line 3.
	//
	//	This is caused entirely by the memory mapping between
	//	the driver chip and the actual display.
	//
	bitWrite( _entry_state, left_right, l2r );
	return( queue_transfer( mc_send_inst, entry_state | _entry_state, flag ));
}

bool LCD::autoscroll( bool on, Signal *flag ) {
	bitWrite( _entry_state, auto_scroll, on );
	return( queue_transfer( mc_send_inst, entry_state | _entry_state, flag ));
}

bool LCD::display( bool on, Signal *flag ) {
	bitWrite( _display_state, display_on, on );
	return( queue_transfer( mc_send_inst, display_state | _display_state, flag ));
}

bool LCD::cursor( bool on, Signal *flag ) {
	bitWrite( _display_state, cursor_on, on );
	return( queue_transfer( mc_send_inst, display_state | _display_state, flag ));
}

bool LCD::blink( bool on, Signal *flag ) {
	bitWrite( _display_state, blink_on, on );
	return( queue_transfer( mc_send_inst, display_state | _display_state, flag ));
}

bool LCD::position( byte row, byte col, Signal *flag ) {

	TRACE_LCD( console.println());
	TRACE_LCD( console.write( 'P' ));
	TRACE_LCD( console.print_hex( col ));
	TRACE_LCD( console.print_hex( row ));

	//
	//	We will modify the column value to correctly
	//	reflect the memory mapped position required.
	//
	//	For ODD rows we add the fixed offset
	//
	if( row & 1 ) col += 0x40;
	//
	//	For the repeated rows add the total number of columns
	//
	if( row & 2 ) col += _cols;
	//
	//	Transmit command
	//
	return( queue_transfer( mc_send_inst, set_position | col, flag ));
}

bool LCD::index( byte posn, Signal *flag ) {

	TRACE_LCD( console.println());
	TRACE_LCD( console.write( 'I' ));
	TRACE_LCD( console.print_hex( posn ));

	//
	//	similar to the position() routine, but views
	//	the buffer simply as a series of bytes starting
	//	at the top left and finishing at the bottom.
	//
	//	This "ignores" the underlying memory mapping which
	//	effectively shuffles the rows.
	//
	//	Line 0
	//
	if( posn < _cols ) return( queue_transfer( mc_send_inst, set_position | posn, flag ));
	//
	//	Line 1
	//
	if(( posn -= _cols ) < _cols ) return( queue_transfer( mc_send_inst, set_position |( 0x40 + posn ), flag ));
	//
	//	Line 2
	//
	if(( posn -= _cols ) < _cols ) return( queue_transfer( mc_send_inst, set_position |( _cols + posn ), flag ));
	//
	//	Line 3
	//
	if(( posn -= _cols ) < _cols ) return( queue_transfer( mc_send_inst, set_position |( 0x40 + _cols + posn ), flag ));
	//
	//	Out of bounds, put it top left, position 0.
	//
	return( queue_transfer( mc_send_inst, set_position, flag ));
}

bool LCD::write( byte val, Signal *flag ) {

	TRACE_LCD( console.write( 'W' ));
	TRACE_LCD( console.print_hex( val ));

	return( queue_transfer( mc_send_data, val, flag ));
}


//
//	EOF
//
