#pragma once

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
	inline TData GetValue() const {
		if (!m_isCached)
		CALIB_LOG_FATAL("not Cached variable used")

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
{ if (! CACHE_MEMBER.IsCached() ) \
  {             CACHE_MEMBER.SetCache( VALUEPATH );} \
return CACHE_MEMBER.GetValue(); }
