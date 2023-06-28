//__________________________________________________[C++IMPLEMENTATION]
//////////////////////////////////////////////////////////////////////////////
// Name:           polar/src/TPolControl.cxx
// Purpose:        Control of the polarized ion source
// Description:    Implements methods to control Ralf Hertenberger's polarized ion source via RS232
// Keywords:
// Author:         R. Lutter
// Mailto:         <a href=mailto:rudi.lutter@physik.uni-muenchen.de>R. Lutter</a>
// Revision:       $Id: TPolControl.cxx,v 1.5 2007-07-27 11:17:23 Rudolf.Lutter Exp $       
// Date:           
//////////////////////////////////////////////////////////////////////////////

namespace std {} using namespace std;

#include <cstdlib>
#include <iostream>
#include <sstream>
#include <iomanip>

#include "TROOT.h"
#include "TSystem.h"
#include "TObjString.h"
#include "TDatime.h"

#include "TPolControl.h"

#include "TMrbLogger.h"
#include "SetColor.h"

ClassImp(TPolControl)

extern TMrbLogger * gMrbLog;			// access to message logger

TPolControl::TPolControl(const Char_t * Device) {
//__________________________________________________________________[C++ CTOR]
//////////////////////////////////////////////////////////////////////////////
// Name:           TPolControl
// Purpose:        Control the polarized ion source
// Description:    Class to control Ralf's polarized ion source via RS232.
// Keywords:       
//////////////////////////////////////////////////////////////////////////////

	if (Device) fSerIO = new TMrbSerialComm(Device); else fSerIO = NULL;
	fSubdevPattern = BIT(0);
	fWaitAV = 500;
	fWaitAR = 500;
	fWaitSD = 500;
	fHisto = NULL;
	fNameTable.Delete();
	fNameTable.SetName("Adc/Dac Names");
	fMonitorLayout.Delete();
	fMonitorData = NULL;
	fVerboseMode = kFALSE;
}

Int_t TPolControl::ReadAdc(Int_t Subdev, Int_t Channel, Bool_t LowResolution) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TPolControl::ReadAdc
// Purpose:        Read ADC value
// Arguments:      Int_t Subdev          -- subdevice
//                 Int_t Channel         -- (relative) adc channel to be read
//                 Bool_t LowResolution  -- read high/low resolution adc
// Results:        Int_t AdcValue        -- adc value
// Exceptions:     Value=-1 on error
// Description:    Reads data from given adc channel
//                 either with high or low resolution.
//                 Channel adressing is RELATIVE to subdevice.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	if (!this->IsOnline()) {
		gMrbLog->Err()	<< "Not in ONLINE mode" << endl;
		gMrbLog->Flush(this->ClassName(), "ReadAdc");
		return(-1);
	} else if (!fSerIO->IsOpen()) {
		gMrbLog->Err()	<< "Device not open - " << fSerIO->GetName() << endl;
		gMrbLog->Flush(this->ClassName(), "ReadAdc");
		return(-1);
	}
	if (!this->CheckAdcChannel(Subdev, Channel, "ReadAdc")) return(-1);

	Int_t chn = Subdev * kPolNofAdcs + Channel;

	TString cmd;
	if (LowResolution) {
		cmd = "AH";
		cmd += chn;
		fSerIO->WriteData(cmd.Data());
	} else {
		cmd = "AV";
		cmd += chn;
		fSerIO->WriteData(cmd.Data());
		gSystem->Sleep(fWaitAV);
		fSerIO->WriteData("AR");
		gSystem->Sleep(fWaitAR);
	}
	Int_t nBytes;
	TString result;
	for (Int_t i = 0; i < 4; i++) {
		if ((nBytes = fSerIO->ReadData(result)) > 0) break;
		gSystem->Sleep(100);
	}
	if (nBytes == 0) {
		gMrbLog->Err()	<< "Read error for channel " << chn
						<< ": byte count zero" << endl;
		gMrbLog->Flush(this->ClassName(), "ReadAdc");
		return(-1);
	}
	result = result.Strip(TString::kTrailing, '\n');
	result = result.Strip(TString::kBoth);
	if (result.IsDigit()) {
		Int_t value = result.Atoi();
		if (value < 0) return(kPolAdcRange + value); else return(-value);
	} else {
		gMrbLog->Err()	<< "Read error for channel " << chn
						<< ": cmd=" << cmd << ", result=" << result << endl;
		gMrbLog->Flush(this->ClassName(), "ReadAdc");
		return(-1);
	}
}

Int_t TPolControl::ReadAdc(Int_t AbsChannel, Bool_t LowResolution) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TPolControl::ReadAdc
// Purpose:        Read ADC value
// Arguments:      Int_t Channel         -- (absolute) adc channel to be read
//                 Bool_t LowResolution  -- read high/low resolution adc
// Results:        Int_t AdcValue        -- adc value
// Exceptions:     Value=-1 on error
// Description:    Reads data from given adc channel
//                 either with high or low resolution.
//                 Channel adressing is ABSOLUTE.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	if (!this->IsOnline()) {
		gMrbLog->Err()	<< "Not in ONLINE mode" << endl;
		gMrbLog->Flush(this->ClassName(), "ReadAdc");
		return(-1);
	} else if (!fSerIO->IsOpen()) {
		gMrbLog->Err()	<< "Device not open - " << fSerIO->GetName() << endl;
		gMrbLog->Flush(this->ClassName(), "ReadAdc");
		return(-1);
	}
	if (!this->CheckAdcChannel(AbsChannel, "ReadAdc")) return(-1);
	return(this->ReadAdc(AbsChannel / kPolNofAdcs, AbsChannel % kPolNofAdcs, LowResolution));
}

Int_t TPolControl::ReadAdc(const Char_t * ChnName, Bool_t LowResolution) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TPolControl::ReadAdc
// Purpose:        Read ADC value
// Arguments:      Char_t * ChnName      -- channel name
//                 Bool_t LowResolution  -- read high/low resolution adc
// Results:        Int_t AdcValue        -- adc value
// Exceptions:     Value=-1 on error
// Description:    Reads data from given adc channel
//                 either with high or low resolution.
//                 Channel adressing is BY NAME.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TMrbNamedX * nx;
	Int_t chn;
	TString entryName;

	if (!this->IsOnline()) {
		gMrbLog->Err()	<< "Not in ONLINE mode" << endl;
		gMrbLog->Flush(this->ClassName(), "ReadAdc");
		return(-1);
	} else if (!fSerIO->IsOpen()) {
		gMrbLog->Err()	<< "Device not open - " << fSerIO->GetName() << endl;
		gMrbLog->Flush(this->ClassName(), "ReadAdc");
		return(-1);
	}

	nx = fNameTable.FindByName(ChnName);

	if (nx == NULL) {
		gMrbLog->Err()	<< "Channel name not found in table - " << ChnName << endl;
		gMrbLog->Flush(this->ClassName(), "ReadAdc");
		return(-1);
	}
	entryName = nx->GetName();
	if (entryName.Index(".A", 0) == -1) {
		gMrbLog->Err()	<< "Not an ADC - " << ChnName << endl;
		gMrbLog->Flush(this->ClassName(), "ReadAdc");
		return(-1);
	}
	chn = nx->GetIndex();

	return(this->ReadAdc(chn / kPolNofAdcs, chn % kPolNofAdcs, LowResolution));
}

Int_t TPolControl::SetDac(Int_t AbsChannel, Double_t DacValue, Int_t ReadBack, Bool_t LowResolution) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TPolControl::SetDac
// Purpose:        Set DAC value
// Arguments:      Int_t AbsChannel      -- (absolute) dac channel to be addressed
//                 Double_t DacValue     -- value to be set
//                 Int_t ReadBack        -- (absolute) adc channel for readback
//                 Bool_t LowResolution  -- read with high/low resolution
// Results:        Int_t AdcValue        -- adc value read back
// Exceptions:     Value=-1 on error
// Description:    Sets given dac channel and reads result from readback adc.
//                 Channel adressing is ABSOLUTE.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	if (!this->IsOnline()) {
		gMrbLog->Err()	<< "Not in ONLINE mode" << endl;
		gMrbLog->Flush(this->ClassName(), "SetDac");
		return(-1);
	} else if (!fSerIO->IsOpen()) {
		gMrbLog->Err()	<< "Device not open - " << fSerIO->GetName() << endl;
		gMrbLog->Flush(this->ClassName(), "SetDac");
		return(-1);
	}
	if (!this->CheckDacChannel(AbsChannel, "SetDac")) return(-1);
	if (!this->CheckDacValue(DacValue, "SetDac")) return(-1);
	if (ReadBack != -1 && !this->CheckAdcChannel(ReadBack, "SetDac")) return(-1);

	Int_t dacVal = (Int_t) (DacValue * kPolDacRange / (Double_t) kPolMaxVoltage);

	TString cmd = "SD";
	cmd += AbsChannel;
	cmd += "=";
	cmd += dacVal;
	fSerIO->WriteData(cmd.Data());
	gSystem->Sleep(fWaitSD);
	if (ReadBack != -1) {
		return(this->ReadAdc(ReadBack, LowResolution));
	} else {
		return(0);
	}
}

Int_t TPolControl::SetDac(Int_t Subdev, Int_t Channel, Double_t DacValue, Int_t ReadBack, Bool_t LowResolution) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TPolControl::SetDac
// Purpose:        Set DAC value
// Arguments:      Int_t Subdev          -- subdevice
//                 Int_t Channel         -- (relative) dac channel to be addressed
//                 Double_t DacValue     -- value to be set
//                 Int_t ReadBack        -- (relative) adc channel for readback
//                 Bool_t LowResolution  -- read with high/low resolution
// Results:        Int_t AdcValue        -- adc value read back
// Exceptions:     Value=-1 on error
// Description:    Sets given dac channel and reads result from readback adc
//                 Channel adressing is RELATIVE to subdevice.
//                 Dac channel and readback adc have to be taken from SAME subdevice.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	if (!this->IsOnline()) {
		gMrbLog->Err()	<< "Not in ONLINE mode" << endl;
		gMrbLog->Flush(this->ClassName(), "SetDac");
		return(-1);
	} else if (!fSerIO->IsOpen()) {
		gMrbLog->Err()	<< "Device not open - " << fSerIO->GetName() << endl;
		gMrbLog->Flush(this->ClassName(), "SetDac");
		return(-1);
	}
	if (!this->CheckDacChannel(Subdev, Channel, "SetDac")) return(-1);
	Channel += Subdev * kPolNofAdcs;
	if (ReadBack != -1) {
		if (!this->CheckAdcChannel(Subdev, ReadBack, "SetDac")) return(-1);
		ReadBack += Subdev * kPolNofAdcs;
	}
	return(this->SetDac(Channel, DacValue, ReadBack, LowResolution));
}

Int_t TPolControl::SetDac(const Char_t * ChnName, Double_t DacValue, const Char_t *  ReadBack, Bool_t LowResolution) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TPolControl::SetDac
// Purpose:        Set DAC value
// Arguments:      Char_t * ChnName      -- channel name
//                 Double_t DacValue     -- value to be set
//                 Char_t * ReadBack     -- adc channel for readback
//                 Bool_t LowResolution  -- read with high/low resolution
// Results:        Int_t AdcValue        -- adc value read back
// Exceptions:     Value=-1 on error
// Description:    Sets given dac channel and reads result from readback adc
//                 Channel adressing is BY NAME.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TMrbNamedX * dac;
	TMrbNamedX * rdb;
	Int_t dacChn, rdbChn;
	TString entryName;

	if (!this->IsOnline()) {
		gMrbLog->Err()	<< "Not in ONLINE mode" << endl;
		gMrbLog->Flush(this->ClassName(), "SetDac");
		return(-1);
	} else if (!fSerIO->IsOpen()) {
		gMrbLog->Err()	<< "Device not open - " << fSerIO->GetName() << endl;
		gMrbLog->Flush(this->ClassName(), "SetDac");
		return(-1);
	}

	entryName = ChnName;
	entryName += ".D";
	dac = fNameTable.FindByName(entryName.Data());

	if (dac == NULL) {
		gMrbLog->Err()	<< "DAC not found in name table - " << ChnName << endl;
		gMrbLog->Flush(this->ClassName(), "SetDac");
		return(-1);
	}
	dacChn = dac->GetIndex();

	if (ReadBack != NULL) {
		entryName = ReadBack;
		entryName += ".A";
		rdb = fNameTable.FindByName(entryName.Data());
		if (rdb == NULL) {
			gMrbLog->Err()	<< "ADC not found in name table - " << ReadBack << endl;
			gMrbLog->Flush(this->ClassName(), "SetDac");
			return(-1);
		}
		rdbChn = rdb->GetIndex();
	} else {
		rdbChn = -1;
	}

	return(this->SetDac(dacChn, DacValue, rdbChn, LowResolution));
}

TH1F * TPolControl::Plot(	const Char_t * HistoName,
							Int_t X, Int_t Y,
							Double_t Xmin, Double_t Xmax, Double_t DeltaX,
							const Char_t * Xaxis, const Char_t * Yaxis) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TPolControl::Plot
// Purpose:        Plot Y = f(X)
// Arguments:      Char_t * HistoName  -- name
//                 Int_t X             -- (absolute) dac channel X
//                 Int_t Y             -- (absolute) adc channel Y
//                 Double_t Xmin       -- lower limit
//                 Double_t Xmax       -- upper limit
//                 Double_t DeltaX     -- step width X
//                 Char_t * Xaxis      -- text X
//                 Char_t * Yaxis      -- text Y
// Results:        TH1F * Histogram    -- resulting histogram
// Exceptions:
// Description:    Take a plot. Channel adressing is ABSOLUTE.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	if (!this->IsOnline()) {
		gMrbLog->Err()	<< "Not in ONLINE mode" << endl;
		gMrbLog->Flush(this->ClassName(), "Plot");
		return(NULL);
	} else if (!fSerIO->IsOpen()) {
		gMrbLog->Err()	<< "Device not open - " << fSerIO->GetName() << endl;
		gMrbLog->Flush(this->ClassName(), "Plot");
		return(NULL);
	}
	if (!this->CheckDacChannel(X, "Plot")) return(NULL);
	if (!this->CheckAdcChannel(Y, "Plot")) return(NULL);
	if (Xmin < 0. || Xmax < 0. || Xmin >= Xmax) {
		gMrbLog->Err()	<< "Illegal range - [" << Xmin << "," << Xmax << "]" << endl;
		gMrbLog->Flush(this->ClassName(), "Plot");
		return(NULL);
	}

	if (!this->CheckDacValue(Xmin, "Plot")) return(NULL);
	if (!this->CheckDacValue(Xmax, "Plot")) return(NULL);

	Int_t steps = (Int_t) ((Xmax - Xmin) / DeltaX + .5);
	if (fHisto) delete fHisto;
	TString hTitle = "Plot Y=f(X), Y=chn";
	hTitle += Y;
	hTitle += ", X=chn";
	hTitle += X;
	hTitle += ", X in [";
	hTitle += Xmin;
	hTitle += " (";
	hTitle += DeltaX;
	hTitle += ") ";
	hTitle += Xmax;
	hTitle += "]";
	fHisto = new TH1F(HistoName, hTitle.Data(), steps, Xmin, Xmax);
	TString hAxis;
	if (Xaxis == NULL || *Xaxis == '\0') {
		hAxis = "DAC chn";
		hAxis += X;
		Xaxis = hAxis.Data();
	}
	fHisto->GetXaxis()->SetTitle(Xaxis);
	fHisto->GetXaxis()->CenterTitle();
	if (Yaxis == NULL || *Yaxis == '\0') {
		hAxis = "ADC chn";
		hAxis += Y;
		Yaxis = hAxis.Data();
	}
	fHisto->GetYaxis()->SetTitle(Yaxis);
	fHisto->GetYaxis()->CenterTitle();
	Double_t dacValue = Xmin;
	for (Int_t i = 0; i < steps; i++, dacValue += DeltaX) {
		if (dacValue > Xmax) break;
		this->SetDac(X, dacValue);
		Double_t yVal = ((Double_t) this->ReadAdc(Y)) * kPolMaxVoltage / kPolAdcRange;
		fHisto->Fill((Axis_t) dacValue, (Stat_t) yVal);
	}
	return(fHisto);
}

TH1F * TPolControl::Plot(	const Char_t * HistoName,
							Int_t Subdev, Int_t X, Int_t Y,
							Double_t Xmin, Double_t Xmax, Double_t DeltaX,
							const Char_t * Xaxis, const Char_t * Yaxis) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TPolControl::Plot
// Purpose:        Plot Y = f(X)
// Arguments:      Char_t * HistoName  -- name
//                 Int_t Subdev        -- subdevice
//                 Int_t X             -- (relative) dac channel X
//                 Int_t Y             -- (relative) adc channel Y
//                 Double_t Xmin       -- lower limit
//                 Double_t Xmax       -- upper limit
//                 Double_t DeltaX     -- step width X
//                 Char_t * Xaxis      -- text X
//                 Char_t * Yaxis      -- text Y
// Results:        TH1F * Histogram    -- resulting histogram
// Exceptions:
// Description:    Take a plot.
//                 Channel adressing is RELATIVE to subdevice.
//                 X and Y have to be taken from SAME subdevice.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	if (!this->IsOnline()) {
		gMrbLog->Err()	<< "Not in ONLINE mode" << endl;
		gMrbLog->Flush(this->ClassName(), "Plot");
		return(NULL);
	} else if (!fSerIO->IsOpen()) {
		gMrbLog->Err()	<< "Device not open - " << fSerIO->GetName() << endl;
		gMrbLog->Flush(this->ClassName(), "Plot");
		return(NULL);
	}
	if (!this->CheckDacChannel(Subdev, X, "Plot")) return(NULL);
	if (!this->CheckAdcChannel(Subdev, Y, "Plot")) return(NULL);
	X += Subdev * kPolNofAdcs;
	Y += Subdev * kPolNofAdcs;
	return(this->Plot(HistoName, X, Y, Xmin, Xmax, DeltaX, Xaxis, Yaxis));
}

TH1F * TPolControl::Plot(	const Char_t * HistoName,
							const Char_t * X, const Char_t * Y,
							Double_t Xmin, Double_t Xmax, Double_t DeltaX,
							const Char_t * Xaxis, const Char_t * Yaxis) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TPolControl::Plot
// Purpose:        Plot Y = f(X)
// Arguments:      Char_t * HistoName  -- name
//                 Char_t *  X         -- dac channel X
//                 Char_t *  Y         -- adc channel Y
//                 Double_t Xmin       -- lower limit
//                 Double_t Xmax       -- upper limit
//                 Double_t DeltaX     -- step width X
//                 Char_t * Xaxis      -- text X
//                 Char_t * Yaxis      -- text Y
// Results:        TH1F * Histogram    -- resulting histogram
// Exceptions:
// Description:    Take a plot. Channel adressing is BY NAME.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TMrbNamedX * dacX;
	TMrbNamedX * adcY;
	Int_t chnX, chnY;
	TString entryName;

	if (!this->IsOnline()) {
		gMrbLog->Err()	<< "Not in ONLINE mode" << endl;
		gMrbLog->Flush(this->ClassName(), "Plot");
		return(NULL);
	} else if (!fSerIO->IsOpen()) {
		gMrbLog->Err()	<< "Device not open - " << fSerIO->GetName() << endl;
		gMrbLog->Flush(this->ClassName(), "Plot");
		return(NULL);
	}

	entryName = X;
	entryName += ".D";
	dacX = fNameTable.FindByName(entryName.Data());

	if (dacX == NULL) {
		gMrbLog->Err()	<< "DAC not found in name table - " << X << endl;
		gMrbLog->Flush(this->ClassName(), "Plot");
		return(NULL);
	}
	chnX = dacX->GetIndex();
	if (Xaxis == NULL) Xaxis = dacX->GetTitle();

	entryName = Y;
	entryName += ".A";
	adcY = fNameTable.FindByName(entryName.Data());
	if (adcY == NULL) {
		gMrbLog->Err()	<< "ADC not found in name table - " << Y << endl;
		gMrbLog->Flush(this->ClassName(), "Plot");
		return(NULL);
	}
	chnY = adcY->GetIndex();
	if (Yaxis == NULL) Yaxis = adcY->GetTitle();

	return(this->Plot(HistoName, chnX, chnY, Xmin, Xmax, DeltaX, Xaxis, Yaxis));
}

TNtuple * TPolControl::Monitor(	const Char_t * FileName,
								const Char_t * Comment,
								const Char_t * LofChannels,
								Int_t NofRecords, Int_t Period) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TPolControl::Monitor
// Purpose:        Monitor ADCs and store data as ntuples
// Arguments:      Char_t * FileName     -- name of resulting root file
//                 Char_t * Comment      -- commenting text
//                 Char_t * LofChannels  -- channels to be monitored
//                 Int_t NofRecords      -- number of snapshots to be taken
//                 Int_t Period          -- monitor period
// Results:        TNtuple * MonitorData -- data
// Exceptions:
// Description:    Monitor ADC data and write it to root file.
//                 Channel list has to be given as a colon-separated list of
//                 ADC names (see also TNtuple class).
//                 Naming conventions:
//                     (1)   <"xyz">          search name in name table
//                     (2)   <subdev>-<chn>   use ADC with given relative addr
//                     (3)   <abschn>         use ADC with given absolute addr
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	if (!this->IsOnline()) {
		gMrbLog->Err()	<< "Not in ONLINE mode" << endl;
		gMrbLog->Flush(this->ClassName(), "Monitor");
		return(NULL);
	} else if (this->MonitorIsOn()) {
		gMrbLog->Err()	<< "Can't create a new monitor - monitoring already in progress ("
						<< (this->IsWriting() ? "WRITE" : "READ") << ")" << endl;
		gMrbLog->Flush(this->ClassName(), "Monitor");
		return(NULL);
	} else if (!fSerIO->IsOpen()) {
		gMrbLog->Err()	<< "Device not open - " << fSerIO->GetName() << endl;
		gMrbLog->Flush(this->ClassName(), "Monitor");
		return(NULL);
	}

	TString varList = "time:";
	fMonitorLayout.Delete();
	fMonitorLayout.SetName("MonitorLayout");
	TString numString = "123456789";
	Int_t chn;
	Int_t sd;
	TString lofChannels = LofChannels;
	TString adcName;
	Int_t from = 0;
	Int_t nofAdcs = 0;
	while (lofChannels.Tokenize(adcName, from, ":")) {
		nofAdcs++;
		TString a1 = adcName(0);
		if (numString.Index(a1.Data(), 0) != -1) {
			Int_t n = adcName.Index("-", 0);
			if (n != -1) {
				TString subdev = adcName(0, n);
				if (!subdev.IsDigit()) {
					gMrbLog->Err()	<< "Illegal subdevice - " << subdev << endl;
					gMrbLog->Flush(this->ClassName(), "Monitor");
					return(NULL);
				}
				sd = subdev.Atoi();
				TString channel = adcName(n + 1, adcName.Length() - n - 1);
				if (!channel.IsDigit()) {
					gMrbLog->Err()	<< "Illegal channel - " << channel << endl;
					gMrbLog->Flush(this->ClassName(), "Monitor");
					return(NULL);
				}
				chn = channel.Atoi();
				if (!this->CheckAdcChannel(sd, chn, "Monitor")) return(NULL);
				adcName = "S";
				adcName += sd;
				adcName += "C";
				adcName += chn;
				this->AddName(&fMonitorLayout, adcName.Data(), sd, chn, "A", "");
				varList += adcName;
			} else {
				if (!adcName.IsDigit()) {
					gMrbLog->Err()	<< "Illegal channel - " << adcName << endl;
					gMrbLog->Flush(this->ClassName(), "Monitor");
					return(NULL);
				}
				chn = adcName.Atoi();
				if (!this->CheckAdcChannel(chn, "Monitor")) return(NULL);
				adcName = "C";
				adcName += chn;
				sd = chn / kPolNofAdcs;
				chn %= kPolNofAdcs;
				this->AddName(&fMonitorLayout, adcName.Data(), sd, chn, "A", "");
				varList += adcName;
			}
		} else {
			TMrbNamedX * nx = this->Find(&fNameTable, adcName.Data(), "A", "Monitor", kFALSE);
			if (nx) 	{
				fMonitorLayout.AddNamedX(nx);
				if (nofAdcs > 1) varList += ":";
				varList += adcName;
			} else {
				gMrbLog->Err()	<< "Illegal ADC name - " << adcName << endl;
				gMrbLog->Flush(this->ClassName(), "Monitor");
				return(NULL);
			}
		}
	}
	if (nofAdcs <= 0) {
		gMrbLog->Err()	<< "List of channels is empty" << endl;
		gMrbLog->Flush(this->ClassName(), "Monitor");
		return(NULL);
	}

	fMonitorFile = FileName;
	fMonitorRootFile = new TFile(FileName, "RECREATE");
	if (fMonitorRootFile->IsZombie()) {
		gMrbLog->Err()	<< "Can't open file \"" << FileName << "\" for monitoring" << endl;
		gMrbLog->Flush(this->ClassName(), "Monitor");
		return(NULL);
	}

	fMonitorData = new TNtuple("MonitorData", Comment, varList.Data());
	if (fMonitorData->IsZombie()) {
		gMrbLog->Err()	<< "Can't create a ntuple for monitoring" << endl;
		gMrbLog->Flush(this->ClassName(), "Monitor");
		return(NULL);
	}

	fWriteFlag = kTRUE; 		// we are writing monitor data to file
	fMonitorLayout.Write(NULL, TObject::kSingleKey); 	// first of all write layout data

	fNofRecords = NofRecords;	// number of records to write
	fRecordCount = 0;			// number of records written so far

	this->PrintMonitorLayout();

	if (Period == 0) Period = fMonitorData->GetNvar() - 1;

	TTimer * t = new TTimer(Period * 1000); 	// create a timer, resolution is seconds
	t->SetObject(this);							// connect to it
	if (fNofRecords == 0) {
		gMrbLog->Out()	<< "Monitoring started (period=" << Period << "s, call StopMonitor() to stop)" << endl;
		fNofRecords = 1000000;
	} else {
		gMrbLog->Out()	<< "Monitoring started (period=" << Period << "s, stop after " << fNofRecords << " record(s) or on StopMonitor())" << endl;
	}
	gMrbLog->Flush(this->ClassName(), "Monitor", setblue);
	fStopFlag = kFALSE;
	t->TurnOn();
	return(fMonitorData);
}

Bool_t TPolControl::HandleTimer(TTimer * Timer) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TPolControl::HandleTimer
// Purpose:        Get notified on timer timeouts
// Arguments:      TTimer * Timer   -- timer object
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Will be called on timer events.
//                 Reads ADCs specified by monitor layout.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	Int_t nofData;
	TArrayF monData;
	TMrbNamedX * nx;
	TDatime timeStamp;
	union t2f { UInt_t t; Float_t f; } t2f;

	nofData = fMonitorLayout.GetLast(); 	 		// number of ntuple elements
	monData.Set(nofData + 1);						// + timestamp
	monData.Reset();

	t2f.t = timeStamp.Get();
	monData[0] = t2f.f;

	for (Int_t i = 1; i <= nofData; i++) {
		nx = (TMrbNamedX *) fMonitorLayout[i];
		monData[i] = (Float_t) this->ReadAdc(nx->GetIndex()) * kPolMaxVoltage / kPolAdcRange;
	}
	fMonitorData->Fill(monData.GetArray());
	fRecordCount++;

	if (this->IsVerbose()) this->PrintRecordData(cout, fRecordCount, t2f.t, monData.GetArray() + 1, nofData);

	if ((fRecordCount >= fNofRecords) || fStopFlag) {
		Timer->TurnOff();
		fMonitorRootFile->Write();
		fMonitorRootFile->Close();
		fMonitorData = NULL;
		delete Timer;
		gMrbLog->Out()	<< "Monitoring stopped (" << fRecordCount << " record(s))" << endl;
		gMrbLog->Flush(this->ClassName(), "StopMonitor", setblue);
	}
	return(kTRUE);
}

TNtuple * TPolControl::Monitor(	const Char_t * FileName) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TPolControl::Monitor
// Purpose:        Read monitor data from root file
// Arguments:      Char_t * FileName     -- name of resulting root file
// Results:        TNtuple * MonitorData -- data
// Exceptions:
// Description:    Opens root file and initializes ntuple access.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	if (this->MonitorIsOn()) {
		gMrbLog->Err()	<< "Can't create a new monitor - monitoring already in progress ("
						<< (this->IsWriting() ? "WRITE" : "READ") << ")" << endl;
		gMrbLog->Flush(this->ClassName(), "Monitor");
		return(NULL);
	}

	fMonitorFile = FileName;
	fMonitorRootFile = new TFile(FileName, "READ");
	if (fMonitorRootFile->IsZombie()) {
		gMrbLog->Err()	<< "Can't open file \"" << FileName << "\"" << endl;
		gMrbLog->Flush(this->ClassName(), "Monitor");
		return(NULL);
	}

	fMonitorLayout.Delete();
	fMonitorLayout.SetName("MonitorLayout");
	fMonitorLayout.AddNamedX((TMrbLofNamedX *) fMonitorRootFile->Get("TMrbLofNamedX"));

	fWriteFlag = kFALSE;

	fMonitorData = (TNtuple *) fMonitorRootFile->Get("MonitorData");
	this->PrintMonitorLayout();

	return(fMonitorData);
}

Int_t TPolControl::GetNofEntries() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TPolControl::GetNofEntries
// Purpose:        Number of entries
// Arguments:      --
// Results:        Int_t NofEntries   -- number of records in root file
// Exceptions:
// Description:    Returns number of entries
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	if (!this->MonitorIsOn()) {
		gMrbLog->Err()	<< "Monitor not active" << endl;
		gMrbLog->Flush(this->ClassName(), "GetNofEntries");
		return(-1);
	} else if (!this->IsReading()) {
		gMrbLog->Err()	<< "Monitor not in READ mode" << endl;
		gMrbLog->Flush(this->ClassName(), "GetNofEntries");
		return(-1);
	}
	return((Int_t) fMonitorData->GetEntries());
}

Int_t TPolControl::GetNofADCs() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TPolControl::GetNofADCs
// Purpose:        Number of ADCs
// Arguments:      --
// Results:        Int_t NofADCs   -- number of adcs in ntuple
// Exceptions:
// Description:    Returns number of ADCs
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	if (!this->MonitorIsOn()) {
		gMrbLog->Err()	<< "Monitor not active" << endl;
		gMrbLog->Flush(this->ClassName(), "GetNofADCs");
		return(-1);
	} else if (!this->IsReading()) {
		gMrbLog->Err()	<< "Monitor not in READ mode" << endl;
		gMrbLog->Flush(this->ClassName(), "GetNofADCs");
		return(-1);
	}
	return(fMonitorData->GetNvar());
}

Int_t TPolControl::GetEntry(Int_t EntryNumber, UInt_t & TimeStamp, TArrayF & Data) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TPolControl::GetEntry
// Purpose:        Read monitor data from root file
// Arguments:      Int_t EntryNumber     -- entry to be read
//                 UInt_t & TimeStamp    -- time stamp (secs since 1.1.1970)
//                 TArrayF & Data        -- monitor data
// Results:        Int_t NofData         -- number of data elements
// Exceptions:
// Description:    Reads given entry from tree.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	union t2f { UInt_t t; Float_t f; } t2f;

	if (!this->MonitorIsOn()) {
		gMrbLog->Err()	<< "Monitor not active" << endl;
		gMrbLog->Flush(this->ClassName(), "GetEntry");
		return(-1);
	} else if (!this->IsReading()) {
		gMrbLog->Err()	<< "Monitor not in READ mode" << endl;
		gMrbLog->Flush(this->ClassName(), "GetEntry");
		return(-1);
	}

	Int_t nofEntries = (Int_t) fMonitorData->GetEntries();
	if (EntryNumber >= nofEntries) {
		gMrbLog->Err()	<< "Entry out of range - " << EntryNumber
						<< " (has to be in [0," << nofEntries - 1 << "])" << endl;
		gMrbLog->Flush(this->ClassName(), "GetEntry");
		return(-1);
	}

	fMonitorData->ResetBranchAddresses();
	fMonitorData->GetEntry(EntryNumber);
	t2f.f = *fMonitorData->GetArgs();
	TimeStamp = t2f.t;
	Int_t nofData = fMonitorData->GetNvar() - 1;
	Data.Set(nofData, fMonitorData->GetArgs() + 1);
	return(nofData);
}

Bool_t TPolControl::StopMonitor() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TPolControl::StopMonitor
// Purpose:        Stop monitoring
// Arguments:      --
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Sets stop flag to immediately stop monitoring.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	if (!this->MonitorIsOn()) {
		gMrbLog->Err()	<< "Monitoring not active" << endl;
		gMrbLog->Flush(this->ClassName(), "Monitor");
		return(kFALSE);
	} else {
		fStopFlag = kTRUE;
		if (this->IsReading()) {
			fMonitorRootFile->Close();
			fMonitorData = NULL;
			gMrbLog->Out()	<< "Replay of monitor data stopped, file \"" << fMonitorFile << "\" closed" << endl;
			gMrbLog->Flush(this->ClassName(), "StopMonitor", setblue);
		}
		return(kTRUE);
	}
}

void TPolControl::Print() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TPolControl::Print
// Purpose:        Output settings to cout
// Arguments:      --
// Results:        --
// Exceptions:
// Description:    Outputs settings to cout.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	UInt_t pat;
	Int_t sd;
	Int_t ch0;

	cout	<< endl;
	cout	<< "Polarized Ion Source Control:" << endl;
	cout	<< "-----------------------------------------------------------------" << endl;

	cout	<< "Subdevices             : ";
	pat = fSubdevPattern;
	sd = 0;
	while (pat)	{
		if (pat & 1) cout << sd << " ";
		sd++;
		pat >>= 1;
	}
	cout	<< endl;

	cout	<< "ADC Channels           : ";
	pat = fSubdevPattern;
	sd = 0;
	ch0 = 0;
	while (pat) {
		if (pat & 1) cout << sd << ":[" << ch0 << ".." << (ch0 + kPolNofAdcs - 1) << "] ";
		sd++;
		ch0 += kPolNofAdcs;
		pat >>= 1;
	}
	cout	<< endl;

	cout	<< "DAC Channels           : ";
	pat = fSubdevPattern;
	sd = 0;
	ch0 = 0;
	while (pat) {
		if (pat & 1) cout << sd << ":[" << ch0 << ".." << (ch0 + kPolNofDacs - 1) << "] ";
		sd++;
		ch0 += kPolNofAdcs;
		pat >>= 1;
	}
	cout	<< endl;

	if (this->IsOnline()) {
		cout	<< "Wait states            : "	<< fWaitAV << " ms (AV), "
												<< fWaitAR << " ms (AR), "
												<< fWaitSD << " ms (SD)" << endl;

		fSerIO->Print();
	}
}

void TPolControl::SetWait(Int_t WaitAV, Int_t WaitAR, Int_t WaitSD) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TPolControl::SetWait
// Purpose:        Set wait states
// Arguments:      Int_t WaitAV     -- msecs to wait after AV command
//                 Int_t WaitAR     -- msecs to wait after AR command
//                 Int_t WaitSD     -- msecs to wait after SD command
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Defines several wait states.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	if (!this->IsOnline()) {
		gMrbLog->Err()	<< "Not in ONLINE mode" << endl;
		gMrbLog->Flush(this->ClassName(), "SetWait");
		return;
	};

	fWaitAV = WaitAV;
	fWaitAR = (WaitAR > 0) ? WaitAR : WaitAV;
	fWaitSD = (WaitSD > 0) ? WaitSD : WaitAV;
}

void TPolControl::SetSubdevs(Int_t S1, Int_t S2, Int_t S3, Int_t S4, Int_t S5) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TPolControl::SetSubdevs
// Purpose:        Define list if subdevices
// Arguments:      SX      -- subdevice #X
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Defines subdevices.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	fSubdevPattern = 0;
	if (S1 >= 0) fSubdevPattern |= 0x1 << S1;
	if (S2 >= 0) fSubdevPattern |= 0x1 << S2;
	if (S3 >= 0) fSubdevPattern |= 0x1 << S3;
	if (S4 >= 0) fSubdevPattern |= 0x1 << S4;
	if (S5 >= 0) fSubdevPattern |= 0x1 << S5;
}

Int_t TPolControl::ReadNameTable(const Char_t * NameTableFile) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TPolControl::ReadNameTable
// Purpose:        Read adc/dac specs from file
// Arguments:      Char_t * NameTableFile   -- file name
// Results:        Int_t NofEntries         -- number of entries read
// Exceptions:
// Description:    Inputs adc/dac specs from file
//                 File format has to be:
//                      <name>:<subdevice>:<channel>:<type>[:<comment>]
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	ifstream f(NameTableFile, ios::in);
	if (!f.good()) {
		gMrbLog->Err() << gSystem->GetError() << " (" << gSystem->GetErrno() << ")" << " - " << NameTableFile << endl;
		gMrbLog->Flush(this->ClassName(), "ReadNameTable");
		return(0);
	}

	Int_t nofEntries = 0;
	Int_t lineNo = 0;
	TString line;
	while (kTRUE) {
		line.ReadLine(f, kFALSE);
		if (f.eof()) break;
		line = line.Strip(TString::kBoth);
		lineNo++;
		if (this->AddName(line.Data())) {
			nofEntries++;
		} else {
			gMrbLog->Err() << NameTableFile << " (line " << lineNo << "): Ignored" << endl;
			gMrbLog->Flush(this->ClassName(), "ReadNameTable");
		}
	}
	fNameTableFile = NameTableFile;
	return(nofEntries);
}

TMrbNamedX * TPolControl::Find(const Char_t * Name, const Char_t * Type) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TPolControl::Find
// Purpose:        Find a given entry in name table
// Arguments:      Char_t * Name         -- adc/dac name
//                 Char_t * Type         -- "A" for ADC, "D" for DAC
// Results:        TMrbNamedX * Entry    -- table entry
// Exceptions:
// Description:    Searches for an entry by its name and type.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	return(this->Find(&fNameTable, Name, Type));
}

TMrbNamedX * TPolControl::Find(Int_t Subdev, Int_t Channel, const Char_t * Type) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TPolControl::Find
// Purpose:        Find a given entry in name table
// Arguments:      Int_t Subdev          -- subdevice
//                 Int_t Channel         -- channel
//                 Char_t * Type         -- "A" for ADC, "D" for DAC
// Results:        TMrbNamedX * Entry    -- table entry
// Exceptions:
// Description:    Searches for an entry by its address and type.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	return(this->Find(&fNameTable, Subdev, Channel, Type));
}

TMrbNamedX * TPolControl::Find(TMrbLofNamedX * Table, const Char_t * Name, const Char_t * Type,
											const Char_t * Method, Bool_t Verbose) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TPolControl::Find
// Purpose:        Find a given entry
// Arguments:      TMbrLofNamedX * Table -- table address
//                 Char_t * Name         -- adc/dac name
//                 Char_t * Type         -- "A" for ADC, "D" for DAC
//                 Char_t * Method       -- calling method
//                 Bool_t Verbose        -- kTRUE to activate error output
// Results:        TMrbNamedX * Entry    -- table entry
// Exceptions:
// Description:    Searches for an entry by its name and type.
//                 ** Method is PROTECTED **
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TString type;
	TMrbNamedX * nx;
	TString entryName;
	Bool_t isAdc;

	type = Type;
	entryName = Name;
	if (type.CompareTo("A") == 0) {
		entryName += ".A";
		isAdc = kTRUE;
	} else if (type.CompareTo("D") == 0) {
		entryName += ".D";
		isAdc = kFALSE;
	} else {
		gMrbLog->Err()	<< "Illegal type - " << type << " (should be \"A\" for ADC or \"D\" for DAC" << endl;
		gMrbLog->Flush(this->ClassName(), Method);
		return(NULL);
	}
	
	if ((nx = Table->FindByName(entryName.Data())) == NULL) {
		if (Verbose) {
			gMrbLog->Err()	<< (isAdc ? "ADC" : "DAC") << " not found - " << Name << endl;
			gMrbLog->Flush(this->ClassName(), Method);
		}
		return(NULL);
	}
	return(nx);
}

TMrbNamedX * TPolControl::Find(TMrbLofNamedX * Table, Int_t Subdev, Int_t Channel, const Char_t * Type,
									const Char_t * Method, Bool_t Verbose) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TPolControl::Find
// Purpose:        Find a given entry
// Arguments:      TMbrLofNamedX * Table -- table address
//                 Int_t Subdev          -- subdevice
//                 Int_t Channel         -- channel
//                 Char_t * Type         -- "A" for ADC, "D" for DAC
//                 Char_t * Method       -- calling method
//                 Bool_t Verbose        -- kTRUE to activate error output
// Results:        TMrbNamedX * Entry    -- table entry
// Exceptions:
// Description:    Searches for an entry by its address and type.
//                 ** Method is PROTECTED **
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TString type;
	TMrbNamedX * nx;
	Int_t chn;
	Bool_t isAdc;
	TString entryName;

	type = Type;
	if (type.CompareTo("A") == 0) isAdc = kTRUE;
	else if (type.CompareTo("D") == 0) isAdc = kFALSE;
	else {
		gMrbLog->Err()	<< "Illegal type - " << type << " (should be \"A\" for ADC or \"D\" for DAC" << endl;
		gMrbLog->Flush(this->ClassName(), Method);
		return(NULL);
	}
		
	if (!this->CheckSubdev(Subdev, Method)) return(NULL);

	if (isAdc) {
		if (!this->CheckAdcChannel(Subdev, Channel, Method)) return(NULL);
	} else {
		if (!this->CheckDacChannel(Subdev, Channel, Method)) return(NULL);
	}

	chn = Subdev * kPolNofAdcs + Channel;
	if ((nx = Table->FindByIndex(chn)) == NULL) {
		if (Verbose) {
			gMrbLog->Err()	<< (isAdc ? "ADC" : "DAC") << " not found - channel " << chn
							<< " (subdev=" << Subdev << ", chn=" << Channel << ")" << endl;
			gMrbLog->Flush(this->ClassName(), Method);
		}
		return(NULL);
	}
	entryName = nx->GetName();
	if (isAdc && (entryName.Index(".A", 0) == -1)) {
		entryName = entryName(0, entryName.Length() - 2);
		gMrbLog->Err()	<< "Not an ADC - " << entryName << " at channel " << chn
						<< " (subdev=" << Subdev << ", chn=" << Channel << ")" << endl;
		gMrbLog->Flush(this->ClassName(), Method);
		return(NULL);
	} else if (!isAdc && (entryName.Index(".D", 0) == -1)) {
		entryName = entryName(0, entryName.Length() - 2);
		gMrbLog->Err()	<< "Not a DAC - " << entryName << " at channel " << chn
						<< " (subdev=" << Subdev << ", chn=" << Channel << ")" << endl;
		gMrbLog->Flush(this->ClassName(), Method);
		return(NULL);
	} else return(nx);
}

TMrbNamedX * TPolControl::AddName(const Char_t * NameString) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TPolControl::AddName
// Purpose:        Add an entry to name table
// Arguments:      Char_t * NameString   -- string
// Results:        TMrbNamedX * Entry    -- table entry
// Exceptions:
// Description:    Adds one entry to name table
//                 Format has to be:
//                      <name>:<subdevice>:<channel>:<type>[:<comment>]
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TString str = NameString;
	TObjArray * specs = str.Tokenize(":");
	Int_t n = specs->GetEntries();
	if (n < kPolNofFields - 1) {
		gMrbLog->Err() << "Illegal format - " << str << endl;
		gMrbLog->Flush(this->ClassName(), "AddName");
		delete specs;
		return(NULL);
	}
	Int_t sd, chn;
	TString name = ((TObjString *) specs->At(kPolIdxName))->GetString();
	TString type = ((TObjString *) specs->At(kPolIdxType))->GetString();
	TString subdev = ((TObjString *) specs->At(kPolIdxSubdev))->GetString(); sd = subdev.Atoi();
	TString channel = ((TObjString *) specs->At(kPolIdxChannel))->GetString(); chn = channel.Atoi();
	TString comment = "";
	if (n == kPolNofFields) comment = ((TObjString *) specs->At(kPolIdxComment))->GetString();

	delete specs;
	return(this->AddName(&fNameTable, name.Data(), sd, chn, type.Data(), comment.Data()));
}

TMrbNamedX * TPolControl::AddName(const Char_t * Name, Int_t Subdev, Int_t Channel,
													const Char_t * Type, const Char_t * Comment) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TPolControl::AddName
// Purpose:        Add an entry to name table
// Arguments:      Char_t * Name         -- adc/dac name
//                 Int_t Subdev          -- subdevice
//                 Int_t Channel         -- channel
//                 Char_t * Type         -- "A" for ADC, "D" for DAC
//                 Char_t * Comment      -- comment
// Results:        TMrbNamedX * Entry    -- table entry
// Exceptions:
// Description:    Adds one entry to name table
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	return(this->AddName(&fNameTable, Name, Subdev, Channel, Type, Comment));
}

TMrbNamedX * TPolControl::AddName(TMrbLofNamedX * Table, const Char_t * Name, Int_t Subdev, Int_t Channel,
													const Char_t * Type, const Char_t * Comment) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TPolControl::AddName
// Purpose:        Add an entry to given table
// Arguments:      TMbrLofNamedX * Table -- table address
//                 Char_t * Name         -- adc/dac name
//                 Int_t Subdev          -- subdevice
//                 Int_t Channel         -- channel
//                 Char_t * Type         -- "A" for ADC, "D" for DAC
//                 Char_t * Comment      -- comment
// Results:        TMrbNamedX * Entry    -- table entry
// Exceptions:
// Description:    Adds one entry to table (name table or monitor layout).
//                 ** Method is PROTECTED **
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TString type;
	Bool_t isAdc;
	TMrbNamedX * nx;
	TString entryName;

	type = Type;
	entryName = Name;
	if (type.CompareTo("A") == 0) {
		entryName += ".A";
		isAdc = kTRUE;
	} else if (type.CompareTo("D") == 0) {
		entryName += ".D";
		isAdc = kFALSE;
	} else {
		gMrbLog->Err()	<< "Illegal type - " << type << " (should be \"A\" for ADC or \"D\" for DAC" << endl;
		gMrbLog->Flush(this->ClassName(), "AddName");
		return(NULL);
	}
	
	if (!this->CheckSubdev(Subdev, "AddName")) return(NULL);

	if (isAdc) {
		if (!this->CheckAdcChannel(Subdev, Channel, "AddName")) return(NULL);
	} else {
		if (!this->CheckDacChannel(Subdev, Channel, "AddName")) return(NULL);
	}

	if ((nx = Table->FindByName(Name)) != NULL) {
		gMrbLog->Err()	<< "Name already in table - " << Name << " (chn " << nx->GetIndex() << ")" << endl;
		gMrbLog->Flush(this->ClassName(), "AddName");
		return(NULL);
	}

	nx = new TMrbNamedX(Subdev * kPolNofAdcs + Channel, entryName.Data(), Comment);
	Table->AddNamedX(nx);
	return(nx);
}

void TPolControl::PrintNameTable() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TPolControl::PrintNameTable
// Purpose:        Output contents of name table
// Arguments:      --
// Results:        --
// Exceptions:
// Description:    Outputs name table to cout
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	if (fNameTable.GetLast() == -1) {
		gMrbLog->Err()	<< "Name table is empty" << endl;
		gMrbLog->Flush(this->ClassName(), "PrintNameTable");
		return;
	}

	cout	<< endl;
	cout	<< "Name table (" << (fNameTable.GetLast() + 1) << " entries read from file " << fNameTableFile << "):" << endl;
	cout	<< "-----------------------------------------------------------------------------------" << endl;
	cout	<< "Name      Type      AbsChn    Subdev    RelChn" << endl;
	cout	<< "..................................................................................." << endl;
	this->PrintTable(&fNameTable);
}

void TPolControl::PrintMonitorLayout() {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TPolControl::PrintMonitorLayout
// Purpose:        Output monitor layout
// Arguments:      --
// Results:        --
// Exceptions:
// Description:    Outputs monitor layout to cout
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	if (!this->MonitorIsOn()) {
		gMrbLog->Err()	<< "Monitor not active" << endl;
		gMrbLog->Flush(this->ClassName(), "PrintMonitorLayout");
		return;
	}

	if (fMonitorLayout.GetLast() == -1) {
		gMrbLog->Err()	<< "Monitor layout is empty" << endl;
		gMrbLog->Flush(this->ClassName(), "PrintMonitorLayout");
		return;
	}

	cout	<< endl;
	cout	<< "Monitor layout (file " << fMonitorFile << "):" << endl;
	cout	<< "-----------------------------------------------------------------------------------" << endl;
	cout	<< "Name      Type      AbsChn    Subdev    RelChn" << endl;
	cout	<< "..................................................................................." << endl;
	this->PrintTable(&fMonitorLayout);
}

void TPolControl::PrintRecords(Int_t Start, Int_t Stop) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TPolControl::PrintRecords
// Purpose:        Output record data to cout
// Arguments:      Int_t Start     -- record to start with
//                 Int_t Stop      -- record to stop with
// Results:        --
// Exceptions:
// Description:    Outputs record data to cout.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	UInt_t ts;
	TArrayF data;

	if (!this->MonitorIsOn()) {
		gMrbLog->Err()	<< "Monitor not active" << endl;
		gMrbLog->Flush(this->ClassName(), "PrintRecords");
		return;
	} else if (!this->IsReading()) {
		gMrbLog->Err()	<< "Monitor not in READ mode" << endl;
		gMrbLog->Flush(this->ClassName(), "PrintRecords");
		return;
	}
	Int_t nofRecords = this->GetNofEntries();
	if (Stop < 0) Stop = nofRecords - 1;
	if (Start < 0 || Start > Stop || Stop > nofRecords - 1) {
		gMrbLog->Err()	<< "Limits out of range (should be in [0," << nofRecords - 1 << "])" << endl;
		gMrbLog->Flush(this->ClassName(), "PrintRecords");
		return;
	}
	for (Int_t i = Start; i <= Stop; i++) {
		Int_t n = this->GetEntry(i, ts, data);
		this->PrintRecordData(cout, i, ts, data.GetArray(), n);
	}
}

void TPolControl::PrintRecords(const Char_t * File, Int_t Start, Int_t Stop) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TPolControl::PrintRecords
// Purpose:        Output record data to file
// Arguments:      Int_t Start     -- record to start with
//                 Int_t Stop      -- record to stop with
// Results:        --
// Exceptions:
// Description:    Outputs record data to file.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	ofstream f;
	UInt_t ts;
	TArrayF data;

	if (!this->MonitorIsOn()) {
		gMrbLog->Err()	<< "Monitor not active" << endl;
		gMrbLog->Flush(this->ClassName(), "PrintRecords");
		return;
	} else if (!this->IsReading()) {
		gMrbLog->Err()	<< "Monitor not in READ mode" << endl;
		gMrbLog->Flush(this->ClassName(), "PrintRecords");
		return;
	}
	Int_t nofRecords = this->GetNofEntries();
	if (Stop < 0) Stop = nofRecords - 1;
	if (Start < 0 || Start > Stop || Stop > nofRecords - 1) {
		gMrbLog->Err()	<< "Limits out of range (should be in [0," << nofRecords - 1 << "])" << endl;
		gMrbLog->Flush(this->ClassName(), "PrintRecords");
		return;
	}
	f.open(File, ios::out);
	if (!f.good()) {
		gMrbLog->Err()	<< gSystem->GetError() << " - " << File << endl;
		gMrbLog->Flush(this->ClassName(), "PrintRecords");
		return;
	}
	for (Int_t i = Start; i <= Stop; i++) {
		Int_t n = this->GetEntry(i, ts, data);
		this->PrintRecordData(f, i, ts, data.GetArray(), n);
	}
	f.close();
	gMrbLog->Out()	<< Stop - Start + 1 << " records written to file \"" << File << "\" (ASCII)" << endl;
	gMrbLog->Flush(this->ClassName(), "PrintRecords", setblue);
}

Bool_t TPolControl::CheckAdcChannel(Int_t Subdev, Int_t Channel, const char * Method) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TPolControl::CheckAdcChannel
// Purpose:        Check if adc channel legal
// Arguments:      Int_t Subdev          -- subdevice
//                 Int_t Channel         -- adc channel to be addressed
//                 Char_t * Method       -- calling method
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Check if adc channel is legal.
//                 ** Method is PROTECTED **
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	if (!this->CheckSubdev(Subdev, Method)) return(kFALSE);
	if (Channel < 0 || Channel >= kPolNofAdcs) {
		if (Method) {
			gMrbLog->Err()	<< "Illegal ADC channel - " << Channel
							<< " (should be in [0," << (kPolNofAdcs - 1) << "])" << endl;
			gMrbLog->Flush(this->ClassName(), Method);
		}
		return(kFALSE);
	}
	return(kTRUE);
}

Bool_t TPolControl::CheckAdcChannel(Int_t AbsChannel, const char * Method) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TPolControl::CheckAdcChannel
// Purpose:        Check if adc channel legal
// Arguments:      Int_t AbsChannel      -- adc channel to be addressed
//                 Char_t * Method       -- calling method
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Check if adc channel is legal.
//                 ** Method is PROTECTED **
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	Int_t subdev = AbsChannel / kPolNofAdcs;
	if (!this->CheckSubdev(subdev)) {
		if (Method) {
			gMrbLog->Err()	<< "Illegal ADC channel - " << AbsChannel
							<< " (Subdevice " << subdev << " not active)" << endl;
			gMrbLog->Flush(this->ClassName(), Method);
		}
		return(kFALSE);
	}
	return(kTRUE);
}

Bool_t TPolControl::CheckDacChannel(Int_t Subdev, Int_t Channel, const char * Method) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TPolControl::CheckDacChannel
// Purpose:        Check if dac channel legal
// Arguments:      Int_t Subdev          -- subdevice
//                 Int_t Channel         -- dac channel to be addressed
//                 Char_t * Method       -- calling method
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Check if dac channel is legal.
//                 ** Method is PROTECTED **
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	if (!this->CheckSubdev(Subdev, Method)) return(kFALSE);
	if (Channel < 0 || Channel >= kPolNofDacs) {
		if (Method) {
			gMrbLog->Err()	<< "Illegal DAC channel - " << Channel
							<< " (should be in [0," << (kPolNofDacs - 1) << "])" << endl;
			gMrbLog->Flush(this->ClassName(), Method);
		}
		return(kFALSE);
	}
	return(kTRUE);
}

Bool_t TPolControl::CheckDacChannel(Int_t AbsChannel, const char * Method) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TPolControl::CheckDacChannel
// Purpose:        Check if dac channel legal
// Arguments:      Int_t Channel         -- adc channel to be addressed
//                 Char_t * Method       -- calling method
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Check if dac channel is legal.
//                 ** Method is PROTECTED **
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	Int_t subdev = AbsChannel / kPolNofAdcs;
	if (!this->CheckSubdev(subdev)) {
		if (Method) {
			gMrbLog->Err()	<< "Illegal DAC channel - " << AbsChannel
							<< " (Subdevice " << subdev << " not active)" << endl;
			gMrbLog->Flush(this->ClassName(), Method);
		}
		return(kFALSE);
	}
	Int_t chn = AbsChannel % kPolNofAdcs;
	if (chn >= kPolNofDacs) {
		if (Method) {
			Int_t ch0 = subdev * kPolNofDacs;
			gMrbLog->Err()	<< "Illegal DAC channel - " << AbsChannel
							<< " (should be in [" << ch0 << "," << (ch0 + kPolNofDacs - 1) << "])" << endl;
			gMrbLog->Flush(this->ClassName(), Method);
		}
		return(kFALSE);
	}
	return(kTRUE);
}

Bool_t TPolControl::CheckDacValue(Double_t DacValue, const char * Method) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TPolControl::CheckDacValue
// Purpose:        Check if dac value legal
// Arguments:      Double_t DacValue     -- dac value
//                 Char_t * Method       -- calling method
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Check if dac value is legal.
//                 ** Method is PROTECTED **
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	if (DacValue < 0 || DacValue > (Double_t) kPolMaxVoltage) {
		if (Method) {
			gMrbLog->Err()	<< "Illegal DAC value - " << DacValue
							<< " (should be in [0.," << kPolMaxVoltage << ".0])" << endl;
			gMrbLog->Flush(this->ClassName(), Method);
		}
		return(kFALSE);
	} else {
		return(kTRUE);
	}
}

Bool_t TPolControl::CheckSubdev(Int_t Subdev, const char * Method) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TPolControl::CheckSubdev
// Purpose:        Check if subdevice legal
// Arguments:      Int_t Subdev          -- subdevice
//                 Char_t * Method       -- calling method
// Results:        kTRUE/kFALSE
// Exceptions:
// Description:    Check if subdevice is legal.
//                 ** Method is PROTECTED **
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	if ((fSubdevPattern & (0x1 << Subdev)) == 0) {
		if (Method) {
			gMrbLog->Err()	<< "Illegal subdevice - " << Subdev << endl;
			gMrbLog->Flush(this->ClassName(), Method);
		}
		return(kFALSE);
	} else {
		return(kTRUE);
	}
}

void TPolControl::PrintTable(TMrbLofNamedX * Table) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TPolControl::PrintTable
// Purpose:        Output contents of given table
// Arguments:      TMrbLofNamedX * Table   -- table addr
// Results:        --
// Exceptions:
// Description:    Outputs table contents to cout. Table address is either
//                 name table or monitor layout.
//                 ** Method is PROTECTED **
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TMrbNamedX * nx;
	Int_t chn, sd;
	TString entryName;
	Bool_t isAdc;

	nx = (TMrbNamedX *) Table->First();
	while (nx) {
		entryName = nx->GetName();
		isAdc = entryName.Index(".A", 0) != -1;
		entryName = entryName(0, entryName.Length() - 2);
		cout	<< setw(10) << setiosflags(ios::left) << entryName.Data() << setiosflags(ios::right);
		cout	<< setw(10) << setiosflags(ios::left);
		if (isAdc) cout << "A"; else cout << "D";
		cout	<< setiosflags(ios::right);
		chn = nx->GetIndex();
		cout	<< setw(6) << chn;
		sd = chn / kPolNofAdcs;
		cout	<< setw(10) << sd;
		chn = chn % kPolNofAdcs;
		cout	<< setw(10) << chn;
		cout	<< "   " << nx->GetTitle() << endl;
		nx = (TMrbNamedX *) Table->After(nx);
	}
	cout << endl;
}

void TPolControl::PrintRecordData(ostream & Out, Int_t RecordNumber, UInt_t TimeStamp,
																	Float_t * Data, Int_t NofData) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TPolControl::PrintRecordData
// Purpose:        Output record data
// Arguments:      Int_t RecordNumber   -- record number
//                 TArrayF & Data       -- data
//                 Int_t NofData        -- number of data elements
// Results:        --
// Exceptions:
// Description:    Outputs record data to ostream.
//                 ** Method is PROTECTED **
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	Int_t idx;
	TDatime * d;
	
	Out	<< RecordNumber << "-[";
	d = new TDatime(TimeStamp);
	Out	<< d->AsString() << "] ";
	delete d;
	Out	<< setprecision(4) << setiosflags(ios::fixed);
	idx = 2;
	for (Int_t i = 0; i < NofData; i++, idx++, Data++) {
		Out	<< setw(8) << *Data;
		if (((i + 1) % 10) == 0) Out << endl << "                                                    ";
	}
	Out << endl;
}

const Char_t * TPolControl::TimeStampAsString(TString & TimeString, UInt_t TimeStamp) {
//________________________________________________________________[C++ METHOD]
//////////////////////////////////////////////////////////////////////////////
// Name:           TPolControl::TimeStampAsString
// Purpose:        Convert time stamp to string
// Arguments:      UInt_t TimeStamp     -- time stamp (secs since 1.1.1970)
// Results:        Char_t * TimeString  -- ASCII representation
// Exceptions:
// Description:    Converts seconds since 1.1.1970 to ascii.
// Keywords:
//////////////////////////////////////////////////////////////////////////////

	TDatime * d;
	d = new TDatime(TimeStamp);
	TimeString = d->AsString();
	return(TimeString.Data());
}
