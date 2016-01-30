import ROOT

newIdx = {}

# transparent grey for uncertainty bands (as in HIG-14-034)
colorToAdapt = ROOT.gROOT.GetColor(12)
transGrey_idx = ROOT.gROOT.GetListOfColors().GetSize() + 1
transGrey = ROOT.TColor(transGrey_idx, colorToAdapt.GetRed(), colorToAdapt.GetGreen(), colorToAdapt.GetBlue(), "", 0.5)

newIdx["transGrey"] = transGrey_idx
