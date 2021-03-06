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


#ifndef _UtlDefs_h_
#define _UtlDefs_h_

// SYSTEM INCLUDES
// APPLICATION INCLUDES
// DEFINES
#ifndef FALSE
#define FALSE ((UtlBoolean)(1==0))
#endif

#ifndef TRUE
#define TRUE ((UtlBoolean)(1==1))
#endif

#ifndef NULL
#define NULL 0
#endif

#define UTL_NOT_FOUND ((size_t)-1)

// MACROS
// EXTERNAL FUNCTIONS
// EXTERNAL VARIABLES
// CONSTANTS
// STRUCTS
// TYPEDEFS

typedef int UtlBoolean ;

#if defined(_MSC_VER) && !defined(__cplusplus)
#  define inline __inline
#endif // _DEBUG

#if _MSC_VER
#define snprintf _snprintf
#endif

/** FORMAT_INTLL is a string containing the format length specifier
 *  for printing an intll with the 'd', 'x', etc. format specifiers.  E.g.:
 *      intll xyz;
 *      printf("The value is %" FORMAT_INTLL "d", xyz);
 *  Note that the '%' before and the format specifier after must be provided.
 *  This must be a \#define, since this specifier isn't standardized.
 */
#if defined(_WIN32)
#  define   FORMAT_INTLL   "I64"
#elif defined(__pingtel_on_posix__)
#  if __WORDSIZE == 64
#    define   FORMAT_INTLL   "l"
#  else
#    define   FORMAT_INTLL   "ll"
#  endif
#else
#  error Unsupported target platform.
#endif

typedef const char* const UtlContainableType ;

// FORWARD DECLARATIONS

#endif // _UtlDefs_h_

