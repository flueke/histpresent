#ifndef __TMrbVarWdwCommon_h__
#define __TMrbVarWdwCommon_h__

//_________________________________________________[C++ CLASS DEFINITION FILE]
//////////////////////////////////////////////////////////////////////////////
// Name:           utils/inc/TMrbVarWdwCommon.h
// Purpose:        Common defs for variables and windows
// Description:    Common class definitions to be used within MARaBOU
// Author:         R. Lutter
// Revision:       $Id: TMrbVarWdwCommon.h,v 1.6 2010-11-22 11:41:07 Marabou Exp $
// Date:
// Keywords:
//////////////////////////////////////////////////////////////////////////////

enum EMrbVarWdwType		{	kVarI			=   0x1,		// var of type int
							kVarF			=   0x2,		// var of type float
							kVarS			=   0x4,		// var of type string
							kVarB			=   0x8,		// var of type boolean
							kWindowI		=   0x10,		// wdw of type int
							kWindowF		=   0x20,		// wdw of type float
							kWindow2D		=   0x40,		// 2-dimensional window
							kFunction1D		=	0x80,		// 1-dim function (TF1)
							kFunction2D		=	0x100,		// 2-dim function (TF2)
							kWindowPtr		=   0x400,		// pointer to window
							kVarIsArray		=   0x1000, 	// array of variables
							kHasInitValues	=   0x8000, 	// var/wdw has explicit init values
							kIsRangeChecked =   0x10000,	// range has to be checked
							kIsVariable 	=   kVarI | kVarF | kVarS | kVarB,
							kIsWindow1D 	=   kWindowI | kWindowF,
							kIsWindow2D 	=   kWindow2D,
							kIsWindow		=   kIsWindow1D | kIsWindow2D,
							kIsFunction		=   kFunction1D | kFunction2D,
							kVarOrWindow	=   kIsVariable | kIsWindow,
							kStatus 		=   kVarIsArray | kIsRangeChecked,
							kVarOrWindowOrFunction	=   kIsVariable | kIsWindow | kIsFunction
						};

enum EMrbVarWdwOutput	{	kOutputNormal	=   0x1,
							kOutputShort	=   0x2
						};

const SMrbNamedXShort kMrbVarWdwOptions[] =
						{
							{kIsVariable,				"VARIABLES" },
							{kIsWindow1D,				"WINDOWS"   },
							{kIsFunction,				"FUNCTIONS"   },
							{kVarOrWindowOrFunction,	"DEFAULT"   },
							{0, 						NULL		}
						};

const SMrbNamedXShort kMrbVarWdwOutput[] =
						{
							{kOutputNormal, 	"NORMAL"    },
							{kOutputShort,		"SHORT"	  },
							{kOutputNormal, 	"DEFAULT"   },
							{0, 				NULL		}
						};

#endif
