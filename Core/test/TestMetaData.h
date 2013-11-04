#pragma once

class TestMetaData {
public:
	TestMetaData() :
			iGlobalMetaData(0) {
	}

	struct LocalData {
		LocalData() :
				iMetaData(0) {
		}

		int iMetaData;
	};

	typedef LocalData LocalMetaDataType;

	void SetLocalMetaData(LocalMetaDataType * pipelineMetaData) {
		m_localData = pipelineMetaData;
	}

	void ClearContent() {
	}

	LocalMetaDataType * GetLocalMetaData() const {
		assert(m_localData != NULL);
		return m_localData;
	}
	int iGlobalMetaData;

private:

	LocalMetaDataType * m_localData;
};
