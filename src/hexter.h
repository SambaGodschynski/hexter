/* hexter DSSI software synthesizer plugin
 *
 * Copyright (C) 2004 Sean Bolton and others.
 *
 * Portions of this file may have come from Chris Cannam and Steve
 * Harris's public domain DSSI example code.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be
 * useful, but WITHOUT ANY WARRANTY; without even the implied
 * warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 * PURPOSE.  See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public
 * License along with this library; if not, write to the Free
 * Software Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307, USA.
 */

#ifndef _HEXTER_H
#define _HEXTER_H

/* ==== debugging ==== */

/* DSSP_DEBUG bits */
#define DB_DSSI    1   /* DSSI interface */
#define DB_AUDIO   2   /* audio output */
#define DB_NOTE    4   /* note on/off, voice allocation */
#define DB_DATA    8   /* 'load' configure commands, patchbank handling */
#define DB_MAIN   16   /* GUI main program flow */
#define DB_OSC    32   /* GUI OSC handling */
#define DB_IO     64   /* GUI patch file input/output */
#define DB_GUI   128   /* GUI GUI callbacks, updating, etc. */

/* If you want debug information, define DSSP_DEBUG to the DB_* bits you're
 * interested in getting debug information about, bitwise-ORed together.
 * Otherwise, leave it undefined. */
// #define DSSP_DEBUG (1+8+16+32+64)

#ifdef DSSP_DEBUG

#include <stdio.h>
#define DSSP_DEBUG_INIT(x)
#define DEBUG_MESSAGE(type, fmt...) { if (DSSP_DEBUG & type) fprintf(stderr, "hexter.so" fmt); }
#define GUIDB_MESSAGE(type, fmt...) { if (DSSP_DEBUG & type) fprintf(stderr, "hexter_gtk" fmt); }
// -FIX-:
// #include "message_buffer.h"
// #define DSSP_DEBUG_INIT(x)  mb_init(x)
// #define DEBUG_MESSAGE(type, fmt...) { \-
//     if (DSSP_DEBUG & type) { \-
//         char _m[256]; \-
//         snprintf(_m, 255, fmt); \-
//         add_message(_m); \-
//     } \-
// }

#else  /* !DSSP_DEBUG */

#define DEBUG_MESSAGE(type, fmt...)
#define GUIDB_MESSAGE(type, fmt...)
#define DSSP_DEBUG_INIT(x)

#endif  /* DSSP_DEBUG */

/* ==== end of debugging ==== */

#define HEXTER_MAX_POLYPHONY     64
#define HEXTER_DEFAULT_POLYPHONY  6

#define HEXTER_NUGGET_SIZE    64

#define HEXTER_PORT_OUTPUT  0
#define HEXTER_PORT_TUNING  1
#define HEXTER_PORTS_COUNT  2

#endif /* _HEXTER_H */
