/* Copyright (c) 2013 - All Rights Reserved
 *   Thomas Hauth  <Thomas.Hauth@cern.ch>
 *   Joram Berger  <Joram.Berger@cern.ch>
 *   Dominik Haitz <Dominik.Haitz@kit.edu>
 */

#pragma once

#include <sstream>
#include <vector>

#include "Artus/Utility/interface/easylogging++.h"

#include <TFile.h>

#include <boost/foreach.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>

#include "Artus/Core/interface/Cpp11Support.h"
#include "Artus/Core/interface/GlobalInclude.h"
#include "Artus/Core/interface/ProcessNodeBase.h"

class ArtusConfig {
public:
	// loads the json config file as given in the command line parameters
	ArtusConfig(int argc, char** argv);
	
	// parses the json config from a stringstream
	ArtusConfig( std::stringstream & sStream );

	void SaveConfig(TFile* outputFile) const;

	stringvector const& GetInputFiles() const
	{
		return m_fileNames;
	}

	// run this method to add all the producers/filter/consumer which are
	// listed in your configuration file
	// outputFile can be null, if no root is to be used
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

	typedef std::pair< ProcessNodeType, std::string > NodeTypePair;

	static NodeTypePair ParseProcessNode ( std::string const& sInp );

private:

	void InitConfig();

	std::pair < bool, el::Level> parseLogLevel(std::string const& inpString) const;

	// load the global produce list from configuration and
	// use the factory object to add these producers to the pipeline runner
	// don't use directly but call LoadConfiguration
	template<class TPipelineRunner, class TFactory, class TGlobalSettings>
	void LoadGlobalProducer( TPipelineRunner& runner, TFactory & factory ) {

		typedef typename TPipelineRunner::producer_base_type producer_base_type;
		typedef typename TPipelineRunner::filter_base_type filter_base_type;

		TGlobalSettings gSettings = GetGlobalSettings< TGlobalSettings >();
		stringvector globalProds = gSettings.GetGlobalProcessors();
		for ( stringvector::const_iterator it = globalProds.begin();
			it != globalProds.end(); it ++ ) {

			NodeTypePair ntype = ParseProcessNode( *it );

			if (ntype.first == ProcessNodeType::Producer ) {
				producer_base_type * gProd = factory.createProducer ( ntype.second );

				if ( gProd == ARTUS_CPP11_NULLPTR ){
					ARTUS_LOG_FATAL( "Error: Global producer with id " + ntype.second + " not found" );
				} else {
					runner.AddGlobalProducer( gProd );
				}
			} else if (ntype.first == ProcessNodeType::Filter ) {
				filter_base_type * gProd = factory.createFilter ( ntype.second );

				if ( gProd == ARTUS_CPP11_NULLPTR ){
					ARTUS_LOG_FATAL( "Error: Global filter with id " + ntype.second + " not found" );
				} else {
					runner.AddGlobalFilter( gProd );
				}
			}
		}
	}

	// create all pipelines and add all filter/consumer/producer according the
	// pipelines configuration
	// don't use directly but call LoadConfiguration
	template<class TPipelineInitializer, class TPipelineRunner, class TFactory>
	void LoadPipelines(TPipelineInitializer& pInit, TPipelineRunner& runner,
			TFactory & factory,
			// can be null
			TFile * outputFile)
	{
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

			pipeline_type* pLine = new pipeline_type; //CreateDefaultPipeline();

			// add local producer
			stringvector localProducers = pset.GetProcessors();
			for ( stringvector::const_iterator it = localProducers.begin();
				it != localProducers.end(); it ++ ) {

					NodeTypePair ntype = ParseProcessNode( *it );

					if (ntype.first == ProcessNodeType::Producer ) {
						producer_base_type * pProducer = factory.createProducer ( ntype.second );

						if ( pProducer == ARTUS_CPP11_NULLPTR ){
							 ARTUS_LOG_FATAL( "Error: Local Producer with id " + ntype.second + " not found" );
						} else {
							pLine->AddProducer ( pProducer );
						}
					} else if (ntype.first == ProcessNodeType::Filter ) {
						filter_base_type * pProducer = factory.createFilter ( ntype.second );

						if ( pProducer == ARTUS_CPP11_NULLPTR ){
							 ARTUS_LOG_FATAL( "Error: Local Filter with id " + ntype.second + " not found" );
						} else {
							pLine->AddFilter ( pProducer );
						}
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

	std::string m_jsonConfigFileName;
	std::string m_outputPath;
	stringvector m_fileNames;
	boost::property_tree::ptree m_propTreeRoot;

	el::Level m_minimumLogLevel;

};

