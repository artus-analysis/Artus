#include <boost/algorithm/string.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>

#include <TFile.h>

#include "Artus/Configuration/interface/ArtusConfig.h"

#include "Artus/Example/interface/TraxTypes.h"
#include "Artus/Example/interface/TraxEventProvider.h"
#include "Artus/Example/interface/TraxPipelineRunner.h"
#include "Artus/Example/interface/TraxPipelineInitializer.h"

int main(int argc, char** argv) {

	ArtusConfig myConfig(argc, argv);

	boost::scoped_ptr < TFile
			> rootOutputFile(
					new TFile(myConfig.GetOutputPath().c_str(), "RECREATE"));

	TraxEventProvider evtProvider(myConfig.getInputFiles());

	TraxPipelineInitilizer pInit;
	TraxPipelineRunner runner;

	myConfig.loadPipelines(pInit, runner, rootOutputFile.get());

	// fix me
	TraxPipelineSettings settings;

	runner.RunPipelines<TraxTypes>(evtProvider, settings);

	rootOutputFile->Close();

	return 0;
}
