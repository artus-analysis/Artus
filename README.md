Artus	
=====
[![Build Status](https://travis-ci.org/artus-analysis/Artus.svg)](https://travis-ci.org/artus-analysis/Artus)

Documentation is found in the [wiki](https://github.com/artus-analysis/Artus/wiki).

(a dedicated documentation for HarryPlotter can be found [here](https://github.com/artus-analysis/Artus/blob/master/HarryPlotter/README.md "HarryPlotter documentation"))

[//]: # (Comment) Example
[//]: # (Comment) -------
[//]: # (Comment) 
[//]: # (Comment) To help understand the framework concept, a comprehensive example is included. To try it out in a CMSSW environment,
[//]: # (Comment) run the following code:
[//]: # (Comment) 
[//]: # (Comment) ````
[//]: # (Comment) scram p CMSSW_6_2_3
[//]: # (Comment) cd CMSSW_6_2_3/
[//]: # (Comment) cmsenv
[//]: # (Comment) cd src/
[//]: # (Comment) git clone https://ekptrac.physik.uni-karlsruhe.de/git/Artus
[//]: # (Comment) scram b -j4
[//]: # (Comment) scram b runtests
[//]: # (Comment) cd Artus/Example/data/
[//]: # (Comment) artusExample exampleConfig.json
[//]: # (Comment) root -l sample_output.root  
[//]: # (Comment) ````
[//]: # (Comment) 
[//]: # (Comment) This will produce the output file "sample_output.root" which contains two folders with histograms and different
[//]: # (Comment) filter settings.
[//]: # (Comment) You can also run the example with a different configuration file:
[//]: # (Comment) 
[//]: # (Comment) artusExample exampleConfigNtuple.json
[//]: # (Comment) 
[//]: # (Comment) This will produce the output file sample_output_ntuple.root which uses the NtupleConsumerBase class to produce
[//]: # (Comment) ROOT Ntuples instead of histograms.
[//]: # (Comment) 
[//]: # (Comment) 
[//]: # (Comment) Standalone CMake compile
[//]: # (Comment) -----------------------
[//]: # (Comment) 
[//]: # (Comment) To compile standalone, make sure you have a recent ROOT and cmake installation available in your system. Furthermore,
[//]: # (Comment) you should have at least gcc 4.7 to benefit from the C++11 support.
[//]: # (Comment) 
[//]: # (Comment) ````
[//]: # (Comment) git clone https://ekptrac.physik.uni-karlsruhe.de/git/Artus
[//]: # (Comment) cd Artus
[//]: # (Comment) cmake .
[//]: # (Comment) make -j4
[//]: # (Comment) cd Example/data/
[//]: # (Comment) ../../artusExample exampleConfig.json
[//]: # (Comment) root -l sample_output.root
[//]: # (Comment) ````
[//]: # (Comment) 
[//]: # (Comment) This will produce the output file "sample_output.root" which contains two folders with histograms and different
[//]: # (Comment) filter settings.
[//]: # (Comment) You can also run the example with a different configuration file:
[//]: # (Comment) 
[//]: # (Comment) ../../artusExample exampleConfigNtuple.json
[//]: # (Comment) 
[//]: # (Comment) This will produce the output file sample_output_ntuple.root which uses the NtupleConsumerBase class to produce
[//]: # (Comment) ROOT Ntuples instead of histograms.
[//]: # (Comment) 
[//]: # (Comment) 
[//]: # (Comment) Code checks and static analysis
[//]: # (Comment) -------------------------------
[//]: # (Comment) 
[//]: # (Comment) Information about compiler warnings and static code analysis can be found at [Core/docs/linter.md](Core/docs/linter.md).
[//]: # (Comment) 
