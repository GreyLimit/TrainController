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
#include "Environment.h"
#include "Parameters.h"
#include "Configuration.h"
#include "Trace.h"
#include "Critical.h"
#include "TWI.h"
#include "LCD.h"
#include "Errors.h"
#include "Task.h"
#include "Console.h"


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
	mc_idle
};
static const LCD::mc_state LCD::mc_init_medium_delay[] PROGMEM = {
	mc_inst_high_enable, mc_transmit_buffer, mc_wait_on_done, mc_set_delay_10us, mc_delay_wait,
	mc_inst_high_disable, mc_transmit_buffer, mc_wait_on_done, mc_set_delay_150us, mc_delay_wait,
	mc_idle
};
static const LCD::mc_state LCD::mc_init_short_delay[] PROGMEM = {
	mc_inst_high_enable, mc_transmit_buffer, mc_wait_on_done, mc_set_delay_10us, mc_delay_wait,
	mc_inst_high_disable, mc_transmit_buffer, mc_wait_on_done, mc_set_delay_37us, mc_delay_wait,
	mc_idle
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
	mc_idle
#else
	mc_inst_high_enable, mc_transmit_buffer, mc_wait_on_done, mc_set_delay_10us, mc_delay_wait,
	mc_inst_high_disable, mc_transmit_buffer, mc_wait_on_done, mc_set_delay_37us, mc_delay_wait,
	mc_inst_low_enable, mc_transmit_buffer, mc_wait_on_done, mc_set_delay_10us, mc_delay_wait,
	mc_inst_low_disable, mc_transmit_buffer, mc_wait_on_done, mc_set_delay_1600us, mc_delay_wait,
	mc_idle
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
	mc_idle
#else
	mc_data_high_enable, mc_transmit_buffer, mc_wait_on_done, mc_set_delay_10us, mc_delay_wait,
	mc_data_high_disable, mc_transmit_buffer, mc_wait_on_done, mc_set_delay_37us, mc_delay_wait,
	mc_data_low_enable, mc_transmit_buffer, mc_wait_on_done, mc_set_delay_10us, mc_delay_wait,
	mc_data_low_disable, mc_transmit_buffer, mc_wait_on_done, mc_set_delay_37us, mc_delay_wait,
	mc_idle
#endif
};


//
//	EOF
//
