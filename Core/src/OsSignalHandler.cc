/* Copyright (c) 2013 - All Rights Reserved
 *   Thomas Hauth  <Thomas.Hauth@cern.ch>
 *   Joram Berger  <Joram.Berger@cern.ch>
 *   Dominik Haitz <Dominik.Haitz@kit.edu>
 */

#include "Artus/Core/interface/OsSignalHandler.h"
#include "Artus/Utility/interface/ArtusLogging.h"

#include <csignal>
#include <atomic>

std::atomic<bool> osSignal_hasSIGINT(false);

void osRegisterHandler()
{
	signal(SIGINT, osSignalHandler);
}

void osUnregisterHandler()
{
	signal(SIGINT, SIG_DFL);
}

bool osHasSIGINT()
{
	return osSignal_hasSIGINT.load();
}

void osSignalHandler(int signum)
{
	LOG(INFO) << "Signal SIGINT received";
	osSignal_hasSIGINT.store(true);
}

void osSignalReset()
{
	osSignal_hasSIGINT.store(false);
}
