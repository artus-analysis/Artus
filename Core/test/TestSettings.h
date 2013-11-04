#pragma once

class TestSettings {
public:

	TestSettings() :
			m_Level(1) {

	}

	std::string ToString() const {
		return "Test setting";
	}

	IMPL_PROPERTY(unsigned int, Level)

};
