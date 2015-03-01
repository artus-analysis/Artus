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

  

=======
		HARRY PLOTTER - the plot wizard
===============================================================================

HarryPlotter can create Matplotlib and ROOT plots from any ROOT Ntuple or 
histogram and is optimized for scientific plots in High Energy particle physics.

The main object is one large python dictionary (plotdict) which holds all 
necessary plot metadata and even the actual plot data itself.
A comprehensive library of helper functions is used to create and modify histogram or 
plot data.

DONT FORGET TO SOURCE THE ini_harry.sh INITIALIZATION SCRIPT!

It is located in the HarryPlotter/Plotting/scripts folder.

If you prefer to have only the HarryPlotter scripts and not the Artus source 
code available you can use git's sparse checkout feature by runnig the script

Utility/scripts/onlyHarryPlotter.sh

after you performed a regular clone fo the Artus repository.

1. Workflow overview
-------------------------------------------------------------------------------

There are several ways an user can put together the plotdict:
	1. With Harry's built-in command-line parser `harry` (custom 
		shell scripts are possible)
	2. With custom python scripts
	3. With json config files

Type `harry -h` to display a list with the available options.

The `get_basic_parser` function returns the parser, and 
`create_dictionary_from_parser` creates a harry-compliant plotdict.

The plotdict is given to the `plot` function in the harrycore. Some general 
options are processed, and finally the `function_selector` decides which plot
function is called to do the actual plotting.

The actual plotting function (e.g. `plot_1d`) then modifies or creates plot
and histogram data according to the dictionary entries.


2. Using the command-line parser
-------------------------------------------------------------------------------

If you have sourced `harry_ini.sh`, the `harry` executable will be available.
For a basic usage, you need to provide a plotname/quantity, the path to a 
root file, the folder (branch) where to look in and you want to plot:
    harry -p theta -i src/Artus/Example/data/sample_output.root --folder highPt

If you want to know which folders/quantities are available in  a root file, try
    harry -i src/Artus/Example/data/sample_output.root --folder highPt
    --quantities

##### Possible quantities
When plotting from an Ntuple, possible quantities for one axis are not only the
dictionary variables itself, but also expressions like `-x '2*muonpt'` or 
`-x 'zmass-90'`.
Combinations of ntuple quantities are also possible, e.g.
`-x 'mupluspt/muminuspt'` or `-x 'npv-rho'`.
Logical expressions like `-x '(zpt>30)'` (which return 0 or 1 for each event) 
can be used, as well as CMath expressions like 
`-x ' -1 * log(tan(muonTheta/2))'`.

##### Plotname conventions
The plotname serves as (x-)quantity, filename and axis labels if these are not 
explicitly given via -x, --filename or --xname.

##### Profile plots
For a profile plot, type `y_x`, e.g. `harry -p softLepPt_hardLepPt --folder mm 
--files <path/to/higgs/tuple>/output.root`

##### Iteration over different variables
You can specify a list of x/folder/weight/files arguments to be iterated over.
The other of these arguments have to have either the same length or length one
(so that the same value is taken for each of the other elements.)
Examples:
- Plot two quantities from one file:
    harry -i <path/to/higgs/tuple>/output.root -x hardLepPt softLepPt 
    --folder mm -L hardLepton softLepton --xname leptonpt
(here, harry iterates over x but takes files/folder/.. as constant)

- Plot the same quantity from different files with different weights:
    harry -p softLepPt -i <path/to/higgs/tuple>/output1.root
    <path/to/higgs/tuple>/output.root --folder mm  --weight 1 2

##### Stacked plots
Harry will stack samples if they belong to the same group and `stack` has been 
specified as marker, e.g.:
    harry -p zmass -i data.root signal.root background.root --folder folder1
    --groups data mc mc -m o stack stack

##### Formatting options
Harry can be excessively configured via command line, especially for the 
formatting and layout options. A nice plot can be created with long commands
such as
    harry -p zmass -i dimuon.root dielectron.root 
    --folder "all_AK5PFJetsCHSL1L2L3Res" -C lightgrey cornflowerblue 
    --log --xlog -x 1 200 -m stack "-" -l 19.8 -e 8 --author dhaitz
    --date today --nbins 1990 --xname "Dilepton invariant mass / GeV"
    -k "$ \mu^{+}\mu^{-}$" "$ e^{+}e^{-}$" --yname Events
    --xticks  1 3 10 30 90 200 
    --text "Z,0.8,0.92, $ \Upsilon$ (123S),0.43,0.76, J/$ \Psi$,0.2,0.67, $ \Psi$',0.22,0.44"
    --title "own work" -y 10 300000


##### Saving (and reading) the plot configuration as JSON file
If you're happy with your current plot configuration, you can save it via 
    harry <arguments> --writejson myjsonfile
If you later want to recreate the plot with the same settings, type
    harry --readjson myjsonfile
You can also overwrite the settings from the json config, e.g.
    harry --readjson myjsonfile  --xlims 0 10


3. Further explanation for some keywords 
-------------------------------------------------------------------------------

##### --weight
When plotting from an ntuple, you can have Harry apply a weight expression,
which can be e.g.
    --weight "19.8 * weight * (muonpt>15 && electronpt>20)"
The weight can be a number, a ntuple variable or a logical expression 
(containing ntuple variables). Combinations with *, +, &&, ||, ... are possible.

##### --listfunctions
`harry --listfunctions` will display you a list of functions that are 
available in the harry core (and in your analysis files) with docstrings.
This shows you which functions exist so you can import them in your analysis-
specific plot functions.


3. Adding your own modules
-------------------------------------------------------------------------------

The `my_analysis` file in the scripts/ folder contains a template for an 
analysis-specific extension of harry.
    harry <arguments> --websync
will automatically copy the created plot to your public EKP webspace.

    harry myanalysis_plot1 <arguments>
will call the myanalysis_plot1 function, where a custom analysis plot can be
created using the plotdict entries.



Last edited: hauth, 19.03.2014

