///
///	trace.h		A file where control of code and data tracing
///			options are consolidated.
///
///	Copyright (c) 2021 Jeff Penfold.  All right reserved.
///
///	This library is free software; you can redistribute it and/or
///	modify it under the terms of the GNU Lesser General Public
///	License as published by the Free Software Foundation; either
///	version 2.1 of the License, or (at your option) any later version.
///
///	This library is distributed in the hope that it will be useful,
///	but WITHOUT ANY WARRANTY; without even the implied warranty of
///	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
///	Lesser General Public License for more details.
///
///	You should have received a copy of the GNU Lesser General Public
///	License along with this library; if not, write to the Free Software
///	Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301
///	USA
///

#ifndef _TRACE_H_
#define _TRACE_H_

//
//	Define any number of the following macros to facilitate the
//	control of trace output to the console.
//
//	ENABLE_TRACE_TWI	Trace Data IO across the IIC bus
//	ENABLE_TRACE_LCD	Trace the screen buffer actions
//
//	Definition of any of these will automatically disable the
//	output of "proper" console traffic from the firmware to
//	avoid "mixing up" the output and rendering any trace output
//	undecipherable.
//
//#define ENABLE_TRACE_TWI
//#define ENABLE_TRACE_LCD

//
//	The macros definitions for each traceable unit.
//
#ifdef ENABLE_TRACE_TWI
#define TRACE_TWI(c)	c
#ifndef DISABLE_OUTPUT
#define DISABLE_OUTPUT
#endif
#else
#define TRACE_TWI(c)
#endif

#ifdef ENABLE_TRACE_LCD
#define TRACE_LCD(c)	c
#ifndef DISABLE_OUTPUT
#define DISABLE_OUTPUT
#endif
#else
#define TRACE_LCD(c)
#endif

//
//	Finally, if the console output from the firmware has been
//	disabled then we need to configure that macro appropriately.
//
//	We fake the code working if there is no actual code.
//
#ifdef DISABLE_OUTPUT
#define FIRMWARE_OUTPUT(c)		true
#define SYNCHRONOUS_BUFFER
#else
#define FIRMWARE_OUTPUT(c)		c
#endif


#endif

//
//	EOF
//
