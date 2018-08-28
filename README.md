Artus	
=====
[![Build Status](https://travis-ci.org/artus-analysis/Artus.svg)](https://travis-ci.org/artus-analysis/Artus)

Documentation is found in the [wiki](https://github.com/artus-analysis/Artus/wiki).

(a dedicated documentation for HarryPlotter can be found [here](https://github.com/artus-analysis/Artus/blob/master/HarryPlotter/README.md "HarryPlotter documentation"))

[//]: #Example
[//]: #-------
[//]: #
[//]: #To help understand the framework concept, a comprehensive example is included. To try it out in a CMSSW environment,
[//]: #run the following code:
[//]: #
[//]: #````
[//]: #scram p CMSSW_6_2_3
[//]: #cd CMSSW_6_2_3/
[//]: #cmsenv
[//]: #cd src/
[//]: #git clone https://ekptrac.physik.uni-karlsruhe.de/git/Artus
[//]: #scram b -j4
[//]: #scram b runtests
[//]: #cd Artus/Example/data/
[//]: #artusExample exampleConfig.json
[//]: #root -l sample_output.root  
[//]: #````
[//]: #
[//]: #This will produce the output file "sample_output.root" which contains two folders with histograms and different
[//]: #filter settings.
[//]: #You can also run the example with a different configuration file:
[//]: #
[//]: #artusExample exampleConfigNtuple.json
[//]: #
[//]: #This will produce the output file sample_output_ntuple.root which uses the NtupleConsumerBase class to produce
[//]: #ROOT Ntuples instead of histograms.
[//]: #
[//]: #
[//]: #Standalone CMake compile
[//]: #-----------------------
[//]: #
[//]: #To compile standalone, make sure you have a recent ROOT and cmake installation available in your system. Furthermore,
[//]: #you should have at least gcc 4.7 to benefit from the C++11 support.
[//]: #
[//]: #````
[//]: #git clone https://ekptrac.physik.uni-karlsruhe.de/git/Artus
[//]: #cd Artus
[//]: #cmake .
[//]: #make -j4
[//]: #cd Example/data/
[//]: #../../artusExample exampleConfig.json
[//]: #root -l sample_output.root
[//]: #````
[//]: #
[//]: #This will produce the output file "sample_output.root" which contains two folders with histograms and different
[//]: #filter settings.
[//]: #You can also run the example with a different configuration file:
[//]: #
[//]: #../../artusExample exampleConfigNtuple.json
[//]: #
[//]: #This will produce the output file sample_output_ntuple.root which uses the NtupleConsumerBase class to produce
[//]: #ROOT Ntuples instead of histograms.
[//]: #
[//]: #
[//]: #Code checks and static analysis
[//]: #-------------------------------
[//]: #
[//]: #Information about compiler warnings and static code analysis can be found at [Core/docs/linter.md](Core/docs/linter.md).
[//]: #
