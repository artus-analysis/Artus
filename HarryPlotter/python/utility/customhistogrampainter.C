
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
				w	 = (ymax-ymin)*width;
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
			//	 Draw box with histogram statistics and/or fit parameters
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
