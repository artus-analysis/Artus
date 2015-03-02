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

  


HARRY PLOTTER - the plot wizard
=====

HarryPlotter can create Matplotlib and ROOT plots from any ROOT Ntuple or
histogram and is optimized for scientific plots in High Energy particle physics.

DONT FORGET TO SOURCE THE ini_harry.sh INITIALIZATION SCRIPT!
It is located in the HarryPlotter/scripts folder.

If you prefer to have only the HarryPlotter scripts and not the Artus source
code available you can use git's sparse checkout feature by runnig the script

Utility/scripts/onlyHarryPlotter.sh

after you performed a regular clone fo the Artus repository.



1. Workflow overview
-------------------------------------------------------------------------------

The `harry.py` executable calls a script of the same name in the HarryPlotter/python
folder which instantiates the core one or (for parallel plotting) multiple times.
One core instance = one plot.

The core class (located in HarryPlotter/python/core.py) handles detection,
registration and execution of processors.

There are three types of processors:

- Input modules, which create root histograms from e.g. root files
- Analysis modules, which perform analysis or editing tasks or create new
histograms from the existing ones
- Plot modules, which create the actual plots from the available histograms

All modules are located in the respective subdirectories in the python/ folder.
Type `harry.py --list-available-modules` to get a list of the available modules
with docstrings.

Modules take/add (meta-)information as well as the actual root histograms
from/to the plotData.plotdict dictionary. An actual plot is created by running
at least one input and plot module with valid parameters (existing file etc.)



2. Using the command-line parser
-------------------------------------------------------------------------------

If you have sourced `harry_ini.sh`, the `harry.py` executable will be available.
Type `harry.py -h` (or just `harry.py`) to get a list of all command-line options.
For a basic usage, you need to provide a quantity, the path to a
root file and the folder (branch) where to look in:
    `harry.py -x theta -i src/Artus/Example/data/sample_output.root --folder highPt`

If you want to know which folders/quantities are available in  a root file, try
    `harry.py -i src/Artus/Example/data/sample_output.root --folder highPt --quantities`

##### Possible quantities
When plotting from an Ntuple, possible quantities for one axis are not only the
ntuple variables itself, but also expressions like `-x '2*muonpt'` or
`-x "zmass-90"`.
Combinations of ntuple quantities are also possible, e.g.
`-x "mupluspt/muminuspt"` or `-x "npv-rho"`.
Logical expressions like `-x '(zpt>30)'` (which return 0 or 1 for each event)
can be used, as well as CMath expressions like
`-x " -1 * log(tan(muonTheta/2))"`.
See [TTree::Draw() documentation](https://root.cern.ch/root/htmldoc/TTree.html#TTree:Draw@2 "TTree Draw options")
for more information.



##### Profile plots
If the `-x` and `-y` arguments are set, a 2D histogram is created. If you want
to have a profile plot, you have to supply `prof` to the --tree-draw-options
keyword.

##### Iteration over different variables
You can specify a list of quantities/folder/weight/files arguments to be iterated over.
The other ones of these arguments have to have either the same length or length one
(so that the same value is taken for each of the other elements.)
Examples:

- Plot two quantities from one file:
    `harry.py -i <path/to/higgs/tuple>/output.root -x hardLepPt softLepPt -f mm`
(here, harry iterates over x but takes files/folder/.. as constant)

- Plot the same quantity from different files with different weights:
   `harry.py -i <path/to/root/tuple>/output1.root <path/to/root/tuple>/output2.root -x softLepPt  -f mm  --weight 1 2`

##### Merge input histograms
Harry will merge histograms if they have the same argument for `--nicks`. E.g. here,
signal.root and background.root are merged as both get the nickname 'mc':
    `harry.py -p zmass -i data.root signal.root background.root -f folder1 --nicks data mc mc`

##### Stacked plots
Harry will stack samples if they have the same argument for `--stacks`. E.g. here,
signal.root and background.root are stacked as both get the stackname 'mc':
    `harry.py -p zmass -i data.root signal.root background.root -f folder1 --stacks data mc mc`

##### Formatting options
harry.py can be excessively configured via command line, especially for the
formatting and layout options. Type `harry.py -h` (or just `harry.py`) to get
a list of all options.

##### --weight
When plotting from an ntuple, you can have Harry apply a weight expression,
which can be e.g.
    `--weight "19.8 * weight * (muonpt>15 && electronpt>20)"`
The weight can be a number, a ntuple variable or a logical expression
(containing ntuple variables). Combinations with *, +, &&, ||, ... are possible.
See [TTree::Draw() documentation](https://root.cern.ch/root/htmldoc/TTree.html#TTree:Draw@2 "TTree Draw options")
for more information.

##### Saving (and reading) the plot configuration as JSON file
If you're happy with your current plot configuration, you can save it via
    `harry.py <arguments> --export-json myjsonfile.json`
If you later want read the settings from the json file, type
    `harry.py -j myjsonfile.json`
You can also overwrite the settings from the json config, e.g.
    `harry.py -j myjsonfile.json  --xlims 0 10`


3. Adding your own modules
-------------------------------------------------------------------------------

Have at look at the existing modules in the python/*_modules folders and
understand how they work. To add your own modules, simply place them in the
respective directory.

Last edited: dhaitz, 02.03.2015

