/* Appendix A.  Appendix A - Sample Implementation

   This implementation consists of 5 files: uuid.h, uuid.c, sysdep.h,
   sysdep.c and utest.c.  The uuid.* files are the system independent
   implementation of the UUID generation algorithms described above,
   with all the optimizations described above except efficient state
   sharing across processes included.  The code has been tested on Linux
   (Red Hat 4.0) with GCC (2.7.2), and Windows NT 4.0 with VC++ 5.0.
   The code assumes 64-bit integer support, which makes it much clearer.

   All the following source files should have the following copyright
   notice included:

copyrt.h
*/
/*
** Copyright (c) 1990- 1993, 1996 Open Software Foundation, Inc.
** Copyright (c) 1989 by Hewlett-Packard Company, Palo Alto, Ca. &
** Digital Equipment Corporation, Maynard, Mass.
** Copyright (c) 1998 Microsoft.
** To anyone who acknowledges that this file is provided "AS IS"
** without any express or implied warranty: permission to use, copy,
** modify, and distribute this file for any purpose is hereby
** granted without fee, provided that the above copyright notices and
** this notice appears in all source code copies, and that none of
** the names of Open Software Foundation, Inc., Hewlett-Packard
** Company, Microsoft, or Digital Equipment Corporation be used in
** advertising or publicity pertaining to distribution of the software
** without specific, written prior permission. Neither Open Software
** Foundation, Inc., Hewlett-Packard Company, Microsoft, nor Digital
** Equipment Corporation makes any representations about the
** suitability of this software for any purpose.
*/
