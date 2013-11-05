#pragma once

class SettingsBase {
public:

	// todo: set prop tree

	// todo: read from file
	virtual size_t GetLevel() const {
		return 1;
	}

	virtual std::string ToString() const {
		return "SettingsBase - ToString";
	}

};
