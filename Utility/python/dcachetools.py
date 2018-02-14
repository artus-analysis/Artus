
# -*- coding: utf-8 -*-

import logging
import Artus.Utility.logger as logger
log = logging.getLogger(__name__)

import copy



dcap_local_replacements = {
	"dcap://dcache-cms-dcap.desy.de//pnfs/desy.de/cms/tier2/" : "/pnfs/desy.de/cms/tier2/",
	"dcap://grid-dcap-extern.physik.rwth-aachen.de/pnfs/physik.rwth-aachen.de/cms/" : "/pnfs/physik.rwth-aachen.de/cms/",
	"dcap://dcnrgdcap.gridka.de:22125//pnfs/gridka.de/dcms/disk-only/" : "/pnfs/gridka.de/dcms/disk-only/",
	#"dcap://dccmsdcap.gridka.de:22125//pnfs/gridka.de/cms/disk-only/" : "root://cmsxrootd.gridka.de//",
}

dcap_xrd_replacements = {
	"dcap://dcache-cms-dcap.desy.de//pnfs/desy.de/cms/tier2/" : "root://dcache-cms-xrootd.desy.de:1094/",
	"dcap://grid-dcap-extern.physik.rwth-aachen.de/pnfs/physik.rwth-aachen.de/cms/" : "root://grid-vo-cms.physik.rwth-aachen.de:1094/",
	"dcap://dcnrgdcap.gridka.de:22125//pnfs/gridka.de/dcms/disk-only/" : "root://cmsxrootd.gridka.de//pnfs/gridka.de/dcms/disk-only/",
	#"dcap://dccmsdcap.gridka.de:22125//pnfs/gridka.de/cms/disk-only/" : "root://cmsxrootd.gridka.de//",
}

srm_local_replacements = {
	"srm://dcache-se-cms.desy.de:8443/srm/managerv2?SFN=/pnfs/desy.de/cms/tier2/" : "/pnfs/desy.de/cms/tier2/",
	"srm://grid-srm.physik.rwth-aachen.de:8443/srm/managerv2?SFN=/pnfs/physik.rwth-aachen.de/cms/" : "/pnfs/physik.rwth-aachen.de/cms/",
	"srm://dgridsrm-fzk.gridka.de:8443/srm/managerv2?SFN=/pnfs/gridka.de/dcms/disk-only/" : "/pnfs/gridka.de/dcms/disk-only/",
	#"" : "dcap://dccmsdcap.gridka.de:22125//pnfs/gridka.de/cms/disk-only/",
}

srm_dcap_replacements = {
	"srm://dcache-se-cms.desy.de:8443/srm/managerv2?SFN=/pnfs/desy.de/cms/tier2/" : "dcap://dcache-cms-dcap.desy.de//pnfs/desy.de/cms/tier2/",
	"srm://grid-srm.physik.rwth-aachen.de:8443/srm/managerv2?SFN=/pnfs/physik.rwth-aachen.de/cms/" : "dcap://grid-dcap-extern.physik.rwth-aachen.de/pnfs/physik.rwth-aachen.de/cms/",
	"srm://dgridsrm-fzk.gridka.de:8443/srm/managerv2?SFN=/pnfs/gridka.de/dcms/disk-only/" : "dcap://dcnrgdcap.gridka.de:22125//pnfs/gridka.de/dcms/disk-only/",
	#"" : "dcap://dccmsdcap.gridka.de:22125//pnfs/gridka.de/cms/disk-only/",
}

xrd_local_replacements = {
	"root://dcache-cms-xrootd.desy.de:1094/" : "/pnfs/desy.de/cms/tier2",
	"root://grid-vo-cms.physik.rwth-aachen.de:1094/" : "/pnfs/physik.rwth-aachen.de/cms",
	"root://cmsxrootd.gridka.de//pnfs/gridka.de/dcms/disk-only/" : "/pnfs/gridka.de/dcms/disk-only",
	#"root://cmsxrootd.gridka.de//" : "",
}

xrd_srm_replacements = {
	"root://dcache-cms-xrootd.desy.de:1094/" : "srm://dcache-se-cms.desy.de:8443/srm/managerv2?SFN=/pnfs/desy.de/cms/tier2/",
	"root://grid-vo-cms.physik.rwth-aachen.de:1094/" : "srm://grid-srm.physik.rwth-aachen.de:8443/srm/managerv2?SFN=/pnfs/physik.rwth-aachen.de/cms/",
	"root://cmsxrootd.gridka.de//pnfs/gridka.de/dcms/disk-only/" : "srm://dgridsrm-fzk.gridka.de:8443/srm/managerv2?SFN=/pnfs/gridka.de/dcms/disk-only/",
	#"root://cmsxrootd.gridka.de//" : "",
}

xrd_xrd_replacements = {
	"root://dcache-cms-xrootd.desy.de:1094/" : "root://cms-xrd-global.cern.ch/",
	"root://grid-vo-cms.physik.rwth-aachen.de:1094/" : "root://cms-xrd-global.cern.ch/",
	"root://cmsxrootd.gridka.de//pnfs/gridka.de/dcms/disk-only/" : "root://cms-xrd-global.cern.ch/",
	#"root://cmsxrootd.gridka.de//" : "",
}


def dcap2local(path):
	result = copy.deepcopy(path)
	for src, dst in dcap_local_replacements.iteritems():
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

def local2xrd(path):
	result = copy.deepcopy(path)
	for dst, src in xrd_local_replacements.iteritems():
		result = result.replace(src, dst)
	return result

def local2dcap(path):
	result = copy.deepcopy(path)
	for dst, src in dcap_local_replacements.iteritems():
		result = result.replace(src, dst)
	return result

def local2srm(path):
	result = copy.deepcopy(path)
	for dst, src in srm_local_replacements.iteritems():
		result = result.replace(src, dst)
	return result

def srm2dcap(path):
	result = copy.deepcopy(path)
	for src, dst in srm_dcap_replacements.iteritems():
		result = result.replace(src, dst)
	return result

def srm2local(path):
	result = copy.deepcopy(path)
	for src, dst in srm_local_replacements.iteritems():
		result = result.replace(src, dst)
	return result

def srm2xrd(path):
	result = copy.deepcopy(path)
	for dst, src in xrd_srm_replacements.iteritems():
		result = result.replace(src, dst)
	return result

def xrd2dcap(path):
	result = copy.deepcopy(path)
	for dst, src in dcap_xrd_replacements.iteritems():
		result = result.replace(src, dst)
	return result

def xrd2local(path):
	result = copy.deepcopy(path)
	for src, dst in xrd_local_replacements.iteritems():
		result = result.replace(src, dst)
	return result

def xrd2srm(path):
	result = copy.deepcopy(path)
	for src, dst in xrd_srm_replacements.iteritems():
		result = result.replace(src, dst)
	return result

def xrd2xrd(path):
	result = copy.deepcopy(path)
	for src, dst in xrd_xrd_replacements.iteritems():
		result = result.replace(src, dst)
	return result

