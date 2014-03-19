		ArtusWrapper and JsonTools
===============================================================================


1. Constructing Json Config Dict with the Command Line Tools
-------------------------------------------------------------------------------

Simple Json config dict merged from two files:
	`artusExample.py @$CMSSW_BASE/src/Artus/Example/data/jsonConfigExamples/wrapperParameters/wrapperParameters1.cfg`

Simple base configs gets extended by two pipeline configs:
	`artusExample.py @$CMSSW_BASE/src/Artus/Example/data/jsonConfigExamples/wrapperParameters/wrapperParameters2.cfg`

More complex construction of pipeline variations with outer products of Json dicts
	`artusExample.py @$CMSSW_BASE/src/Artus/Example/data/jsonConfigExamples/wrapperParameters/wrapperParameters3.cfg`
	`artusExample.py @$CMSSW_BASE/src/Artus/Example/data/jsonConfigExamples/wrapperParameters/wrapperParameters4.cfg`


2. Treatment of comments and includes in Json Config Dicts
-------------------------------------------------------------------------------

Comment options:
	`artusExample.py @$CMSSW_BASE/src/Artus/Example/data/jsonConfigExamples/includesAndComments/wrapperParameters1.cfg`

Include options:
	`artusExample.py @$CMSSW_BASE/src/Artus/Example/data/jsonConfigExamples/includesAndComments/wrapperParameters2.cfg`

