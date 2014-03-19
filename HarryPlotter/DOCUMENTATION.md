		HARRY PLOTTER - the plot wizard
===============================================================================

HarryPlotter can create Matplotlib and ROOT plots from any ROOT Ntuple or 
histogram and is optimized for scientific plots in High Energy particle physics.

The main object is one large python dictionary (plotdict) which holds all 
necessary plot metadata and even the actual plot data itself.
A comprehensive library of helper functions is used to create and modify histogram or 
plot data.

DONT FORGET TO SOURCE THE ini_harry.sh INITIALIZATION SCRIPT!

It is located in the Artus/HarryPlotter/scripts folder.


1. Workflow overview
-------------------------------------------------------------------------------

There are several ways an user can put together the plotdict:
	1. With Harry's built-in command-line parser (shell scripts are possible)
	3. With python scripts
	4. With json config files

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

If you have sourced the harry_ini.sh, the `harry` executable will be available.
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

