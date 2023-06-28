#ifndef __GlobDef_h__
#define __GlobDef_h__

//_________________________________________________[C++ CLASS DEFINITION FILE]
//////////////////////////////////////////////////////////////////////////////
// Name:           utils/inc/GlobDef.h
// Purpose:        How to define global/external vars
// Description:    Defines objects in MARaBOU
//                 Example: GLOBAL(TH1F *,histo,histo = NULL);
//                          1) local instance (_MARABOU_MAIN_ defined)
//                                 TH1F * histo = NULL;
//                          2) external reference (_MARABOU_MAIN_ undefined)
//                                 extern TH1F * histo;
// Author:         R. Lutter
// Revision:       $Id: GlobDef.h,v 1.3 2004-11-16 13:30:26 rudi Exp $       
// Date:           
// Keywords:
//////////////////////////////////////////////////////////////////////////////

#ifdef _MARABOU_MAIN_
#	define GLOBAL(signature,name,initString)	signature initString
#else
#	define GLOBAL(signature,name,initString)	extern signature name
#endif
#endif
