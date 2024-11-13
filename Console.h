///
///	Console - code to contain the console object.
///
/// 	Firmware for an Arduino Uno R3 and Motor shield (with other
///	components) to create a simple and free standing "Train
///	Controller" to be as simple as possible.
///
///	Copyright (C) 2020, Jeff Penfold, jeff.penfold@googlemail.com
///	
///	This program is free software: you can redistribute it and/or modify
///	it under the terms of the GNU General Public License as published by
///	the Free Software Foundation, either version 3 of the License, or
///	(at your option) any later version.
///
///	This program is distributed in the hope that it will be useful,
///	but WITHOUT ANY WARRANTY; without even the implied warranty of
///	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
///	GNU General Public License for more details.
///
///	You should have received a copy of the GNU General Public License
///	along with this program.  If not, see <https://www.gnu.org/licenses/>.
///

#ifndef _CONSOLE_H_
#define _CONSOLE_H_

//
//	Bring in USART definitions
//
#include "Parameters.h"
#include "Byte_Queue.h"
#include "USART.h"

//
//	Define the console buffer sizes, if not predefined elsewhere.
//
#ifndef CONSOLE_INPUT
#define CONSOLE_INPUT	32
#endif
#ifndef CONSOLE_OUTPUT
#define CONSOLE_OUTPUT	128
#endif

//
//	The CONSOLE device
//	==================
//
extern USART_IO		console;

//
//	The call to initialise it.
//
extern void initialise_console( USART_line_speed speed );
 

#endif

//
//	EOF
//
