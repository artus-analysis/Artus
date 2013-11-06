#pragma once

#include "Artus/Core/interface/GlobalInclude.h"

/*

Base class for all Plotting-related classes. Stores
Caption, Name and root output folder for the consumer.

*/

template<typename TPlotType>
class PlotBase {
public:
	PlotBase(std::string sName, std::string sFolder) :
			m_sName(sName) {
		SetRootFileFolder(sFolder);
		SetNameAndCaption(sName);
	}

	void SetNameAndCaption(std::string sName) {
		m_sName = m_sCaption = sName;
	}

	std::string GetName() const {
		return m_sName;
	}
	std::string GetCaption() const {
		return m_sCaption;
	}

private:

	std::string m_sName;
	std::string m_sCaption;

IMPL_PROPERTY(std::string, RootFileFolder)

};
