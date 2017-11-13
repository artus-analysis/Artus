/* Copyright (c) 2011 - All Rights Reserved
 *   Fred Stober <stober@cern.ch>
 */

#ifndef KAPPA_LSWATCHER_H
#define KAPPA_LSWATCHER_H

#include <Kappa/DataFormats/interface/KInfo.h>

class LSWatcher
{
public:
	LSWatcher();
	bool Changed(KEventInfo *meta_event = 0);
private:
	bool changed;
	run_id run;
	lumi_id lumi;
};

#endif
