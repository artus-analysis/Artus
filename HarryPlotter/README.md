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

