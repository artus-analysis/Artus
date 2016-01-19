
#include "Artus/Utility/interface/ArtusDefineLogging.h"

#include "Artus/Configuration/interface/ArtusConfig.h"
#include "Artus/Configuration/interface/RootEnvironment.h"

#include "Artus/KappaAnalysis/interface/KappaTypes.h"
#include "Artus/KappaAnalysis/interface/KappaEventProvider.h"
#include "Artus/KappaAnalysis/interface/KappaFactory.h"


int main(int argc, char** argv) {

	// parse the command line and load the
	ArtusConfig config(argc, argv);

	// load the global settings from the config file
	KappaSettings settings = config.GetSettings<KappaSettings>();

	// create the output root environment (which automatically saves the config into the root file)
	RootEnvironment rootEnvironment(config);

	// prepare reading the input trees
	FileInterface2 fileInterface(config.GetInputFiles());
	KappaEventProvider<KappaTypes> eventProvider(fileInterface, (settings.GetInputIsData() ? DataInput : McInput), settings.GetBatchMode());
	eventProvider.WireEvent(settings);

	// the pipeline initializer will setup the pipeline, with
	// all the attached Producer, Filer and Consumer
	KappaPipelineInitializer<KappaTypes> pipelineInitializer;
	
	// the factory will manage the producers/filters/consumers
	KappaFactory factory;

	// initialize the pipeline runner
	KappaPipelineRunner pipelineRunner(true);

	// load the pipeline with their configuration from the config file
	config.LoadConfiguration(pipelineInitializer, pipelineRunner, factory, rootEnvironment.GetRootFile());

	// run all the configured pipelines
	pipelineRunner.RunPipelines(eventProvider, settings);

	// save and close output root file
	rootEnvironment.Close();

	return 0;
}
