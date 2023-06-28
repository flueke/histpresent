#include "TMath.h"
#include "TNamed.h"
#include "TPad.h"
#include "TView.h"
//#include "TPol3Dm.h"
//#include "TPolyMarker3D.h"
#include "TText3D.h"
#include <map>
#include <iostream>
using namespace std;


static const Int_t gN_A = 5;
static Float_t gX_A[gN_A] = { 0, 4, 7, 6, 1};
static Float_t gY_A[gN_A] = { 0,11, 0, 3, 3};

static const Int_t gN_B = 12;
static Float_t gX_B[gN_B] = { 0, 0, 5, 6, 6, 5, 0, 5, 6, 6, 5, 0};
static Float_t gY_B[gN_B] = { 0,11,11,10, 7, 6, 6, 6, 5, 1, 0, 0};

static const Int_t gN_C = 10;
static Float_t gX_C[gN_C] = { 7, 6, 3, 1, 0, 0, 1, 3, 6, 7};
static Float_t gY_C[gN_C] = { 1, 0, 0, 1, 3, 8,10,11,11,10};

static const Int_t gN_D = 9;
static Float_t gX_D[gN_D] = { 0, 0, 5, 6, 7, 7, 6, 5, 0};
static Float_t gY_D[gN_D] = { 0,11,11,10, 8, 3, 1, 0, 0};

static const Int_t gN_E = 7;
static Float_t gX_E[gN_E] = { 7, 0, 0, 7, 0, 0, 7};
static Float_t gY_E[gN_E] = { 0, 0, 6, 6, 6,11,11}; 

static const Int_t gN_F = 6;
static Float_t gX_F[gN_F] = { 1, 1, 7, 1, 1, 7};
static Float_t gY_F[gN_F] = { 0, 6, 6, 6,11,11};

static const Int_t gN_G = 12;
static Float_t gX_G[gN_G] = { 5, 7, 7, 6, 3, 1, 0, 0, 1, 2, 5, 7};
static Float_t gY_G[gN_G] = { 5, 5, 1, 0, 0, 1, 3, 8,10,11,11,10};

static const Int_t gN_H = 7;
static Float_t gX_H[gN_H] = { 0, 0, 0, 7, 7, 7, 7};
static Float_t gY_H[gN_H] = { 0,11, 6, 6, 0,11, 0};

static const Int_t gN_I = 6;
static Float_t gX_I[gN_I] = { 2, 6, 4, 4, 2, 6};
static Float_t gY_I[gN_I] = { 0, 0, 0,11,11,11};

static const Int_t gN_J = 6;
static Float_t gX_J[gN_J] = { 1, 2, 5, 7, 7, 3};
static Float_t gY_J[gN_J] = { 1, 0, 0, 2,11,11};

static const Int_t gN_K = 6;
static Float_t gX_K[gN_K] = { 0, 0, 0, 6, 0, 6};
static Float_t gY_K[gN_K] = { 0,11, 5,11, 5, 0};

static const Int_t gN_L = 3;
static Float_t gX_L[gN_L] = { 7, 0, 0};
static Float_t gY_L[gN_L] = { 0, 0,11};

static const Int_t gN_M = 5;
static Float_t gX_M[gN_M] = { 0, 0, 4, 7, 7};
static Float_t gY_M[gN_M] = { 0,11, 4,11, 0};

static const Int_t gN_N = 4;
static Float_t gX_N[gN_N] = { 0, 0, 7, 7};
static Float_t gY_N[gN_N] = { 0,11, 0,11};

static const Int_t gN_O = 9;
static Float_t gX_O[gN_O] = { 2, 0, 0, 2, 5, 7, 7, 5, 2};
static Float_t gY_O[gN_O] = { 0, 3, 8,11,11, 8, 3, 0, 0};

static const Int_t gN_P = 7;
static Float_t gX_P[gN_P] = { 0, 0, 5, 7, 7, 5, 0};
static Float_t gY_P[gN_P] = { 0,11,11, 9, 7, 5, 5};

static const Int_t gN_Q = 12;
static Float_t gX_Q[gN_Q] = { 2, 0, 0, 2, 5, 7, 7, 5, 2, 4, 5, 6};
static Float_t gY_Q[gN_Q] = { 0, 2, 9,11,11, 9, 2, 0, 0, 0,-1,-2};

static const Int_t gN_R = 10;
static Float_t gX_R[gN_R] = { 0, 0, 5, 7, 7, 5, 0, 5, 7, 7};
static Float_t gY_R[gN_R] = { 0,11,11, 9, 7, 5, 5, 5, 3, 0};

static const Int_t gN_S = 15;
static Float_t gX_S[gN_S] = { 0, 0, 1, 5, 6, 7, 7, 5, 2, 0, 0, 1, 2, 5, 6};
static Float_t gY_S[gN_S] = { 2, 1, 0, 0, 1, 2, 4, 5, 6, 7, 8,10,11,11,10};

static const Int_t gN_T = 4;
static Float_t gX_T[gN_T] = {4, 4, 0, 8};
static Float_t gY_T[gN_T] = {0,11,11,11};

static const Int_t gN_U = 6;
static Float_t gX_U[gN_U] = { 0, 0, 2, 5, 7, 7};
static Float_t gY_U[gN_U] = {11, 2, 0, 0, 2,11}; 

static const Int_t gN_V = 3 ;
static Float_t gX_V[gN_V] = { 1, 4, 7};
static Float_t gY_V[gN_V] = {11, 0,11};

static const Int_t gN_W = 5;
static Float_t gX_W[gN_W] = { 0, 2, 4, 6, 8};
static Float_t gY_W[gN_W] = {11, 0, 8, 0,11}; 

static const Int_t gN_X = 5;
static Float_t gX_X[gN_X] = { 1, 7, 4, 7, 1};
static Float_t gY_X[gN_X] = { 1,11, 6, 1,11};

static const Int_t gN_Y = 6;
static Float_t gX_Y[gN_Y] = { 0, 4, 7, 4, 1};
static Float_t gY_Y[gN_Y] = {11, 5,11, 5, 0};

static const Int_t gN_Z = 4;
static Float_t gX_Z[gN_Z] = { 0, 7, 0, 7};
static Float_t gY_Z[gN_Z] = {11,11, 0, 0};

static const Int_t gN_a = 12;
static Float_t gX_a[gN_a] = { 7, 2, 0, 0, 2, 5, 7, 7, 7, 5, 2, 1};
static Float_t gY_a[gN_a] = { 5, 5, 4, 1, 0, 0, 1, 0, 7, 8, 8, 7};

static const Int_t gN_b = 10;
static Float_t gX_b[gN_b] = { 0, 0, 5, 6, 7, 7, 6, 4, 2, 0}; 
static Float_t gY_b[gN_b] = {11, 0, 0, 1, 2, 6, 7, 8, 8, 8};

static const Int_t gN_c = 8;
static Float_t gX_c[gN_c] = { 6, 5, 2, 0, 0, 2, 5, 6};
static Float_t gY_c[gN_c] = { 1, 0, 0, 2, 6, 8, 8, 7};

static const Int_t gN_d = 11;
static Float_t gX_d[gN_d] = { 7, 7, 7, 5, 2, 0, 0, 2, 5, 6, 7};
static Float_t gY_d[gN_d] = { 0,11, 1, 0, 0, 2, 6, 8, 8, 7, 6};

static const Int_t gN_e = 10;
static Float_t gX_e[gN_e] = { 7, 6, 2, 0, 0, 2, 5, 7, 7, 0};
static Float_t gY_e[gN_e] = { 1, 0, 0, 2, 6, 8, 8, 6, 4, 4};

static const Int_t gN_f = 8;
static Float_t gX_f[gN_f] = { 3, 3, 1, 6, 3, 3, 4, 6};
static Float_t gY_f[gN_f] = { 0, 8, 8, 8, 8,10,11,11};

static const Int_t gN_g = 15;
static Float_t gX_g[gN_g] = { 1, 2, 5, 7, 7, 7, 6, 5, 2, 0, 0, 2, 5, 6, 7};
static Float_t gY_g[gN_g] = {-2,-3,-3,-1, 8, 6, 7, 8, 8, 6, 2, 0, 0, 1, 1};

static const Int_t gN_h = 8;
static Float_t gX_h[gN_h] = { 0, 0, 0, 1, 2, 5, 6, 6};
static Float_t gY_h[gN_h] = {11, 0, 6, 7, 8, 8, 7, 0};  

static const Int_t gN_i = 8;
static Float_t gX_i[gN_i] = { 0, 6, 3, 3, 1,-5, 3, 3};
static Float_t gY_i[gN_i] = { 0, 0, 0, 8, 8,-5,10,11};
static const Int_t gN_j = 7;
static Float_t gX_j[gN_j] = { 0, 3, 3, 1,-5, 3, 3};
static Float_t gY_j[gN_j] = {-3,-3, 8, 8,-5,10,11};

static const Int_t gN_k = 6;
static Float_t gX_k[gN_k] = { 0, 0, 0, 5, 0, 5};
static Float_t gY_k[gN_k] = { 0,11, 4, 8, 4, 0};

static const Int_t gN_l = 5;
static Float_t gX_l[gN_l] = { 0, 3, 3, 4, 6};
static Float_t gY_l[gN_l] = {11,11, 1, 0, 0};

static const Int_t gN_m = 9;
static Float_t gX_m[gN_m] = { 0, 0, 3, 3, 3, 4, 5, 6, 6};
static Float_t gY_m[gN_m] = { 0, 8, 8, 0, 7, 8, 8, 7, 0};

static const Int_t gN_n = 7;
static Float_t gX_n[gN_n] = { 0, 0, 0, 2, 5, 6, 6};
static Float_t gY_n[gN_n] = { 0, 8, 7, 8, 8, 7, 0};

static const Int_t gN_o = 9;
static Float_t gX_o[gN_o] = { 2, 0, 0, 2, 5, 7, 7, 5, 2};
static Float_t gY_o[gN_o] = { 0, 2, 6, 8, 8, 6, 2, 0, 0};

static const Int_t gN_p = 10;
static Float_t gX_p[gN_p] = { 0, 0, 0, 2, 5, 7, 7, 5, 2, 0};
static Float_t gY_p[gN_p] = {-3, 8, 7, 8, 8, 6, 2, 0, 0, 1};

static const Int_t gN_q = 10;
static Float_t gX_q[gN_q] = { 7, 7, 7, 5, 2, 0, 0, 2, 5, 7};
static Float_t gY_q[gN_q] = {-3, 8, 7, 8, 8, 6, 2, 0, 0, 1};

static const Int_t gN_r = 6;
static Float_t gX_r[gN_r] = { 0, 0, 0, 2, 4, 5};
static Float_t gY_r[gN_r] = { 0, 8, 6, 8, 8, 7};

static const Int_t gN_t = 8;
static Float_t gX_t[gN_t] = { 3, 3, 1, 5, 3, 3, 4, 6};
static Float_t gY_t[gN_t] = {11, 8, 8, 8, 8, 1, 0, 0};

static const Int_t gN_s = 12;
static Float_t gX_s[gN_s] = { 0, 1, 5, 7, 7, 6, 1, 0, 0, 1, 7, 7};
static Float_t gY_s[gN_s] = { 1, 0, 0, 1, 3, 4, 4, 5, 7, 8, 8, 7};

static const Int_t gN_u = 6;
static Float_t gX_u[gN_u] = { 0, 0, 2, 5, 7, 7};
static Float_t gY_u[gN_u] = { 8, 1, 0, 0, 1, 8};

static const Int_t gN_v = 3;
static Float_t gX_v[gN_v] = { 1, 4, 7};
static Float_t gY_v[gN_v] = { 8, 0, 8};

static const Int_t gN_w = 5;
static Float_t gX_w[gN_w] = { 0, 2, 4, 6, 8};
static Float_t gY_w[gN_w] = { 8, 0, 6, 0, 8};

static const Int_t gN_x = 5;
static Float_t gX_x[gN_x] = { 1, 7, 4, 1, 7};
static Float_t gY_x[gN_x] = { 8, 0, 4, 0, 8};

static const Int_t gN_y = 6;
static Float_t gX_y[gN_y] = { 1, 4, 7, 4, 3.5, 2};
static Float_t gY_y[gN_y] = { 8, 0, 8, 0, -1,-3};

static const Int_t gN_z = 4;
static Float_t gX_z[gN_z] = { 0, 7, 0, 7};
static Float_t gY_z[gN_z] = { 8, 8, 0, 0};

static const Int_t gN_0 = 19;
static Float_t gX_0[gN_0] = { 0, 0, 1, 2, 5, 6, 7, 7, 6, 5, 2, 1, 0,-5, 3, 4, 4, 3, 3};
static Float_t gY_0[gN_0] = { 3, 8,10,11,11,10, 8, 3, 1, 0, 0, 1, 3,-5, 6, 6, 5, 5, 6};

static const Int_t gN_1 = 6;
static Float_t gX_1[gN_1] = { 0, 1, 3, 3, 1, 5};
static Float_t gY_1[gN_1] = {10,11,11, 0, 0, 0};

static const Int_t gN_2 = 9;
static Float_t gX_2[gN_2] = { 1, 1, 2, 5, 6, 7, 7, 0, 7};
static Float_t gY_2[gN_2] = { 9,10,11,11,10, 9, 7, 0, 0};

static const Int_t gN_3 = 15;
static Float_t gX_3[gN_3] = { 0, 1, 5, 7, 7, 6, 5, 2, 5, 6, 7, 7, 5, 1, 0};
static Float_t gY_3[gN_3] = {10,11,11, 9, 8, 7, 6, 6, 6, 5, 4, 2, 0, 0, 1};

static const Int_t gN_4 = 5;
static Float_t gX_4[gN_4] = { 7, 0, 0, 5, 5};
static Float_t gY_4[gN_4] = { 3, 3, 4,11, 0};

static const Int_t gN_5 = 9;
static Float_t gX_5[gN_5] = { 0, 1, 4, 6, 6, 4, 1, 1, 6};
static Float_t gY_5[gN_5] = { 1, 0, 0, 2, 5, 7, 7,11,11};

static const Int_t gN_6 = 13;
static Float_t gX_6[gN_6] = { 6, 5, 2, 1, 0, 0, 2, 4, 6, 6, 4, 2, 0};
static Float_t gY_6[gN_6] = {10,11,11,10, 8, 2, 0, 0, 2, 5, 7, 7, 6}; 

static const Int_t gN_7 = 3;
static Float_t gX_7[gN_7] = { 0, 7, 3};
static Float_t gY_7[gN_7] = {11,11, 0};

static const Int_t gN_8 = 17;
static Float_t gX_8[gN_8] = { 2, 0, 0, 2, 5, 7, 7, 5, 2, 0, 0, 2, 5, 7, 7, 5, 2};
static Float_t gY_8[gN_8] = { 0, 2, 4, 6, 6, 8,10,11,11,10, 8, 6, 6, 4, 2, 0, 0};

static const Int_t gN_9 = 13;
static Float_t gX_9[gN_9] = { 1, 2, 5, 6, 7, 7, 5, 3, 1, 1, 3, 5, 7};
static Float_t gY_9[gN_9] = { 1, 0, 0, 1, 3, 9,11,11, 9, 6, 4, 4, 5};

static const Int_t gN_eq = 5;
static Float_t gX_eq[gN_eq] = { 0, 7, -5, 7, 0};
static Float_t gY_eq[gN_eq] = { 3, 3, -5, 6, 6};

static const Int_t gN_minus = 2;
static Float_t gX_minus[gN_minus] = { 1, 5};
static Float_t gY_minus[gN_minus] = { 4, 4};

static const Int_t gN_dot = 5;
static Float_t gX_dot[gN_dot] = { 3, 4, 4, 3, 3};
static Float_t gY_dot[gN_dot] = {-1,-1, 0, 0,-1};

static const Int_t gN_colon = 11;
static Float_t gX_colon[gN_colon] = { 3, 4, 4, 3, 3,-5, 3, 4, 4, 3, 3};
static Float_t gY_colon[gN_colon] = { 1, 1, 0, 0, 1,-5, 5, 5, 4, 4, 5};

static const Int_t gN_semicolon = 6;
static Float_t gX_semicolon[gN_semicolon] = { 1, 3, 3,-5, 3, 3};
static Float_t gY_semicolon[gN_semicolon] = {-3, 0, 1,-5, 3, 4};

static const Int_t gN_comma = 3;
static Float_t gX_comma[gN_comma] = { 1, 3, 3};
static Float_t gY_comma[gN_comma] = {-3, 0, 1};

static const Int_t gN_slash = 2;
static Float_t gX_slash[gN_slash] = { 2, 6};
static Float_t gY_slash[gN_slash] = {-1,11};
static const Int_t gN_bslash = 2;
static Float_t gX_bslash[gN_bslash] = { 6, 2};
static Float_t gY_bslash[gN_bslash] = {-1,11};

static const Int_t gN_dollar = 15;
static Float_t gX_dollar[gN_dollar] = { 0, 1, 5, 6, 6, 5, 1, 0, 0, 1, 5, 6,-5, 3, 3};
static Float_t gY_dollar[gN_dollar] = { 1, 0, 0, 1, 3, 4, 5, 6, 8, 9, 9, 8,-5,-2,11};

static const Int_t gN_hash = 11;
static Float_t gX_hash[gN_hash] = { 1, 4,-5, 4, 7,-5, 0, 7,-5, 1, 8};
static Float_t gY_hash[gN_hash] = { 0,10,-5, 0,10,-5, 3, 3,-5, 7, 7};

static const Int_t gN_at = 21;
static Float_t gX_at[gN_at] = { 7, 7,-5,7, 6, 5, 4, 4, 5, 7, 9, 9, 7, 5, 3, 2, 2, 3, 4, 6, 8};
static Float_t gY_at[gN_at] = { 6, 1,-5,6, 7, 7, 6, 1, 0, 0, 1, 7, 9,10, 9, 6,-1,-2,-3,-3,-2};

static const Int_t gN_mul = 14;
static Float_t gX_mul[gN_mul] = {    1,3,-5, 3, 3,-5, 5, 3,-5, 1.5, 3,-5, 4.5, 3};
static Float_t gY_mul[gN_mul] = { 5.5, 4,-5, 7, 4,-5, 5.5, 4,-5, 2, 4,-5, 2, 4};

static const Int_t gN_us = 2;
static Float_t gX_us[gN_us] = { 0, 7};
static Float_t gY_us[gN_us] = {-1,-1};

static const Int_t gN_plus = 5;
static Float_t gX_plus[gN_plus] = {1, 7,-5, 4, 4};
static Float_t gY_plus[gN_plus] = {4, 4,-5, 0, 7};

static const Int_t gN_exl = 5;
static Float_t gX_exl[gN_exl] = { 3, 3,-5, 3, 3};
static Float_t gY_exl[gN_exl] = { 0, 1,-5, 3,10};

static const Int_t gN_quest = 11;
static Float_t gX_quest[gN_quest] = { 4, 4,-5, 4, 4, 6, 6, 5, 3, 2, 1};
static Float_t gY_quest[gN_quest] = {-1, 0,-5, 2, 4, 5, 8,10,10, 9, 7};

static const Int_t gN_paleft = 6;
static Float_t gX_paleft[gN_paleft] = { 4, 3, 2, 2, 3, 4};
static Float_t gY_paleft[gN_paleft] = {-2, 0, 3, 6, 9,11};

static const Int_t gN_paright = 6 ;
static Float_t gX_paright[gN_paright] = { 2, 3, 4, 4, 3, 2};
static Float_t gY_paright[gN_paright] = {-2, 0, 3, 6, 9,11};

static const Int_t gN_brleft = 4;
static Float_t gX_brleft[gN_brleft] = { 4, 2, 2, 4};
static Float_t gY_brleft[gN_brleft] = {-2, -2, 11,11};

static const Int_t gN_brright = 4 ;
static Float_t gX_brright[gN_brright] = { 2, 4, 4, 2};
static Float_t gY_brright[gN_brright] = {-2,-2,11,11};

static const Float_t gChHeight = 11;
static const Float_t gChWidth = 9;

typedef map<char, Float_t *> coords;
typedef map<char, Int_t>      ccount;

coords xchar; 
coords ychar; 
ccount nchar;

ClassImp(TText3D)
ClassImp(TPol3Dtext)

TPol3Dtext::TPol3Dtext(Int_t n, TText3D *text)
:  TPolyLine3D(n) , fText(text)
{
	fTitle = text->GetText();
}
;
TPol3Dtext::~TPol3Dtext()
{}
;
const char * TPol3Dtext::GetText() const {
	printf ("TPol3Dtext::GetText(): %s\n",  fText->GetText());
	return fText->GetText();
};            
void TPol3Dtext::SetText(const char * text) {
	printf ("TPol3Dtext::SetText(): %p\n",  fText);
//	Dump();
	fText->SetText(text); 
//	fText->Compute(); 
//	fText->Paint();
	gPad->Modified();
	gPad->Update();
};
//____________________________________________________________________

TText3D::TText3D( Double_t x, Double_t y, Double_t z, 
			Double_t dx, Double_t dy, Double_t dz, const char * text)
			: TPolyLine3D(1), fText(text),fX(x), fY(y), fZ(z),fDirX(dx), fDirY(dy), fDirZ(dz)
			
{
	fListOfLetters = new TList();
	fListOfLetters->SetOwner();
	printf ("ctor TText3D: %p %p\n", this, fListOfLetters);
	fTextSize = 0.08;
	fTextColor = kBlack;
	fLineWidth = 3;
	SetName("TText3D");
	SetTitle("TText3D");
	xchar['A'] = gX_A;	ychar['A'] = gY_A;	nchar['A'] = gN_A;
	xchar['B'] = gX_B;	ychar['B'] = gY_B;	nchar['B'] = gN_B;
	xchar['C'] = gX_C;	ychar['C'] = gY_C;	nchar['C'] = gN_C;
	xchar['D'] = gX_D;	ychar['D'] = gY_D;	nchar['D'] = gN_D;
	xchar['E'] = gX_E;	ychar['E'] = gY_E;	nchar['E'] = gN_E;
	xchar['F'] = gX_F;	ychar['F'] = gY_F;	nchar['F'] = gN_F;
	xchar['G'] = gX_G;	ychar['G'] = gY_G;	nchar['G'] = gN_G;
	xchar['H'] = gX_H;	ychar['H'] = gY_H;	nchar['H'] = gN_H;
	xchar['I'] = gX_I;	ychar['I'] = gY_I;	nchar['I'] = gN_I;
	xchar['J'] = gX_J;	ychar['J'] = gY_J;	nchar['J'] = gN_J;
	xchar['K'] = gX_K;	ychar['K'] = gY_K;	nchar['K'] = gN_K;
	xchar['L'] = gX_L;	ychar['L'] = gY_L;	nchar['L'] = gN_L;
	xchar['M'] = gX_M;	ychar['M'] = gY_M;	nchar['M'] = gN_M;
	xchar['N'] = gX_N;	ychar['N'] = gY_N;	nchar['N'] = gN_N;
	xchar['O'] = gX_O;	ychar['O'] = gY_O;	nchar['O'] = gN_O;
	xchar['P'] = gX_P;	ychar['P'] = gY_P;	nchar['P'] = gN_P;
	xchar['Q'] = gX_Q;	ychar['Q'] = gY_Q;	nchar['Q'] = gN_Q;
	xchar['R'] = gX_R;	ychar['R'] = gY_R;	nchar['R'] = gN_R;
	xchar['S'] = gX_S;	ychar['S'] = gY_S;	nchar['S'] = gN_S;
	xchar['T'] = gX_T;	ychar['T'] = gY_T;	nchar['T'] = gN_T;
	xchar['U'] = gX_U;	ychar['U'] = gY_U;	nchar['U'] = gN_U;
	xchar['V'] = gX_V;	ychar['V'] = gY_V;	nchar['V'] = gN_V;
	xchar['W'] = gX_W;	ychar['W'] = gY_W;	nchar['W'] = gN_W;
	xchar['X'] = gX_X;	ychar['X'] = gY_X;	nchar['X'] = gN_X;
	xchar['Y'] = gX_Y;	ychar['Y'] = gY_Y;	nchar['Y'] = gN_Y;
	xchar['Z'] = gX_Z;	ychar['Z'] = gY_Z;	nchar['Z'] = gN_Z;
	
	// lower case
	
	xchar['a'] = gX_a;	ychar['a'] = gY_a;	nchar['a'] = gN_a;
	xchar['b'] = gX_b;	ychar['b'] = gY_b;	nchar['b'] = gN_b;
	xchar['c'] = gX_c;	ychar['c'] = gY_c;	nchar['c'] = gN_c;
	xchar['d'] = gX_d;	ychar['d'] = gY_d;	nchar['d'] = gN_d;
	xchar['e'] = gX_e;	ychar['e'] = gY_e;	nchar['e'] = gN_e;
	xchar['f'] = gX_f;	ychar['f'] = gY_f;	nchar['f'] = gN_f;
	xchar['g'] = gX_g;	ychar['g'] = gY_g;	nchar['g'] = gN_g;
	xchar['h'] = gX_h;	ychar['h'] = gY_h;	nchar['h'] = gN_h;
	xchar['i'] = gX_i;	ychar['i'] = gY_i;	nchar['i'] = gN_i;
	xchar['j'] = gX_j;	ychar['j'] = gY_j;	nchar['j'] = gN_j;
	xchar['k'] = gX_k;	ychar['k'] = gY_k;	nchar['k'] = gN_k;
	xchar['l'] = gX_l;	ychar['l'] = gY_l;	nchar['l'] = gN_l;
	xchar['m'] = gX_m;	ychar['m'] = gY_m;	nchar['m'] = gN_m;
	xchar['n'] = gX_n;	ychar['n'] = gY_n;	nchar['n'] = gN_n;
	xchar['o'] = gX_o;	ychar['o'] = gY_o;	nchar['o'] = gN_o;
	xchar['p'] = gX_p;	ychar['p'] = gY_p;	nchar['p'] = gN_p;
	xchar['q'] = gX_q;	ychar['q'] = gY_q;	nchar['q'] = gN_q;
	xchar['r'] = gX_r;	ychar['r'] = gY_r;	nchar['r'] = gN_r;
	xchar['s'] = gX_s;	ychar['s'] = gY_s;	nchar['s'] = gN_s;
	xchar['t'] = gX_t;	ychar['t'] = gY_t;	nchar['t'] = gN_t;
	xchar['u'] = gX_u;	ychar['u'] = gY_u;	nchar['u'] = gN_u;
	xchar['v'] = gX_v;	ychar['v'] = gY_v;	nchar['v'] = gN_v;
	xchar['w'] = gX_w;	ychar['w'] = gY_w;	nchar['w'] = gN_w;
	xchar['x'] = gX_x;	ychar['x'] = gY_x;	nchar['x'] = gN_x;
	xchar['y'] = gX_y;	ychar['y'] = gY_y;	nchar['y'] = gN_y;
	xchar['z'] = gX_z;	ychar['z'] = gY_z;	nchar['z'] = gN_z;
	xchar['z'] = gX_z;	ychar['z'] = gY_z;	nchar['z'] = gN_z;
	xchar['0'] = gX_0;	ychar['0'] = gY_0;	nchar['0'] = gN_0;
	xchar['1'] = gX_1;	ychar['1'] = gY_1;	nchar['1'] = gN_1;
	xchar['2'] = gX_2;	ychar['2'] = gY_2;	nchar['2'] = gN_2;
	xchar['3'] = gX_3;	ychar['3'] = gY_3;	nchar['3'] = gN_3;
	xchar['4'] = gX_4;	ychar['4'] = gY_4;	nchar['4'] = gN_4;
	xchar['5'] = gX_5;	ychar['5'] = gY_5;	nchar['5'] = gN_5;
	xchar['6'] = gX_6;	ychar['6'] = gY_6;	nchar['6'] = gN_6;
	xchar['7'] = gX_7;	ychar['7'] = gY_7;	nchar['7'] = gN_7;
	xchar['8'] = gX_8;	ychar['8'] = gY_8;	nchar['8'] = gN_8;
	xchar['9'] = gX_9;	ychar['9'] = gY_9;	nchar['9'] = gN_9;

	xchar['='] = gX_eq;	ychar['='] = gY_eq;	nchar['='] = gN_eq;
	xchar['.'] = gX_dot; ychar['.'] = gY_dot; nchar['.'] = gN_dot;	
	xchar[','] = gX_comma; ychar[','] = gY_comma; nchar[','] = gN_comma;	
	xchar['-'] = gX_minus; ychar['-'] = gY_minus; nchar['-'] = gN_minus;
	xchar['/'] = gX_slash; ychar['/'] = gY_slash; nchar['/'] = gN_slash;
	xchar['\\'] = gX_bslash; ychar['\\'] = gY_bslash; nchar['\\'] = gN_bslash;
	xchar['@'] = gX_at; ychar['@'] = gY_at; nchar['@'] = gN_at;
	xchar['*'] = gX_mul; ychar['*'] = gY_mul; nchar['*'] = gN_mul;
	xchar['_'] = gX_us; ychar['_'] = gY_us; nchar['_'] = gN_us;
	xchar['#'] = gX_hash; ychar['#'] = gY_hash; nchar['#'] = gN_hash;
	xchar['('] = gX_paleft; ychar['('] = gY_paleft; nchar['('] = gN_paleft;
	xchar[')'] = gX_paright; ychar[')'] = gY_paright; nchar[')'] = gN_paright;
	xchar['['] = gX_brleft; ychar['['] = gY_brleft; nchar['['] = gN_brleft;
	xchar[']'] = gX_brright; ychar[']'] = gY_brright; nchar[']'] = gN_brright;
	xchar['$'] = gX_dollar; ychar['$'] = gY_dollar; nchar['$'] = gN_dollar;
	xchar[';'] = gX_semicolon; ychar[';'] = gY_semicolon; nchar[';'] = gN_semicolon;
	xchar[':'] = gX_colon; ychar[':'] = gY_colon; nchar[':'] = gN_colon;
	xchar['+'] = gX_plus; ychar['+'] = gY_plus; nchar['+'] = gN_plus;
	xchar['!'] = gX_exl; ychar['!'] = gY_exl; nchar['!'] = gN_exl;
	xchar['?'] = gX_quest; ychar['?'] = gY_quest; nchar['?'] = gN_quest;
	
};
//______________________________________________________________
void TText3D::Paint(const char * /*option*/)
{
	TIter next(fListOfLetters);
	TObject *obj;
	while ( (obj = next()) ) {
		TPolyLine3D * pl = (TPolyLine3D*)obj;
		pl->SetLineColor(fTextColor);
		pl->SetLineWidth(fLineWidth);
		pl->TPolyLine3D::Paint();
	}
//	this->TPolyLine3D::Paint();
};
//______________________________________________________________
TText3D::~TText3D()
{
	fListOfLetters->Delete();
};
//______________________________________________________________

Int_t TText3D::DistancetoPrimitive(Int_t px, Int_t py)
{
	TIter next(fListOfLetters);
	TObject *obj;
	Int_t mdist = 10000000;
	while ( (obj = next()) ) {
		Int_t d = ((TPolyLine3D*)obj)->DistancetoPrimitive(px, py);
		if ( d < mdist )
			mdist = d; 
	}
	return mdist;
}
//______________________________________________________________

void TText3D::ExecuteEvent(Int_t event, Int_t px, Int_t py)
{
	if (event == kButton1Down)
		printf ("TText3D:: %d %s\n", event, fText.Data());
	TPolyLine3D::ExecuteEvent(event, px, py);
}
//______________________________________________________________
	
void TText3D::Compute()
{
	fListOfLetters->Clear();
	Int_t nc = fText.Length();
	Double_t xt = fX;
	Double_t yt = fY;
	Double_t zt = fZ;
	Float_t zchar[100] = {100 * 0.};
	Double_t cosfi = 0;
	Double_t sinfi = 0;
	Double_t costh = 0;
	Double_t sinth = 0;
	Bool_t rotate = kFALSE;
	if ( fDirX != 0 ||  fDirY != 0 ||  fDirZ != 0) {
		Double_t norm = TMath::Sqrt(fDirX*fDirX + fDirY*fDirY+ fDirZ*fDirZ);
		cosfi = fDirX/norm;
		sinfi = fDirY/norm;
		sinth = fDirZ/norm;
		costh = TMath::Sqrt(1 - sinth*sinth);
		printf("TText3D::norm, cosfi, sinfi, costh, sinth %f %f %f %f %f %s\n",
				 norm, cosfi, sinfi, costh, sinth, fText.Data());
			rotate = kTRUE;	
	}
	Double_t *rmin;
	Double_t *rmax;
	TView * v3d = (TView*)gPad->GetView();
	if ( !v3d ) {
		printf("TText3D::Draw cant get TView\n");
		return;
	}
	rmin = v3d->GetRmin();
	rmax = v3d->GetRmax();
	Double_t ch = (rmax[1] - rmin[1]) * fTextSize;
	Double_t cspace = ch *gChWidth / gChHeight;
//	printf("TText3D::Paint nc, ch: %d %f %s\n", nc,ch,  fText.Data());
	TPolyLine3D *pl = NULL;
/*	Float_t big = 10000000;
	Float_t xmin = big, xmax=-big, ymin =big, ymax=-big, zmin =big, zmax=-big;*/
	for (Int_t i= 0; i < nc; i++) {
		Char_t let = fText[i];
		if (nchar.count(let) > 0) {
			Int_t np = nchar[let];
			pl = new TPol3Dtext(np, this);
			pl->SetLineColor(fTextColor);
			pl->SetLineWidth(fLineWidth);
			Float_t *x = (Float_t*)xchar[let];
			Float_t *y = (Float_t*)ychar[let];
			Float_t *z = zchar;
// 			printf("TText3D:: fTextColor: %d\n", fTextColor);
			Int_t ip = 0;
			for (Int_t k = 0; k < np; k++) {
				if ( x[k] < 0 ) { 
// begin of new segment
					fListOfLetters->Add(pl);;
					pl = new TPol3Dtext(np - (ip+1), this);
					pl->SetLineColor(fTextColor);
					pl->SetLineWidth(fLineWidth);
					ip = 0;
				} else {
					Float_t px = ch * x[k]/gChHeight;
					Float_t py = ch * y[k]/gChHeight;
					Float_t pz = ch * z[k]/gChHeight;
//					costh = 1; sinth = 0;
					if ( rotate ) {
						Float_t m[9];
						m[0] =  costh * cosfi; m[1] = -sinfi; m[2] = sinth*cosfi;
						m[3] =  costh * sinfi; m[4] =  cosfi; m[5] = sinth*sinfi;
						m[6] = -sinth;         m[7] =  0;     m[8] = costh;

						Float_t px_s = px;
						Float_t py_s = py;
						Float_t pz_s = pz;
						px = xt + m[0] * px_s + m[1] * py_s + m[2] * pz_s;
						py = yt + m[3] * px_s + m[4] * py_s + m[5] * pz_s;
						pz = zt + m[6] * px_s + m[7] * py_s + m[8] * pz_s;

//					   px = px * cosfi - py * sinfi;
//						py = px_s * sinfi + py * cosfi;
					} else {
						px += xt;
						px += yt;
						pz += zt;
					}
					pl->SetNextPoint(px, py, pz);
// 					Float_t xtt = xt + px;
// 					Float_t ytt = yt + py;
// 					Float_t ztt = zt;
// 					pl->SetNextPoint(xtt, ytt, ztt);
/*					if (xtt < xmin)
						xmin = xtt;
					if (xtt > xmax)
						xmax = xtt;
					if (ytt < ymin)
						ymin = ytt;
					if (ytt > ymax)
						ymax = ytt;
					if (ztt < zmin)
						zmin = ztt;
					if (ztt > zmax)
						zmax = ztt;*/
					ip++;
				}
			}
			if (ip > 0) {
				fListOfLetters->Add(pl);;
//				printf("TText3D::Paint last: np %d\n", ip);
			}
			
		}
		if ( rotate ) {
			xt += (cspace* cosfi);
			yt += (cspace* sinfi);
			zt += (cspace* sinth);
		} else {
			xt += cspace;
		}
	}
/*	this->SetPoint(0, xmin, ymin, zmin);
	this->SetPoint(1, xmax, ymin, zmin);
	this->SetPoint(2, xmax, ymax, zmin);
	this->SetPoint(3, xmin, ymax, zmin);
	this->SetPoint(4, xmin, ymin, zmin);
	this->SetLineWidth(1);
	this->SetLineColor(0);*/
}
//______________________________________________________________
	
void TText3D::Draw(const char * option)
{
	Compute();
	AppendPad(option);
}
;
