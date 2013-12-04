
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
  provider to the pipeline. A default pipeline runner is provided.

* Global & Local Producer

  If you want to compute addi

.. TODO ..

Example
-------

To help understand the framework concept, a comprehensive example is included. To try it out in a CMSSW environment,
run the following code:

scram p CMSSW_6_2_3
cd CMSSW_6_2_3/
cmsenv
cd src/
git clone /afs/cern.ch/work/h/hauth/git/Artus
scram b -j4
scram b runtests
cd Artus/Example/data/
artusExample exampleConfig.json
root -l sample_output.root  

This will produce the output file "sample_output.root" which contains two folders with histograms and different
filter settings.
You can also run the example with a different configuration file:

artusExample exampleConfigNtuple.json

This will produce the output file sample_output_ntuple.root which uses the NtupleConsumerBase class to produce
ROOT Ntuples instead of histograms.



  


