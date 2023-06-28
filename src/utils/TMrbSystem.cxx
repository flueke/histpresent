//__________________________________________________[C++IMPLEMENTATION]
//////////////////////////////////////////////////////////////////////////////
// Name:           utils/src/TMrbSystem.cxx
// Purpose:        MARaBOU utilities:
//                 Some extensions to TSystem class
// Description:    Implements extended methods for TSystem
// Keywords:
// Author:         R. Lutter
// Mailto:         <a href=mailto:rudi.lutter@physik.uni-muenchen.de>R. Lutter</a>
// Revision:       $Id: TMrbSystem.cxx,v 1.23 2008-04-14 08:30:57 Otto.Schaile Exp $       
// Date:           
//////////////////////////////////////////////////////////////////////////////

namespace std {} using namespace std;


#include <cstdlib>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <fstream>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>


#include "TRegexp.h"
#include "TEnv.h"
#include "TObjArray.h"
#include "TObjString.h"
#include "TMrbSystem.h"
#include "TMrbString.h"
#include "TMrbLogger.h"

#include "SetColor.h"

ClassImp(TMrbSystem)

extern TMrbLogger * gMrbLog;			// access to message logging

//__________________________________________________________________[C++ CTOR]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbSystem
// Purpose:        Some extensions to TSystem
// Description:    Adds some high-level methods based on TSystem class
// Keywords:       |
//////////////////////////////////////////////////////////////////////////////

TMrbSystem::TMrbSystem() {
	if (gMrbLog == NULL) gMrbLog = new TMrbLogger();
}

Int_t TMrbSystem::FindFile(TObjArray & PathList,
						const Char_t * FileName, const Char_t * SearchPath, Bool_t MultipleMatch) const {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbSystem::FindFile
// Purpose:        Find file(s) along a search path
// Arguments:      TObjArray & PathList  -- resulting path list
//                 Char_t * FileName     -- file name, may be wild-carded
//                 Char_t * SearchPath   -- unix search path (:-separated)
//                 Bool_t MultipleMatch  -- kTRUE if multiple hits requested
// Results:        Int_t NofFiles        -- number of files found
// Exceptions:     NofFiles = -1         -- error
//                 NofFiles = 0          -- no file found
// Description:    Returns a list of file paths for a given file name.
//                 Files will be APPENDED to list.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	Int_t n1, n2;
	TString searchPath, fileName;
	TString dir, dirEnt, filePath;
	void * dp;
	const Char_t * fp;
	Bool_t hasWildCard;
	Int_t nofFiles;

	TObjString * os;
	TString fn;
	Bool_t foundDir, foundFile;

	searchPath = SearchPath;
	gSystem->ExpandPathName(searchPath);
	searchPath = searchPath.Strip(TString::kBoth);

	if (searchPath.Index(":", 0) == -1) {
		searchPath += ":";
	}
	if (searchPath(searchPath.Length() - 1) != ':') searchPath += ":";

	fileName = FileName;
	if (fileName.Index("?", 0) != -1) {
		gMrbLog->Err() << "Illegal wild card \"?\" in file name - " << fileName << endl;
		gMrbLog->Flush(this->ClassName(), "FindFile");
		return(-1);
	}

	hasWildCard = (fileName.Index("*", 0) != -1);

	n1 = 0;
	n2 = 0;
	nofFiles = 0;
	TRegexp rx(fileName.Data(), kTRUE);
	foundDir = kFALSE;
	while (n2 != -1) {
		n2 = searchPath.Index(":", n1);
		if (n2 == -1) break;
		dir = searchPath(n1, n2 - n1);
		dir = dir.Strip(TString::kBoth);
		if (dir.Length() == 0 || dir.CompareTo(".") == 0) dir = gSystem->WorkingDirectory();
		dp = gSystem->OpenDirectory(dir.Data());
		if (dp != NULL) {
			foundDir = kTRUE;
			while ( (fp = gSystem->GetDirEntry(dp)) ) {
				dirEnt = fp;
				if (dirEnt.Index(rx, 0) != -1) {
					filePath = dir;
					filePath += "/";
					filePath += dirEnt;
					if (!MultipleMatch) {
						foundFile = kFALSE;
						os = (TObjString *) PathList.First();
						while (os) {
							this->GetBaseName(fn, os->GetString().Data());
							if (dirEnt.CompareTo(fn.Data()) == 0) {
								foundFile = kTRUE;
								break;
							}
							os = (TObjString *) PathList.After(os);
						}
					} else foundFile = kFALSE;
					if (!foundFile) {
						PathList.Add(new TObjString(filePath.Data()));
						nofFiles++;
					}
					if (!hasWildCard && !MultipleMatch) {
						gSystem->FreeDirectory(dp);
						return(1);
					}	
				}
			}
		}
		gSystem->FreeDirectory(dp);
		n1 = n2 + 1;
	}
	if (!foundDir) {
		gMrbLog->Err() << "Can't open path name(s) - " << SearchPath << endl;
		gMrbLog->Flush(this->ClassName(), "FindFile");
		return(-1);
	}
	return(nofFiles);
}

const Char_t * TMrbSystem::GetDirName(TString & DirName, const Char_t * FilePath, EMrbDirMode Mode) const {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbSystem::GetDirName
// Purpose:        Return directory part of file path
// Arguments:      TString & DirName     -- restulting dir name
//                 Char_t * FilePath     -- full path spec
//                 EMrbDirMode Mode      -- how to return directory name
// Results:        Char_t * DirName      -- directory name (same as DirName.Data())
// Exceptions:     
// Description:    Returns "dir part" of a string.
//                 ~ and $ constructs will be expanded before execution
//                 Additional options:
//                   Mode = kMrbDirAppendSlash    -- append / at end of string
//                        = kMrbDirExpandWdir     -- expand working directory
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TString fp = FilePath;
	Int_t n1, n2;

	gSystem->ExpandPathName(fp);							// replace $, ~, etc.

	Bool_t isDir = this->IsDirectory(fp);					// directory?
	Bool_t hasSlash = (fp.Index("/", 0) != -1); 			// path containing slash?
	Bool_t hasTermSlash = (fp(fp.Length() - 1) == '/'); 	// slash at end?

	if (isDir) {											// dir or possibly plain file?
		DirName = fp;
		if (Mode & TMrbSystem::kMrbDirExpandWdir) { 		// working dir to be expanded?
			if (DirName.Index("./") == 0) DirName = DirName(1, DirName.Length() - 1);	// is it "./"?
			else if (DirName(0) != '/') DirName.Prepend("/");							// or local?
			DirName.Prepend(gSystem->WorkingDirectory());
		}
		if (Mode & TMrbSystem::kMrbDirAppendSlash) {		// slash to be appended?
			if (!hasTermSlash) DirName += "/";
		} else {
			if (hasTermSlash) DirName = DirName(0, DirName.Length() - 1);
		}
		return(DirName.Data());
	} else if (!hasSlash) { 								// not a dir
		DirName = "";
		return(DirName.Data());
	}
	
	n1 = 0;
	while (1) { 											// find last slash in path
		if ((n2 = fp.Index("/", n1)) == -1) break;
		n1 = n2 + 1;
	}

	DirName = fp;
	DirName = DirName(0, (Mode & TMrbSystem::kMrbDirAppendSlash) ? n1 : n1 - 1);	// remove slash at end?
	if (Mode & TMrbSystem::kMrbDirExpandWdir) { 									// expand working dir?
		if (DirName.Index("./") == 0) DirName = DirName(1, 1000);					// is it "./"?
		else if (DirName(0) != '/') DirName.Prepend("/");							// or local?
		DirName.Prepend(gSystem->WorkingDirectory());
	}
	return(DirName.Data());
}

const Char_t * TMrbSystem::GetBaseName(TString & BaseName, const Char_t * FilePath) const {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbSystem::GetBaseName
// Purpose:        Return file name part of file path
// Arguments:      TString & BaseName    -- resulting file name
//                 Char_t * FilePath     -- full path spec
// Results:        Char_t * BaseName     -- file name part (same as BaseName.Data())
// Exceptions:     
// Description:    Returns "file name part" of given string.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TString fp = FilePath;
	Int_t n1, n2;

	n1 = 0;
	while (1) {
		if ((n2 = fp.Index("/", n1)) == -1) break;
		n1 = n2 + 1;
	}
	BaseName = FilePath;
	BaseName = BaseName(n1, BaseName.Length() - n1);
	return(BaseName.Data());
}

const Char_t * TMrbSystem::GetExtension(TString & Extension, const Char_t * FileName) const {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbSystem::GetExtension
// Purpose:        Return extension part of file name
// Arguments:      TString & Extension   -- resulting extension
//                 Char_t * FileName     -- file spec
// Results:        Char_t * Extension    -- extension part (same as Extension.Data())
// Exceptions:     
// Description:    Returns extension (including ".")
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TString fp = FileName;
	Int_t n1, n2;

	if (fp.Index(".", 0) == -1) {
		Extension = "";
		return(Extension.Data());
	}

	n1 = 0;
	while (1) {
		if ((n2 = fp.Index(".", n1)) == -1) break;
		n1 = n2 + 1;
	}
	Extension = FileName;
	Extension = Extension(n1, Extension.Length() - n1);
	Extension.Prepend(".");
	return(Extension.Data());
}

Bool_t TMrbSystem::CheckExtension(const Char_t * FileName, const Char_t * Extension) const {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbSystem::CheckExtension
// Purpose:        Check if file has a given extension
// Arguments:      Char_t * FileName     -- file spec
//                 Char_t * Extension    -- extension
// Results:        kTRUE/kFALSE
// Exceptions:     
// Description:    Checks file for a given extension.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TString ext;

	this->GetExtension(ext, FileName);
	return(ext.CompareTo(Extension) == 0);
}

const Char_t * TMrbSystem::GetRelPath(TString & Path, const Char_t * BaseDir) const {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbSystem::GetRelPath
// Purpose:        Get relative path name
// Arguments:      TString & Path        -- full path name
//                 Char_t * BaseDir      -- base directory
// Results:        Char_t * RelPath      -- relative path
// Exceptions:     
// Description:    Returns path name as seen from base dir.
//                 Default base dir is current working dir.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	if (BaseDir == NULL) BaseDir = gSystem->WorkingDirectory();
	TString baseDir = BaseDir;
	if (baseDir(baseDir.Length()) != '/') baseDir += "/";
	if (Path.Index(baseDir, 0) != 0)	return(Path.Data());
	else								return(Path.Data() + baseDir.Length());
}

Int_t TMrbSystem::GetType(const Char_t * Path) const {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbSystem::GetType
// Purpose:        Return file type
// Arguments:      const Char_t * Path   -- path name
// Results:        Int_t Type            -- file type
// Exceptions:     
// Description:    Returns file type
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	FileStat_t stat;
	TString path = gSystem->ExpandPathName(Path);
		
#if ROOTVERSION < 40104
	struct stat sbuf;
	if (lstat(path.Data(), &sbuf) == 0) {
		stat.fIsLink = S_ISLNK(sbuf.st_mode);
		if (stat.fIsLink) {
			if (stat(path.Data(), &sbuf) == -1) return 1;
		}
		stat.fDev	= sbuf.st_dev;
		stat.fIno	= sbuf.st_ino;
		stat.fMode  = sbuf.st_mode;
		stat.fUid	= sbuf.st_uid;
		stat.fGid	= sbuf.st_gid;
		stat.fSize  = sbuf.st_size;
		stat.fMtime = sbuf.st_mtime;
	} else {
		return(1);
	}
#else
	gSystem->GetPathInfo(path.Data(), stat);
#endif
	return(stat.fMode);
}

Bool_t TMrbSystem::CheckAccess(const Char_t * Path, EAccessMode Mode) const {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbSystem::CheckAccess
// Purpose:        Check file permission
// Arguments:      const Char_t * Path   -- path name
// Results:        kTRUE/kFALSE
// Exceptions:     
// Description:    Checks file permission bits
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TString path = gSystem->ExpandPathName(Path);
	return(!gSystem->AccessPathName(path.Data(), Mode));
}

const Char_t * TMrbSystem::GetSymbolicLink(TString & SymLink, const Char_t * Path) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbSystem::GetSymbolicLink
// Purpose:        Return symbolic link
// Arguments:      TString & SymLink     -- Where to store link info
//                 Char_t * Path         -- path name
// Results:        Char_t * SymLink      -- where the link is pointing to
// Exceptions:     
// Description:    Returns symbolic link information/
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	if (this->IsSymbolicLink(Path)) {
		TString path = gSystem->ExpandPathName(Path);
		FILE * p = gSystem->OpenPipe(Form("readlink %s", path.Data()), "r");
		char sl[1000];
		char * result = fgets(sl, 1000, p);
		fclose(p);
		if ( result ) {
			SymLink = sl;
			SymLink(SymLink.Length() - 1) = '\0';
		} else {
			SymLink= "";
		}
	} else {
		SymLink = "";
	}
	return(SymLink.Data());
}

const Char_t * TMrbSystem::Which(TString & Result, const Char_t * Search, const Char_t * File, EAccessMode Mode) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbSystem::Which
// Purpose:        TSystem::Which(), improved
// Arguments:      TString & Result      -- resulting path
//                 Char_t * Search       -- unix-like search path, :-separated
//                 Char_t * File         -- file name to be searched for
//                 EAccessMode Mode      -- access mode
// Results:        Char_t * ResPath      -- same as Result.Data()
// Exceptions:     
// Description:    Performs TSystem::Which().
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TString s = Search;
	gSystem->ExpandPathName(s);
	Result = "";
	const Char_t * res = gSystem->Which(s.Data(), File, Mode);
	if (res) {
		Result = res;
		delete [] res;
	}
	return(Result.Data());
}

Int_t TMrbSystem::Load(const Char_t * LofModules, Bool_t SystemFlag) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbSystem::Load
// Purpose:        TSystem::Load() + Printout
// Arguments:      Char_t * LofModules   -- library module to be loaded
//                 Bool_t SystemFlag     -- kTRUE if it is a system lib
// Results:        Int_t Status          -- -1, 0, 1
// Exceptions:     
// Description:    Performs TSystem::Load and prints result
//                 Argument 'LofModules' is expected to be a :-separated list
//                 of library modules:
//                    lib1:lib2:....
//                 An entry point may be specified by appending it in parens:
//                    lib1(entry1):lib2(entry2)
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TString ldLibPath = gSystem->Getenv("LD_LIBRARY_PATH");
	if (ldLibPath.IsNull()) {
		gMrbLog->Err()	<< "Environment not set - LD_LIBRARY_PATH" << endl;
		gMrbLog->Flush(this->ClassName(), "Load");
		return(-1);
	}

	TString lofModules = LofModules;
	Int_t sts = 0;
	Int_t from = 0;
	Int_t nofLibs = 0;
	TString mod;
	while (lofModules.Tokenize(mod, from, ":")) {
		nofLibs++;
		TString path;
		TString entry = "";
		Int_t lparen = mod.Index("(", 0);
		Int_t rparen = mod.Index(")", 0);
		Bool_t err = (lparen == -1 && rparen != -1) || (lparen != -1 && rparen == -1) || (lparen > rparen);
		if (err) {
			mod.Resize(lparen);
			gMrbLog->Err()	<< "[" << mod << "] Wrong entry spec - ignored" << endl;
			gMrbLog->Flush(this->ClassName(), "Load");
		} else if (lparen != -1) {
			entry = mod(lparen + 1, rparen - lparen - 1);
			mod.Resize(lparen);
		}
		this->Which(path, "$LD_LIBRARY_PATH", mod);
		if (path.IsNull()) {
			gMrbLog->Err()	<< "No such library - " << mod << " (searched on $LD_LIBRARY_PATH)" << endl;
			gMrbLog->Flush(this->ClassName(), "Load");
			return(-1);
		}
		sts = gSystem->Load(path.Data(), entry, SystemFlag);
		this->PrintLoadPath(sts, mod.Data(), path.Data(), entry, SystemFlag);
		if (sts == -1) return(-1);
	}
	return((nofLibs == 1) ? sts : 0);
}

void TMrbSystem::PrintLoadPath(Int_t Status, const Char_t * Module, const Char_t * Path, const Char_t * Entry, Bool_t SystemFlag) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbSystem::PrintLoadPath
// Purpose:        Print load path to cout
// Arguments:      Int_t Status          -- result of TSystem::Load()
//                 Char_t * Module       -- library module to be loaded
//                 CHar_t * Path         -- path library was found on
//                 Char_t * Entry        -- lib entry to be searched for
//                 Bool_t SystemFlag     -- kTRUE if it is a system lib
// Results:        --
// Exceptions:     
// Description:    Outputs load path to cout.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	Bool_t printIt = gEnv->GetValue("TMrbUtils.PrintLoadPath", kFALSE);
	Bool_t hasEntry = (Entry != NULL && *Entry != '\0');
	Bool_t hasPath = (Path != NULL && *Path != '\0');

	TString libType = SystemFlag ? "SystemLib" : "Library";

	TString path;
	if (hasPath)	path = Path;
	else			this->Which(path, "$LD_LIBRARY_PATH", Module);

	if (printIt) {
		if (Status == 0) {
			if (hasEntry)	gMrbLog->Out()	<< libType << " " << Module << " (with entry " << Entry << ") loaded from " << path << endl;
			else			gMrbLog->Out()	<< libType << " " << Module << " loaded from " << path << endl;
		} else if (Status == 1) {
			if (hasEntry)	gMrbLog->Out()	<< libType << " " << Module << " (with entry " << Entry << ") already loaded" << endl;
			else			gMrbLog->Out()	<< libType << " " << Module << " already loaded" << endl;
		} else if (Status == -1) {
			if (hasEntry) {
				this->Which(path, "$LD_LIBRARY_PATH", Module);
				if (path.IsNull())	gMrbLog->Err()	<< libType << " " << Module << " not found on $LD_LIBRARY_PATH" << endl;
				else				gMrbLog->Err()	<< "[" << libType << " " << Module << "] Entry " << Entry << " not found" << endl;
			} else					gMrbLog->Err()	<< libType << " " << Module << " not found on $LD_LIBRARY_PATH" << endl;
		}
		gMrbLog->Flush(this->ClassName(), "Load");
	}
}
			
void TMrbSystem::AddIncludePath(const Char_t * IncludePath) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbSystem::AddIncludePath
// Purpose:        Append an item to $IncludePath
// Arguments:      Char_t * IncludePath         -- include path
// Exceptions:     
// Description:    Adds a string to include defs
//                 IncludePath may be a :-separated string
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TString iclPath = gSystem->GetIncludePath();
	TString newIcl = IncludePath;
	Int_t from = 0;
	TString icl;
	while (newIcl.Tokenize(icl, from, ":")) {
		icl = icl.Strip(TString::kBoth);
		if (!icl.IsNull()) {
			if (!iclPath.Contains(icl.Data())) {
				gSystem->ExpandPathName(icl);
				if (!iclPath.Contains(icl.Data())) {
					iclPath += " -I";
					iclPath += icl;
				}
			}
		}
	}
	gSystem->SetIncludePath(iclPath.Data());
}

const Char_t * TMrbSystem::GetHostName(TString & HostName) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbSystem::GetHostName
// Purpose:        Get host name
// Arguments:      TString & HostName   -- where to store host name
// Exceptions:     
// Description:    Executes gethostname(2),
//                 returns name of current host
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	Char_t hn[100];
	gethostname(hn, 100);
	HostName = hn;
	return(HostName.Data());
}

const Char_t * TMrbSystem::GetDomainName(TString & DomainName) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbSystem::GetDomainName
// Purpose:        Get domain name
// Arguments:      TString & DomainName   -- where to store domain name
// Exceptions:     
// Description:    Executes getdomainname(2),
//                 returns name of current host
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	Char_t dn[100];
	Int_t result = getdomainname(dn, 100);
	if (result == 0)
		DomainName = dn;
	else
		DomainName = "";
	return(DomainName.Data());
}

Bool_t TMrbSystem::GetStat(TMrbLofNamedX & StatBuf, const Char_t * Path) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbSystem::GetStat
// Purpose:        Get file stat bits
// Arguments:      TMrbLofNamedX & StatBuf   -- where to store stat bits
//                 Char_t * Path             -- file path
// Exceptions:     
// Description:    Executes stat(2) and returns stat bits
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	struct stat stbuf;

	StatBuf.Delete();

	if (stat(Path, &stbuf) != 0) return(kFALSE);

	StatBuf.AddNamedX(stbuf.st_dev, "st_dev");
    StatBuf.AddNamedX(stbuf.st_ino, "st_ino");
    StatBuf.AddNamedX(stbuf.st_mode, "st_mode");
    StatBuf.AddNamedX(stbuf.st_nlink, "st_nlink");
    StatBuf.AddNamedX(stbuf.st_uid, "st_uid");
    StatBuf.AddNamedX(stbuf.st_gid, "st_gid");
    StatBuf.AddNamedX(stbuf.st_rdev, "st_rdev");
    StatBuf.AddNamedX(stbuf.st_size, "st_size");
    StatBuf.AddNamedX(stbuf.st_blksize, "st_blksize");
    StatBuf.AddNamedX(stbuf.st_blocks, "st_blocks");
    StatBuf.AddNamedX(stbuf.st_atime, "st_atime");
    StatBuf.AddNamedX(stbuf.st_mtime, "st_mtime");
    StatBuf.AddNamedX(stbuf.st_ctime, "st_ctime");

	return(kTRUE);
}

Bool_t TMrbSystem::IsSameFile(const Char_t * Path1, const Char_t * Path2) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbSystem::GetStat
// Purpose:        Get file stat bits
// Arguments:      Char_t * Path1             -- file path #1
//                 Char_t * Path2             -- file path #2
// Exceptions:     
// Description:    Compares inodes.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TMrbLofNamedX stat1, stat2;
	TMrbNamedX * nx1;
	TMrbNamedX * nx2;
	this->GetStat(stat1, Path1);
	this->GetStat(stat2, Path2);
	nx1 = stat1.FindByName("st_ino");
	nx2 = stat2.FindByName("st_ino");
	return(nx1->GetIndex() == nx2->GetIndex());
}




