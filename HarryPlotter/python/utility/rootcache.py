# -*- coding: utf-8 -*-

import logging
import Artus.Utility.logger as logger
log = logging.getLogger(__name__)

import abc
import copy
import hashlib
import os
import tempfile

import ROOT

import Artus.Utility.tfilecontextmanager as tfilecontextmanager


class Cache(object):
	def __call__(self, function_to_cache):
		self._function_to_cache = function_to_cache
		return self._get_cached
	
	@abc.abstractmethod
	def _get_cached(self, *args, **kwargs):
		pass


class RootFileCache(Cache):
	def __init__(self, cache_dir=None):
		self.cache_name = "cached_object"
		self.cache_dir = cache_dir
		if self.cache_dir is None:
			self.cache_dir = os.path.join(tempfile.gettempdir(), "root_file_cache")
		if not os.path.exists(self.cache_dir):
			os.makedirs(self.cache_dir)
		
	def _determine_cache_file(self, *args, **kwargs):
		tmp_args = copy.deepcopy(args)
		for index, arg in enumerate(tmp_args):
			if isinstance(arg, ROOT.TTree):
				tmp_args[index] = arg.GetName()
			elif isinstance(arg, ROOT.TH1):
				tmp_args[index] = arg.GetName()
		
		tmp_kwargs = copy.deepcopy(kwargs)
		for keyword, arg in tmp_kwargs.iteritems():
			if isinstance(arg, ROOT.TTree):
				tmp_kwargs[keyword] = arg.GetName() # TODO: handle input files/folders
			elif isinstance(arg, ROOT.TH1):
				tmp_kwargs[keyword] = arg.GetName() # TODO: handle binning
		
		args_hash = hashlib.md5("".join([str(a) for a in list(tmp_args)+tmp_kwargs.items()])).hexdigest()
		cache_file = os.path.join(self.cache_dir, args_hash+".root")
		return cache_file
	
	def _get_cached(self, *args, **kwargs):
		cache_file = self._determine_cache_file(*args, **kwargs)
		root_object = None
		if os.path.exists(cache_file):
			with tfilecontextmanager.TFileContextManager(cache_file, "READ") as root_file:
				root_object = root_file.Get(self.cache_name)
				root_object.SetDirectory(0)
				if not root_object is None:
					log.debug("Took cached object from \"{root_file}/{path_to_object}\".".format(root_file=cache_file, path_to_object=self.cache_name))
		
		if root_object is None:
			root_object = self._function_to_cache(*args, **kwargs)
			with tfilecontextmanager.TFileContextManager(cache_file, "RECREATE") as root_file:
				root_file.cd()
				root_object.Write(self.cache_name, ROOT.TObject.kWriteDelete)
				log.debug("Created cache in \"{root_file}/{path_to_object}\".".format(root_file=cache_file, path_to_object=self.cache_name))
				root_object.SetDirectory(0)
		
		return root_object

