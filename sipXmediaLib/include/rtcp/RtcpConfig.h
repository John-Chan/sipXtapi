//
// Copyright (C) 2006-2013 SIPez LLC.  All rights reserved.
//
// Copyright (C) 2004-2006 SIPfoundry Inc.
// Licensed by SIPfoundry under the LGPL license.
//
// Copyright (C) 2004-2006 Pingtel Corp.  All rights reserved.
// Licensed to SIPfoundry under a Contributor Agreement.
//
// $$
///////////////////////////////////////////////////////////////////////////////


/*  Border Guard  */
#ifndef _RTCPCONFIG_H_ /* [ */
#define _RTCPCONFIG_H_


/************************************************************************
 *
 * OK, here's the story...
 *
 * As of this time (4 August 2000) the only components affected by
 * changes to this file are rtcp, mp, cp, and Pinger.cpp.
 *
 * On VxWorks, there is no need to edit this file.  Just set the
 * environment variable USE_RTCP=No and then clean and rebuild the
 * affected libraries (listed above), then rebuild the kernel.
 *
 * On WIN32, it's a little more of a pain.  Clean the subdirectories,
 * edit this file to define or undefine EXCLUDE_RTCP, then rebuild
 * those subdirectories.  Unfortunately, many of the APPs (*.exe files
 * and maybe some *.dll files) recompile Pinger.cpp and link with the
 * resulting object file, so be sure that you do the right thing with
 * those; you are on your own here.  I would just clean the entire tree
 * and rebuild everything after editing this file on WIN32.
 *
 * And, you might just want to do the same thing on VxWorks for safety...
 * If any other components ever make any calls to any RTCP files, be sure
 * that their Makefiles are modified in the same way as the mp/Makefile
 * (search for RTCP in that Makefile to see what I mean).
 ***********************************************************************/

#ifdef _VXWORKS /* [ */
#define EXCLUDE_RTCP
/* #undef EXCLUDE_RTCP */
#endif /* VXWORKS ] */

#ifdef WIN32 /* [ */
#define EXCLUDE_RTCP
#undef EXCLUDE_RTCP 
#endif /* WIN32 ] */

#ifndef EXCLUDE_RTCP /* [ */
#define INCLUDE_RTCP
#define PINGTEL_OSSOCKET
#define PINGTEL_PRINTF
#endif  /* EXCLUDE_RTCP ] */

#include "os/OsIntTypes.h"

typedef uint32_t ssrc_t;
typedef uint32_t rtpts_t;


#undef RTCP_DEBUG_REFCOUNTS /* [ */
#define RTCP_DEBUG_REFCOUNTS /* [ */
#ifdef RTCP_DEBUG_REFCOUNTS /* [ */
#define ADD_RELEASE_CALL_ARGS(x) (x)
#define ADD_RELEASE_PROTO_ARGS(x) x
#define CBASECLASS_CALL_ARGS(x, y) x, y
#define CBASECLASS_PROTO_ARGS(x) x
#else /* RTCP_DEBUG_REFCOUNTS ] [ */
#define ADD_RELEASE_CALL_ARGS(x)
#define ADD_RELEASE_PROTO_ARGS(x) (void)
#define CBASECLASS_CALL_ARGS(x, y)
#define CBASECLASS_PROTO_ARGS(x) ()
#define callLineNum 0
#endif /* RTCP_DEBUG_REFCOUNTS ] */

#endif /* _RTCPCONFIG_H_ ] */
