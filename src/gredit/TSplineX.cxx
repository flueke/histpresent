
/*

 * FIG : Facility for Interactive Generation of figures
 * Copyright (c) 1985-1988 by Supoj Sutanthavibul
 * Parts Copyright (c) 1989-2002 by Brian V. Smith
 * Parts Copyright (c) 1991 by Paul King
 *
 * Any party obtaining a copy of these files is granted, free of charge, a
 * full and unrestricted irrevocable, world-wide, paid up, royalty-free,
 * nonexclusive right and license to deal in this software and
 * documentation files (the "Software"), including without limitation the
 * rights to use, copy, modify, merge, publish and/or distribute copies of
 * the Software, and to permit persons who receive copies from any such
 * party to do so, with the only requirement being that this copyright
 * notice remain intact.
 *
 */

/********************* CURVES FOR SPLINES *****************************
 *                                                                    *
 * The following spline drawing routines are from							 *
 * 																						 *
 *    "X-splines : A Spline Model Designed for the End User"			 *
 * 																						 *
 *    by Carole BLANC and Christophe SCHLICK,								 *
 *    in Proceedings of SIGGRAPH ' 95											 *
 * 																						 *
 * 																						 *
 *    The code to calculate the xspline itself is taken from xfig 	 *
 *   (see Copyright above) 														 *
 *    Mods by Otto Schaile:														 *
 *    Change to c++ calling conventions										 *
 *    Wrap into a ROOT class														 *
 * 																						 *
 **********************************************************************/

#include "Riostream.h"
#include "TObjString.h"
#include "TOrdCollection.h"
#include "TPolyLine.h"
#include "TSplineX.h"
#include "TROOT.h"
#include "TMath.h"
#include "THprLatex.h"
#include "TGMrbSliders.h"


//___________________________________________________________________________/
//                                                                           //
//                     The T S P L I N E X  Class									  //
//                     ==========================									  //
// TSplineX is an implemention of xsplines as described in detail in:		  //
// "X-splines : A Spline Model Designed for the End User"						  //
// by Carole BLANC and Christophe SCHLICK in Proceedings of SIGGRAPH'95 	  //
//																									  //
// A Xspline is defined by the coordinates and a shapefactor of its  		  //
// controlpoints. The shapefactor takes values between -1 and + 1;			  //
// With values < 0 the curve is forced through the controlpoints, 			  //
// with vulues > 0 the curve is an approximation only. The absolute  		  //
// value may be regarded as a string force, with 0 maximum strength			  //
// producing a sharp kink of the resulting at the controlpoint.				  //
// An important property is that position and shapefactor of a 				  //
// controlpoint influences the curve only between this point and  			  //
// its immediate neighbours.																  //
// Xsplines may be open or closed, the number of points calculated			  //
// for the resulting smooth curve is controlled by a precission				  //
// parameter.																					  //
// TSplineX provides methods to construct a xspline and to modify 			  //
// it with a GUI. In addition it provides construction of parallel			  //
// lines and arrows at either end. The area between parallel lines 			  //
// may be filled (see TAttFill). The special case "railwaylike" allows  	  //
// filling with a a sleeper pattern as used in maps for railway tracks.      //
//  																								  //
// TSplineX is accompanied with GUI class TSplineXDialog to ease access 	  //
// to the class methods.																	  //
// 																								  //
// A TSplineXEditor class allows interactive manipulation of the xspline.	  //
//  																								  //
//___________________________________________________________________________//
namespace std {} using namespace std;

#define         HIGH_PRECISION    0.5
#define         LOW_PRECISION     1.0
#define         ZOOM_PRECISION    5.0
#define         ARROW_START       4
#define         MAX_SPLINE_STEP   0.2

/***********************************************************************/

#define Q(s)  (-(s))
#define EQN_NUMERATOR(dim) \
  (A_blend[0]*p0->dim+A_blend[1]*p1->dim+A_blend[2]*p2->dim+A_blend[3]*p3->dim)


ClassImp(TSplineX)
ClassImp(ControlGraph)
ClassImp(ParallelGraph)
ClassImp(PolyLineNoEdit)
ClassImp(RailwaySleeper)

//_____________________________________________________________________

Double_t TSplineX::f_blend(Double_t numerator, Double_t denominator)
{
  Double_t p = 2 * denominator * denominator;
  Double_t u = numerator / denominator;
  Double_t u2 = u * u;

  return (u * u2 * (10 - p + (2*p - 15)*u + (6 - p)*u2));
}
//____________________________________________________________________________________

Double_t TSplineX::g_blend(Double_t u, Double_t q)             /* p equals 2 */
{
  return(u*(q + u*(2*q + u*(8 - 12*q + u*(14*q - 11 + u*(4 - 5*q))))));
}
//____________________________________________________________________________________

Double_t TSplineX::h_blend(Double_t u, Double_t q)
{
  Double_t u2=u*u;
   return (u * (q + u * (2 * q + u2 * (-2*q - u*q))));
}
//____________________________________________________________________________________

void TSplineX::negative_s1_influence(Double_t t, Double_t s1, Double_t *A0, Double_t *A2)
{
  *A0 = h_blend(-t, Q(s1));
  *A2 = g_blend(t, Q(s1));
}

void TSplineX::negative_s2_influence(Double_t t, Double_t s2, Double_t *A1, Double_t *A3)
{
  *A1 = g_blend(1-t, Q(s2));
  *A3 = h_blend(t-1, Q(s2));
}
//____________________________________________________________________________________

void TSplineX::positive_s1_influence(Int_t k, Double_t t, Double_t s1, Double_t *A0, Double_t *A2)
{
  Double_t Tk;

  Tk = k+1+s1;
  *A0 = (t+k+1<Tk) ? f_blend(t+k+1-Tk, k-Tk) : 0.0;

  Tk = k+1-s1;
  *A2 = f_blend(t+k+1-Tk, k+2-Tk);
}
//____________________________________________________________________________________

void TSplineX::positive_s2_influence(Int_t k, Double_t t, Double_t s2, Double_t *A1, Double_t *A3)
{
  Double_t Tk;

  Tk = k+2+s2;
  *A1 = f_blend(t+k+1-Tk, k+1-Tk);

  Tk = k+2-s2;
  *A3 = (t+k+1>Tk) ? f_blend(t+k+1-Tk, k+3-Tk) : 0.0;
}

//____________________________________________________________________________________

void TSplineX::point_adding(Double_t *A_blend, ControlPoint *p0, ControlPoint *p1, ControlPoint *p2, ControlPoint *p3)
{
  Double_t weights_sum;

  weights_sum = A_blend[0] + A_blend[1] + A_blend[2] + A_blend[3];
  if (!add_point(EQN_NUMERATOR(x) / (weights_sum),
		 EQN_NUMERATOR(y) / (weights_sum)))
      too_many_points();
}
//____________________________________________________________________________________

void TSplineX::point_computing(Double_t *A_blend, ControlPoint *p0, ControlPoint *p1, ControlPoint *p2, ControlPoint *p3, Double_t *xs, Double_t *ys)
{
  Double_t weights_sum;

  weights_sum = A_blend[0] + A_blend[1] + A_blend[2] + A_blend[3];

  *xs = (int)TMath::Nint(EQN_NUMERATOR(x) / (weights_sum));
  *ys = (int)TMath::Nint(EQN_NUMERATOR(y) / (weights_sum));
}
//____________________________________________________________________________________

Float_t TSplineX::step_computing(Int_t k, ControlPoint *p0, ControlPoint *p1, ControlPoint *p2, ControlPoint *p3, Double_t s1, Double_t s2, Float_t precision)
{
  Double_t A_blend[4];
  Double_t    xstart, ystart, xend, yend, xmid, ymid, xlength, ylength;
  Double_t    start_to_end_dist;
  Double_t    number_of_steps;
  Float_t  step, angle_cos, scal_prod, xv1, xv2, yv1, yv2, sides_length_prod;

  /* This function computes the step used to draw the segment (p1, p2)
     (xv1, yv1) : coordinates of the vector from middle to origin
     (xv2, yv2) : coordinates of the vector from middle to extremity */

  if ((s1 == 0) && (s2 == 0))
    return(1.0);              /* only one step in case of linear segment */

  /* compute coordinates of the origin */
  if (s1>0) {
     if (s2<0) {
	     positive_s1_influence(k, 0.0, s1, &A_blend[0], &A_blend[2]);
	     negative_s2_influence(0.0, s2, &A_blend[1], &A_blend[3]);
     } else {
	     positive_s1_influence(k, 0.0, s1, &A_blend[0], &A_blend[2]);
	     positive_s2_influence(k, 0.0, s2, &A_blend[1], &A_blend[3]);
     }
     point_computing(A_blend, p0, p1, p2, p3, &xstart, &ystart);
  } else {
      xstart = p1->x;
      ystart = p1->y;
  }

  /* compute coordinates  of the extremity */
  if (s2>0) {
     if (s1<0) {
	     negative_s1_influence(1.0, s1, &A_blend[0], &A_blend[2]);
	     positive_s2_influence(k, 1.0, s2, &A_blend[1], &A_blend[3]);
     } else {
	     positive_s1_influence(k, 1.0, s1, &A_blend[0], &A_blend[2]);
	     positive_s2_influence(k, 1.0, s2, &A_blend[1], &A_blend[3]);
     }
     point_computing(A_blend, p0, p1, p2, p3, &xend, &yend);
  } else {
      xend = p2->x;
      yend = p2->y;
  }

  /* compute coordinates  of the middle */
  if (s2>0) {
     if (s1<0) {
	     negative_s1_influence(0.5, s1, &A_blend[0], &A_blend[2]);
	     positive_s2_influence(k, 0.5, s2, &A_blend[1], &A_blend[3]);
     } else {
	     positive_s1_influence(k, 0.5, s1, &A_blend[0], &A_blend[2]);
	     positive_s2_influence(k, 0.5, s2, &A_blend[1], &A_blend[3]);
	  }
  } else if (s1<0) {
      negative_s1_influence(0.5, s1, &A_blend[0], &A_blend[2]);
      negative_s2_influence(0.5, s2, &A_blend[1], &A_blend[3]);
  } else {
      positive_s1_influence(k, 0.5, s1, &A_blend[0], &A_blend[2]);
      negative_s2_influence(0.5, s2, &A_blend[1], &A_blend[3]);
  }
  point_computing(A_blend, p0, p1, p2, p3, &xmid, &ymid);

  xv1 = xstart - xmid;
  yv1 = ystart - ymid;
  xv2 = xend - xmid;
  yv2 = yend - ymid;

  scal_prod = xv1*xv2 + yv1*yv2;

  sides_length_prod = sqrt((xv1*xv1 + yv1*yv1)*(xv2*xv2 + yv2*yv2));

  /* compute cosinus of origin-middle-extremity angle, which approximates the
     curve of the spline segment */
  if (sides_length_prod == 0.0)
    angle_cos = 0.0;
  else
    angle_cos = scal_prod/sides_length_prod;

  xlength = xend - xstart;
  ylength = yend - ystart;

  start_to_end_dist = sqrt(xlength*xlength + ylength*ylength);

  /* more steps if segment's origin and extremity are remote */
  number_of_steps = sqrt(start_to_end_dist)/2;

  /* more steps if the curve is high */
  number_of_steps += (int)((1 + angle_cos)*10);

  if (number_of_steps == 0)
    step = 1;
  else
    step = precision/number_of_steps;

  if ((step > MAX_SPLINE_STEP) || (step == 0))
    step = MAX_SPLINE_STEP;
//  cout << "precision,number_of_steps,step " <<
//         precision << " " <<number_of_steps << " " <<step << endl;
  return (step);
}
//____________________________________________________________________________________

void TSplineX::spline_segment_computing(Float_t step, Int_t k, ControlPoint *p0, ControlPoint *p1, ControlPoint *p2, ControlPoint *p3, Double_t s1, Double_t s2)
{
  Double_t A_blend[4];
  Double_t t;

  if (s1<0) {
     if (s2<0) {
	 for (t=0.0 ; t<1 ; t+=step) {
	     negative_s1_influence(t, s1, &A_blend[0], &A_blend[2]);
	     negative_s2_influence(t, s2, &A_blend[1], &A_blend[3]);

	     point_adding(A_blend, p0, p1, p2, p3);
	 }
     } else {
	 for (t=0.0 ; t<1 ; t+=step) {
	     negative_s1_influence(t, s1, &A_blend[0], &A_blend[2]);
	     positive_s2_influence(k, t, s2, &A_blend[1], &A_blend[3]);

	     point_adding(A_blend, p0, p1, p2, p3);
	 }
     }
  } else if (s2<0) {
      for (t=0.0 ; t<1 ; t+=step) {
	     positive_s1_influence(k, t, s1, &A_blend[0], &A_blend[2]);
	     negative_s2_influence(t, s2, &A_blend[1], &A_blend[3]);

	     point_adding(A_blend, p0, p1, p2, p3);
      }
  } else {
      for (t=0.0 ; t<1 ; t+=step) {
	     positive_s1_influence(k, t, s1, &A_blend[0], &A_blend[2]);
	     positive_s2_influence(k, t, s2, &A_blend[1], &A_blend[3]);

	     point_adding(A_blend, p0, p1, p2, p3);
      }
  }
}

/********************* MAIN METHODS *************************************/
//
//  Code taken from xfig (see Copyright above)
//  Mods by OS
//  Change to c++ calling conventions
//  Input points and sfactors directly (was via struct F_spline)
#define COPY_CONTROL_POINT(P0, S0, P1, S1) \
      P0 = P1; \
      S0 = S1

#define NEXT_CONTROL_POINTS(P0, S0, P1, S1, P2, S2, P3, S3) \
      COPY_CONTROL_POINT(P0, S0, P1, S1); \
      COPY_CONTROL_POINT(P1, S1, P2, S2); \
      COPY_CONTROL_POINT(P2, S2, P3, S3); \
      COPY_CONTROL_POINT(P3, S3, P3->next, S3->next)

#define INIT_CONTROL_POINTS(POINTS, FACTORS, P0, S0, P1, S1, P2, S2, P3, S3) \
      COPY_CONTROL_POINT(P0, S0, POINTS, FACTORS); \
      COPY_CONTROL_POINT(P1, S1, P0->next, S0->next);               \
      COPY_CONTROL_POINT(P2, S2, P1->next, S1->next);               \
      COPY_CONTROL_POINT(P3, S3, P2->next, S2->next)

#define SPLINE_SEGMENT_LOOP(K, P0, P1, P2, P3, S1, S2, PREC) \
      step = step_computing(K, P0, P1, P2, P3, S1, S2, PREC);    \
      spline_segment_computing(step, K, P0, P1, P2, P3, S1, S2)

//____________________________________________________________________________________

Int_t TSplineX::op_spline(ControlPoint *cpoints, ShapeFactor *sfactors, Float_t precision)
{
  Int_t       k;
  Float_t     step;
  ControlPoint   *p0, *p1, *p2, *p3;
  ShapeFactor *s0, *s1, *s2, *s3;

  fNpoints = 0;

  COPY_CONTROL_POINT(p0, s0, cpoints, sfactors);
  COPY_CONTROL_POINT(p1, s1, p0, s0);
  /* first control point is needed twice for the first segment */
  COPY_CONTROL_POINT(p2, s2, p1->next, s1->next);

  if (p2->next == 0) {
      COPY_CONTROL_POINT(p3, s3, p2, s2);
  } else {
      COPY_CONTROL_POINT(p3, s3, p2->next, s2->next);
  }


  for (k = 0 ;  ; k++) {
      SPLINE_SEGMENT_LOOP(k, p0, p1, p2, p3, s1->s, s2->s, precision);
//      if (DONE)
//        break;
      if (p3->next == 0)
	break;
      NEXT_CONTROL_POINTS(p0, s0, p1, s1, p2, s2, p3, s3);
  }
  /* last control point is needed twice for the last segment */
//  if (!DONE) {
    COPY_CONTROL_POINT(p0, s0, p1, s1);
    COPY_CONTROL_POINT(p1, s1, p2, s2);
    COPY_CONTROL_POINT(p2, s2, p3, s3);
    SPLINE_SEGMENT_LOOP(k, p0, p1, p2, p3, s1->s, s2->s, precision);
//  }

  if (!add_point(p3->x, p3->y))
    too_many_points();

  return 1 ;
}
//____________________________________________________________________________________

Int_t TSplineX::cl_spline(ControlPoint *cpoints, ShapeFactor *sfactors, Float_t precision)
{
  Int_t i, k;
//  int npoints = fNofControlPoints;
  Float_t     step;
//  Double_t    t;
  ControlPoint   *p0, *p1, *p2, *p3, *first;
  ShapeFactor *s0, *s1, *s2, *s3, *s_first;

  fNpoints = 0;

  INIT_CONTROL_POINTS(cpoints, sfactors, p0, s0, p1, s1, p2, s2, p3, s3);
  COPY_CONTROL_POINT(first, s_first, p0, s0);

  for (k = 0 ; p3 != 0 ; k++) {
      SPLINE_SEGMENT_LOOP(k, p0, p1, p2, p3, s1->s, s2->s, precision);
      NEXT_CONTROL_POINTS(p0, s0, p1, s1, p2, s2, p3, s3);
//      if (DONE)
//        break;
  }
  /* when we are at the end, join to the beginning */
    COPY_CONTROL_POINT(p3, s3, first, s_first);
    SPLINE_SEGMENT_LOOP(k, p0, p1, p2, p3, s1->s, s2->s, precision);

    for (i=0; i<2; i++) {
//        if (DONE)
//          break;
        k++;
        NEXT_CONTROL_POINTS(p0, s0, p1, s1, p2, s2, p3, s3);
        SPLINE_SEGMENT_LOOP(k, p0, p1, p2, p3, s1->s, s2->s, precision);
    }

  if (!add_closepoint())
    too_many_points();

  return 1 ;
}
//____________________________________________________________________________________
//
// some useful auxiliaries
//
Double_t TSplineX::Length(Double_t x1, Double_t y1, Double_t x2, Double_t y2)
{
   return TMath::Sqrt((x2 - x1)*(x2 - x1) + (y2 - y1)*(y2 - y1));
}
//_______________________________________________________________________________________

Double_t TSplineX::PhiOfLine(Double_t x1, Double_t y1, Double_t x2, Double_t y2)
{
   Double_t len = Length(x1, y1, x2, y2);
   Double_t phi = 0;
   if (len > 0) {
      Double_t sina = (y2 - y1) / len / fRatioXY ;
      Double_t cosa = (x2 - x1) / len;
      phi = TMath::ATan2(sina, cosa);
      if (phi < 0) phi += 2. * TMath::Pi();
   }
   return phi;
}
//_______________________________________________________________________________________

void TSplineX::Nextpoint(Double_t phi, Double_t x, Double_t y,
              Double_t dist, Double_t* a,  Double_t* b)
{
   Double_t phip = phi;
   if (phip < 0) phip += 2 * TMath::Pi();
   *a = x + dist * TMath::Cos(phip);
   *b = y + 1 * dist * TMath::Sin(phip);
 //  *b = y + fRatioXY * dist * TMath::Sin(phip);
}
//_______________________________________________________________________________________

void TSplineX::Endpoint(Double_t phi, Double_t x, Double_t y,
              Double_t dist, Double_t* a,  Double_t* b)
{
   Double_t phip = phi - 0.5 * TMath::Pi();
   if (phip < 0) phip += 2 * TMath::Pi();
   *a = x + dist * TMath::Cos(phip);
   *b = y + fRatioXY * dist * TMath::Sin(phip);
}
//_______________________________________________________________________________________

void TSplineX::Midpoint(Double_t phi1, Double_t phi2, Double_t x, Double_t y,
              Double_t dist ,Double_t* a, Double_t* b)
{
   Double_t r = dist / TMath::Cos(0.5 * (phi1 - phi2));
   Double_t phi3 = 0.5 * (phi1 + phi2 - TMath::Pi());
   if (phi3 < 0) phi3 += 2 * TMath::Pi();
   *a = x + r * TMath::Cos(phi3);
   *b = y + fRatioXY * r * TMath::Sin(phi3);
}
//_____________________________________________________________________________________

Double_t TSplineX::GetArrowLength(Double_t dist, Double_t alength,Double_t aangle,Double_t aindent_angle)
{
   Double_t deg2rad = TMath::Pi() / 180;
   Double_t p2 = 0.5 * aangle * deg2rad;
   Double_t a2 = aindent_angle *deg2rad;
   Double_t ay = alength * TMath::Tan(p2);
   Double_t dabs = TMath::Abs(dist);
//   cout << ay << " " << dabs << endl;
   Double_t reallength = alength  - ay * TMath::Tan(a2)
           * (1 - dabs / ay);
  return reallength;
}
//____________________________________________________________________________________

TSplineX::TSplineX():
     fShapeFactorList ( NULL) 
   , fControlPointList ( NULL)
   , fNofControlPoints( 0)
   , fNpoints( 0)         
   , fComputeDone ( kFALSE)     
   , fX( 0)               
   , fY( 0)               
   , fCPDrawn ( kFALSE)         
   , fMStyle( 0)          
   , fMSize( 0)           
   , fRailL ( NULL)
   , fRailR ( NULL)          
   , fArrowAtStart ( NULL)  
   , fArrowAtEnd ( NULL)    
   , fPrec( 0)
   , fClosed ( kFALSE)
   , fRailwaylike( 0)
   , fRailwayGage( 0)
   , fSleeperLength( 0)    
   , fSleeperDist( 0)     
   , fLineStyle( 0)
   , fLineWidth( 0)
   , fLineColor( 0)
   , fParallelFill( 0)
   , fPaintArrowAtStart ( kFALSE)
   , fPaintArrowAtEnd ( kFALSE)
   , fArrowFill( 0)
   , fArrowLength( 0)
   , fArrowAngle( 0)
   , fArrowIndentAngle( 0)
   , fRatioXY( 0)
   , fTextList ( NULL)
{
	if ( gDebug > 0 ) 
		cout << "TSplineX: def ctor npoints " << GetLastPoint() + 1 << " " << endl;
//   SetName("TSplineX");

}
//____________________________________________________________________________________

TSplineX::TSplineX(Int_t npoints, Double_t *x, Double_t *y,
         Float_t *sf, Float_t prec, Bool_t closed)

    : fShapeFactorList ( NULL) 
   , fControlPointList ( NULL)
   , fNofControlPoints( 0)
   , fNpoints( 0)         
   , fComputeDone ( kFALSE)     
   , fX( 0)               
   , fY( 0)               
   , fCPDrawn ( kFALSE)         
   , fMStyle( 24)          
   , fMSize( 2)           
   , fRailL ( NULL)
   , fRailR ( NULL)          
   , fArrowAtStart ( NULL)  
   , fArrowAtEnd ( NULL)    
   , fPrec(prec)
   , fClosed (closed)
   , fRailwaylike( 0)
   , fRailwayGage( 0)
   , fSleeperLength( 0)    
   , fSleeperDist( 0)     
   , fSleeperColor(1)
   , fSleeperWidth(1)
   , fLineStyle( 1)
   , fLineWidth( 2)
   , fLineColor( 1)
   , fParallelFill( 0)
   , fPaintArrowAtStart ( kFALSE)
   , fPaintArrowAtEnd ( kFALSE)
   , fArrowFill( 0)
   , fArrowLength( 10)
   , fArrowAngle( 30)
   , fArrowIndentAngle( 15)
   , fRatioXY( 0)
   , fTextList ( NULL)

{
//*-*-*-*-*-*-*-*-*-*-*-*-*-*-*TSplineX constructor-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*
//*-*                          ===================
// Number of points, x, y, shape factors of the controlpoints.
// -1 <= shape factor <= 1; prec controls the number of points
// calculated for the resulting smooth curve.
// The controlpoints of TSplineX are stored in its TGraph parent.
//
	Double_t ww = (Double_t)gPad->GetWw();
	Double_t wh = (Double_t)gPad->GetWh();
	Double_t pxrange = gPad->GetAbsWNDC()*ww;
	Double_t pyrange = gPad->GetAbsHNDC()*wh;
	Double_t xrange  = gPad->GetX2() - gPad->GetX1();
	Double_t yrange  = gPad->GetY2() - gPad->GetY1();
	Double_t pixeltoX  = xrange / pxrange;
	Double_t pixeltoY  = yrange/pyrange;
   fRatioXY = pixeltoY / pixeltoX;
//   SetName("TSplineX");
   fCPGraph.SetName("ControlGraph");
   fCPGraph.SetParent(this);
   if (npoints > 2) fNofControlPoints = npoints;
   else fNofControlPoints = 3;
   if (x && y && sf) {
   	TArrayF sf_temp(fNofControlPoints);
   	if (npoints < 3) {
      	cout << "WARNING: TSplineX needs minimum 3 points, inserted extra point" << endl;
      	TArrayD x_temp(3);
      	TArrayD y_temp(3);
      	x_temp[0] = x[0];
      	y_temp[0] = y[0];
      	x_temp[1] = 0.5 * (x[0] + x[1]);
      	y_temp[1] = 0.5 * (y[0] + y[1]);
      	x_temp[2] = x[1];
      	y_temp[2] = y[1];
      	if (sf == NULL) {
         	sf_temp[0] = -1;
         	sf_temp[1] =  1;
         	sf_temp[2] = -1;
      	} else {
         	sf_temp[0] = sf[0];
         	sf_temp[1] =  1;
         	sf_temp[2] = sf[1];
      	}
      	SetAllControlPoints(fNofControlPoints, x_temp.GetArray(), y_temp.GetArray());
      	SetShapeFactors(fNofControlPoints, sf_temp.GetArray());
   	} else {
      	SetAllControlPoints(fNofControlPoints, x, y);
      	if (sf != NULL) {
         	SetShapeFactors(fNofControlPoints, sf);
      	} else {
         	sf_temp[0] = -1;
         	sf_temp[fNofControlPoints-1] = -1;
         	for (Int_t i = 1; i < fNofControlPoints - 1; i++) {
            	sf_temp[i] = 1;
         	}
         	SetShapeFactors(fNofControlPoints, sf_temp.GetArray());
      	}
   	}
	//   ComputeSpline();
   } else {
      fCPGraph.SetLength(fNofControlPoints);
   }
   gROOT->GetListOfCleanups()->Add(&fPGraphs);
   fPGraphs.Clear();
   fDPolyLines.Clear();
//   cout << "TSplineX* tsx =(TSplineX*)" << this << ";"<< endl;
}
//____________________________________________________________________________________

TSplineX::~TSplineX()
{
//   cout << "~TSplineX(): "<< this  << endl << flush;
   gROOT->GetListOfCleanups()->Remove(&fPGraphs);
   if (fArrowAtStart) {
      gPad->GetListOfPrimitives()->Remove(fArrowAtStart);
      delete fArrowAtStart;
   }
   if (fArrowAtEnd) {
      gPad->GetListOfPrimitives()->Remove(fArrowAtEnd);
      delete fArrowAtEnd;
   }
   if (fRailL) {
      delete fRailL;
      fRailL = NULL;
   }
   if (fRailR) {
      delete fRailR;
      fRailR = NULL;
   }
   RemovePolyLines();
   fPGraphs.Delete();
   Delete_ControlPoints();
   Delete_ShapeFactors();
   cout << "exit ~TSplineX(): "<< this  << endl << flush;

}
//______________________________________________________________________________

Int_t TSplineX::DistancetoPrimitive(Int_t px, Int_t py)
{
   // Compute distance from point px,py to points of TSplineX
   // if railwaylike (double line) look inside
   //
   Int_t distance;

   const Int_t big = 9999;
   const Int_t kMaxDiff = 10;

   // check if point is near one of the graph points
   Int_t i, pxp, pyp, d;
   distance = big;
   if (fRailwaylike > 0 && fDPolyLines.GetSize() > 0) {
      TPolyLine *pl = (TPolyLine*)fDPolyLines.At(0);
      if (pl) {
         Double_t xp = gPad->AbsPixeltoX(px);
         Double_t yp = gPad->AbsPixeltoY(py);
         if (TMath::IsInside(xp, yp, pl->GetLastPoint()+1, pl->GetX(), pl->GetY()))
            distance = 0;
      }
   } else {
   // Somewhere on the graph points?
   	for (i=0;i<fNpoints;i++) {
      	pxp = gPad->XtoAbsPixel(gPad->XtoPad(fX[i]));
      	pyp = gPad->YtoAbsPixel(gPad->YtoPad(fY[i]));
      	d   = TMath::Abs(pxp-px) + TMath::Abs(pyp-py);
      	if (d < distance) distance = d;
			// is at end points prefer ControlGraph
			if (i == 0 || i == (fNpoints-1) ) {
				if (d < kMaxDiff) 
					return big;
			}
   	}
		if (gDebug > 2) {
			cout << "TSplineX::DistancetoPrimitive  " << distance << endl;
		}
      if (distance < kMaxDiff) return distance;
   // check if point is near the connecting line
   	for (i=0;i<fNpoints-1;i++) {
      	d = DistancetoLine(px, py, gPad->XtoPad(fX[i]), gPad->YtoPad(fY[i]), gPad->XtoPad(fX[i+1]), gPad->YtoPad(fY[i+1]));
      	if (d < distance) distance = d;
   	}
   }
   if (distance < kMaxDiff) return distance;
   else                     return big;
}
//_____________________________________________________________________________________

void TSplineX::SetShapeFactors(Int_t npoints, Float_t* s)
{
// *-*-*-*-*-*-*-*-*-*-*-*-*-*-*Set the ShapeFactors-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*
//
// ShapeFactor  > -1 < 0: Force line through point.
//                      : -1  influence of point is wide range
//                      :     leads to big curvature
//                      : near 0, short range, small curvature
//                 ==  0: Treat like an end point, no smooth
//           < 0 && <= 1: Line not forced through point
//                      : 1 smallest influence
//
   if (npoints <= 0 ||  s == 0) {
      cout << "Incomplete arguments" << endl;
      return;
   }
   if (fNofControlPoints != 0 && npoints != fNofControlPoints) {
      cout << "Number of ShapeFactors: " << npoints << " != "
           << "Number of ControlPoints " << endl;
      return;
   }
   fCPGraph.SetAllShapeFactors(npoints, s);
}
//_____________________________________________________________________________________

void TSplineX::SetIsClosed(Bool_t isclosed)
//
// Control if curve is closed of open
{
   if (fClosed != isclosed) {
      fClosed = isclosed;
      if (fRailL) fRailL->SetIsClosed(fClosed);
      if (fRailR) fRailR->SetIsClosed(fClosed);
      if (fPGraphs.GetEntries() > 0) {
         for (Int_t i = 0; i <= fPGraphs.GetLast(); i++) {
            ParallelGraph * gr = (ParallelGraph*)fPGraphs[i];
            if (gr) gr->SetIsClosed(fClosed);
         }
      }
      if (fClosed) {
         fCPGraph.SetShapeFactor(0, (Int_t)(100*fCPGraph.GetShapeFactor(1)));
         fCPGraph.SetShapeFactor(fNofControlPoints-1, (Int_t)(100*fCPGraph.GetShapeFactor(1)));
      } else {
         fCPGraph.SetShapeFactor(0, -100);
         fCPGraph.SetShapeFactor(fNofControlPoints-1, -100);
      }
      ComputeSpline();
   }
}
//_____________________________________________________________________________________

void TSplineX::SetAllControlPoints(Int_t npoints, Double_t* x, Double_t* y)
{
// *-*-*-*-*-*-*-*-*-*-*-*-*-*-*Set ControlPoints-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*
//
   if (npoints <= 0 || x == 0 && y == 0) {
      cout << "Incomplete arguments" << endl;
      return;
   }

   fNofControlPoints = npoints;
   fCPGraph.Set(fNofControlPoints);
   for (Int_t i = 0; i < npoints; i++) {
      fCPGraph.SetPoint(i, x[i], y[i]);
   }
}
//_____________________________________________________________________________________

Int_t TSplineX::ComputeSpline()
{
// *-*-*-*-*-*-*-*-*-*-*-*-*-*-*Compute the spline-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*
   ControlGraph* cg = GetControlGraph();
   if (cg->GetN() <= 0) {
      cout << "ShapeFactors not set" << endl;
      return -1;
   }
//   Dump();
	CopyControlPoints();
   if (fClosed)
      cl_spline(fControlPointList, fShapeFactorList, fPrec);
   else
      op_spline(fControlPointList, fShapeFactorList, fPrec);
   SetGraph();
   if (fRailL && fRailL) {
      fRailL->Compute();
      fRailR->Compute();
//      if (!gPad->GetListOfPrimitives()->FindObject(fRailL)) fRailL->Draw("L");
//      if (!gPad->GetListOfPrimitives()->FindObject(fRailR)) fRailR->Draw("L");
      if ( !fClosed && (fPaintArrowAtStart || fPaintArrowAtEnd)) {
         fRailL->CorrectForArrows(fRatioXY, fArrowLength, fArrowAngle,fArrowIndentAngle,
                                    fPaintArrowAtStart, fPaintArrowAtEnd);
         fRailR->CorrectForArrows( fRatioXY, fArrowLength, fArrowAngle,fArrowIndentAngle,
                                    fPaintArrowAtStart, fPaintArrowAtEnd);
      }
   }
   if (fPGraphs.GetEntries() > 0) {
      for (Int_t i = 0; i <= fPGraphs.GetLast(); i++) {
         ParallelGraph * gr = (ParallelGraph*)fPGraphs[i];
         if (gr) {
            gr->Compute();

            if ((fPaintArrowAtStart || fPaintArrowAtEnd)
                && (fSleeperLength <= 0 || fSleeperDist  <= 0))
              gr->CorrectForArrows( fRatioXY, fArrowLength, fArrowAngle,fArrowIndentAngle,
                                    fPaintArrowAtStart, fPaintArrowAtEnd);
         }
      }
   }
   fComputeDone = kTRUE;
   gPad->Modified();
   gPad->Update();
   return fNpoints;
}
//_____________________________________________________________________________________

Int_t TSplineX::GetResult(Double_t*& x, Double_t*& y)
{
// *-*-*-*-*-*-*-*-*-*-*-*-*-*-*Return values of the resulting spline-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*
//
    x = fX.GetArray();
    y = fY.GetArray();
    return GetLastPoint() + 1;
}
//_____________________________________________________________________________________

void TSplineX::RemovePolyLines()
{
//
// Before the spline is repainted the auxilliary lines
// must be removed

   if (fDPolyLines.GetSize() > 0) {
      TIter next(&fDPolyLines);
      TObject* obj;
      TList * lop = gPad->GetListOfPrimitives();
      while ( (obj=next()) ) {
         lop->Remove(obj);
      }
      fDPolyLines.Delete();
   }
}
//_____________________________________________________________________________________

void TSplineX::Delete_ControlPoints()
{
   if (fControlPointList) {
// 		cout << "TSplineX::Delete_ControlPoints() " << endl;
      while (fControlPointList) {
         ControlPoint * next = fControlPointList->GetNext();
         delete  fControlPointList;
         fControlPointList = next;
      }
   }
   fControlPointList = NULL;
}
//_____________________________________________________________________________________

void TSplineX::Delete_ShapeFactors()
{
   if (fShapeFactorList) {
      while (fShapeFactorList) {
         ShapeFactor * next = fShapeFactorList->GetNext();
         delete  fShapeFactorList;
         fShapeFactorList = next;
      }
   }
   fShapeFactorList = NULL;
}
//_____________________________________________________________________________________

void TSplineX::CopyControlPoints()
{
// *-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*
// Copy ControlPoints and ShapeFactors into a linked list
// as expected by the internally used routines doing the
// real compututations
//

   Double_t xp, yp;
	if (fControlPointList) Delete_ControlPoints();
   if (fShapeFactorList)  Delete_ShapeFactors() ;
   ControlPoint* previous_cp = 0;
   ShapeFactor* previous_sf = 0;
   fNofControlPoints = fCPGraph.GetN();
   for (Int_t i = 0; i < fNofControlPoints; i++) {
      fCPGraph.GetPoint(i, xp, yp);
      ControlPoint * p_cp = new ControlPoint(xp, yp, (ControlPoint*)0);
      if (fControlPointList == 0) fControlPointList = p_cp;
      if (previous_cp != 0) previous_cp->SetNext(p_cp);
      previous_cp = p_cp;
      ShapeFactor* p_sf = new ShapeFactor(fCPGraph.GetShapeFactor(i), (ShapeFactor*)0);
      if (fShapeFactorList == 0) fShapeFactorList = p_sf;
      if (previous_sf != 0) previous_sf->SetNext(p_sf);
      previous_sf = p_sf;
   }
   fX.Set(fNofControlPoints * 50);
   fY.Set(fNofControlPoints * 50);
}
//_____________________________________________________________________________________

Int_t TSplineX::add_point(Double_t x, Double_t y)
//
// This is called by the routines doing the calculation of the spline
{
   if (fNpoints >= fX.GetSize()) {
      fX.Set(fNpoints + 50);
      fY.Set(fNpoints + 50);
   }
   fX[fNpoints] = x;
   fY[fNpoints] = y;
   fNpoints ++;
   return 1;
}
//_____________________________________________________________________________________

Int_t TSplineX::add_closepoint()
//
// This is called by the routines doing the calculation of the spline
{
   if (fNpoints >= fX.GetSize()) {
      fX.Set(fNpoints + 1);
      fY.Set(fNpoints + 1);
   }
   fX[fNpoints] = fX[0];
   fY[fNpoints] = fY[0];
   fNpoints ++;
   return 1;
}
//_____________________________________________________________________________________

void TSplineX::too_many_points()
{
   cout << "too_many_points()" << endl;
}
//_____________________________________________________________________________________

void TSplineX::SetGraph()
{
//
// fill values of the polyline representing the smoothed curve

   SetPolyLine(fNpoints);
   for (Int_t i = 0; i < fNpoints; i++) {
      SetPoint(i, fX[i], fY[i]);
   }
}
//_____________________________________________________________________________________

void TSplineX::DrawControlPoints(Style_t marker,  Size_t size)
//
// Make the controlpoints visible
{
   if (fCPGraph.GetN() <= 0) return;

   if (marker != 0) fMStyle =  marker;
   fCPGraph.SetMarkerStyle(fMStyle);
   if (size > 0)  fMSize = size;
   fCPGraph.SetMarkerSize(fMSize);
   fCPGraph.SetMarkerColor(GetLineColor());
   fCPGraph.Draw("P");
   fCPDrawn = kTRUE;
   gPad->Update();
}
//_____________________________________________________________________________________

void TSplineX::RemoveControlPoints()
//
// Make the controlpoints unvisible
{
   if (fCPGraph.GetN() <= 0) return;

   gPad->GetListOfPrimitives()->Remove(&fCPGraph);
   fCPDrawn = kFALSE;
   gPad->Update();
}
//_____________________________________________________________________________________

ParallelGraph* TSplineX::AddParallelGraph(Double_t dist, Color_t color, Width_t width, Style_t style)
//
// Add a parallel line and set its attributes
{
   ParallelGraph *pgraph = new ParallelGraph (this, dist, fClosed);
   Int_t idx = fPGraphs.GetLast() + 1;
   fPGraphs.AddAtAndExpand(pgraph, idx);
   if (color <= 0) pgraph->SetLineColor(this->GetLineColor());
   else            pgraph->SetLineColor(color);
   if (width <= 0) pgraph->SetLineWidth(this->GetLineWidth());
   else            pgraph->SetLineWidth(width);
   if (style <= 0) pgraph->SetLineStyle(this->GetLineStyle());
   else            pgraph->SetLineStyle(style);
   pgraph->Draw("L");
   fComputeDone = kFALSE;
   return pgraph;
}
//_____________________________________________________________________________________

void  TSplineX::DrawParallelGraphs()
//
// Draw the parallel lines
{
//   if (fRailL) fRailL->Draw("L");
 //  if (fRailR) fRailR->Draw("L");
   for (Int_t i = 0; i < fPGraphs.GetEntries(); i++) {
      TGraph* gr = (TGraph*)fPGraphs[i];
      if (gr) gr->Draw("L");
   }
}
//_____________________________________________________________________________________

void TSplineX::SetRailwaylike(Double_t gage)
{
// Make the spline look like railway tracks
   fRailwayGage = gage;
   if (gage <= 0) {
      fRailwaylike = 0;
      if (fRailL) {delete fRailL; fRailL = NULL;}
      if (fRailR) {delete fRailR; fRailR = NULL;}
   } else {
   	if (fRailL != NULL) {
      	if (fRailL) fRailL->SetDist( gage / 2);
      	if (fRailR) fRailR->SetDist(-gage / 2);
   	} else {
      	fRailwaylike = 1;
      	fRailL = new ParallelGraph (this,  gage / 2 , fClosed);
         fRailL->SetLineColor(this->GetLineColor());
         fRailL->SetLineWidth(this->GetLineWidth());
         fRailL->SetLineStyle(this->GetLineStyle());
         fRailL->SetIsRail();
//         fRailL->Draw("L");
      	fRailR = new ParallelGraph (this, -gage / 2 , fClosed);
         fRailR->SetLineColor(this->GetLineColor());
         fRailR->SetLineWidth(this->GetLineWidth());
         fRailR->SetLineStyle(this->GetLineStyle());
         fRailR->SetIsRail();
//         fRailR->Draw("L");
   	}
   }
   fComputeDone = kFALSE;
   gPad->Modified();
}
//_____________________________________________________________________________________

void TSplineX::Print (Option_t * /*opt*/) const
{
   cout << "TSplineX:" << endl;
   fCPGraph.Print();
}
//_____________________________________________________________________________________

void TSplineX::Paint(Option_t * /*optin*/)
{
//
// This is the complicated part: compute the spline,
// add parallel lines, arrows, railway sleepers if needed
// If required fill the space between rails or inside the
// spline.
	//cout << "TSplineX::Paint " << fComputeDone << endl;
   if (!fComputeDone) ComputeSpline();
   Bool_t aas = fPaintArrowAtStart;
   Bool_t aae = fPaintArrowAtEnd;
   if (fClosed) {
      fPaintArrowAtStart = kFALSE;
      fPaintArrowAtEnd   = kFALSE;
   }
   if (fRailwaylike <= 0)  {
//    simple line, no rail
      TString opt(GetOption());
      if (GetFillStyle() != 0 && fClosed) {
         SetFillStyle(GetFillStyle());
         SetFillColor(GetFillColor());
         opt += "f";
      }
      TPolyLine::Paint(opt);
      if (fArrowAtStart) {
         fArrowAtStart->SetLineColor(GetLineColor());
         if (fArrowAtStart->GetFillStyle() != 0)
            fArrowAtStart->SetFillColor(GetLineColor());
      }
      if (fArrowAtEnd) {
         fArrowAtEnd->SetLineColor(GetLineColor());
         if (fArrowAtEnd->GetFillStyle() != 0)
            fArrowAtEnd->SetFillColor(GetLineColor());
      }
      gPad->Modified();
      gPad->Update();

   }

//   if (fComputeDone) return;
//   fComputeDone = kTRUE;

   if (fArrowAtStart) {
      gPad->GetListOfPrimitives()->Remove(fArrowAtStart);
      delete fArrowAtStart;
      fArrowAtStart = NULL;
   }
   if (fArrowAtEnd) {
      gPad->GetListOfPrimitives()->Remove(fArrowAtEnd);
      delete fArrowAtEnd;
      fArrowAtEnd = NULL;
   }

   if (fDPolyLines.GetSize() > 0) {
      TIter next(&fDPolyLines);
      TObject* obj;
      TList * lop = gPad->GetListOfPrimitives();
      while ( (obj=next()) ) lop->Remove(obj);
      fDPolyLines.Delete();
   }
   if (fPaintArrowAtStart) PaintArrow(0);
   if (fPaintArrowAtEnd)   PaintArrow(1);

   PaintText();
   PolyLineNoEdit * pl = NULL;
   ParallelGraph *lg = NULL;
   ParallelGraph *rg = NULL;
   if (fRailwaylike <= 0) {
		return;
   } else {
      if (fRailL == 0) SetRailwaylike(fRailwayGage);
//  fill parallel graphs
      lg = fRailL;
      rg = fRailR;
   	lg->SetLineColor(GetLineColor());
   	rg->SetLineColor(GetLineColor());
   	lg->SetLineWidth(GetLineWidth());
   	rg->SetLineWidth(GetLineWidth());
      Int_t npoints = 1;
      npoints += lg->GetLastPoint()+1;
      npoints += rg->GetLastPoint()+1;
      if (fPaintArrowAtEnd) npoints += 3;
      if (fPaintArrowAtStart) npoints += 3;
      pl = new PolyLineNoEdit(npoints);
      Double_t * x = pl->GetX();
      Double_t * y = pl->GetY();
      Double_t * px = rg->GetX();
      Double_t * py = rg->GetY();
      Int_t n = 0;
      for (Int_t i = 0; i < rg->GetLastPoint()+ 1; i++) {
         pl->SetPoint(n, px[i], py[i]);
         n++;
      }
      if (fPaintArrowAtEnd) {
         px = fArrowAtEnd->GetX();
         py = fArrowAtEnd->GetY();
         for (Int_t i = 3; i >= 1; i--) {
           pl->SetPoint(n, px[i], py[i]);
           n++;
         }
      }
      px = lg->GetX();
      py = lg->GetY();
      for (Int_t i = lg->GetLastPoint()+ 1 -1 ; i >= 0; i--) {
         pl->SetPoint(n, px[i], py[i]);
         n++;
      }
      if (fPaintArrowAtStart) {
         px = fArrowAtStart->GetX();
         py = fArrowAtStart->GetY();
         for (Int_t i = 3; i >= 1; i--) {
            pl->SetPoint(n, px[i], py[i]);
            n++;
         }
      }
      pl->SetPoint(n, x[0], y[0]);
      pl->SetLineColor(GetLineColor());
      pl->SetLineWidth(GetLineWidth());
      fDPolyLines.Add(pl);
      if (fParallelFill && GetFillStyle() != 0) {
         pl->SetFillStyle(GetFillStyle());
         pl->SetFillColor(GetFillColor());
         pl->Paint("F");
         if (fPaintArrowAtEnd) fArrowAtEnd->Pop();
         if (fPaintArrowAtStart) fArrowAtStart->Pop();
      } else {
			pl->Paint("L");
		}
   }
   fPaintArrowAtStart = aas;
   fPaintArrowAtEnd   = aae;
   gPad->Modified();
   gPad->Update();

   if (fDrawAsInMap == 0 && fDrawSleepers == 0 || fSleeperDist <= 0 ) {
		return;
	}

//  draw railway sleepers
   lg = fRailL;
   rg = fRailR;
   if (lg->GetLastPoint()+1 != rg->GetLastPoint()+1 || lg->GetLastPoint()+1 < 4) {
      cout << "Not enough points on spline" << endl;
      return;
   }
   RailwaySleeper * rwsl;
   Double_t xp[5], yp[5];
   Double_t * xc = this->GetX();
   Double_t * yc = this->GetY();
   Double_t * xl = lg->GetX();
   Double_t * yl = lg->GetY();
   Double_t * xr = rg->GetX();
   Double_t * yr = rg->GetY();
   Double_t needed = fSleeperLength;

   Double_t dist = TMath::Abs(lg->GetDist());
   Double_t available = 0;
   Double_t phi, ax1, ay1, ax2, ay2, x, y;
   
// if fRwSleeperLength > 0 draw real sleepers with length
// gage * fRwSleeperLength
// distance btw sleepers fSleeperDist
// otherwise
// draw a filled box with length fSleeperDist alternating
// with white space as used in maps

	Int_t np = 5;
//	TString drawopt("f");
	Double_t dist_btw_sleepers = fSleeperDist;
	Color_t col = this->GetSleeperColor();
	if (fDrawSleepers> 0 ){
		dist *= fSleeperLength;
		dist_btw_sleepers = fSleeperDist;
		np = 2;
//		drawopt="l";
	}
	Double_t covered_dist = 0;
	Double_t total_length = 0;
	
	for (Int_t i=0; i <GetLastPoint()-1; i++) {
		total_length += Length(xc[i], yc[i], xc[i+1], yc[i+1]);
	}
//	cout << "np, points, total length: " << np << " "  << GetLastPoint() << " " << total_length << endl;
//   Bool_t   box_done = kFALSE;
   Int_t ip = 0;
   phi = PhiOfLine( xc[ip], yc[ip], xc[ip+1], yc[ip+1]);
   Endpoint(phi, xc[0], yc[0], -dist, &ax1, &ay1);
   Endpoint(phi, xc[0], yc[0],  dist, &ax2, &ay2);
   xp[0] = ax1;
   yp[0] = ay1;
   xp[1] = ax2;
   yp[1] = ay2;
   // case real sleeper, put sleeper at start
   if ( np == 2 ) {
		rwsl = new RailwaySleeper(xp, yp, this, col, np);
      fDPolyLines.Add(rwsl);
   }
   available = Length(xc[0], yc[0], xc[1], yc[1]);
   needed = dist_btw_sleepers;
 //  rest = needed;
   Bool_t within_sleeper = kTRUE;
   Double_t dist_on_segment = 0;
   while (covered_dist < total_length) {
		
      while (needed > available) {
			ip += 1;
			if (ip >= GetLastPoint() + 1 - 2) goto ENDOFSPLINE;
			available += Length(xc[ip], yc[ip], xc[ip+1], yc[ip+1]);
		}
		// interpolate on segment
		dist_on_segment = Length(xc[ip], yc[ip], xc[ip+1], yc[ip+1])
								- (available - needed) ;
//		cout << ip << " " << yc[ip] << " " << needed  << " " << available 
//		<< " " << dist_on_segment<< " " << covered_dist << endl;
		x = xc[ip] + (xc[ip+1] - xc[ip]) * (dist_on_segment/ Length(xc[ip], yc[ip], xc[ip+1], yc[ip+1]));
		y = yc[ip] + (yc[ip+1] - yc[ip]) * (dist_on_segment/ Length(xc[ip], yc[ip], xc[ip+1], yc[ip+1]));
      phi = PhiOfLine( xc[ip], yc[ip], xc[ip+1], yc[ip+1]);
		Endpoint(phi, x, y, -dist, &ax1, &ay1);
		Endpoint(phi, x, y,  dist, &ax2, &ay2);
		if (np == 5 && within_sleeper){
			xp[3] = ax1;
			yp[3] = ay1;
			xp[2] = ax2;
			yp[2] = ay2;
			xp[4] = xp[0];
			yp[4] = yp[0];
//			for (Int_t i=0; i <5; i++) 
//				cout <<"xp["<< i << "] \t" << xp[i] <<" yp[" << i << "] \t" << yp[i] <<endl;
			rwsl = new RailwaySleeper(xp, yp, this, col, np);
			fDPolyLines.Add(rwsl);
//			pl->Paint(drawopt);
			within_sleeper = kFALSE;
		} else {
			xp[0] = ax1;
			yp[0] = ay1;
			xp[1] = ax2;
			yp[1] = ay2;
			within_sleeper = kTRUE;
		}
		if (np == 2) {
			rwsl = new RailwaySleeper(xp, yp, this, col, np);
			fDPolyLines.Add(rwsl);
//			pl->Paint(drawopt);
		}
		available -= needed;
		covered_dist += needed;
		// first time needed was 0.5 * dist_btw_sleepers
//		needed = dist_btw_sleepers;
   }
ENDOFSPLINE:
   ip = GetLastPoint() + 1 - 2;
   if ( within_sleeper && np == 5) {
      xp[2] = xl[ip+1];
      yp[2] = yl[ip+1];
      xp[3] = xr[ip+1];
      yp[3] = yr[ip+1];
      xp[4] = xp[0];
      yp[4] = yp[0];
      
      rwsl = new RailwaySleeper(xp, yp, this, col, np);
      fDPolyLines.Add(rwsl);
//      pl->Paint(drawopt);
   }
   if (fClosed || fRailwaylike>0) {
//  avoid closing line at end
		rg->Paint("L");
		lg->Paint("L");
	} else {
		pl->Paint("L");
	}

   gPad->Modified();
   gPad->Update();
}
//______________________________________________________________________

Int_t TSplineX::PointByDist(Double_t dist)
{
	if (dist <= 0 )
		return -1;
	Double_t d = 0;
	Double_t *xc = GetX();
	Double_t *yc = GetY();
	
	for (Int_t i = 0; i< GetLastPoint() -1; i++) {
		d += Length(xc[i], yc[i], xc[i+1], yc[i+1]);
		if (d > dist)
			return i;
	}
	return -1;
}
//_____________________________________________________________________________________

void TSplineX::SetSleeperColor(Color_t color)
{
	fSleeperColor = color;
//   SetLineColor(color);
   if (fDPolyLines.GetSize() > 0) {
      TIter next(&fDPolyLines);
      RailwaySleeper * rs;
      while ( (rs = (RailwaySleeper*)next()) ) {
         rs->SetLineColor(color);
         rs->SetFillColor(color);
      }
      gPad->Update();
   }
   Paint();
}
//_____________________________________________________________________________________

void TSplineX::AddArrow(Int_t where, Double_t length,
                       Double_t angle, Double_t indent_angle,Int_t filled)
{
// Add arrows at ends (0 at start, 1 at end),
// Note: for a railwaylike spline the arrows are part of the spline

   if (where == 0) fPaintArrowAtStart = kTRUE;
   if (where == 1) fPaintArrowAtEnd = kTRUE;
   fArrowLength      = length;
   fArrowAngle       = angle;
   fArrowIndentAngle = indent_angle;
   fArrowFill        = filled;
   fComputeDone      = kFALSE;
   Paint();
}
//_____________________________________________________________________________________

Double_t TSplineX::GetLengthOfSpline()
{
   Double_t* xp;
   Double_t* yp;
   Int_t np = this->GetResult(xp, yp);
   Double_t len = 0;
   for (Int_t i=0; i < np-2; i++) {
     len += Length(xp[i], yp[i], xp[i+1], yp[i+1]);
   }
   return len;
}
//_____________________________________________________________________________________

Double_t TSplineX::GetLengthOfText(HprText *t, Double_t csep, Int_t start, Int_t end, Int_t step, Double_t s0)
{
   TString str(t->GetText());
   Double_t s = s0;
   UInt_t w, h;
   TLatex tt;
   tt.SetTextSize(t->GetTextSize());
   tt.SetTextFont(t->GetTextFont());
   tt.GetTextExtent(w, h, "X");
	Double_t clen = (gPad->AbsPixeltoX((Int_t)w) - gPad->AbsPixeltoX(0));
	Double_t yx = (gPad->AbsPixeltoY((Int_t)w) - gPad->AbsPixeltoY(0)) / clen;
   for (Int_t i=start; i != (end + step); i+=step) {
      TString subs = str[i];
      tt.GetTextExtent(w, h, subs.Data());
      Double_t x, y;
		Double_t phirad = GetPhiXY(s, x, y);
      clen = (gPad->AbsPixeltoX((Int_t)w) - gPad->AbsPixeltoX(0));
      Double_t co = TMath::Cos(phirad);
      Double_t si = TMath::Sin(phirad);
      Double_t xylen = TMath::Sqrt(co*co + yx*yx*si*si);
		s += step * (csep + clen) * xylen;
   }
   return step * (s - s0);
}
//_____________________________________________________________________________________

Double_t TSplineX::GetPhiXY(Double_t s, Double_t &x, Double_t &y)
{
   x = y = 0;
   if (s < 0 || s >= GetLengthOfSpline()) {
//      cout << " GetPhi: s > len " << s << " " << GetLengthOfSpline() << endl;
      return 0;
   }

   Double_t* xp;
   Double_t* yp;
   Int_t np = this->GetResult(xp, yp);
   Double_t len = 0;
   for (Int_t i=0; i < np-1; i++) {
     len += Length(xp[i], yp[i], xp[i+1], yp[i+1]);
     if (len > s) {
        Double_t rest = len - s;
        Double_t d = Length(xp[i], yp[i], xp[i+1], yp[i+1]);
        x =  xp[i] + (xp[i+1] - xp[i]) * (d-rest) / d;
        y =  yp[i] + (yp[i+1] - yp[i]) * (d-rest) / d;
        return PhiOfLine(xp[i], yp[i], xp[i+1], yp[i+1]);
     }
   }
   return 0;
}

//_____________________________________________________________________________________

void TSplineX::AddText(TObject *t)
{
   if ( !fTextList )
      fTextList = new TList();
   fTextList->Add(t);
}
//_____________________________________________________________________________________

void TSplineX::PaintText()
{
   for (Int_t i = 0; i < 3; i++) {
      for (Int_t j = 0; i < 3; i++) {
         fCornersX[i][j] = -1111;
         fCornersY[i][j] = -1111;
      }
   }
   if ( !fTextList ||  fTextList->GetSize() <= 0 ) return;
   Double_t* xp;
   Double_t* yp;
   Int_t np = this->GetResult(xp, yp);
   Double_t direction = 1;
   if ( xp[0] > xp[np-1] )
      direction = -1;
   for (Int_t it = 0; it < fTextList->GetSize(); it++) {
      HprText *t = (HprText*)fTextList->At(it);
      THprLatex latex;
      latex.SetTextSize(t->GetTextSize());
      latex.SetTextFont(t->GetTextFont());
      latex.SetTextColor(t->GetTextColor());
      latex.SetBit(THprLatex::kValignNoShift);
		UInt_t w, h, asc, des;
		latex.GetTextExtent(w, h, "i");
		Double_t csep = 0.1 * (gPad->AbsPixeltoX((Int_t)w) - gPad->AbsPixeltoX(0));
	//   csep = ;
		latex.GetTextExtent(w, h, "X");
		Double_t chShift=  0.5 * (gPad->AbsPixeltoX((Int_t)h) - gPad->AbsPixeltoX(0));
		Double_t hofXp = h;
		Double_t hofX = chShift;
		Double_t clen = (gPad->AbsPixeltoX((Int_t)w) - gPad->AbsPixeltoX(0));
	   Double_t yx = (gPad->AbsPixeltoY((Int_t)w) - gPad->AbsPixeltoY(0)) / clen;

      TString text = t->GetText();
      Int_t nchar  = text.Length();
//      Int_t midc   = TMath::Min(nchar / 2 + 1, nchar-1);
      Int_t midc   = nchar / 2;
      Short_t align = t->GetTextAlign();
		Int_t halign = align / 10;
      Int_t valign = align % 10;
		Double_t s = 0;
		Double_t als = 0, ale = 0;
		if (fArrowAtStart || fArrowAtEnd) {
			Double_t al = GetArrowLength(0.5 * fRailwayGage, fArrowLength, fArrowAngle, fArrowIndentAngle);
			if ( fArrowAtStart )
				als = al;
			if ( fArrowAtEnd )
				ale = al;
      }
      Double_t s0 = GetLengthOfSpline();
      Double_t lot;
      Double_t co;
      Double_t si;
      Double_t xylen;
		Double_t x, y, phi, a, b;
		if ( halign == 1) {
			phi = GetPhiXY(0.05 * s0, x, y);
         co = TMath::Cos(phi);
         si = TMath::Sin(phi);
         xylen = TMath::Sqrt(co*co + yx*yx*si*si);
	      s += (als + t->GetOffset()) * xylen;
         if (direction < 0 ) {
            lot = GetLengthOfText(t, csep, 0, nchar-1, 1, s);
            s += lot;
         }
		} else if ( halign == 2) {
	// center along spline
        lot = GetLengthOfText(t, csep,midc, 0, -1, s0);
			s = 0.5 * s0 - lot;
         if (direction < 0 )
            s += 2 * lot;
		} else if ( halign == 3) {
	// right along spline
			phi = GetPhiXY(0.05 * s0, x, y);
         co = TMath::Cos(phi);
         si = TMath::Sin(phi);
         xylen = TMath::Sqrt(co*co + yx*yx*si*si);
         s0 -=  (ale + t->GetOffset()) * xylen;
         lot = GetLengthOfText(t, csep, nchar-1, 0, -1, s0);
			s = s0 - lot - t->GetOffset() * xylen;
         if (direction < 0 )
            s += lot;
		}
		if ( valign == 2 ) {
	// center vertical
         chShift = 0;
		} else if ( valign == 1 ) {
			chShift = direction * (chShift + 0.5 * fRailwayGage);
		} else {
			chShift = - direction * (chShift + 0.5 * fRailwayGage);
		}

		latex.SetTextAlign(12);
		for (Int_t i=0; i < nchar; i++) {
			TString subs = text[i];
			latex.GetTextExtent(w, h, subs.Data());
			latex.GetTextAscentDescent(asc, des, subs.Data());
			/*
			std::cout << subs.Data() << " w " << w 
			<< " h " << h << " asc " << asc << " des " << des 
			<< " chshift " << chShift 
			<< " hofX " << hofX
			<< std::endl;
			*/
			clen = (gPad->AbsPixeltoX((Int_t)w) - gPad->AbsPixeltoX(0));
			phi = GetPhiXY(s, x, y);
         a = x;
         b = y;
        
//			if ( chShift != 0 ) {
				
				Endpoint(phi, x, y, 
				chShift + hofX *((hofXp-asc)/hofXp + des/hofXp) , &a, &b);
//			}
         co = TMath::Cos(phi);
         si = TMath::Sin(phi);
         xylen = TMath::Sqrt(co*co + yx*yx*si*si);
			phi  *=  (180 / TMath::Pi());
         if ( direction < 0 ) {
            phi += 180;
            if (phi > 360)
               phi -= 360;
         }
			latex.PaintLatex(a, b, phi, t->GetTextSize(), subs.Data());
			s += direction * (csep + clen) * xylen;
// at start
         if ( i == 0 && halign == 1) {
            if        ( valign == 2 ) {
               fCornersX[halign-1][valign-1] = x;
               fCornersY[halign-1][valign-1] = y;
            } else if ( valign == 1 ) {
               fCornersX[halign-1][valign-1] = a;
               fCornersY[halign-1][valign-1] = b;
            } else if ( valign == 3 ) {
               Endpoint(phi, x, y, -0.5 * fRailwayGage, &a, &b);
               fCornersX[halign-1][valign-1] = a;
               fCornersY[halign-1][valign-1] = b;
            }
         }
// at end
         if ( i == text.Length()-1 && halign == 3) {
            if        ( valign == 2 ) {
               fCornersX[halign-1][valign-1] = x;
               fCornersY[halign-1][valign-1] = y;
            } else if ( valign == 1 ) {
               fCornersX[halign-1][valign-1] = a;
               fCornersY[halign-1][valign-1] = b;
            } else if ( valign == 3 ) {
               Endpoint(phi, x, y, -0.5 * fRailwayGage, &a, &b);
               fCornersX[halign-1][valign-1] = a;
               fCornersY[halign-1][valign-1] = b;
            }
         }
//  center
         if ( i == midc && halign == 2) {
            if        ( valign == 2 ) {
               fCornersX[halign-1][valign-1] = x;
               fCornersY[halign-1][valign-1] = y;
            } else if ( valign == 1 ) {
               fCornersX[halign-1][valign-1] = a;
               fCornersY[halign-1][valign-1] = b;
            } else if ( valign == 3 ) {
               Endpoint(phi, x, y, -0.5 * fRailwayGage, &a, &b);
               fCornersX[halign-1][valign-1] = a;
               fCornersY[halign-1][valign-1] = b;
            }
         }
		}
   }
}
//_____________________________________________________________________________________

void TSplineX::PaintArrow(Int_t where)
{
   Double_t* xp;
   Double_t* yp;
   Int_t np = this->GetResult(xp, yp);
   Double_t px1;
   Double_t py1;
   Double_t px2;
   Double_t py2;
   if (where == 1) {
   	px1 = xp[np-2];
   	py1 = yp[np-2];
   	px2 = xp[np-1];
   	py2 = yp[np-1];
   } else {
      px1 = xp[1];
   	py1 = yp[1];
		px2 = xp[0];
		py2 = yp[0];
   }
   Double_t deg2rad = TMath::Pi() / 180;
   Double_t p2 = 0.5 * fArrowAngle * deg2rad;
   Double_t a2 = fArrowIndentAngle *deg2rad;

//   TGraph * arrow = new TGraph(5);
//   Double_t *x = arrow->GetX();
//   Double_t *y = arrow->GetY();
   Double_t x[5], y[5];

   x[0] = x[4] = 0;
   y[0] = y[4] = 0;
   x[1] = - fArrowLength;
   y[1] = fArrowLength * TMath::Tan(p2);
   x[2] = - fArrowLength + y[1] * TMath::Tan(a2);
   y[2] = 0;
   x[3] = - fArrowLength;
   y[3] = -y[1];

   Double_t angle = PhiOfLine(px1, py1, px2, py2);
   Double_t cosang = TMath::Cos(angle);
   Double_t sinang = TMath::Sin(angle);
   Double_t xx, yy;
   for (Int_t i = 0; i < 5; i++) {
      xx = x[i] * cosang - y[i] * sinang;
      yy = x[i] * sinang + y[i] * cosang;
      x[i] = xx + px2;
      y[i] = fRatioXY *yy + py2;
   }
   if (fRailwaylike && fArrowFill == 0) {
//  connect array at end to parallel lines
      ParallelGraph* lg = fRailL;
      ParallelGraph* rg = fRailR;
      xp = lg->GetX();
      yp = lg->GetY();
      np = lg->GetLastPoint()+1;
   	if (where == 1) {
   		px2 = xp[np-1];
   		py2 = yp[np-1];
   	} else {
      	px1 = xp[0];
   		py1 = yp[0];
   	}
      xp = rg->GetX();
      yp = rg->GetY();
      np = rg->GetLastPoint()+1;
   	if (where == 1) {
   		px1 = xp[np-1];
   		py1 = yp[np-1];
   	} else {
      	px2 = xp[0];
   		py2 = yp[0];
   	}
      Double_t px0 = x[0];
      Double_t py0 = y[0];
      Double_t px3 = x[3];
      Double_t py3 = y[3];
      Double_t px1o = x[1];
      Double_t py1o = y[1];
      x[0] = px2;
//      Double_t* xp;
//      Double_t* yp;
//      GetResult(xp, yp);
      y[0] = py2;
      x[1] = px3;
      y[1] = py3;
      x[2] = px0;
      y[2] = py0;
      x[3] = px1o;
      y[3] = py1o;
      x[4] = px1;
      y[4] = py1;
   }

   PolyLineNoEdit * arrow = new PolyLineNoEdit(5, &x[0], &y[0]);
   arrow->SetLineColor(GetLineColor());
   if (fArrowFill != 0) {
//      arrow->SetName("FilledArrow");
      arrow->SetFillStyle(1001);
      arrow->SetFillColor(GetLineColor());
      arrow->Paint("F");
   } else {
//      arrow->SetName("OpenArrow");
      arrow->SetFillStyle(0);
      arrow->SetLineWidth(GetLineWidth());
      arrow->Paint();
//      arrow->Print();
   }
   if (where == 1) fArrowAtEnd = arrow;
   else            fArrowAtStart = arrow;
}
//___________________________________________________________________________

void TSplineX::SetArrowFill(Bool_t filled)
{
   if (filled) {
      if (fArrowAtStart)fArrowAtStart->SetFillStyle(1001);
      if (fArrowAtEnd)fArrowAtEnd->SetFillStyle(1001);
   } else {
      if (fArrowAtStart)fArrowAtStart->SetFillStyle(0);
      if (fArrowAtEnd)fArrowAtEnd->SetFillStyle(0);
   }
   fArrowFill = filled;
};
//___________________________________________________________________________

#if ROOT_VERSION_CODE >= ROOT_VERSION(5,12,0)
   void TSplineX::SavePrimitive(ostream & out, Option_t *)
#else
   void TSplineX:: SavePrimitive(ofstream & out, Option_t *)
#endif
{
   // Save primitive as a C++ statement(s) on output stream out

   cout << "TSplineX::SavePrimitive: " << this << endl;
   char quote = '"';
   out<<"   "<<endl;
   out<<"   Double_t *dum = 0;"<<endl;
   out<<"   Float_t *fum = 0;"<<endl;
   if (gROOT->ClassSaved(TSplineX::Class())) {
      out<<"   ";
   } else {
      out<<"    TSplineX *";
   }

   out<<"splinex = new TSplineX("<<GetNofControlPoints()<<",dum,dum,fum,"
       << fPrec <<",";
       if (fClosed) out << "kTRUE ";
       else         out << "kFALSE";
       out << ");"<<endl;

   SaveFillAttributes(out,"splinex",0,-1);
   SaveLineAttributes(out,"splinex",0,0,0);

   for (Int_t i=0;i<GetNofControlPoints();i++) {
      Double_t x, y;
      Float_t sf;
      GetControlPoint(i, &x, &y, &sf);
      out<<"   splinex->SetControlPoint("<< i << "," << x << "," << y << ","<< sf <<");"<<endl;
   }
   out<<"   splinex->SetRailwayGage("<<GetRailwayGage()<<");"<<endl;
   out<<"   splinex->SetSleeperLength("<<GetSleeperLength()<<");"<<endl;
   out<<"   splinex->SetSleeperDist("<<GetSleeperDist()<<");"<<endl;
   if (fPaintArrowAtStart)
      out<<"   splinex->AddArrow(0,"
   << fArrowLength << ","
   << fArrowAngle << ","
   << fArrowIndentAngle << ","
   << fArrowFill << ");" << endl;
   if (fPaintArrowAtEnd)
      out<<"   splinex->AddArrow(1,"
   << fArrowLength << ","
   << fArrowAngle << ","
   << fArrowIndentAngle << ","
   << fArrowFill << ");" << endl;

//   out<<"   splinex->("<<
   out<<"   splinex->Draw("
      <<quote<<quote<<");"<<endl;
//  Draw must be done before to add possible rails
   Bool_t to_pgr_filled = kFALSE;
   if (fPGraphs.GetEntries() > 0) {
      for (Int_t i = 0; i <= fPGraphs.GetLast(); i++) {
         ParallelGraph * gr = (ParallelGraph*)fPGraphs[i];
         gPad->GetListOfPrimitives()->Remove(gr);
         if (gr && !gr->GetIsRail()) {
            if (!gROOT->ClassSaved(ParallelGraph::Class()))
               out<<"   ParallelGraph *pgr;" << endl;
            out<<"   pgr = splinex->AddParallelGraph("
               << gr->GetDist() << ","
               << gr->GetLineColor() << ","
               << gr->GetLineWidth() << ","
               << gr->GetLineStyle() << ");" << endl;
            if (gr->GetDistToSlave() != 0) {
               out<<"   pgr->SetDistToSlave(" <<gr->GetDistToSlave()<< ");" <<endl;
               out<<"   pgr->SetFillColor(" <<gr->GetFillColor()<< ");" <<endl;
               out<<"   pgr->SetFillStyle(" <<gr->GetFillStyle()<< ");" <<endl;
               to_pgr_filled = kTRUE;
            }
         }
      }
//    fill to parallel graph if needed
      if (to_pgr_filled) {
         out << "   for (Int_t i = 0; i <= splinex->GetParallelGraphs()->GetLast(); i++) {" << endl;
         out << "     pgr = (ParallelGraph*)(*(splinex->GetParallelGraphs()))[i];" << endl;
         out << "     if (pgr->GetDistToSlave() != 0) pgr->FillToSlave(pgr->GetDistToSlave());" << endl;
         out << "   }" << endl;
      }
   }
//   RemoveControlPoints();
//   RemovePolyLines();
//   gPad->GetListOfPrimitives()->ls();
}

//___________________________________________________________________________
//
// ControlGraph, a helper class for TSplineXs,
// To profit from TGraphs methods the Controlpoints are stored
// as a TGraph
//
ControlGraph::ControlGraph (Int_t npoints, Double_t*  x, Double_t* y) :
              TGraph(npoints, x, y),
              fParent(0), fShapeFactors(0), fSelectedPoint (-1), fSelectedX(0),  fSelectedY(0),
              fSelectedShapeFactor(-1),fMixerValues(0), fMixerMinval(0),
              fMixerMaxval(0), fMixerFlags(0), fRowlab(NULL)
{
};
//______________________________________________________________________________

Int_t ControlGraph::DistancetoPrimitive(Int_t px, Int_t py)
{
   // Compute distance from point px,py to points of a graph only.
   // Take care that the smooth curve or the area between rails
   // of the spline takes preference
   //
   Int_t distance;

   // Somewhere on the graph points?
   const Int_t big = 9999;
	const Int_t small = 1;
	const Int_t kMaxDiff = 3;

   // check if point is near one of the graph points
   Int_t i, pxp, pyp, d;
   distance = big;

   for (i=0;i<fNpoints;i++) {
      pxp = gPad->XtoAbsPixel(gPad->XtoPad(fX[i]));
      pyp = gPad->YtoAbsPixel(gPad->YtoPad(fY[i]));
      d   = TMath::Abs(pxp-px) + TMath::Abs(pyp-py);
      if (d < distance) distance = d;
   }
   if (gDebug == 3) {
      cout << "ControlGraph::DistancetoPrimitive  "<< px << " " << py << " "  << distance << endl;
      cout << "fParent::DistancetoPrimitive  " << fParent->DistancetoPrimitive(px, py) << endl;
   }
//   if (distance < kMaxDiff) return distance;
   if (distance < kMaxDiff) return small;
   // Take care that the smooth curve or the area between rails
// of the spline takes preference
//
//   if (fParent->DistancetoPrimitive(px, py) <= kMaxDiff)  return big;
//   else                     return big;
//   for (i=0;i<fNpoints-1;i++) {
//      d = DistancetoLine(px, py, gPad->XtoPad(fX[i]), gPad->YtoPad(fY[i]), gPad->XtoPad(fX[i+1]), gPad->YtoPad(fY[i+1]));
//      if (d < distance) distance = d;
//   }
//   return distance;
	return big;
}
//_____________________________________________________________________________________

void TSplineX::ExecuteEvent(Int_t event, Int_t px, Int_t py)
{
//
//  The spline is shifted by shifting its ControlPoints
//
    fCPGraph.ExecuteEvent(event, px, py);
}
//_____________________________________________________________________________________

void TSplineX::Pop()
{
//
// Pop also its parallel graphs
//
   TObject::Pop();
   for (Int_t i = 0; i < fPGraphs.GetEntries(); i++) {
      TGraph* gr = (TGraph*)fPGraphs[i];
      if (gr) gr->TObject::Pop();
   }
}
//_____________________________________________________________________________________

void ControlGraph::ExecuteEvent(Int_t event, Int_t px, Int_t py) {
//
	Double_t d;
	if (gDebug > 0 && event != 21 && event != 51) 
		std::cout << "ControlGraph::ExecuteEvent " <<event << std::endl;
   if (event == kButton3Down) {
//      cout << "kButton3Down " << endl;
       for (Int_t i=0; i < fNpoints; i++) {
          d = TMath::Sqrt(TMath::Power(px -  gPad->XtoAbsPixel(fX[i]), 2)
                     + TMath::Power(py - gPad->YtoAbsPixel(fY[i]), 2));
//         cout << d << endl;
         if (d < 10) {
            fSelectedX = fX[i];
            fSelectedY = fY[i];
            fSelectedPoint = i;
            fSelectedShapeFactor = fShapeFactors[i];
            break;
         }
      }
   } else if (event == kButton3Up) {
      cout << "kButton3Up " << endl;

   } else {
		Int_t pxc = px;
		Int_t pyc = py;
/*		if (event == kButton1Down) {
			Int_t iclosest = 0;
			Double_t closest = 1e20;
			
			for (Int_t i=0; i < fNpoints; i++) {
				d = TMath::Sqrt(TMath::Power(px -  gPad->XtoAbsPixel(fX[i]), 2)
								+ TMath::Power(py - gPad->YtoAbsPixel(fY[i]), 2));
				if (d < closest) {
					closest = d;
					iclosest = i;
				}
			}
			pxc = gPad->XtoAbsPixel(fX[iclosest]);
			pyc = gPad->YtoAbsPixel(fY[iclosest]);
			if ( gDebug == 1 ) {
				cout << "ControlGraph::ExecuteEvent " 
						<< px << " " << py << " pxc, pyc "
						<< pxc << " " << pyc <<endl;
			}
		}*/
		TGraph::ExecuteEvent(event, pxc, pyc);
   }
   if (event == kButton1Up && fParent) fParent->ComputeSpline();
 //  cout <<  "exit ExecuteEvent " << fParent->GetFillColor() << endl;
}
//_____________________________________________________________________________________

void ControlGraph::SetParent(TSplineX* parent)
{
  fParent = parent;
}
//_____________________________________________________________________________________

void ControlGraph::SetAllShapeFactors(Int_t npoints, Float_t* sfactors)
{
   fShapeFactors.Set(npoints, sfactors);
}
//_____________________________________________________________________________________

void ControlGraph::SetOneShapeFactor(Int_t ipoint, Double_t x, Double_t y, Float_t sfactor)
{
// This method is invoked from the ContextMenu
//
   if (!fParent) return;
   if (ipoint >=0 && ipoint < fShapeFactors.GetSize() ) {
      fShapeFactors[ipoint] = sfactor;
      SetPoint(ipoint, x, y);
      cout << "Set ShapeFactor[" << ipoint << "] = " << sfactor << endl;
      fParent->ComputeSpline();
   }
   fSelectedShapeFactor = sfactor;
   fSelectedPoint  = ipoint;
}
//_____________________________________________________________________________________

void ControlGraph::GetControlPoint(Int_t ipoint, Double_t *x, Double_t *y, Float_t *sfactor)
{
   if (ipoint >=0 && ipoint < fShapeFactors.GetSize() ) {
      *sfactor = fShapeFactors[ipoint];
      *x = fX[ipoint];
      *y = fY[ipoint];
   }
}
//_____________________________________________________________________________________

void ControlGraph::SetControlPoint(Int_t ipoint, Double_t x, Double_t y, Float_t sfactor)
{
   if (ipoint >=0 && ipoint < fShapeFactors.GetSize() ) {
      fShapeFactors[ipoint] = sfactor;
      SetPoint(ipoint, x, y);
//      fParent->ComputeSpline();
   }
}
//_____________________________________________________________________________________

void ControlGraph::SetShapeFactor(Int_t ip, Int_t val)
// this is invoked from ControlGraphMixer
// val = shapefactor * 100
{
   if (ip >= 0 && ip < fShapeFactors.GetSize()) {
      fShapeFactors[ip] = (Float_t)val / 100.;
      fParent->ComputeSpline();
   }
}
//_____________________________________________________________________________________
Int_t ControlGraph::InsertPoint()
{
// This method is invoked from the ContextMenu
// When inserting a point also a ShapeFactor must be inserted
// for this point


   Int_t npoints_before =  GetN();
   Int_t ipoint = TGraph::InsertPoint();
   cout << "CG_InsertPoint() "  <<ipoint << endl;
   if (GetN() > npoints_before) {
      TArrayF temp(fShapeFactors);
      fShapeFactors.Set(GetN());
      for (Int_t i = 0; i < ipoint; i++) fShapeFactors[i] = temp[i];
      fShapeFactors[ipoint] = 0.5;
      for (Int_t i = ipoint +1 ; i < fShapeFactors.GetSize(); i++) {
         fShapeFactors[i] = temp[i - 1];
      }
      fParent->ComputeSpline();
   }
   return ipoint;
}
//_____________________________________________________________________________________

Int_t ControlGraph::RemovePoint()
{
// This method is invoked from the ContextMenu
// When removing a point also its ShapeFactor must be removed
   Int_t npoints_before =  GetN();
   cout << "ControlGraph::RemovePoint() at: " << gPad->GetEventX()
        << " " << gPad->GetEventY() << endl;

   Int_t ipoint = TGraph::RemovePoint();
   cout << "ipoint: " << ipoint << endl;

   if (GetN() < npoints_before) {
      TArrayF temp(fShapeFactors);
      fShapeFactors.Set(GetN());
      for (Int_t i = 0; i < ipoint; i++) fShapeFactors[i] = temp[i];
//      fShapeFactors[ipoint] = 0.5;
      for (Int_t i = ipoint; i < fShapeFactors.GetSize(); i++) {
         fShapeFactors[i] = temp[i + 1];
      }
      fParent->ComputeSpline();
   }
   return ipoint;
}
//_____________________________________________________________________________________

void ControlGraph::ControlGraphMixer()
{
// This method is invoked from the ContextMenu
// It presents a widget with sliders to control the
// shapefactors

   if (!fParent) return;
   if (fRowlab == NULL)
		fRowlab = new TOrdCollection;
	else
		fRowlab->Delete();
   Int_t nrows = GetN();
	fMixerValues.Set(nrows);
	fMixerMinval.Set(nrows);
	fMixerMaxval.Set(nrows);
	fMixerFlags.Set(nrows);
   Double_t * x = GetX();
   Double_t * y = GetY();
   for(Int_t i = 0; i < nrows; i++) {
      fMixerValues[i] = (Int_t) (100 * fShapeFactors[i]);
      fMixerMinval[i] = -100;
      fMixerMaxval[i] =  100;
      fMixerFlags[i] = 0;
      if (fParent->GetIsClosed()) {
         fMixerFlags[i] = 2;
      } else {
         if (i != 0 && i != nrows-1)fMixerFlags[i] = 2;
      }
      Int_t ix = (Int_t)x[i];
      Int_t iy = (Int_t)y[i];
      fRowlab->Add(new TObjString(Form("%d, %d", ix, iy)));
   }
   TGMrbSliders * sliders = new TGMrbSliders("ControlGraphMixer", nrows,
                       fMixerMinval.GetArray(), fMixerMaxval.GetArray(), fMixerValues.GetArray(),
                       fRowlab, fMixerFlags.GetArray(), NULL);
   sliders->Connect("SliderEvent(Int_t, Int_t)",this->ClassName() , this,
               "SetShapeFactor(Int_t, Int_t)");
}
//_____________________________________________________________________________________

PolyLineNoEdit::PolyLineNoEdit(Int_t np, Double_t * x, Double_t * y)
                : TPolyLine(np, x, y)
{
   // This is a special polyline which cannot be picked
   // with the mouse and which is not written out by SavePrimitive
}
//_____________________________________________________________________________________

RailwaySleeper::RailwaySleeper(Double_t * x, Double_t * y,
                TSplineX * parent, Color_t color, Int_t np)
                : TPolyLine(np, x, y), fParent(parent)
{
   // railway sleepers are special filled polylines,
   // its ExecuteEvent method is diverted to its TSplineX parent
	SetLineColor(color);
	if ( np == 5) {
		SetFillColor(color);
		SetFillStyle(1003);
		Paint("f");
	} else {
		SetLineWidth(parent->GetSleeperWidth());
		Paint("l");
	}
}
//_____________________________________________________________________________________
void RailwaySleeper::ExecuteEvent(Int_t event, Int_t px, Int_t py)
{
   if (fParent) fParent->ExecuteEvent(event, px,py);
}
//_____________________________________________________________________________________

void RailwaySleeper::Draw(Option_t * opt)
{
//   TPolyLine::Draw(opt);
	TPolyLine::Paint(opt);
}

//_____________________________________________________________________________________
//
//
ParallelGraph::ParallelGraph ()
               : fParent(0),fSlave(0),fDistToSlave(0), fMaster(0), fDist(0),fClosed(1), fIsRail(0)
{
}
//_____________________________________________________________________________________
//
//
ParallelGraph::ParallelGraph (TSplineX *ograph, Double_t dist, Bool_t closed)
               : fParent(ograph),fSlave(0),fDistToSlave(0), fMaster(0), fDist(dist),fClosed(closed), fIsRail(0)
{
}
//_____________________________________________________________________________________

ParallelGraph::~ParallelGraph()
{
   if (fMaster) fMaster->SetSlave(NULL);
   if (fParent) fParent->RecursiveRemove(this);
};

//_____________________________________________________________________________________

void ParallelGraph::Compute()
{
   Int_t n = fParent->GetLastPoint() + 1;
   if (n <= 0)fParent->ComputeSpline();

   if (n <= 2) {
      cout << "A TSplineX must have minimum 3 points" << endl;
      return;
   }
   SetPolyLine(n);
   Double_t phi1, phi2;
   Double_t* xo = fParent->GetX();
   Double_t* yo = fParent->GetY();
   Double_t* xp = this->GetX();
   Double_t* yp = this->GetY();

   if (fClosed) {
      Double_t d = TMath::Sqrt((xo[0]- xo[n-1])*(xo[0]- xo[n-1])
                             + (yo[0]- yo[n-1])*(yo[0]- yo[n-1]));
      if (d > 0.001 * TMath::Abs(fDist)) {
         cout << "Closed graph: 1. and last point dont coincide" << endl;
         return;
      }
   }
   if (fClosed) {
      phi1 =  fParent->PhiOfLine(xo[n-2], yo[n-2], xo[n-1], yo[n-1]);
      phi2 =  fParent->PhiOfLine(xo[0], yo[0], xo[1], yo[1]);
       fParent->Midpoint(phi1, phi2, xo[0], yo[0], fDist, &xp[0], &yp[0]);
      xp[n-1] = xp[0];
      yp[n-1] = yp[0];
   } else {
      phi1 =  fParent->PhiOfLine(xo[0], yo[0], xo[1], yo[1]);
       fParent->Endpoint(phi1, xo[0], yo[0], fDist, &xp[0], &yp[0]);
      phi1 =  fParent->PhiOfLine(xo[n-2], yo[n-2], xo[n-1], yo[n-1]);
       fParent->Endpoint(phi1, xo[n-1], yo[n-1], fDist, &xp[n-1], &yp[n-1]);
   }
   for (Int_t i = 1; i <= n - 2; i++) {
      phi1 =  fParent->PhiOfLine(xo[i-1], yo[i-1], xo[i], yo[i]);
      phi2 =  fParent->PhiOfLine(xo[i], yo[i], xo[i+1], yo[i+1]);
       fParent->Midpoint(phi1, phi2, xo[i], yo[i], fDist, &xp[i], &yp[i]);
   }
};
//_____________________________________________________________________________________

void ParallelGraph::CorrectForArrows(Double_t rxy, Double_t alength,Double_t aangle,Double_t aindent_angle,
                                     Bool_t at_start, Bool_t at_end)
{
// if a TSlineX has parallels their lengths must be adjusted
// to account for the lengths of the arrows
//  no arrows with railway sleepers

//   if (fSleeperLength > 0 && fSleeperDist > 0) return kTRUE;

   Double_t chop, seglen, xm, ym;
   Int_t ip = 0;

   Double_t* xp = this->GetX();
   Double_t* yp = this->GetY();
   Int_t n = fParent->GetLastPoint() + 1;
   Double_t reallength = fParent->GetArrowLength(fDist, alength, aangle, aindent_angle);
//   cout << "CorrectForArrows:  " << alength << " " << reallength << endl;

   if (at_end) {
      chop = reallength ;
      ip = n - 1;
      Double_t phi = fParent->PhiOfLine(xp[ip-1], yp[ip-1], xp[ip], yp[ip] );
      Double_t c2 = TMath::Cos(phi)* TMath::Cos(phi);
      Double_t s2 = TMath::Sin(phi)* TMath::Sin(phi);
      chop = chop * TMath::Sqrt(c2 + rxy * rxy * s2);
      while (1) {
         seglen = fParent->Length(xp[ip], yp[ip], xp[ip-1], yp[ip-1] );
         if (chop <= seglen) {
            xm = xp[ip-1] + (xp[ip] - xp[ip-1]) * (1 - chop/seglen);
            ym = yp[ip-1] + (yp[ip] - yp[ip-1]) * (1 - chop/seglen);
            xp[ip] = xm;
            yp[ip] = ym;
            this->SetPolyLine(ip+1);
            break;
         } else {
            chop -= seglen;
            ip -= 1;
            if (ip < 1) break;
         }
      }
   }
   xp = this->GetX();
   yp = this->GetY();
   n = this->GetLastPoint()+1;
   if (at_start) {
      chop = reallength;
      ip = 0;
      Double_t phi = fParent->PhiOfLine(xp[ip-1], yp[ip-1], xp[ip], yp[ip] );
      Double_t c2 = TMath::Cos(phi)* TMath::Cos(phi);
      Double_t s2 = TMath::Sin(phi)* TMath::Sin(phi);
      chop = chop * TMath::Sqrt(c2 + rxy * rxy * s2);
      while (1) {
         seglen = fParent->Length(xp[ip], yp[ip], xp[ip+1], yp[ip+1] );
//         cout << ip << " " << yp[ip] << " " << chop << " " << seglen << " " << endl;
         if (chop <= seglen) {
            xm = xp[ip] + (xp[ip+1] - xp[ip]) * (chop/seglen);
            ym = yp[ip] + (yp[ip+1] - yp[ip]) * (chop/seglen);
            xp[ip] = xm;
            yp[ip] = ym;
            break;
         } else {
            chop -= seglen;
            ip += 1;
            if (ip >= n-1) break;
         }
      }
      if (ip > 0) {
         Int_t nn = n - ip;
         for (Int_t i = 0; i < nn ; i++) {
            xp[i] = xp[ip];
            yp[i] = yp[ip];
            ip++;
         }
//         cout << "nn " << nn << endl;
         this->SetPolyLine(nn);
      }
   }
   return;
}

//_____________________________________________________________________________________

void ParallelGraph::Remove(Option_t * /*opt*/)
{
   if (fMaster) fMaster->SetSlave(NULL);
   delete this;
}
//_____________________________________________________________________________________

void ParallelGraph::ExecuteEvent(Int_t event, Int_t px, Int_t py) {
//  execute
   if (fParent) fParent->ExecuteEvent(event, px, py);
}
//_____________________________________________________________________________________

Int_t ParallelGraph::DistancetoPrimitive(Int_t px, Int_t py)
{
   Int_t distance;

   const Int_t big = 9999;
   const Int_t kMaxDiff = 10;

   // check if point is near one of the graph points
   Int_t i, pxp, pyp, d;
   distance = big;
   if (fIsRail) {
      return big;
   } else {
   // check if point is near one of the graph points
   	for (i=0; i < GetLastPoint()+1; i++) {
      	pxp = gPad->XtoAbsPixel(gPad->XtoPad(fX[i]));
      	pyp = gPad->YtoAbsPixel(gPad->YtoPad(fY[i]));
      	d   = TMath::Abs(pxp-px) + TMath::Abs(pyp-py);
      	if (d < distance) distance = d;
   	}
      if (distance < kMaxDiff) return distance;
   // check if point is near the connecting line
   	for (i=0; i < GetLastPoint() +1 -1; i++) {
      	d = DistancetoLine(px, py, gPad->XtoPad(fX[i]), gPad->YtoPad(fY[i]), gPad->XtoPad(fX[i+1]), gPad->YtoPad(fY[i+1]));
      	if (d < distance) distance = d;
   	}
   }
   if (distance < kMaxDiff) return distance;
   else                     return big;
}
//_____________________________________________________________________________________

void ParallelGraph::ClearFillToSlave()
{
// parallel graphs may be filled to neighbours
   fSlave = NULL;
   if (fSlave) fSlave->SetMaster(NULL);
   fParent->NeedReCompute();
   fParent->Paint();
}
//_____________________________________________________________________________________

void ParallelGraph::FillToSlave(Double_t dist)
{
// parallel graphs may be filled to neighbours
   TObjArray * pgl = fParent->GetParallelGraphs();
   if (pgl->GetSize() < 2) {
      cout << "No Partner available" << endl;
      return;
   }
   Double_t mindist = 1e20;
   TIter next(pgl);
   ParallelGraph *pa;
   while ( (pa = (ParallelGraph *)next()) ) {
      if (pa == this) continue;
      if (dist != 0) {
         if (TMath::Abs(GetDist() -  pa->GetDist() - dist) < 0.1) {
            fSlave = pa;
            fDistToSlave = GetDist() -  pa->GetDist();
            break;
         }
      } else {

         if (TMath::Abs(GetDist() -  pa->GetDist()) < mindist) {
            mindist = TMath::Abs(GetDist() -  pa->GetDist());
            fDistToSlave = GetDist() -  pa->GetDist();
            fSlave = pa;
         }
      }
   }
   cout <<  "ParallelGraph::FillToSlave " << dist << " " << fSlave << endl;
   if (fSlave) fSlave->SetMaster(this);
   fParent->NeedReCompute();
   fParent->Paint();
}
//_____________________________________________________________________________________

void ParallelGraph::Paint(Option_t * /*option*/)
{
   TPolyLine::Paint();
   if (fSlave) {
 //     cout <<  "ParallelGraph::Paint() to fSlave" << endl;
      ParallelGraph * lg = this;
      ParallelGraph * rg = fSlave;
      Int_t npoints = 1;
      npoints += lg->GetLastPoint()+1;
      npoints += rg->GetLastPoint()+1;
      PolyLineNoEdit * pl = new PolyLineNoEdit(npoints);
      Double_t * x = pl->GetX();
      Double_t * y = pl->GetY();
      Double_t * px = rg->GetX();
      Double_t * py = rg->GetY();
      Int_t n = 0;
      for (Int_t i = 0; i < rg->GetLastPoint()+1; i++) {
         pl->SetPoint(n, px[i], py[i]);
         n++;
      }
      px = lg->GetX();
      py = lg->GetY();
      for (Int_t i = lg->GetLastPoint()+1 -1 ; i >= 0; i--) {
//         cout << n<< " " << px[i]<< " " << py[i] << endl;
         pl->SetPoint(n, px[i], py[i]);
         n++;
      }
      pl->SetPoint(n, x[0], y[0]);
      pl->SetLineColor(GetLineColor());
      pl->SetLineWidth(GetLineWidth());
      fParent->GetDPolyLines()->Add(pl);
      pl->SetFillStyle(GetFillStyle());
      pl->SetFillColor(GetFillColor());
      pl->Paint("F");
      gPad->Modified();
      gPad->Update();
   }
}
//_____________________________________________________________________________________

void ParallelGraph::Pop()
{
//
// Pop its parent TSplineX
//
   cout << "ParallelGraph::Pop() "<< endl;
   fParent->Pop();
}
//_____________________________________________________________________________________
