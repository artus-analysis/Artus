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
import Artus.Utility.tools as tools


class Cache(object):
	def __call__(self, function_to_cache):
		self._function_to_cache = function_to_cache
		return self._get_cached
	
	@abc.abstractmethod
	def _get_cached(self, *args, **kwargs):
		pass


class RootFileCache(Cache):
	def __init__(self, cache_dir=None, args_groups=[[0,2],[1,3,4],[5,7,8,10,11,12],[6,9]], kwargs_groups=[["root_file_names","friend_files"],["path_to_trees","friend_folders","friend_aliases",],["root_histogram","name","binning","option","proxy_prefix","scan"],["variable_expression","weight_selection"]]):
		self.cache_name = "cached_object"
		self.cache_dir = os.path.expandvars(cache_dir)
		if self.cache_dir is None:
			self.cache_dir = os.path.join(tempfile.gettempdir(), "root_file_cache")
		if not os.path.exists(self.cache_dir):
			try:
				os.makedirs(self.cache_dir)
			except:
				# this message does not work due to missing logging handlers
				#log.warning("Unable to write caches to \"{cache_dir}\"! Run without caching.".format(cache_dir=self.cache_dir))
				self.cache_dir = None
		
		self.args_groups = copy.deepcopy(args_groups)
		flat_args_groups = tools.flattenList(self.args_groups)
		missing_indices = list(set(range(max(flat_args_groups)+1))-set(flat_args_groups))
		if len(missing_indices) > 0:
			self.args_groups.append(missing_indices)
		self.args_groups.append([max(flat_args_groups)+1])
		self.kwargs_groups = copy.deepcopy(kwargs_groups)
		
	def _determine_cache_file(self, *args, **kwargs):
		if self.cache_dir is None:
			return None
		
		tmp_args = copy.deepcopy(args)
		for index, arg in enumerate(tmp_args):
			if isinstance(arg, ROOT.TObject):
				tmp_args[index] = arg.GetName()

		tmp_kwargs = copy.deepcopy(kwargs)
		tmp_kwargs.pop("redo_cache")
		for keyword, arg in tmp_kwargs.iteritems():
			if isinstance(arg, ROOT.TObject):
				tmp_kwargs[keyword] = arg.GetName()
		
		hashes = []
		for args_group in self.args_groups[:-1]:
			args_group_list = [tmp_args[index] for index in args_group if index < len(tmp_args)]
			if len(args_group_list) > 0:
				args_group_hash = hashlib.md5("".join([str(arg) for arg in args_group_list])).hexdigest()
				hashes.append(args_group_hash)
		
		args_group_list = [tmp_args[index] for index in range(self.args_groups[-1][0], len(tmp_args))]
		if len(args_group_list) > 0:
			args_group_hash = hashlib.md5("".join([str(arg) for arg in args_group_list])).hexdigest()
			hashes.append(args_group_hash)
		
		for kwargs_group in self.kwargs_groups:
			kwargs_group_list = {key : tmp_kwargs[key] for key in kwargs_group if key in tmp_kwargs}
			if len(kwargs_group_list) > 0:
				kwargs_group_hash = hashlib.md5("".join([str(kwarg) for kwarg in kwargs_group_list.items()])).hexdigest()
				hashes.append(kwargs_group_hash)
		
		kwargs_group_list = {key : tmp_kwargs[key] for key in list(set(tmp_kwargs.keys())-set(tools.flattenList(self.kwargs_groups)))}
		if len(kwargs_group_list) > 0:
			kwargs_group_hash = hashlib.md5("".join([str(kwarg) for kwarg in kwargs_group_list.items()])).hexdigest()
			hashes.append(kwargs_group_hash)
		
		cache_file = os.path.join(self.cache_dir, *hashes)+".root"
		return cache_file
	
	def _get_cached(self, *args, **kwargs):
		cache_file = self._determine_cache_file(*args, **kwargs)
		root_tree = None
		root_object = None
		tmp_files = []
		cache_found = False
		if (not kwargs.get("redo_cache", False)) and cache_file and os.path.exists(cache_file):
			try:
				with tfilecontextmanager.TFileContextManager(cache_file, "READ") as root_file:
					root_object = root_file.Get(self.cache_name)
					root_object.SetDirectory(0)
					if (not root_object is None) and (not root_object == None):
						cache_found = True
						log.debug("Took cached object from \"{root_file}/{path_to_object}\".".format(root_file=cache_file, path_to_object=self.cache_name))
			except:
				root_tree = None
				root_object = None
				cache_found = False
		
		if root_object is None:
			root_tree, root_object, tmp_files = self._function_to_cache(*args, **kwargs)
			if cache_file and (not cache_found) and (not root_object is None) and (not root_object == None):
				try:
					full_cache_dir = os.path.dirname(cache_file)
					if not os.path.exists(full_cache_dir):
						os.makedirs(full_cache_dir)
					
					with tfilecontextmanager.TFileContextManager(cache_file, "RECREATE") as root_file:
						root_file.cd()
						root_object.Write(self.cache_name, ROOT.TObject.kWriteDelete)
						log.debug("Created cache in \"{root_file}/{path_to_object}\".".format(root_file=cache_file, path_to_object=self.cache_name))
						root_object.SetDirectory(0)
				except:
					pass
		
		return root_tree, root_object, tmp_files
