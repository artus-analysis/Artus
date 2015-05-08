
#include "Hoption.h"
#include "TBox.h"
#include "TColor.h"
#include "TF1.h"
#include "TH1.h"
#include "THistPainter.h"
#include "TList.h"
#include "TPad.h"
#include "TStyle.h"

#include <iostream>

class CustomHistogramPainter : public THistPainter
{
public:
	CustomHistogramPainter(THistPainter* painter) :
		THistPainter(*painter)
	{
	}
	ClassDef(CustomHistogramPainter, 1);

	virtual void PaintHist(Option_t * option)
	{
		/**
		Copied from https://root.cern.ch/root/html/src/THistPainter.cxx.html#ImNmtE
		Added option ("HRTOL") to draw histograms from right to left
		*/
		Int_t nch = strlen(option)+1;
		char choption[nch];
		strlcpy(choption,option,nch);
		for (Int_t i=0;i<nch;i++) choption[i] = toupper(choption[i]);
		
		bool inverseHorizontally = strstr(choption, "HRTOL");
		if (! inverseHorizontally) {
			THistPainter::PaintHist(option);
		}
		else {
			/* Begin_html
			<a href="#HP01b">Control routine to draw 1D histograms.</a>
			End_html */
/*
			//On iOS: do not highlight hist, if part of it was selected.
			//Never executes on non-iOS platform.
			if (gPad->PadInHighlightMode() && gPad->GetSelected() != fH)
				return;

			static char chopth[17];

			Int_t htype, oldhtype;
			Int_t i, j, first, last, nbins, fixbin;
			Double_t c1, yb;
			yb = 0;

			strlcpy(chopth, "                ",17);

			Double_t ymin = Hparam.ymin;
			Double_t ymax = Hparam.ymax;
			Double_t baroffset = fH->GetBarOffset();
			Double_t barwidth  = fH->GetBarWidth();
			Double_t baroffsetsave = gStyle->GetBarOffset();
			Double_t barwidthsave  = gStyle->GetBarWidth();
			gStyle->SetBarOffset(baroffset);
			gStyle->SetBarWidth(barwidth);

			// Create "LIFE" structure to keep current histogram status

			first = Hparam.xfirst;
			last  = Hparam.xlast;
			nbins = last - first + 1;

			Double_t *keepx = 0;
			Double_t *keepy = 0;
			if (fXaxis->GetXbins()->fN) fixbin = 0;
			else fixbin = 1;
			if (fixbin) keepx = new Double_t[2];
			else keepx = new Double_t[nbins+1];
			keepy = new Double_t[nbins];
			Double_t logymin = 0;
			if (Hoption.Logy) logymin = TMath::Power(10,ymin);

			// Loop on histogram bins

			for (j=first; j<=last;j++) {
				c1 = Hparam.factor*fH->GetBinContent(j);
				if (TMath::Abs(ymax-ymin) > 0) {
					if (Hoption.Logy) yb = TMath::Log10(TMath::Max(c1,.1*logymin));
					else yb = c1;
				}
				if (!Hoption.Line) {
					yb = TMath::Max(yb, ymin);
					yb = TMath::Min(yb, ymax);
				}
				keepy[j-first] = yb;
			}

			//				  Draw histogram according to value of FillStyle and FillColor

			if (fixbin) { keepx[0] = Hparam.xmin; keepx[1] = Hparam.xmax; }
			else {
				for (i=0; i<nbins; i++) keepx[i] = fXaxis->GetBinLowEdge(i+first);
				keepx[nbins] = fXaxis->GetBinUpEdge(nbins-1+first);
			}

			// Prepare Fill area (systematic with option "Bar").

			oldhtype = fH->GetFillStyle();
			htype = oldhtype;
			if (Hoption.Bar) {
				if (htype == 0 || htype == 1000) htype = 1001;
			}

			Width_t lw = (Width_t)fH->GetLineWidth();

			// Code option for GrapHist

			if (Hoption.Line) chopth[0] = 'L';
			if (Hoption.Star) chopth[1] = '*';
			if (Hoption.Mark) chopth[2] = 'P';
			if (Hoption.Mark == 10) chopth[3] = '0';
			if (Hoption.Line || Hoption.Curve || Hoption.Hist || Hoption.Bar) {
				if (Hoption.Curve) chopth[3] = 'C';
				if (Hoption.Hist > 0) chopth[4] = 'H';
				else if (Hoption.Bar) chopth[5] = 'B';
				if (fH->GetFillColor() && htype) {
					if (Hoption.Logy) {
						chopth[6] = '1';
					}
					if (Hoption.Hist > 0 || Hoption.Curve || Hoption.Line) {
						chopth[7] = 'F';
					}
				}
			}
			if (!fixbin && strlen(chopth)) {
				chopth[8] = 'N';
			}

			if (Hoption.Fill == 2) chopth[13] = '2';

			// Option LOGX

			if (Hoption.Logx) {
				chopth[9]  = 'G';
				chopth[10] = 'X';
				if (fixbin) {
					keepx[0] = TMath::Power(10,keepx[0]);
					keepx[1] = TMath::Power(10,keepx[1]);
				}
			}

			if (Hoption.Off) {
				chopth[11] = ']';
				chopth[12] = '[';
			}

			// Draw the histogram

			TGraph graph;
			graph.SetLineWidth(lw);
			graph.SetLineStyle(fH->GetLineStyle());
			graph.SetLineColor(fH->GetLineColor());
			graph.SetFillStyle(htype);
			graph.SetFillColor(fH->GetFillColor());
			graph.SetMarkerStyle(fH->GetMarkerStyle());
			graph.SetMarkerSize(fH->GetMarkerSize());
			graph.SetMarkerColor(fH->GetMarkerColor());
			if (!Hoption.Same) graph.ResetBit(TGraph::kClipFrame);

			graph.PaintGrapHist(nbins, keepx, keepy ,chopth);

			delete [] keepx;
			delete [] keepy;
			gStyle->SetBarOffset(baroffsetsave);
			gStyle->SetBarWidth(barwidthsave);

			htype=oldhtype;
*/
		}
	}

	virtual void PaintBarH(Option_t * option)
	{
		/**
		Copied from https://root.cern.ch/root/html/src/THistPainter.cxx.html#s5vHLC
		Added option ("RTOL") to draw histograms from right to left
		*/
		Int_t nch = strlen(option)+1;
		char choption[nch];
		strlcpy(choption,option,nch);
		for (Int_t i=0;i<nch;i++) choption[i] = toupper(choption[i]);
		
		bool inverseHorizontally = strstr(choption, "RTOL");
		if (! inverseHorizontally) {
			THistPainter::PaintBarH(option);
		}
		else {
			/* Begin_html
			<a href="#HP10">Draw a bar char in a rotated pad (X vertical, Y horizontal).</a>
			End_html */

			gPad->SetVertical(kFALSE);

			PaintInitH();

			TAxis *xaxis = fXaxis;
			TAxis *yaxis = fYaxis;
			if (!strcmp(xaxis->GetName(),"xaxis")) {
				fXaxis = yaxis;
				fYaxis = xaxis;
			}

			PaintFrame();

			Int_t bar = 0;//Hoption.Bar - 20;
			Double_t xmin,xmax,ymin,ymax,umin,umax,w;
			Double_t offset = fH->GetBarOffset();
			Double_t width  = fH->GetBarWidth();
			TBox box;
			Int_t hcolor = fH->GetFillColor();
			if (hcolor == gPad->GetFrameFillColor()) ++hcolor;
			Int_t hstyle = fH->GetFillStyle();
			box.SetFillColor(hcolor);
			box.SetFillStyle(hstyle);
			for (Int_t bin=fYaxis->GetFirst();bin<=fYaxis->GetLast();bin++) {
				ymin = gPad->YtoPad(fYaxis->GetBinLowEdge(bin));
				ymax = gPad->YtoPad(fYaxis->GetBinUpEdge(bin));
				xmin = (inverseHorizontally ? -gPad->XtoPad(fH->GetBinContent(bin)) : gPad->GetUxmin());
				xmax = (inverseHorizontally ? gPad->GetUxmax() : gPad->XtoPad(fH->GetBinContent(bin)));
				if (xmax < gPad->GetUxmin()) continue;
				if (xmax > gPad->GetUxmax()) xmax = gPad->GetUxmax();
				if (xmin < gPad->GetUxmin()) xmin = gPad->GetUxmin();
				if (gStyle->GetHistMinimumZero() && xmin < 0)
					xmin=TMath::Min(0.,gPad->GetUxmax());
				w = (ymax-ymin)*width;
				ymin += offset*(ymax-ymin);
				ymax = ymin + w;
				if (bar < 1) {
					box.PaintBox(xmin,ymin,xmax,ymax);
				} else {
					umin = ymin + bar*(ymax-ymin)/10.;
					umax = ymax - bar*(ymax-ymin)/10.;
					box.SetFillColor(TColor::GetColorDark(hcolor)); //dark
					box.PaintBox(xmin,ymin,xmax,umin);
					box.SetFillColor(hcolor);
					box.PaintBox(xmin,umin,xmax,umax);
					box.SetFillColor(TColor::GetColorBright(hcolor)); //bright
					box.PaintBox(xmin,umax,xmax,ymax);
				}
			}

			PaintTitle();
			// Draw box with histogram statistics and/or fit parameters
			if (false) {
			//if (Hoption.Same != 1 && !fH->TestBit(TH1::kNoStats)) {  // bit set via TH1::SetStats
				TIter next(fFunctions);
				TObject *obj = 0;
				while ((obj = next())) {
					if (obj->InheritsFrom(TF1::Class())) break;
					obj = 0;
				}
				PaintStat(gStyle->GetOptStat(),(TF1*)obj);
			}

			PaintAxis(kFALSE);
			fXaxis = xaxis;
			fYaxis = yaxis;
		}
	}
};

class CustomHistogram : public TH1
{
public:
	ClassDef(CustomHistogram, 1);
	
	TVirtualHistPainter *GetPainter(Option_t *option="")
	{
		fPainter = new CustomHistogramPainter(dynamic_cast<THistPainter*>(TH1::GetPainter(option)));
		return fPainter;
	}
};
