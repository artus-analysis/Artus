/* Copyright (c) 2013 - All Rights Reserved
 *   Thomas Hauth  <Thomas.Hauth@cern.ch>
 *   Joram Berger  <Joram.Berger@cern.ch>
 *   Dominik Haitz <Dominik.Haitz@kit.edu>
 */

#pragma once
/// Implements a property with Get, Set and Has Methods
#define IMPL_PROPERTY(TYPE, SNAME)                          \
private:                                                    \
TYPE m_##SNAME;                                             \
public:                                                     \
TYPE Get##SNAME() const { return m_##SNAME; }               \
bool Has##SNAME() const { return &m_##SNAME; }              \
TYPE const& GetRef##SNAME() const { return m_##SNAME; }     \
TYPE const* GetPt##SNAME() const { return &m_##SNAME; }     \
void Set##SNAME(TYPE val) { m_##SNAME = val; }

#define IMPL_PROPERTY_READONLY(TYPE, SNAME)                 \
private:                                                    \
TYPE m_##SNAME;                                             \
public:                                                     \
TYPE Get##SNAME() const { return m_##SNAME; }               \
private:                                                    \
void Set##SNAME(TYPE val) { m_##SNAME = val; }
