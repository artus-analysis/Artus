# -*- coding: utf-8 -*-

import logging
import Artus.Utility.logger as logger
log = logging.getLogger(__name__)

import Artus.Configuration.artusWrapper as artusWrapper


class KappaAnalysisWrapper(artusWrapper.ArtusWrapper):

	def _initArgumentParser(self, userArgParsers=None):
		super(KappaAnalysisWrapper, self)._initArgumentParser(userArgParsers)
		#self._parser.set_defaults(ld_library_paths=["$CMSSW_BASE/src/Kappa/lib/"])

