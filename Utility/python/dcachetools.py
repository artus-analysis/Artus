
# -*- coding: utf-8 -*-

import logging
import Artus.Utility.logger as logger
log = logging.getLogger(__name__)

import copy


xrd_srm_replacements = {
	"root://dcache-cms-xrootd.desy.de:1094/" : "srm://dcache-se-cms.desy.de:8443/srm/managerv2?SFN=/pnfs/desy.de/cms/tier2/",
	"root://grid-vo-cms.physik.rwth-aachen.de:1094/" : "srm://grid-srm.physik.rwth-aachen.de:8443/srm/managerv2?SFN=/pnfs/physik.rwth-aachen.de/cms/",
	"root://cmsxrootd.gridka.de//pnfs/gridka.de/dcms/disk-only/" : "srm://dgridsrm-fzk.gridka.de:8443/srm/managerv2?SFN=/pnfs/gridka.de/dcms/disk-only/",
	#"root://cmsxrootd.gridka.de//" : "",
}

srm_dcap_replacements = {
	"srm://dcache-se-cms.desy.de:8443/srm/managerv2?SFN=/pnfs/desy.de/cms/tier2/" : "dcap://dcache-cms-dcap.desy.de//pnfs/desy.de/cms/tier2/",
	"srm://grid-srm.physik.rwth-aachen.de:8443/srm/managerv2?SFN=/pnfs/physik.rwth-aachen.de/cms/" : "dcap://grid-dcap-extern.physik.rwth-aachen.de/pnfs/physik.rwth-aachen.de/cms/",
	"srm://dgridsrm-fzk.gridka.de:8443/srm/managerv2?SFN=/pnfs/gridka.de/dcms/disk-only/" : "dcap://dcnrgdcap.gridka.de:22125//pnfs/gridka.de/dcms/disk-only/",
	#"" : "dcap://dccmsdcap.gridka.de:22125//pnfs/gridka.de/cms/disk-only/",
}

dcap_xrd_replacements = {
	"dcap://dcache-cms-dcap.desy.de//pnfs/desy.de/cms/tier2/" : "root://dcache-cms-xrootd.desy.de:1094/",
	"dcap://grid-dcap-extern.physik.rwth-aachen.de/pnfs/physik.rwth-aachen.de/cms/" : "root://grid-vo-cms.physik.rwth-aachen.de:1094/",
	"dcap://dcnrgdcap.gridka.de:22125//pnfs/gridka.de/dcms/disk-only/" : "root://cmsxrootd.gridka.de//pnfs/gridka.de/dcms/disk-only/",
	#"dcap://dccmsdcap.gridka.de:22125//pnfs/gridka.de/cms/disk-only/" : "root://cmsxrootd.gridka.de//",
}

def xrd2srm(path):
	result = copy.deepcopy(path)
	for src, dst in xrd_srm_replacements.iteritems():
		result = result.replace(src, dst)
	return result

def srm2xrd(path):
	result = copy.deepcopy(path)
	for dst, src in xrd_srm_replacements.iteritems():
		result = result.replace(src, dst)
	return result

def srm2dcap(path):
	result = copy.deepcopy(path)
	for src, dst in srm_dcap_replacements.iteritems():
		result = result.replace(src, dst)
	return result

def dcap2srm(path):
	result = copy.deepcopy(path)
	for dst, src in srm_dcap_replacements.iteritems():
		result = result.replace(src, dst)
	return result

def dcap2xrd(path):
	result = copy.deepcopy(path)
	for src, dst in dcap_xrd_replacements.iteritems():
		result = result.replace(src, dst)
	return result

def xrd2dcap(path):
	result = copy.deepcopy(path)
	for dst, src in dcap_xrd_replacements.iteritems():
		result = result.replace(src, dst)
	return result
