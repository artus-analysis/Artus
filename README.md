Artus	
=====
	
Introduction
------------

Artus is a framework for event-based data processing and analysis. It is written in C++ and uses python/json
as configuration language. It can be build inside of a CMSSW environment using the scram tool or standalone 
with cmake. 

It accepts chunked data as input from *Provider* classes, processes and filters it with *Filters* and 
*Producers* which are grouped in *Pipelines* and hands them to *Consumers*, which are also part of a *Pipeline*.

As you might already have guessed, the Artus framework consist of various components, which are grouped
and connected together to form the overall event processing workflow. Some components need to be implemented
by the user for the specific use-case, others are general and can be used as-is.

Here is a list of the main components and their description:

* Event Provider

  Load the event data from a source ( file / network ) and hand the data contained in a c++ class to other
  components for further processing.
  Bases classes for loading from ROOT files ( Provider/interface/RootEventProvider.h ) and Kappa files 
  ( Provider/interface/KappaEventProvider.h ) exist and can be used to customize for a specific analysis.
  
* Pipeline

  A pipeline groups various filter settings and a specific set of consumers together. Only events which passed
  all filters of a pipeline will also be handed to the consumer of the pipeline and, in turn show up in a histogram
  etc.
  A default Pipeline is provided and normally there is no customization needed.
  
* Pipeline Runner

  A pipeline runner holds a set of pipelines and their configuration and passes all events provided by the event
  provider to the pipeline. A default pipeline runner is provided and normally there is customization needed.

* Global & Local Producer

  If you want to enrich the loaded event with addiotional information, you can do so with producers. 
  Producers can either be executed once per event ( global producer ) or for each pipeline individually ( local
  producer )

* Global & Local Filter

  In some cases, certain events should not show up in the final event collection. You can define your own filters
  to drop events at any time during the processing. As with producers, you can define filters which run once per 
  event ( global filter ) and filter which run for every pipeline ( local filter ). 
  Global filters are also useful to drop unwanted events early on from the processing chain to safe processing time.
  The outcome of the filter producer for events is stored in FilterResult class. The information in this class can 
  later be used to do an easy cut flow information plots.

* Consumer

  Consumer can only be part of a pipeline and they are only executed once all filters and producers have been run. 
  They generate the output of the pipeline. This can either be histograms or other type of plots on a liste of n-tuples.

Typical Artus processing chain
------------------------------

The following schemactic visualize the control flow in an easy artus setup. The event provider
loads data from disc and hands them to the first producer, Json producer. This one checks in the 
Json file whether the event was taken during valid detector timeframe and stores its result. 
The JsonFilter, depending on the outcome of JsonProducer, either drops the event or continues
processing.
The correction producer applies the jet energy correction for all jets in the evend and continues.

Now the local processing for the three configured pipelins commences. Two of these pipelines have 
filters to only accept event with a certain pt region. Every pipeline has a consumer which plots 
the transeverse momentum into a histogram.

````
 [ Event Provider ]
	||
	\/
 [ JsonProducer (global) ] 
        ||
        \/
 [ JsonFilter (global) ]
        ||
        \/
 [ CorrectionProducer (global) ]
	||
	||==============================||===============================
        ||				||				||
        \/				\/				\/
 < Pipeline High Pt >		< Pipeline Low Pt >		< Pipeline All Pt >
        ||                              ||                              ||
        \/                              \/                              \/  
 [ FilterHighPt (local] ]        [ FilterHighPt (local) ]	[ ConsumerPlotPt (local) ]
        ||                              ||                              
        \/                              \/                              
 [ ConsumerPlotPt ]		[ ConsumerPlotPt ]
````

The Artus data flow:
![Artus data flow](https://github.com/artus-analysis/Artus/blob/master/Core/doc/artus_dataflow.svg "The Artus data flow")

The Artus pipeline structure:
![Artus pipeline structure](https://github.com/artus-analysis/Artus/blob/master/Core/doc/artus_pipelines.svg "The Artus pipeline structure")


Example
-------

To help understand the framework concept, a comprehensive example is included. To try it out in a CMSSW environment,
run the following code:

````
scram p CMSSW_6_2_3
cd CMSSW_6_2_3/
cmsenv
cd src/
git clone https://ekptrac.physik.uni-karlsruhe.de/git/Artus
scram b -j4
scram b runtests
cd Artus/Example/data/
artusExample exampleConfig.json
root -l sample_output.root  
````

This will produce the output file "sample_output.root" which contains two folders with histograms and different
filter settings.
You can also run the example with a different configuration file:

artusExample exampleConfigNtuple.json

This will produce the output file sample_output_ntuple.root which uses the NtupleConsumerBase class to produce
ROOT Ntuples instead of histograms.


Standalone CMake compile
-----------------------

To compile standalone, make sure you have a recent ROOT and cmake installation available in your system. Furthermore,
you should have at least gcc 4.7 to benefit from the C++11 support.

````
git clone https://ekptrac.physik.uni-karlsruhe.de/git/Artus
cd Artus
cmake .
make -j4
cd Example/data/
../../artusExample exampleConfig.json
root -l sample_output.root
````

This will produce the output file "sample_output.root" which contains two folders with histograms and different
filter settings.
You can also run the example with a different configuration file:

../../artusExample exampleConfigNtuple.json

This will produce the output file sample_output_ntuple.root which uses the NtupleConsumerBase class to produce
ROOT Ntuples instead of histograms.

