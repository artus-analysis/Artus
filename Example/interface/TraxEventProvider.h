#pragma once

#include "Artus/Provider/interface/RootEventProvider.h"

#include "TraxTypes.h"
#include "TraxEventData.h"

class TraxEventProvider: public RootEventProvider<TraxTypes::event_type> {

};
