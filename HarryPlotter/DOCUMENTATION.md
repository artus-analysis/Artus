-------------------------------------------------------------------------------
		HARRY PLOTTER - the plot wizard
-------------------------------------------------------------------------------

HarryPlotter can create Matplotlib and ROOT plots from any ROOT Ntuple or 
histogram, but is optimized for scientific plots in High Energy particle physics.

The main object is one large python dictionary (plotdict) which holds all 
necessary plot metadata and even the actual plot data itself.
A large library of helper functions is used to create and modify histogram or 
plot data.

DONT FORGET TO SOURCE THE harry_ini.sh INITIALIZATION SCRIPT!


Step 1: Creating the plotdict dictionary
-------------------------------------------------------------------------------
There are several ways an user can put together the plotdict:
	1. With Harry's built-in command-line parser (shell scripts are possible)
	3. With python scripts
	4. With json config files

Type `harry --help` to display a list with the available options.

The `get_basic_parser` function returns the parser, and 
`create_dictionary_from_parser` creates a complete plotdict.


Step 2: Processing the plotdict dictionary
-------------------------------------------------------------------------------
The plotdict is given to the `plot` function in the harrycore.
If 

