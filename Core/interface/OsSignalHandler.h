
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
