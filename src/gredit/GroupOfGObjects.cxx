namespace std {} using namespace std;

#include "GroupOfGObjects.h"
#include "TROOT.h"
#include "TBox.h"
#include "TPave.h"
#include "TLine.h"
#include "TCurlyLine.h"
#include "TCurlyArc.h"
#include "TArrow.h"
#include "TMarker.h"
#include "TMath.h"
#include "TText.h"
#include "TEllipse.h"
#include "TPolyLine.h"
#include "TSplineX.h"
#include "HprEditBits.h"
#include "iostream"

//________________________________________________________________

Double_t XtoNDC(TPad *pad, Double_t x)
{
	Double_t x1, x2, y1, y2;
	pad->GetRange(x1, y1, x2, y2);
	return (x - x1) / (x2 - x1);
};
Double_t XLtoNDC(TPad *pad, Double_t x)
{
	Double_t x1, x2, y1, y2;
	pad->GetRange(x1, y1, x2, y2);
	return x / (x2 - x1);
};

Double_t YtoNDC(TPad *pad,Double_t  y)
{
	Double_t x1, x2, y1, y2;
	pad->GetRange(x1, y1, x2, y2);
	return (y - y1) / (y2 - y1);
};
Double_t YLtoNDC(TPad *pad, Double_t y)
{
	Double_t x1, x2, y1, y2;
	pad->GetRange(x1, y1, x2, y2);
	return y / (y2 - y1);
};

//________________________________________________________________

ClassImp(GroupOfGObjects)
//________________________________________________________________

GroupOfGObjects::GroupOfGObjects(const char * name, Double_t x, Double_t y, TCutG* cut)
{
	fXorigin  = x;
	fYorigin  = y;
	fXLowEdge = 0;      // Bounding Box X Low Edge
	fYLowEdge = 0;      // Bounding Box Y Low Edge
	fXUpEdge  = 0;       // Bounding Box X Upper Edge
	fYUpEdge  = 0;       // Bounding Box Y Upper Edge
	fIsVisible = kTRUE;
	fForceVerticalShiftOnly = kFALSE;
	fForceHorizontalShiftOnly = kFALSE;
	SetName(name);
	if (cut) SetEnclosingCut(cut);
	gROOT->GetListOfCleanups()->Add(this);
};
//________________________________________________________________

GroupOfGObjects::~GroupOfGObjects()
{
	BindReleaseObjects(kFALSE);
	fMembers.Clear();
	gPad->GetListOfPrimitives()->Remove(this);
	gROOT->GetListOfCleanups()->Remove(this);
};

//________________________________________________________________
void GroupOfGObjects::RecursiveRemove(TObject * obj)
{
	fMembers.Remove(obj);
};
//________________________________________________________________
void GroupOfGObjects::SetPlane(Int_t plane)
{
	TIter next(&fMembers);
	TObject * obj;
	while ( (obj = next()) ) {
		HprElement* hpre = dynamic_cast<HprElement*>(obj);
		if (hpre) hpre->SetPlane(plane);
	}
};
//________________________________________________________________
void GroupOfGObjects::Print(Option_t */*chopt*/) const
{
	cout << ClassName() << " --- " << GetName() << " --- " <<  GetName() << endl;
	TIter next(&fMembers);
	TObject * obj;
	while ( (obj = next()) ) {
	  cout << obj->ClassName() << endl;
	  obj->Print();
	}
	cout << "-------------------------------------------------------- "<< endl;
};
//________________________________________________________________

void GroupOfGObjects::AddMember(TObject * obj, Option_t *option)
{
	fMembers.Add(obj, option);
	obj->SetBit(kIsBound);
};
//________________________________________________________________

Bool_t GroupOfGObjects::BindMember(TObject * obj)
{
	TObject * o;
	o = fMembers.FindObject(obj);
	if (o) {o->SetBit(kIsBound); return kTRUE;}
	else   return kFALSE;
};
//________________________________________________________________

Bool_t GroupOfGObjects::FreeMember(TObject * obj)
{
	TObject * o;
	o = fMembers.FindObject(obj);
	if (o) {o->ResetBit(kIsBound); return kTRUE;}
	else   return kFALSE;
};
//________________________________________________________________

void GroupOfGObjects::BindReleaseObjects(Bool_t bind)
{
	TObjOptLink *lnk = (TObjOptLink*)fMembers.FirstLink();
	TObject * obj;
	while ( lnk ) {
		obj = lnk->GetObject();
		if (bind) obj->SetBit(kIsBound);
		else      obj->ResetBit(kIsBound);
		lnk = (TObjOptLink*)lnk->Next();
	}
};
//________________________________________________________________

void GroupOfGObjects::ListObjects()
{
	Int_t ind = 0;
	TIter next(&fMembers);
	TObject * obj;
	while ( (obj = next()) ) {
		cout << "" << obj->ClassName()<< " * l" << ind << " = " 
		<< "(" << obj->ClassName()<< "*)" << obj << "; " << obj->GetName();
		if (obj->TestBit(kIsBound) )
			cout << " Is bound";
		cout 	<< endl; 
		ind++;
	}
}

//________________________________________________________________

void GroupOfGObjects::DeleteObjects()
{
//   TObjOptLink *lnk = (TObjOptLink*)fMembers.FirstLink();
//   TObject * obj;
//   while ( lnk ) {
//      obj = lnk->GetObject();
//     if (obj) delete obj;
//     lnk = (TObjOptLink*)lnk->Next();
//   }
	TIter next(&fMembers);
	TObject * obj;
	while ( (obj = next()) ) {
	  delete obj;
	}
	delete this;
};
//________________________________________________________________

void GroupOfGObjects::SetEnclosingCut(TCutG *cut)
{
	if (!cut) return;
	Double_t * xc = cut->GetX();
	Double_t * yc = cut->GetY();
	Set(cut->GetN());
	Double_t * x = GetX();
	Double_t * y = GetY();
	for (Int_t i = 0; i < GetN(); i++) {
		x[i] = xc[i] - fXorigin;
		y[i] = yc[i] - fYorigin;
	}
	SetBit(kIsEnclosingCut);
	SetLineWidth(1);
	SetLineStyle(3);
}
//________________________________________________________________

Int_t GroupOfGObjects::AddMembersToList(TPad * pad, Double_t xoff_c, Double_t yoff_c,
													 Double_t scaleNDC, Double_t scaleG, Double_t angle, 
													 Int_t align, Int_t mirror, Int_t draw_cut, Int_t plane)
{
////////////////////////////////////////////////////////////////////
//
//  Add members to an external list, typical a ListOfPrimitives
//  scaleNDCX, Y apply to TextSizes etc.
//  applying a scale factor (scaleG), a tranlation and rotation
//
////////////////////////////////////////////////////////////////////

	TObject * obj;
	TObject * clone;
	Double_t xt, yt;
//  calculate offset acc to alignment
	Double_t xoff = xoff_c;
	Double_t yoff = yoff_c;
	Double_t dx2 = 0.5 * (fXUpEdge - fXLowEdge);
	Double_t dy2 = 0.5 * (fYUpEdge - fYLowEdge);
	if      (align/10 == 1) xoff += scaleG * dx2;
	else if (align/10 == 3) xoff -= scaleG * dx2;
	if      (align%10 == 1) yoff += scaleG * dy2;
	else if (align%10 == 3)
		yoff -= scaleG * dy2;
	fAngle = angle;
	fAlign = align;
	fMirror = 0;
	TList * list_in_cut = 0;
	if (draw_cut != 0 && this->GetN()){
		clone = this->Clone();
		GroupOfGObjects * cut = (GroupOfGObjects*)clone;
		list_in_cut = cut->GetMemberList();
		list_in_cut->Clear();
		Double_t * x = cut->GetX();
		Double_t * y = cut->GetY();
		for (Int_t i = 0; i < cut->GetN(); i++) {
			Transform(x[i], y[i], xoff,yoff, scaleG, &xt, &yt);
			x[i] = xt;
			y[i] = yt;
		}
		cut->SetLineWidth(1);
		cut->SetLineStyle(2);
		pad->cd();
		cut->Draw("L");
	}
	fMirror = mirror;
	TObjOptLink *lnk = (TObjOptLink*)fMembers.FirstLink();
	while ( lnk ) {
		obj = lnk->GetObject();
		clone = obj->Clone();
		clone->SetBit(kIsBound);
		if (plane > 0) {
			HprElement* hpre = dynamic_cast<HprElement*>(clone);
			if (hpre) {
				hpre->SetPlane(plane);
//            cout << " SetPlane " << plane << endl;
			}
		}

		if (list_in_cut)list_in_cut->Add(clone);

		if (clone->InheritsFrom("TArrow")) {
			TArrow * b = (TArrow*)clone;
			Transform(b->GetX1(), b->GetY1(), xoff,yoff, scaleG, &xt, &yt);
			b->SetX1(xt);
			b->SetY1(yt);
			Transform(b->GetX2(), b->GetY2(), xoff,yoff, scaleG, &xt, &yt);
			b->SetX2(xt);
			b->SetY2(yt);
			b->SetArrowSize(b->GetArrowSize() * scaleNDC);

		} else if (clone->InheritsFrom("TCurlyArc")) {
			TCurlyArc* b = (TCurlyArc*)clone;
			Transform(b->GetStartX(), b->GetStartY(), xoff,yoff, scaleG, &xt, &yt);
			b->SetCenter(xt, yt);
			b->SetRadius(scaleG * b->GetRadius());
			Double_t phi = b->GetPhimin() + angle;
			if (phi > 360) phi -= 360;
			b->SetPhimin(phi);
			phi = b->GetPhimax() + angle;
			if (phi > 360) phi -= 360;
			b->SetPhimax(phi);
			b->Paint(b->GetDrawOption());

		} else if (clone->InheritsFrom("TCurlyLine")) {
			TCurlyLine * b = (TCurlyLine*)clone;
			Transform(b->GetStartX(), b->GetStartY(), xoff,yoff, scaleG, &xt, &yt);
			b->SetStartPoint(xt, yt);
			Transform(b->GetEndX(), b->GetEndY(), xoff,yoff, scaleG, &xt, &yt);
			b->SetEndPoint(xt, yt);
			b->SetWaveLength(b->GetWaveLength() * scaleNDC);
			b->SetAmplitude(b->GetAmplitude() * scaleNDC);
			b->Paint(b->GetDrawOption());

		} else if (clone->InheritsFrom("TMarker")) {
			TMarker * b = (TMarker*)clone;
			Transform(b->GetX(), b->GetY(), xoff,yoff, scaleG, &xt, &yt);
			b->SetX(xt);
			b->SetY(yt);
			b->SetMarkerSize(b->GetMarkerSize() * scaleNDC);

		} else if (clone->InheritsFrom("TText")) {
			TText * b = (TText*)clone;
			Transform(b->GetX(), b->GetY(), xoff,yoff, scaleG, &xt, &yt);
			b->SetX(xt);
			b->SetY(yt);
			b->SetTextSize(b->GetTextSize() * scaleNDC);

		} else if (clone->InheritsFrom("TEllipse")) {
			TEllipse * b = (TEllipse*)clone;
			Transform(b->GetX1(), b->GetY1(), xoff,yoff, scaleG, &xt, &yt);
			b->SetR1(scaleG * b->GetR1());
			b->SetR2(scaleG * b->GetR2());
			if (b->GetPhimin() != 0 || b->GetPhimax() != 360) {
				Double_t phi = b->GetPhimin() + angle;
				if (phi > 360) phi -= 360;
				b->SetPhimin(phi);
				phi = b->GetPhimax() + angle;
				if (phi > 360) phi -= 360;
				b->SetPhimax(phi);
			}
			b->SetX1(xt);
			b->SetY1(yt);

		} else if (clone->InheritsFrom("TSplineX")) {
			TSplineX* b = ( TSplineX*)clone;
			TGraph* gr = (TGraph*)b->GetControlGraph();
			Double_t * x = gr->GetX();
			Double_t * y = gr->GetY();
//         either first or last point
			b->GetControlGraph()->SetParent(b);
			Double_t* xtt = new Double_t[gr->GetN()];
			Double_t* ytt = new Double_t[gr->GetN()];
			for (Int_t i = 0; i < gr->GetN(); i++) {
				Transform(x[i], y[i], xoff,yoff, scaleG, &xtt[i], &ytt[i]);
 //           xt[i] = x[i] + xoff;
 //          yt[i] = y[i] + yoff;
			}
			b->SetAllControlPoints(gr->GetN(), xtt, ytt);
			delete [] xtt;
			delete [] ytt;

		} else if (clone->InheritsFrom("TGraph")) {
			TGraph * b = (TGraph *)clone;
			Double_t * x = b->GetX();
			Double_t * y = b->GetY();
			for (Int_t i = 0; i < b->GetN(); i++) {
				Transform(x[i], y[i], xoff,yoff, scaleG, &xt, &yt);
				x[i] = xt;
				y[i] = yt;
			}

		} else if (clone->InheritsFrom("TPad")) {
			TPad * b = (TPad*)clone;
			Double_t x1 = b->GetAbsXlowNDC();
			Double_t y1 = b->GetAbsYlowNDC();
			Double_t x2 = x1 + b->GetAbsWNDC();
			Double_t y2 = y1 + b->GetAbsHNDC();
			b->GetListOfPrimitives()->ls();
			Double_t xt2, yt2;
//         convert to user
			x1 = x1 * (fXUpEdge - fXLowEdge);
			y1 = y1 * (fYUpEdge - fYLowEdge);
			x2 = x2 * (fXUpEdge - fXLowEdge);
			y2 = y2 * (fYUpEdge - fYLowEdge);

			Transform(x1, y1, xoff - scaleG * dx2,yoff - scaleG * dy2, scaleG, &xt, &yt);
			Transform(x2, y2, xoff - scaleG * dx2,yoff - scaleG * dy2, scaleG, &xt2, &yt2);

			x1 = (xt - pad->GetX1()) / (pad->GetX2() - pad->GetX1());
			y1 = (yt - pad->GetY1()) / (pad->GetY2() - pad->GetY1());
			x2 = (xt2 - pad->GetX1()) / (pad->GetX2() - pad->GetX1());
			y2 = (yt2 - pad->GetY1()) / (pad->GetY2() - pad->GetY1());
			b->SetPad(x1, y1, x2, y2);
			if (!pad->InheritsFrom("TCanvas")) {
				pad->cd();
				b->Draw();
			}

		} else if (clone->InheritsFrom("TPave")) {
			TPave * b = (TPave*)clone;
			Double_t x1 = b->GetX1NDC();
			Double_t y1 = b->GetY1NDC();
			Double_t x2 = b->GetX2NDC();
			Double_t y2 = b->GetY2NDC();
			Double_t xt2, yt2;
//         convert to user
			x1 = x1 * (fXUpEdge - fXLowEdge);
			y1 = y1 * (fYUpEdge - fYLowEdge);
			x2 = x2 * (fXUpEdge - fXLowEdge);
			y2 = y2 * (fYUpEdge - fYLowEdge);
			cout << "ent TPave: x1, x2: " << x1 << " " <<x2 << endl;
			Transform(x1, y1, xoff - scaleG * dx2,yoff - scaleG * dy2, scaleG, &xt, &yt);
			Transform(x2, y2, xoff - scaleG * dx2,yoff - scaleG * dy2, scaleG, &xt2, &yt2);
			cout << "exi TPave: x1, x2: " << x1 << " " <<x2 << endl;
			if ( fMirror%10 == 1) {
				xt += 2 * dx2;
				xt2 += 2 * dx2;
			}
			x1 = (xt - pad->GetX1()) / (pad->GetX2() - pad->GetX1());
			y1 = (yt - pad->GetY1()) / (pad->GetY2() - pad->GetY1());
			x2 = (xt2 - pad->GetX1()) / (pad->GetX2() - pad->GetX1());
			y2 = (yt2 - pad->GetY1()) / (pad->GetY2() - pad->GetY1());
			b->SetX1NDC(x1);
			b->SetY1NDC(y1);
			b->SetX2NDC(x2);
			b->SetY2NDC(y2);

		} else if (clone->InheritsFrom("TBox")) {
			TBox * b = (TBox*)clone;
			Transform(b->GetX1(), b->GetY1(), xoff,yoff, scaleG, &xt, &yt);
			b->SetX1(xt);
			b->SetY1(yt);
			Transform(b->GetX2(), b->GetY2(), xoff,yoff, scaleG, &xt, &yt);
			b->SetX2(xt);
			b->SetY2(yt);

		} else if (clone->InheritsFrom("TLine")){
			TLine * b = (TLine*)clone;
			cout << "ent TLine: x1, x2: " << b->GetX1()<< " " <<b->GetX2()<< endl;
			Transform(b->GetX1(), b->GetY1(), xoff,yoff, scaleG, &xt, &yt);
			b->SetX1(xt);
			b->SetY1(yt);
			Transform(b->GetX2(), b->GetY2(), xoff,yoff, scaleG, &xt, &yt);
			b->SetX2(xt);
			b->SetY2(yt);
			cout << "exi TLine: x1, x2: " << b->GetX1()<< " " <<b->GetX2()<< endl;

		} else {
//         cout << clone->ClassName() << " not yet implemented" << endl;
			clone = 0;
		}
		if (clone) {
			if (!clone->InheritsFrom("TPad") || pad->InheritsFrom("TCanvas")) {
				pad->GetListOfPrimitives()->Add(clone, lnk->GetOption());
				if (clone->InheritsFrom("TSplineX")) {
					TSplineX* b = ( TSplineX*)clone;
					b->NeedReCompute();
					b->Paint();
				}
			}
		}
		lnk = (TObjOptLink*)lnk->Next();
	}
	return 1;
};
//____________________________________________________________________________

void  GroupOfGObjects::Transform(Double_t x, Double_t y, Double_t xoff, Double_t yoff,
					Double_t scaleG, Double_t* xt, Double_t* yt)
{
	Double_t xx = scaleG * x;
	Double_t yy = scaleG * y;
	*xt = xx;
	*yt = yy;
	if ( fMirror/10 == 1 ) {
//		Double_t ym = 0.5 * (fYUpEdge + fYLowEdge);
//		Double_t dy = ym - *yt;
		*yt = -*yt;
	}
	if ( fMirror%10 == 1 ) {
//		Double_t xm = 0.5 * (fXUpEdge + fXLowEdge);
//		Double_t dx = xm - *xt;
		*xt = -*xt;
	}
	if (fAngle != 0) {
		Double_t ar = fAngle * TMath::Pi() / 180.;
		*xt = xx * TMath::Cos(ar) - yy * TMath::Sin(ar);
		*yt = xx * TMath::Sin(ar) + yy * TMath::Cos(ar);
	}
	*xt += xoff;
	*yt += yoff;
}
//____________________________________________________________________________

void  GroupOfGObjects::ExecuteEvent(Int_t event, Int_t px, Int_t py)
{
	static Double_t xEnclosingCut = 0, yEnclosingCut = 0;

	TCutG::ExecuteEvent(event, px, py);
	switch (event) {

	case kButton1Down:
		xEnclosingCut = GetX()[0];
		yEnclosingCut = GetY()[0];
		break;
	case kButton1Up:
		{
		Double_t xshift = GetX()[0] -  xEnclosingCut;
		Double_t yshift = GetY()[0] -  yEnclosingCut;
		if (fForceVerticalShiftOnly)    xshift = 0;
		if (fForceHorizontalShiftOnly) yshift = 0;
		ShiftObjects(xshift, yshift, kFALSE);
		}
		break;
	default:
		break;
	}
}
//____________________________________________________________________________

void GroupOfGObjects::ShiftObjects(Double_t xoff, Double_t yoff, Bool_t shiftcut)
{
	TObject * obj;
	TIter next(GetMemberList());
	while ( (obj = next()) ) {
		if (obj->InheritsFrom("EditMarker")) continue;
		if (obj->InheritsFrom("TPave")) {
			TPave * b = (TPave*)obj;
			Double_t xoffNDC = xoff / (gPad->GetX2() - gPad->GetX1());
			Double_t yoffNDC = yoff / (gPad->GetY2() - gPad->GetY1());
			b->SetX1NDC(b->GetX1NDC() + xoffNDC);
			b->SetY1NDC(b->GetY1NDC() + yoffNDC);
			b->SetX2NDC(b->GetX2NDC() + xoffNDC);
			b->SetY2NDC(b->GetY2NDC() + yoffNDC);

		} else if (obj->InheritsFrom("TBox")) {
			TBox * b = (TBox*)obj;
			b->SetX1(b->GetX1() + xoff);
			b->SetX2(b->GetX2() + xoff);
			b->SetY1(b->GetY1() + yoff);
			b->SetY2(b->GetY2() + yoff);

		} else if (obj->InheritsFrom("TPad")){
			TPad * b = (TPad*)obj;
			Double_t x1, y1;
			Double_t xoffNDC = xoff / (gPad->GetX2() - gPad->GetX1());
			Double_t yoffNDC = yoff / (gPad->GetY2() - gPad->GetY1());
			x1 = b->GetAbsXlowNDC() + xoffNDC;
			y1 = b->GetAbsYlowNDC() + yoffNDC;
			b->SetPad(x1, y1, x1 + b->GetWNDC(), y1 + b->GetHNDC());

		} else if (obj->InheritsFrom("TLine")){
			TLine * b = (TLine*)obj;
			b->SetX1(b->GetX1() + xoff);
			b->SetX2(b->GetX2() + xoff);
			b->SetY1(b->GetY1() + yoff);
			b->SetY2(b->GetY2() + yoff);

		} else if (obj->InheritsFrom("TArrow")) {
			TArrow * b = (TArrow*)obj;
			b->SetX1(b->GetX1() + xoff);
			b->SetX2(b->GetX2() + xoff);
			b->SetY1(b->GetY1() + yoff);
			b->SetY2(b->GetY2() + yoff);

		} else if (obj->InheritsFrom("TCurlyArc")) {
			TCurlyArc * b = (TCurlyArc*)obj;
			b->SetStartPoint(b->GetStartX() + xoff, b->GetStartY() + yoff);

		} else if (obj->InheritsFrom("TCurlyLine")) {
			TCurlyLine * b = (TCurlyLine*)obj;
			b->SetStartPoint(b->GetStartX() + xoff, b->GetStartY() + yoff);
			b->SetEndPoint(b->GetEndX() + xoff, b->GetEndY() + yoff);

		} else if (obj->InheritsFrom("TMarker")) {
		TMarker * b = (TMarker*)obj;
			b->SetX(b->GetX() + xoff);
			b->SetY(b->GetY() + yoff);

		} else if (obj->InheritsFrom("TText")) {
		TText * b = (TText*)obj;
			b->SetX(b->GetX() + xoff);
			b->SetY(b->GetY() + yoff);

		} else if (obj->InheritsFrom("TEllipse")) {
			TEllipse * b = (TEllipse*)obj;
			b->SetX1(b->GetX1() + xoff);
			b->SetY1(b->GetY1() + yoff);

		} else if (obj->InheritsFrom("TSplineX")) {
			TSplineX* b = ( TSplineX*)obj;
			ControlGraph* gr = b->GetControlGraph();
			Double_t * x = gr->GetX();
			Double_t * y = gr->GetY();
			Double_t* xt = new Double_t[gr->GetN()];
			Double_t* yt = new Double_t[gr->GetN()];
			for (Int_t i = 0; i < gr->GetN(); i++) {
				xt[i] = x[i] + xoff;
				yt[i] = y[i] + yoff;
			}
			b->SetAllControlPoints(gr->GetN(), xt, yt);
			b->NeedReCompute();
			b->Paint();
			delete [] xt;
			delete [] yt;

		} else if (obj->InheritsFrom("TGraph")
					&& strncmp(obj->GetName(), "ParellelG", 9)) {
//       Parallel graphs are handled by its TSplineX
				cout << "Shift TGraph: " << obj->GetName() << endl;

			TGraph * b = (TGraph *)obj;
			Double_t * x = b->GetX();
			if (!x) {
				cout << "Shift TGraph with 0 points" << endl;
				continue;
			}
			Double_t * y = b->GetY();
//         either first or last point
			for (Int_t i = 0; i < b->GetN(); i++) {
//            if (IsInside(x[i],y[i])) {
//					cout << "Shift TGraph, i, x[i], y[i] " 
//					<< i << " " << x[i]<< " " <<y[i]<< endl;
					x[i] += xoff;
					y[i] += yoff;
//            }
			}
		} else {
			if ( gDebug > 0 )
				cout << obj->ClassName() << " not yet implemented" << endl;
		}
		if (shiftcut) {
			Double_t * x = GetX();
			Double_t * y = GetY();
			for (Int_t i = 0; i < GetN(); i++) {
				x[i] += xoff;
				y[i] += yoff;
			}
		}
	}
	gPad->Modified();
}
//______________________________________________________________________________
Bool_t GroupOfGObjects::SloppyInside(Double_t x, Double_t y)
{
//   cut->Dump();
	Double_t m = 0.1;
	if (IsInside(x,y)) return kTRUE;
	if (IsInside(x + m,y + m)) return kTRUE;
	if (IsInside(x - m,y - m)) return kTRUE;
	if (IsInside(x + m,y - m)) return kTRUE;
	if (IsInside(x - m,y + m)) return kTRUE;
	return kFALSE;
}
