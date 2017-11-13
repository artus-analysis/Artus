
#pragma once

#include <iostream>

#include <TObject.h>
#include <TH1D.h>
#include <TH2D.h>
#include <TProfile.h>
#include <TProfile2D.h>
#include <TDirectory.h>
#include <TFile.h>
#include <TGraphErrors.h>

#include "Artus/KappaTools/interface/String.h"

#include "ArtusLogging.h"


class RootFileHelper {
public:

	template<class T>
	static T* SafeGet(TDirectory* directory, std::string const& objectName,
	                 bool detachFromDirectory=true)
	{
		T* object = static_cast<T*>(directory->Get(objectName.c_str()));
		if (object == nullptr)
		{
			LOG(FATAL) << "Cannot load \"" << objectName << "\" from directory \"" << directory->GetName() << "\"!";
		}
		if (detachFromDirectory)
		{
			object->SetDirectory(0);
			object->SetName(randomstr(object->GetName()));
		}
		return object;
	}

	template<class T>
	static T* SafeGet(std::string const& fileName, std::string const& objectName,
	                 bool detachFromDirectory=true)
	{
		TFile* rootFile = new TFile(fileName.c_str(), "READ");
		T* object = RootFileHelper::SafeGet<T>(rootFile, objectName, detachFromDirectory);
		if (detachFromDirectory)
		{
			rootFile->Close();
			delete rootFile;
		}
		return object;
	}
	
	template<class T>
	static std::vector<T*> SafeGetVector(TDirectory* directory,
	                                     std::vector<std::string> const& objectNames,
	                                     bool detachFromDirectory=true)
	{
		std::vector<T*> objects(objectNames.size());
		size_t index = 0;
		for (std::vector<std::string>::const_iterator objectName = objectNames.begin();
		     objectName != objectNames.end(); ++objectName)
		{
			objects[index++] = RootFileHelper::SafeGet<T>(directory, *objectName, detachFromDirectory);
		}
		return objects;
	}
	
	template<class T>
	static std::vector<T*> SafeGetVector(std::vector<TDirectory*> const& directories,
	                                     std::string const& objectName,
	                                     bool detachFromDirectory=true)
	{
		std::vector<T*> objects(directories.size());
		size_t index = 0;
		for (std::vector<TDirectory*>::const_iterator directory = directories.begin();
		     directory != directories.end(); ++directory)
		{
			objects[index++] = RootFileHelper::SafeGet<T>(*directory, objectName, detachFromDirectory);
		}
		return objects;
	}
	
	template<class T>
	static std::vector<T*> SafeGetVector(std::string const& fileName,
	                                     std::vector<std::string> const& objectNames,
	                                     bool detachFromDirectory=true)
	{
		TFile* rootFile = new TFile(fileName.c_str(), "READ");
		std::vector<T*> objects = RootFileHelper::SafeGetVector<T>(rootFile, objectNames, detachFromDirectory);
		if (detachFromDirectory)
		{
			rootFile->Close();
			delete rootFile;
		}
		return objects;
	}
	
	template<class T>
	static std::vector<T*> SafeGetVector(std::vector<std::string> const& fileNames,
	                                     std::string const& objectName,
	                                     bool detachFromDirectory=true)
	{
		std::vector<T*> objects(fileNames.size());
		size_t index = 0;
		for (std::vector<std::string>::const_iterator fileName = fileNames.begin();
		     fileName != fileNames.end(); ++fileName)
		{
			TFile* rootFile = new TFile(fileName->c_str(), "READ");
			objects[index++] = RootFileHelper::SafeGet<T>(rootFile, objectName, detachFromDirectory);
			if (detachFromDirectory)
			{
				rootFile->Close();
				delete rootFile;
			}
		}
		return objects;
	}
	
	template<class TKey, class T>
	static std::map<TKey, T*> SafeGetMap(TDirectory* directory,
	                                     std::map<TKey, std::string> const& objectNames,
	                                     bool detachFromDirectory=true)
	{
		std::map<TKey, T*> objects;
		for (typename std::map<TKey, std::string>::const_iterator objectName = objectNames.begin();
		     objectName != objectNames.end(); ++objectName)
		{
			objects[objectName->first] = RootFileHelper::SafeGet<T>(directory, objectName->second, detachFromDirectory);
		}
		return objects;
	}
	
	template<class TKey, class T>
	static std::map<TKey, T*> SafeGetMap(std::map<TKey, TDirectory*> const& directories,
	                                     std::string const& objectName,
	                                     bool detachFromDirectory=true)
	{
		std::map<TKey, T*> objects;
		for (typename std::map<TKey, TDirectory*>::const_iterator directory = directories.begin();
		     directory != directories.end(); ++directory)
		{
			objects[directory->first] = RootFileHelper::SafeGet<T>(directory->second, objectName, detachFromDirectory);
		}
		return objects;
	}
	
	template<class TKey, class T>
	static std::map<TKey, T*> SafeGetMap(std::string const& fileName,
	                                     std::map<TKey, std::string> const& objectNames,
	                                     bool detachFromDirectory=true)
	{
		TFile* rootFile = new TFile(fileName.c_str(), "READ");
		std::map<TKey, T*> objects = RootFileHelper::SafeGetMap<TKey, T>(rootFile, objectNames, detachFromDirectory);
		if (detachFromDirectory)
		{
			rootFile->Close();
			delete rootFile;
		}
		return objects;
	}
	
	template<class TKey, class T>
	static std::map<TKey, T*> SafeGetMap(std::map<TKey, std::string> const& fileNames,
	                                     std::string const& objectName,
	                                     bool detachFromDirectory=true)
	{
		std::map<TKey, T*> objects;
		for (typename std::map<TKey, std::string>::const_iterator fileName = fileNames.begin();
		     fileName != fileNames.end(); ++fileName)
		{
			TFile* rootFile = new TFile(fileName->second.c_str(), "READ");
			objects[fileName->first] = RootFileHelper::SafeGet<T>(rootFile, objectName, detachFromDirectory);
			if (detachFromDirectory)
			{
				rootFile->Close();
				delete rootFile;
			}
		}
		return objects;
	}
	
	template<class TKey, class T>
	static std::map<TKey, std::vector<T*> > SafeGetMap(TDirectory* directory,
	                                                   std::map<TKey, std::vector<std::string> > const& objectNames,
	                                                   bool detachFromDirectory=true)
	{
		std::map<TKey, std::vector<T*> > objects;
		for (typename std::map<TKey, std::vector<std::string> >::const_iterator objectName = objectNames.begin();
		     objectName != objectNames.end(); ++objectName)
		{
			objects[objectName->first] = RootFileHelper::SafeGetVector<T>(directory, objectName->second, detachFromDirectory);
		}
		return objects;
	}
	
	template<class TKey, class T>
	static std::map<TKey, std::vector<T*> > SafeGetMap(std::map<TKey, std::vector<TDirectory*> > const& directories,
	                                                   std::string const& objectName,
	                                                   bool detachFromDirectory=true)
	{
		std::map<TKey, std::vector<T*> > objects;
		for (typename std::map<TKey, std::vector<TDirectory*> >::const_iterator directory = directories.begin();
		     directory != directories.end(); ++directory)
		{
			objects[directory->first] = RootFileHelper::SafeGetVector<T>(directory->second, objectName, detachFromDirectory);
		}
		return objects;
	}
	
	template<class TKey, class T>
	static std::map<TKey, std::vector<T*> > SafeGetMap(std::string const& fileName,
	                                                   std::map<TKey, std::vector<std::string> > const& objectNames,
	                                                   bool detachFromDirectory=true)
	{
		TFile* rootFile = new TFile(fileName.c_str(), "READ");
		std::map<TKey, std::vector<T*> > objects = RootFileHelper::SafeGetMap<TKey, T>(rootFile, objectNames, detachFromDirectory);
		if (detachFromDirectory)
		{
			rootFile->Close();
			delete rootFile;
		}
		return objects;
	}
	
	template<class TKey, class T>
	static std::map<TKey, std::vector<T*> > SafeGetMap(std::map<TKey, std::vector<std::string> > const& fileNames,
	                                                   std::string const& objectName,
	                                                   bool detachFromDirectory=true)
	{
		std::map<TKey, std::vector<T*> > objects;
		for (typename std::map<TKey, std::vector<std::string> >::const_iterator fileName = fileNames.begin();
		     fileName != fileNames.end(); ++fileName)
		{
			objects[fileName->first] = RootFileHelper::SafeGetVector<T>(fileName->second, objectName, detachFromDirectory);
		}
		return objects;
	}

	static void SafeCd(TDirectory* directory, std::string const& dirName);
	static TH1D* GetStandaloneTH1D_1(std::string sName, std::string sCaption,
			int binCount, double dCustomBins[255]);
	static TH1D* GetStandaloneTH1D_2(std::string sName, std::string sCaption,
			int binCount, double min, double max);
	static TProfile* GetStandaloneTProfile(std::string m_sName,
			std::string m_sCaption, int m_iBinXCount, double m_dBinXLower,
			double m_dBinXUpper);
	static TGraphErrors* GetStandaloneTGraphErrors(int i);
	static TH2D* GetStandaloneTH2D_1(std::string m_sName,
			std::string m_sCaption, int m_iBinXCount, double m_dBinXLower,
			double m_dBinXUpper, int m_iBinYCount, double m_dBinYLower,
			double m_dBinYUpper);
	static TProfile2D* GetStandaloneTProfile2D(std::string m_sName,
			std::string m_sCaption, int m_iBinXCount, double m_dBinXLower,
			double m_dBinXUpper, int m_iBinYCount, double m_dBinYLower,
			double m_dBinYUpper);
	
	static void WriteRootObject(TDirectory* directory, TObject* object, std::string path);

};
