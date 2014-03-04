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

#include "Artus/Core/interface/Cpp11Support.h"
#include "Artus/Core/interface/GlobalInclude.h"

class ArtusConfig {
public:
	ArtusConfig(int argc, char** argv);
	
	void SaveConfig(TFile* outputFile) const;

	stringvector const& GetInputFiles() const
	{
		return m_fileNames;
	}

	// run this method to add all the producers/filter/consumer which are
	// listed in your configuration file
	template<class TPipelineInitializer, class TPipelineRunner, class TFactory>
	void LoadConfiguration(TPipelineInitializer& pInit, TPipelineRunner& runner,
			TFactory & factory,
			TFile * outputFile)
	{
		typedef typename TPipelineRunner::global_setting_type global_setting_type;

		LoadGlobalProducer <TPipelineRunner,TFactory, global_setting_type > (runner, factory);
		LoadPipelines< TPipelineInitializer, TPipelineRunner>(pInit, runner, factory, outputFile);
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

	// load the global produce list from configuration and
	// use the factory object to add these producers to the pipeline runner
	// don't use directly but call LoadConfiguration
	template<class TPipelineRunner, class TFactory, class TGlobalSettings>
	void LoadGlobalProducer( TPipelineRunner& runner, TFactory & factory ) {

		typedef typename TPipelineRunner::producer_base_type producer_base_type;

		TGlobalSettings gSettings = GetGlobalSettings< TGlobalSettings >();
		stringvector globalProds = gSettings.GetGlobalProducers();
		for ( stringvector::const_iterator it = globalProds.begin();
			it != globalProds.end(); it ++ ) {
			producer_base_type * gProd = factory.createProducer ( *it );

			if ( gProd == ARTUS_CPP11_NULLPTR ){
				std::cout << "Error: Global producer with id " + (*it) + " not found" << std::endl;
				exit(1);
			} else {
				runner.AddGlobalProducer( gProd );
			}
		}
	}

	// create all pipelines and add all filter/consumer/producer according the
	// pipelines configuration
	// don't use directly but call LoadConfiguration
	template<class TPipelineInitializer, class TPipelineRunner, class TFactory>
	void LoadPipelines(TPipelineInitializer& pInit, TPipelineRunner& runner,
			TFactory & factory,
			TFile * outputFile)
	{
		assert(outputFile);

		typedef typename TPipelineInitializer::setting_type setting_type;
		typedef typename TPipelineInitializer::pipeline_type pipeline_type;
		typedef typename TPipelineRunner::filter_base_type filter_base_type;
		typedef typename TPipelineRunner::producer_base_type producer_base_type;
		typedef typename TPipelineRunner::consumer_base_type consumer_base_type;

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

			// add local producer
			stringvector localProducers = pset.GetLocalProducers();
			for ( stringvector::const_iterator it = localProducers.begin();
				it != localProducers.end(); it ++ ) {
					producer_base_type * pProducer = factory.createProducer ( *it );

					if ( pProducer == ARTUS_CPP11_NULLPTR ){
						std::cout << "Error: Local Producer with id " + (*it) + " not found" << std::endl;
						exit(1);
					} else {
						pLine->AddProducer ( pProducer );
					}
				}

			// add local filter
			stringvector localFilters = pset.GetFilters();
			for ( stringvector::const_iterator it = localFilters.begin();
				it != localFilters.end(); it ++ ) {
					filter_base_type * pFilter = factory.createFilter ( *it );

					if ( pFilter == ARTUS_CPP11_NULLPTR ){
						std::cout << "Error: Filter with id " + (*it) + " not found" << std::endl;
						exit(1);
					} else {
						pLine->AddFilter ( pFilter );
					}
				}

			// add consumer
			stringvector localConsumers = pset.GetConsumers();
			for ( stringvector::const_iterator it = localConsumers.begin();
				it != localConsumers.end(); it ++ ) {
					consumer_base_type * pConsumer = factory.createConsumer ( *it );

					// special case for consumer:
					// it is ok if they cannot be created here, because some might
					// only be produced in the InitPipeline method below
					// for example when using an alias for a set of producer
					if ( pConsumer != ARTUS_CPP11_NULLPTR ){
						pLine->AddConsumer ( pConsumer );
					}
				}


			pLine->InitPipeline(pset, pInit);
			runner.AddPipeline(pLine);
		}
	}

	std::string m_jsonConfig;
	std::string m_outputPath;
	stringvector m_fileNames;
	boost::property_tree::ptree m_propTreeRoot;
};

