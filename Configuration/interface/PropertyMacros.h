
#pragma once
/// Implements a property with Get, Set and Has Methods
#define IMPL_PROPERTY(TYPE, SNAME) \
private: \
	TYPE m_##SNAME; \
public: \
	TYPE Get##SNAME() const { \
		return m_##SNAME; \
	} \
	bool Has##SNAME() const { \
		return &m_##SNAME; \
	} \
	TYPE const& GetRef##SNAME() const { \
		return m_##SNAME; \
	} \
	TYPE const* GetPt##SNAME() const { \
		return &m_##SNAME; \
	} \
	void Set##SNAME(TYPE val) { \
		m_##SNAME = val; \
	}

#define IMPL_PROPERTY_INITIALIZE(TYPE, SNAME, VAL) \
private: \
	TYPE m_##SNAME = VAL; \
public: \
	TYPE Get##SNAME() const { return m_##SNAME; \
	} \
	bool Has##SNAME() const { \
		return &m_##SNAME; \
	} \
	TYPE const& GetRef##SNAME() const { \
		return m_##SNAME; \
	} \
	TYPE const* GetPt##SNAME() const { \
		return &m_##SNAME; \
	} \
	void Set##SNAME(TYPE val) { \
		m_##SNAME = val; \
	}

#define IMPL_PROPERTY_READONLY(TYPE, SNAME) \
private: \
	TYPE m_##SNAME; \
	void Set##SNAME(TYPE val) { \
		m_##SNAME = val; \
	} \
public: \
	TYPE Get##SNAME() const { \
		return m_##SNAME; \
	}
