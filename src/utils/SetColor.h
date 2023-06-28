#ifndef __SetColor_h__
#define __SetColor_h__

//_________________________________________________[C++ CLASS DEFINITION FILE]
//////////////////////////////////////////////////////////////////////////////
// Name:           utils/inc/SetColor.h
// Purpose:        Define utilities to be used with MARaBOU
// Description:    Common class definitions to be used within MARaBOU
// Author:         R. Lutter
// Revision:       $Id: SetColor.h,v 1.4 2004-11-16 13:30:27 rudi Exp $       
// Date:           
// Keywords:
//////////////////////////////////////////////////////////////////////////////

//______________________________________________________________[Bits & Bytes]
//////////////////////////////////////////////////////////////////////////////
// Name:           set<color>
// Purpose:        Define a foreground color
// Description:    Defines esc sequences to set foreground colors
//                 in vt100-style output:
//                     cout << set<color> << .... << setblack << endl;
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

static const char setblack[]	=	"\033[39m"; 	// set "default" color - may be other than "black"
static const char setred[]		=	"\033[31m";
static const char setgreen[]	=	"\033[32m";
static const char setyellow[]	=	"\033[33m";
static const char setblue[] 	=	"\033[34m";
static const char setmagenta[]	=	"\033[35m";
static const char setcyan[] 	=	"\033[36m";
static const char bell[] 		=	"\007\007\007";

#endif
