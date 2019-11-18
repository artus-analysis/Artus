#!/bin/sh

# create datacards and corresponding ROOT file with templates
rm -rf combine_example
${CMSSW_BASE}/src/Artus/Example/scripts/combineHarvesterExample.py -o combine_example --log-level debug;
tree combine_example

# convert datacards and templates into RooWorkspace files: text2workspace
combineTool.py -M T2W -o workspace.root -m 0 --parallel 8 -i combine_example/datacards/{categories_combined,per_category/*}/datacard.txt
tree combine_example

# max. Likelihood fits
# best fit parameters saved into workspace
combineTool.py -M MultiDimFit --redefineSignalPOIs r -P r --floatOtherPOIs 1 --algo singles --robustFit 1 \
	-d combine_example/datacards/{categories_combined,per_category/*}/workspace.root \
	--there -m 0 --parallel 8 -n .r.best_fit
tree combine_example

# nuisance impacts
for WORKSPACE in combine_example/datacards/{categories_combined,per_category/*}/workspace.root
do
	pushd `dirname ${WORKSPACE}`
	
	# initial fit
	combineTool.py -M Impacts --doInitialFit \
		--allPars --redefineSignalPOIs r --setParameters r=1 --robustFit 1 \
		-m 0 -d `basename ${WORKSPACE}` --parallel 8
	
	# fits for all nuisance parameters
	combineTool.py -M Impacts --doFits --X-rtd FITTER_NEW_CROSSING_ALGO --X-rtd FITTER_NEVER_GIVE_UP --setRobustFitAlgo Minuit2,Migrad \
		--allPars --redefineSignalPOIs r --setParameters r=1 --robustFit 1 \
		-m 0 -d `basename ${WORKSPACE}` --parallel 8
	
	# collect results
	combineTool.py -M Impacts  -o impacts.json \
		--allPars --redefineSignalPOIs r --setParameters r=1 \
		-m 0 -d `basename ${WORKSPACE}` --parallel 8
	
	# plot results
	plotImpacts.py -i impacts.json -o impacts
	
	popd
	
	if [ -x "$(command -v www_publish.py)" ]
	then
		if [ ! -d websync/`date +%Y_%m_%d`/statistics/nuisance_impacts/`dirname ${WORKSPACE} | sed -e "s@combine_example/datacards/@@g"` ]
		then
			mkdir -p websync/`date +%Y_%m_%d`/statistics/nuisance_impacts/`dirname ${WORKSPACE} | sed -e "s@combine_example/datacards/@@g"`
		fi
		
		cp `echo ${WORKSPACE} | sed -e "s@workspace.root@impacts.*@g"` websync/`date +%Y_%m_%d`/statistics/nuisance_impacts/`dirname ${WORKSPACE} | sed -e "s@combine_example/datacards/@@g"`
		
		${CMSSW_BASE}/src/Artus/HarryPlotter/scripts/www_publish.py \
				-i websync/`date +%Y_%m_%d`/statistics/nuisance_impacts/`dirname ${WORKSPACE} | sed -e "s@combine_example/datacards/@@g"` \
				-o statistics/nuisance_impacts/`dirname ${WORKSPACE} | sed -e "s@combine_example/datacards/@@g"`
	fi
done

# plotting
annotate-trees.py combine_example/datacards/per_category/inclusive/higgsCombine*.MultiDimFit.mH0.root -t limit --branches index --values 1
annotate-trees.py combine_example/datacards/categories_combined/higgsCombine*.MultiDimFit.mH0.root -t limit --branches index --values 2
annotate-trees.py combine_example/datacards/per_category/cat_sig/higgsCombine*.MultiDimFit.mH0.root -t limit --branches index --values 3
annotate-trees.py combine_example/datacards/per_category/cat_bkg/higgsCombine*.MultiDimFit.mH0.root -t limit --branches index --values 4

harry.py -d "combine_example/datacards/per_category/inclusive combine_example/datacards/categories_combined combine_example/datacards/per_category/cat_sig combine_example/datacards/per_category/cat_bkg" \
	-i higgsCombine.r.best_fit.MultiDimFit.mH0.root \
	-f limit --tree-draw-options TGraph --redo-cache \
	-x index --x-label "" --x-ticks 1 2 3 4 --x-tick-labels "Inclusive" "Combined" "Signal cat." "Background cat." --x-lims 0.5 4.5 \
	-y r --y-label "Best Fit Signal Strength r" \
	-w "quantileExpected<0.0" "(quantileExpected>0.0)*(quantileExpected<0.4)*(Entry$%3==1)" "(quantileExpected>0.0)*(quantileExpected<0.4)*(Entry$%3==2)" \
	--nicks central_noplot p1s_noplot m1s_noplot \
	--analysis-modules GraphCombination --graph-error-modes abs --graph-result-nicks result \
	--graph-values-nicks central_noplot --graph-high-errors-nicks p1s_noplot --graph-low-errors-nicks m1s_noplot \
	-m P --line-widths 2 -C kRed --www statistics/fits --formats pdf png --filename no_systematics

