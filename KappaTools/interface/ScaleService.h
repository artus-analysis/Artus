/* Copyright (c) 2012 - All Rights Reserved
 *   Fred Stober <stober@cern.ch>
 *   Manuel Zeise <zeise@cern.ch>
 */

#ifndef SCALESERVICE_H
#define SCALESERVICE_H

#include <Kappa/DataFormats/interface/Kappa.h>
#include <set>
#include <TH1D.h>
typedef int hlt_prescale_id;

class ScaleService
{
public:
	ScaleService(const unsigned long long _events, const double _xsec_mc, const double _lumi_est, const double _lumi_db,
		const std::map<run_id, std::map<lumi_id, double> > &_recorded,
		const std::map<std::string, hlt_prescale_id> &_prescaleIDs,
		const std::map<run_id, std::map<hlt_prescale_id, int> > &_prescale,
		const std::map<run_id, std::map<hlt_prescale_id, std::map<lumi_id, int> > > &_prescaleDetail,
		const std::vector<double> &_pileup_weight)
		: events(_events), xsec_mc(_xsec_mc), lumi_est(_lumi_est), lumi_db(_lumi_db),
			recorded(_recorded), prescaleIDs(_prescaleIDs), prescale(_prescale), prescaleDetail(_prescaleDetail),
			pileup_weight(_pileup_weight)
	{
	}

	void print();

	inline int prescale_DataDB(size_t hltIdx, KLumiInfo *info_lumi) const
	{
		std::map<std::string, hlt_prescale_id>::const_iterator itIdx =
			prescaleIDs.find(info_lumi->hltNames[hltIdx]);
		assert(itIdx != prescaleIDs.end());
		hlt_prescale_id psIdx = itIdx->second;

		run_id nRun = info_lumi->nRun;
		std::map<run_id, std::map<hlt_prescale_id, int> >::const_iterator itPSRun =
			prescale.find(nRun);
		assert(itPSRun != prescale.end());
		std::map<hlt_prescale_id, int>::const_iterator itPSHLT =
			itPSRun->second.find(psIdx);
		assert(itPSHLT != itPSRun->second.end());

		int ps = itPSHLT->second;

		std::map<run_id, std::map<hlt_prescale_id, std::map<lumi_id, int> > >::const_iterator itPSDRun =
			prescaleDetail.find(nRun);
		if (itPSDRun == prescaleDetail.end())
			return ps;
		std::map<hlt_prescale_id, std::map<lumi_id, int> >::const_iterator itPSDHLT =
			itPSDRun->second.find(psIdx);
		if (itPSDHLT == itPSDRun->second.end())
			return ps;
		lumi_id nLumi = info_lumi->nLumi;
		std::map<lumi_id, int>::const_iterator itPSDLumi =
			itPSDHLT->second.find(nLumi);
		if (itPSDLumi == itPSDHLT->second.end())
			return ps;
		return itPSDLumi->second;
	}

	inline double weight_Data(size_t hltIdx, KLumiInfo *info_lumi)
	{
		int ps_info = info_lumi->hltPrescales[hltIdx];
		if (prescaleIDs.size() == 0)
			return ps_info;
		int ps_db = prescale_DataDB(hltIdx, info_lumi);
		if (ps_db != ps_info)
		{
//			std::cerr << "Differing prescales found! " << ps_db << " " << ps_info << " in "
//				<< info_lumi->nRun << ":" << info_lumi->nLumi << std::endl;
			brokenPS[info_lumi->nRun].insert(info_lumi->nLumi);
//			exit(1);
		}
//		assert(ps_db == ps_info);
		return ps_info;
	}

	inline double weight_MC(KGenEventInfo *info_event_gen) const
	{
		return info_event_gen->weight * pileup_weight[int(info_event_gen->nPUMean)];
	}

	inline double scale_Data() const
	{
		return lumi_db;
	}

	inline double scale_MC() const
	{
		return 1.0 / (xsec_mc / events);
	}

	const std::map<run_id, std::set<lumi_id> > &getBrokenPS()
	{
		return brokenPS;
	}

private:
	unsigned long long events;
	double xsec_mc, lumi_est, lumi_db;

	std::map<run_id, std::set<lumi_id> > brokenPS;

	const std::map<run_id, std::map<lumi_id, double> > &recorded;

	const std::map<std::string, hlt_prescale_id> &prescaleIDs;
	const std::map<run_id, std::map<hlt_prescale_id, int> > &prescale;
	const std::map<run_id, std::map<hlt_prescale_id, std::map<lumi_id, int> > > &prescaleDetail;

	const std::vector<double> &pileup_weight;
};

class ScaleServiceFactory
{
public:
	ScaleServiceFactory(const bool _doPrescales = true, TH1D *pu_data = 0, TH1D *pu_mc = 0) {};

	void registerMC(KLumiInfo *info_lumi) {};
	void registerData(KDataLumiInfo *info_lumi) {};
	void registerLF(std::string lumiPath) {};
	ScaleService *finish(unsigned long long events, const double userXsec = -1, const double userLumi = -1) { return 0; };

private:
	std::map<run_id, std::map<lumi_id, double> > recorded;

	std::map<std::string, hlt_prescale_id> prescaleIDs;
	std::map<run_id, std::map<hlt_prescale_id, int> > prescale;
	std::map<run_id, std::map<hlt_prescale_id, std::map<lumi_id, int> > > prescaleDetail;

	std::vector<double> pileup_weight;
};

#endif
