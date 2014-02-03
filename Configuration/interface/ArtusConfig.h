/* Copyright (c) 2013 - All Rights Reserved
 *   Thomas Hauth  <Thomas.Hauth@cern.ch>
 *   Joram Berger  <Joram.Berger@cern.ch>
 *   Dominik Haitz <Dominik.Haitz@kit.edu>
 */

#pragma once

#include <TFile.h>

#include <boost/foreach.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>

#include "Artus/Core/interface/GlobalInclude.h"

class ArtusConfig {
public:
	ArtusConfig(int argc, char** argv);

	stringvector const& GetInputFiles() const
	{
		return m_fileNames;
	}

	template<class TPipelineInitializer, class TPipelineRunner>
	void LoadPipelines(TPipelineInitializer& pInit, TPipelineRunner& runner,
			TFile * outputFile)
	{
		assert(outputFile);

		typedef typename TPipelineInitializer::setting_type setting_type;
		typedef typename TPipelineInitializer::pipeline_type pipeline_type;

		BOOST_FOREACH(boost::property_tree::ptree::value_type& v,
				m_propTreeRoot.get_child("Pipelines"))
		{
			setting_type pset;

			// the key name of the dictionary will also become the 
			// pipeline name
			std::string sKeyName = v.first.data();

			// set up the Settings class access to the property tree
			// in order to be able to load additional settings
			pset.SetName(sKeyName);
			pset.SetPropTreePath("Pipelines." + sKeyName);
			pset.SetPropTree(&m_propTreeRoot);
			pset.SetRootOutFile(outputFile);
			//pset->m_globalSettings = &gset;

			std::cout << " %% Adding new pipeline " << sKeyName << std::endl;

			pipeline_type* pLine = new pipeline_type; //CreateDefaultPipeline();

			pLine->InitPipeline(pset, pInit);
			runner.AddPipeline(pLine);
		}
	}

	template<class TGlobalSettings>
	TGlobalSettings GetGlobalSettings()
	{
		TGlobalSettings pset;
		pset.SetPropTreePath("");
		pset.SetPropTree(&m_propTreeRoot);
		return pset;
	}

	std::string const& GetOutputPath() const
	{
		return m_outputPath;
	}

private:

	std::string m_outputPath;
	stringvector m_fileNames;
	boost::property_tree::ptree m_propTreeRoot;
};

