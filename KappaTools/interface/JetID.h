/* Copyright (c) 2010 - All Rights Reserved
 *   Fred Stober <stober@cern.ch>
 *   Manuel Zeise <zeise@cern.ch>
 */

#ifndef KAPPA_JETID_H
#define KAPPA_JETID_H

#include <Kappa/DataFormats/interface/KJetMET.h>

template<typename T>
struct JetIDLoose
{
	typedef const T &argument_type;
	bool operator()(const T &jet) const { return true; }
};

template<>
struct JetIDLoose<KCaloJet>
{
	typedef const KCaloJet &argument_type;
	bool operator()(const KCaloJet &jet) const;
};

template<>
struct JetIDLoose<KBasicJet>
{
	typedef const KBasicJet &argument_type;
	bool operator()(const KBasicJet &jet) const;
};

template<typename T>
struct JetIDTight
{
	typedef const T &argument_type;
	bool operator()(const T &jet) const { return true; }
};

template<>
struct JetIDTight<KBasicJet>
{
	typedef const KBasicJet &argument_type;
	bool operator()(const KBasicJet &jet) const;
};

template<typename T>
struct JetIDLooseFail
{
	typedef const T &argument_type;
	bool operator()(const T &jet) const { return !selector(jet); }
private:
	JetIDLoose<T> selector;
};

#endif
