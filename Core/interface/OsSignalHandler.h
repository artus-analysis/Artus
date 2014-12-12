/* Copyright (c) 2013 - All Rights Reserved
 *   Thomas Hauth  <Thomas.Hauth@cern.ch>
 *   Joram Berger  <Joram.Berger@cern.ch>
 *   Dominik Haitz <Dominik.Haitz@kit.edu>
 */

#pragma once

// registering signal hander with Os
void osRegisterHandler();

// unregister installed signals
void osUnregisterHandler();

// call this to query whether the SIGINT signal was raised
bool osHasSIGINT();

// signal handler called by the OS
void osSignalHandler(int signum);

// sets all signals back to not-fired
void osSignalReset();
