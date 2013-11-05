#include <boost/algorithm/string.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>

#include "Artus/Configuration/interface/ArtusConfig.h"

#include "Artus/Example/interface/TraxTypes.h"
#include "Artus/Example/interface/TraxEventProvider.h"
#include "Artus/Example/interface/TraxPipelineRunner.h"
#include "Artus/Example/interface/TraxPipelineInitializer.h"

int main(int argc, char** argv) {

	ArtusConfig myConfig(argc, argv);

	TraxEventProvider evtProvider(myConfig.getInputFiles());

	TraxPipelineInitilizer pInit;
	TraxPipelineRunner runner;

	myConfig.loadPipelines(pInit, runner);

	// fix me
	TraxPipelineSettings settings;

	runner.RunPipelines<TraxTypes>(evtProvider, settings);

	return 0;
}
