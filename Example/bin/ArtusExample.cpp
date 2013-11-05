#include <boost/algorithm/string.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>

#include "Artus/Configuration/interface/ArtusConfig.h"

#include "Artus/Example/interface/TraxTypes.h"
#include "Artus/Example/interface/TraxEventProvider.h"
#include "Artus/Example/interface/TraxPipelineRunner.h"
#include "Artus/Example/interface/TraxPipelineInitializer.h"

int main(int argc, char** argv) {

	// install signal
	// did not work
	//signal(SIGSEGV, handler);   // install our handler

	// setup Global Settings

	ArtusConfig myConfig(argc, argv);

	TraxEventProvider evtProvider(myConfig.getInputFiles());

	TraxPipelineInitilizer pInit;
	TraxPipelineRunner runner;
	//TraxPip

	myConfig.loadPipelines(pInit);

	// fix me
	TraxPipelineSettings settings;

	runner.RunPipelines<TraxTypes>(evtProvider, settings);

	/*
	 // removes the old file
	 std::string sRootOutputFilename = g_sOutputPath + ".root";

	 //TODO: close file to free memory of already written histos
	 g_resFile = new TFile(sRootOutputFilename.c_str(), "RECREATE");
	 CALIB_LOG_FILE("Writing to the root file " << sRootOutputFilename)


	 MassRecoGlobalSettings gset;

	 if (g_propTree.get < std::string > ("InputType", "mc") == "data") {
	 gset.SetJsonFile(g_propTree.get < std::string > ("JsonFile"));
	 g_inputType = DataInput;
	 } else {
	 }

	 MassRecoPipelineInitializer plineInit;
	 MassRecoPipelineSettings* pset = NULL;

	 EventPipelineRunner<MassRecoPipeline, MassRecoGlobalMetaDataProducerBase> pRunner;

	 BOOST_FOREACH(boost::property_tree::ptree::value_type & v,
	 g_propTree.get_child("Pipelines")) {
	 pset = new MassRecoPipelineSettings();
	 pset->SetPropTree(&g_propTree);

	 std::string sKeyName = v.first.data();
	 pset->SetSettingsRoot("Pipelines." + sKeyName);
	 pset->SetRootOutFile(g_resFile);
	 pset->m_globalSettings = &gset;

	 g_pipeSettings.push_back(pset);
	 }

	 // get pointers to the interesting collections
	 //typedef std::map<std::string, KDataPFJets*> PfMap;
	 //PfMap pfJets;

	 stringvector globalProducer = PropertyTreeSupport::GetAsStringList(
	 &g_propTree, "GlobalProducer");

	 AddGlobalMetaProducer(globalProducer, pRunner, g_propTree);

	 for (PipelineSettingsVector::iterator it = g_pipeSettings.begin();
	 it != g_pipeSettings.end(); it++) {

	 std::cout << " %% Adding new pipeline" << std::endl;

	 MassRecoPipeline* pLine = new MassRecoPipeline; //CreateDefaultPipeline();

	 pLine->InitPipeline(**it, plineInit);
	 pRunner.AddPipeline(pLine);

	 }

	 // delete the pipeline settings
	 for (PipelineSettingsVector::iterator it = g_pipeSettings.begin();
	 it != g_pipeSettings.end(); it++) {
	 delete *it;
	 }

	 MassRecoPipelineSettings settings;
	 settings.m_globalSettings = &gset;

	 #ifdef USE_PERFTOOLS
	 ProfilerStart("closure.prof");
	 #endif
	 //HeapProfilerStart( "resp_cuts.heap");

	 long long nEvents = evtProvider.GetOverallEventCount();
	 CALIB_LOG("Running over " << nEvents << " Events");

	 pRunner.RunPipelines<MassRecoEventData, MassRecoMetaData,
	 MassRecoPipelineSettings>(evtProvider, settings);

	 //HeapProfilerStop();
	 #ifdef USE_PERFTOOLS
	 ProfilerStop();
	 #endif

	 g_resFile->Close();
	 //g_logFile->close();

	 CALIB_LOG_FILE("Output file " << sRootOutputFilename << " closed.")

	 //delete g_logFile;

	 return 0;
	 }
	 */

	return 0;
}
