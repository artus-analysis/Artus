#ifndef TBVECTOR_H
#define TBVECTOR_H

#include <string>
#include <iostream>
#include <iterator>
#include <vector>

// This predates the initializer list from C++11
// many instances could be replaced if no backwards compatiblity is needed
template<typename T>
class ivector : public std::vector<T>
{
public:
	ivector();
	ivector(const T &item);
	ivector(const std::vector<T> &in);
	// Append items
	ivector<T> &operator,(const T &item);
	ivector<T> &operator&(const T &item);
	// Append vectors
	ivector<T> operator,(const ivector<T> &in);
	ivector<T> operator&(const ivector<T> &in);
	ivector<T> &operator&=(const ivector<T> &in);
	// Append the items itself
	ivector<T> operator+(const T &item);
	ivector<T> operator+(const ivector<T> &in);
	// Multiply items
	ivector<T> operator*(const unsigned int times);
	ivector<T> operator^(const unsigned int times);
	// Interleave vectors
	ivector<T> operator|(const ivector<T> &in);
	// Reverse vector
	ivector<T> getreverse() const;
	// Test existance
	bool has(const T &item) const;
	// Get subset of vector
	ivector<T> subset(int a, int b = -1) const;
	// Display vector with this delimeter
	static std::string delimeter;
};

template<typename T>
ivector<T> _V();
template<typename T>
ivector<T> _V(T x);
ivector<std::string> _V(const char *x);

template<typename T>
ivector<T> Collect(const ivector<ivector<T> > &in, const unsigned int idx);
template<typename T>
ivector<T> Flatten(const ivector<ivector<T> > &in);

template<typename T2, typename T1>
ivector<T1*> ConvertPtrVector(const ivector<T2*> &in);
template<typename T2, typename T1>
ivector<T1> ConvertVector(const ivector<T2> &in);

template<typename T>
std::ostream &operator<<(std::ostream &os, const ivector<T> &v);

#include "Vector.hxx"

#endif
