/* Copyright (c) 2013 - All Rights Reserved
 *   Thomas Hauth  <Thomas.Hauth@cern.ch>
 *   Joram Berger  <Joram.Berger@cern.ch>
 *   Dominik Haitz <Dominik.Haitz@kit.edu>
 */

#pragma once

#include <assert.h>

/*
 * Convenience class to implement for an arbitrary type TData. This class is useful
 * in conjunction with reading from a Boost PropertyTree or similar
 */
template<class TData>
class VarCache {
public:
	VarCache() :
			m_isCached(false) {

	}

	/*
	 * Sets the cached varible to a certain value.
	 */
	inline void SetCache(TData t) const {
		m_val = t;
		m_isCached = true;
	}

	/*
	 * Returns the cached value
	 */
	inline TData& GetValue() const {
		if (! m_isCached)
			LOG(FATAL) << "Non-cached variable used!";
		return m_val;
	}

	/*
	 * Returns true, if the value has already been cached
	 */
	inline bool IsCached() const {
		return m_isCached;
	}

private:
	mutable bool m_isCached;
	mutable TData m_val;
};

/*
 * Implements a implicit caching using the VarCache class
 */
#define RETURN_CACHED(CACHE_MEMBER,VALUEPATH) \
{ \
	if (! CACHE_MEMBER.IsCached()) \
	{ \
		CACHE_MEMBER.SetCache( VALUEPATH ); \
	} \
	return CACHE_MEMBER.GetValue(); \
}
