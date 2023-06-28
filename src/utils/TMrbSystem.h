#ifndef __TMrbSystem_h__
#define __TMrbSystem_h__

namespace std {} using namespace std;

#if ROOT_VERSION_CODE <= ROOT_VERSION(4,01,00)
 enum EFileModeMask {
   kS_IFMT   = 0170000,   // bitmask for the file type bitfields
   kS_IFSOCK = 0140000,   // socket
   kS_IFLNK  = 0120000,   // symbolic link
   kS_IFREG  = 0100000,   // regular file
   kS_IFBLK  = 0060000,   // block device
   kS_IFDIR  = 0040000,   // directory
   kS_IFCHR  = 0020000,   // character device
   kS_IFIFO  = 0010000,   // fifo
   kS_ISUID  = 0004000,   // set UID bit
   kS_ISGID  = 0002000,   // set GID bit
   kS_ISVTX  = 0001000,   // sticky bit
   kS_IRWXU  = 00700,     // mask for file owner permissions
   kS_IRUSR  = 00400,     // owner has read permission
   kS_IWUSR  = 00200,     // owner has write permission
   kS_IXUSR  = 00100,     // owner has execute permission
   kS_IRWXG  = 00070,     // mask for group permissions
   kS_IRGRP  = 00040,     // group has read permission
   kS_IWGRP  = 00020,     // group has write permission
   kS_IXGRP  = 00010,     // group has execute permission
   kS_IRWXO  = 00007,     // mask for permissions for others (not in group)
   kS_IROTH  = 00004,     // others have read permission
   kS_IWOTH  = 00002,     // others have write permisson
   kS_IXOTH  = 00001      // others have execute permission
};

inline Bool_t R_ISDIR(Int_t mode)  { return ((mode & kS_IFMT) == kS_IFDIR); }
inline Bool_t R_ISCHR(Int_t mode)  { return ((mode & kS_IFMT) == kS_IFCHR); }
inline Bool_t R_ISBLK(Int_t mode)  { return ((mode & kS_IFMT) == kS_IFBLK); }
inline Bool_t R_ISREG(Int_t mode)  { return ((mode & kS_IFMT) == kS_IFREG); }
inline Bool_t R_ISLNK(Int_t mode)  { return ((mode & kS_IFMT) == kS_IFLNK); }
inline Bool_t R_ISFIFO(Int_t mode) { return ((mode & kS_IFMT) == kS_IFIFO); }
inline Bool_t R_ISSOCK(Int_t mode) { return ((mode & kS_IFMT) == kS_IFSOCK); }

struct FileStat_t {
   Long_t   fDev;          // device id
   Long_t   fIno;          // inode
   Int_t    fMode;         // protection (combination of EFileModeMask bits)
   Int_t    fUid;          // user id of owner
   Int_t    fGid;          // group id of owner
   Long64_t fSize;         // total size in bytes
   Long_t   fMtime;        // modification date
   Bool_t   fIsLink;       // symbolic link
   FileStat_t() : fDev(0), fIno(0), fMode(0), fUid(0), fGid(0), fSize(0),
                  fMtime(0), fIsLink(kFALSE) { }
};
#endif

//_________________________________________________[C++ CLASS DEFINITION FILE]
//////////////////////////////////////////////////////////////////////////////
// Name:           utils/inc/TMrbSystem.h
// Purpose:        Define utilities to be used with MARaBOU
// Class:          TMrbSystem    -- extension to TSystem class
// Description:    Common class definitions to be used within MARaBOU
// Author:         R. Lutter
// Revision:       $Id: TMrbSystem.h,v 1.17 2007-09-06 11:25:32 Rudolf.Lutter Exp $       
// Date:           
// Keywords:
//////////////////////////////////////////////////////////////////////////////

#include "TSystem.h"
#include "TObjArray.h"

#include "TMrbNamedX.h"
#include "TMrbLofNamedX.h"

//______________________________________________________[C++ CLASS DEFINITION]
//////////////////////////////////////////////////////////////////////////////
// Name:           TMrbSystem
// Purpose:        Some extensions to TSystem
// Description:    Some high-level methods based on TSystem class.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

class TMrbSystem: public TObject {

	public:
		enum EMrbFileType		{	kMrbRegularFile		= kS_IFREG,
									kMrbDirectory 		= kS_IFDIR,
									kMrbSymbolicLink 	= kS_IFLNK,
									kMrbCharDev 	 	= kS_IFCHR,
									kMrbBlockDev 	 	= kS_IFBLK,
									kMrbFifo	 	 	= kS_IFIFO,
									kMrbSocket	 	 	= kS_IFSOCK,
									kMrbExecutable		= kS_IXUSR | kS_IXGRP | kS_IXOTH,
								};
	
		enum EMrbDirMode		{	kMrbDirAsIs 		= 0,
									kMrbDirAppendSlash	= BIT(0),
									kMrbDirReturnDot	= BIT(1),
									kMrbDirExpandWdir 	= BIT(2)
								};
	
	public:
		TMrbSystem();						// default ctor
		~TMrbSystem() {};					// default dtor

		Int_t FindFile(TObjArray & PathList,
				const Char_t * FileName, const Char_t * SearchPath = "", Bool_t MultipleMatch = kFALSE) const;

		const Char_t * GetDirName(TString & DirName, const Char_t * FilePath, EMrbDirMode Mode = kMrbDirAsIs) const;
		const Char_t * GetBaseName(TString & FileName, const Char_t * FilePath) const;
		const Char_t * GetExtension(TString & Extension, const Char_t * FileName) const;
		Bool_t CheckExtension(const Char_t * FileName, const Char_t * Extension) const;
		const Char_t * GetRelPath(TString & Path, const Char_t * BaseDir = NULL) const;

		inline const Char_t * WorkingDirectory() const { return(gSystem->WorkingDirectory()); };
		inline Bool_t ChangeDirectory(const Char_t * Path) const { return(gSystem->ChangeDirectory(Path)); };
		
																						// check file type
		Int_t GetType(const Char_t * Path) const;

		inline Bool_t IsRegular(const Char_t * Path) const { return(this->CheckType(Path, TMrbSystem::kMrbRegularFile)); };
		inline Bool_t IsExecutable(const Char_t * Path) const { return((this->GetType(Path) & TMrbSystem::kMrbExecutable) != 0); };
		inline Bool_t IsDirectory(const Char_t * Path) const { return(this->CheckType(Path, TMrbSystem::kMrbDirectory)); };
		inline Bool_t IsSymbolicLink(const Char_t * Path) const { return(this->CheckType(Path, TMrbSystem::kMrbSymbolicLink)); };

		inline Bool_t Exists(const Char_t * Path) const { return(this->CheckAccess(Path, kFileExists)); };
		inline Bool_t HasReadPermission(const Char_t * Path) const { return(this->CheckAccess(Path, kReadPermission)); };
		inline Bool_t HasWritePermission(const Char_t * Path) const { return(this->CheckAccess(Path, kWritePermission)); };
		inline Bool_t HasExecutePermission(const Char_t * Path) const { return(this->CheckAccess(Path, kExecutePermission)); };
		
		const Char_t * GetSymbolicLink(TString & SymLink, const Char_t * Path);

		const Char_t * GetHostName(TString & HostName); 		// get host name
		const Char_t * GetDomainName(TString & DomainName); 	// get domain name

		const Char_t * Which(TString & Result, const Char_t * Search, const Char_t * File, EAccessMode Mode = kFileExists);

		Bool_t GetStat(TMrbLofNamedX & StatBuf, const Char_t * Path);  // perform stat(2) call
		Bool_t IsSameFile(const Char_t * Path1, const Char_t * Path2);	// check if same inode

		inline void SetIncludePath(const Char_t * IncludePath) { gSystem->SetIncludePath(IncludePath); };
		inline const Char_t * GetIncludePath() { return(gSystem->GetIncludePath()); };
		void AddIncludePath(const Char_t * IncludePath);

		Int_t Load(const char * LofModules, Bool_t SystemFlag = kFALSE);
		void PrintLoadPath(Int_t Status, const Char_t * Module, const Char_t * Path = "", const Char_t * Entry = "", Bool_t SystemFlag = kFALSE);

		inline void Help() { gSystem->Exec(Form("mrbHelp %s", this->ClassName())); };

	protected:
		inline Bool_t CheckType(const Char_t * Path, EMrbFileType Type) const {	return((this->GetType(Path) & Type) == Type); };
		Bool_t CheckAccess(const Char_t * Path, EAccessMode Mode) const;
	
	ClassDef(TMrbSystem, 1) 	// [Utils] Some extensions to TSystem class
};

#endif
