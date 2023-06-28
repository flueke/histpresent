#ifndef __TPolControl_h__
#define __TPolControl_h__

//_________________________________________________[C++ CLASS DEFINITION FILE]
//////////////////////////////////////////////////////////////////////////////
// Name:           polar/inc/TPolControl.h
// Purpose:        Class defs to control the polarized ion source
// Description:    Class definitions to control Ralf Hertenberger's polarized ion source
// Author:         R. Lutter
// Revision:       $Id: TPolControl.h,v 1.4 2005-09-09 06:59:14 Rudolf.Lutter Exp $       
// Date:           
// Keywords:
//////////////////////////////////////////////////////////////////////////////

#include "Rtypes.h"

#include "TH1.h"
#include "TNtuple.h"
#include "TTimer.h"
#include "TFile.h"

#include "TMrbNamedX.h"
#include "TMrbLofNamedX.h"
#include "TMrbSerialComm.h"

//______________________________________________________[C++ CLASS DEFINITION]
//////////////////////////////////////////////////////////////////////////////
// Name:           TPolControl
// Purpose:        Control the polaraized ion source
// Description:    Controls the polarized ion source via a RS232 interface
// Keywords:
//////////////////////////////////////////////////////////////////////////////

class TPolControl : public TObject {

	public:
		enum	{	kPolNofAdcs 	= 32		};		// number of adcs per subdevice
		enum	{	kPolNofDacs 	= 20		};		// number of dacs per subdevice
		enum	{	kPolAdcRange 	= 32768 	};		// adc range
		enum	{	kPolDacRange 	= 4095		};		// dac range
		enum	{	kPolMaxVoltage 	= 10		};		// max voltage

		enum	{	kPolIdxName, kPolIdxSubdev, kPolIdxChannel, kPolIdxType, kPolIdxComment };
		enum	{	kPolNofFields	= kPolIdxComment - kPolIdxName + 1	};

 	public:
		TPolControl(const Char_t * Device = NULL);
		virtual ~TPolControl() {};

		Int_t ReadAdc(Int_t Subdev, Int_t Channel, Bool_t LowResolution = kFALSE);	// read adc, relative addr
		Int_t ReadAdc(Int_t AbsChannel, Bool_t LowResolution = kFALSE); 			// read adc, absolute addr
		Int_t ReadAdc(const Char_t * ChnName, Bool_t LowResolution = kFALSE);		// read adc, addressed by name

		Int_t SetDac(Int_t Subdev, Int_t Channel, Double_t DacValue,				// set dac, relative addr
								Int_t ReadBack = -1, Bool_t LowResolution = kFALSE);
		Int_t SetDac(Int_t AbsChannel, Double_t DacValue,							// set dac, absolute addr
								Int_t ReadBack = -1, Bool_t LowResolution = kFALSE);
		Int_t SetDac(const Char_t * ChnName, Double_t DacValue,						// set dac, addressed by name
								const Char_t * ReadBack = NULL, Bool_t LowResolution = kFALSE);	// set dac
		void SetWait(Int_t WaitAV, Int_t WaitAR = 0, Int_t WaitSD = 0); 			// set wait states
		void SetSubdevs(Int_t S1, Int_t S2 = -1, Int_t S3 = -1, Int_t S4 = -1, Int_t S5 = -1);	// define subdevices

		Int_t ReadNameTable(const Char_t * FileName = "polNames.dat");		// read name table data from file
		void ClearNameTable() { fNameTable.Delete(); };
		inline TMrbLofNamedX * GetNameTable() { return(&fNameTable); }; 	// get list of adc/dac names
		TMrbNamedX * AddName(const Char_t * NameString);	// add an item to name table
		TMrbNamedX * AddName(const Char_t * Name, Int_t Subdevice, Int_t Channel,
								const Char_t * Type = "A", const Char_t * Comment = NULL);
		TMrbNamedX * Find(const Char_t * Name, const Char_t * Type = "A");
		TMrbNamedX * Find(Int_t Subdev, Int_t Channel, const Char_t * Type = "A");
//		TMrbNamedX * Find(Int_t AbsChannel, const Char_t * Type = "A"); // not yet implemented

		TH1F * Plot(	const Char_t * HistoName,					// plot, relative addr
						Int_t Subdev, Int_t X, Int_t Y,
						Double_t Xmin, Double_t Xmax, Double_t DeltaX,
						const Char_t * Xaxis = NULL, const Char_t * Yaxis = NULL);

		TH1F * Plot(	const Char_t * HistoName,					// plot, absolute addr
						Int_t X, Int_t Y,
						Double_t Xmin, Double_t Xmax, Double_t DeltaX,
						const Char_t * Xaxis = NULL, const Char_t * Yaxis = NULL);

		TH1F * Plot(	const Char_t * HistoName,					// plot, addr by name
						const Char_t * X, const Char_t * Y,
						Double_t Xmin, Double_t Xmax, Double_t DeltaX,
						const Char_t * Xaxis = NULL, const Char_t * Yaxis = NULL);

		inline TMrbSerialComm * SerIO() { return(fSerIO); };		// serial i/o port
		inline Bool_t IsOnline() { return(fSerIO != NULL); };		// online/offline mode

		void Draw(const Option_t * Option) { TObject::Draw(Option); };
		inline void Draw() { if (fHisto) fHisto->Draw(); };

		TNtuple * Monitor(const Char_t * FileName,					// monitor data to root file
						const Char_t * Comment,
						const Char_t * LofChannels,
						Int_t NofRecords = 0, Int_t Period = 0);

		TNtuple * Monitor(const Char_t * FileName); 	// retrieve monitor data from file
		Int_t GetNofADCs();								// number of adcs in ntuple
		Int_t GetNofEntries();							// number of entries in file
		Int_t GetEntry(Int_t EntryNumber, UInt_t & TimeStamp, TArrayF & Data);	// get record from ntuple

		Bool_t StopMonitor();										// stop monitoring
		inline Bool_t IsWriting() { return((fMonitorData != NULL) && fWriteFlag); };
		inline Bool_t IsReading() { return((fMonitorData != NULL) && !fWriteFlag); };
		inline Bool_t MonitorIsOn() { return(fMonitorData != NULL); };

		const Char_t * TimeStampAsString(TString & TimeString, UInt_t TimeStamp);

		inline void SetVerboseMode(Bool_t VerboseFlag = kTRUE) { fVerboseMode = VerboseFlag; };
		inline Bool_t IsVerbose() { return(fVerboseMode); };

		inline TMrbLofNamedX * GetMonitorLayout() { return(&fMonitorLayout); }; // get monitor layout

		void Print(Option_t * Option) const { TObject::Print(Option); }
		virtual void Print();										// print settings

		void PrintNameTable();										// print adc/dac names
		
		void PrintMonitorLayout();									// print monitor layout

		void PrintRecords(Int_t Start = 0, Int_t Stop = -1); 		// print record data
		void PrintRecords(const Char_t * FileName, Int_t Start = 0, Int_t Stop = -1);

		Bool_t HandleTimer(TTimer * Timer); 						// used by Monitor() method

		inline void Help() { gSystem->Exec(Form("mrbHelp %s", this->ClassName())); };

	protected:
		Bool_t CheckAdcChannel(Int_t Subdev, Int_t Channel, const Char_t * Method = NULL);
		Bool_t CheckAdcChannel(Int_t AbsChannel, const Char_t * Method = NULL);
		Bool_t CheckDacChannel(Int_t Subdev, Int_t Channel, const Char_t * Method = NULL);
		Bool_t CheckDacChannel(Int_t AbsChannel, const Char_t * Method = NULL);
		Bool_t CheckDacValue(Double_t DacValue, const Char_t * Method = NULL);
		Bool_t CheckSubdev(Int_t Subdev, const Char_t * Method = NULL);

		TMrbNamedX * AddName(TMrbLofNamedX * Table, const Char_t * Name, Int_t Subdevice, Int_t Channel,
								const Char_t * Type, const Char_t * Comment);
		TMrbNamedX * Find(TMrbLofNamedX * Table, const Char_t * Name, const Char_t * Type,
											const Char_t * Method = "Find", Bool_t Verbose = kTRUE);
		TMrbNamedX * Find(TMrbLofNamedX * Table, Int_t Subdev, Int_t Channel, const Char_t * Type,
											const Char_t * Method = "Find", Bool_t Verbose = kTRUE);
		void PrintTable(TMrbLofNamedX * Table);
		void PrintRecordData(ostream & Out, Int_t RecordNumber, UInt_t TimeStamp, Float_t * Data, Int_t NofData);

	protected:
		TMrbSerialComm * fSerIO;		// serial i/o

		Bool_t fVerboseMode;

		TH1F * fHisto;					//! histogram to store XY plot

		TString fNameTableFile; 		//! file where to get names from
		TMrbLofNamedX fNameTable;		// name table

		UInt_t fSubdevPattern;			// bit pattern for subdevs

		Int_t fWaitAV;					// wait states
		Int_t fWaitAR;
		Int_t fWaitSD;

		TString fMonitorFile;			//! file for monitor data
		TFile * fMonitorRootFile;		//! root file containung monitor ntuples
		TNtuple * fMonitorData; 		//! monitor data
		TMrbLofNamedX fMonitorLayout;	// current monitor layout
		Bool_t fWriteFlag;				// kTRUE if writing monitor data to file
		Bool_t fStopFlag;				// kTRUE if monitoring has to be stopped
		Int_t fNofRecords;				// number of records to be written/read
		Int_t fRecordCount;				// number of records processed so far

	ClassDef(TPolControl, 1) 		// [Polar] Control polarized ion source
};

#endif
