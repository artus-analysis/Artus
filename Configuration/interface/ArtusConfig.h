#pragma once

#include <boost/foreach.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>

#include "Artus/Core/interface/GlobalInclude.h"

class ArtusConfig {
public:
	ArtusConfig(int argc, char** argv);

	stringvector const& getInputFiles() const {
		return m_fileNames;
	}

	template<class TPipelineInitializer, class TPipelineRunner>
	void loadPipelines(TPipelineInitializer & pInit, TPipelineRunner & runner) {

		typedef typename TPipelineInitializer::setting_type setting_type;
		typedef typename TPipelineInitializer::pipeline_type pipeline_type;

		BOOST_FOREACH(boost::property_tree::ptree::value_type & v,
				m_propTreeRoot.get_child("Pipelines"))
		{
			setting_type pset;
			;
			//pset->SetPropTree(&m_propTreeRoot);

			std::string sKeyName = v.first.data();

			//pset->SetSettingsRoot("Pipelines." + sKeyName);
			//pset->SetRootOutFile(g_resFile);
			//pset->m_globalSettings = &gset;

			std::cout << " %% Adding new pipeline" << sKeyName << std::endl;

			pipeline_type* pLine = new pipeline_type; //CreateDefaultPipeline();

			pLine->InitPipeline(pset, pInit);
			runner.AddPipeline(pLine);
		}
	}

private:

	std::string m_outputPath;
	stringvector m_fileNames;
	boost::property_tree::ptree m_propTreeRoot;
};

