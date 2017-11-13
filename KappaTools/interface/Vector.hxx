
#include "Artus/KappaTools/interface/StringTools.h"

template<typename T>
ivector<T>::ivector()
{
}

template<typename T>
ivector<T>::ivector(const T &item)
{
	this->push_back(item);
}

template<typename T>
ivector<T>::ivector(const std::vector<T> &in)
{
	copy(in.begin(), in.end(), back_inserter(*this));
}

template<typename T>
ivector<T> &ivector<T>::operator&(const T &item)
{
	this->push_back(item);
	return *this;
}

template<typename T>
ivector<T> &ivector<T>::operator,(const T &item)
{
	return ivector<T>::operator&(item);
}

template<typename T>
ivector<T> ivector<T>::operator&(const ivector<T> &in)
{
	ivector<T> out = *this;
	copy(in.begin(), in.end(), back_inserter(out));
	return out;
}

template<typename T>
ivector<T> ivector<T>::operator,(const ivector<T> &in)
{
	return ivector<T>::operator&(in);
}

template<typename T>
ivector<T> &ivector<T>::operator&=(const ivector<T> &in)
{
	copy(in.begin(), in.end(), back_inserter(*this));
	return *this;
}

template<typename T>
ivector<T> ivector<T>::operator*(const unsigned int times)
{
	ivector<T> out;
	for (typename ivector<T>::const_iterator it = this->begin(); it != this->end(); ++it)
		for (unsigned int i = 0; i < times; ++i)
			out.push_back(*it);
	return out;
}

template<typename T>
ivector<T> ivector<T>::operator^(const unsigned int times)
{
	ivector<T> out;
	for (unsigned int i = 0; i < times; ++i)
		out = (*this & out);
	return out;
}

template<typename T>
ivector<T> ivector<T>::operator+(const T &item)
{
	ivector<T> out;
	for (typename ivector<T>::iterator it = this->begin(); it != this->end(); ++it)
		out.push_back(*it + item);
	return out;
}

template<typename T>
ivector<T> ivector<T>::operator+(const ivector<T> &in)
{
	ivector<T> out;
	for (size_t i = 0; i < this->size(); ++i)
		out.push_back(this->at(i) + in[i % in.size()]);
	return out;
}

template<typename T>
ivector<T> ivector<T>::operator|(const ivector<T> &in)
{
	ivector<T> out;
	typename ivector<T>::const_iterator ileft = this->begin();
	typename ivector<T>::const_iterator ileft_end = this->end();
	typename ivector<T>::const_iterator iright = in.begin();
	while ((ileft < this->end()) && (iright < in.end()))
	{
		out.push_back(*ileft); ++ileft;
		out.push_back(*iright); ++iright;
	}
	copy(ileft, ileft_end, back_inserter(out));
	copy(iright, in.end(), back_inserter(out));
	return out;
}

template<typename T>
ivector<T> ivector<T>::getreverse() const
{
	ivector<T> result;
	for (typename ivector<T>::const_reverse_iterator it = this->rbegin(); it < this->rend(); ++it)
		result.push_back(*it);
	return result;
}

template<typename T>
bool ivector<T>::has(const T &item) const
{
	for (typename ivector<T>::const_iterator it = this->begin(); it < this->end(); ++it)
		if (*it == item)
			return true;
	return false;
}

template<typename T>
ivector<T> ivector<T>::subset(int a, int b) const
{
	ivector<T> result;
	if (this->empty())
		return result;
	if ((a < 0) || ((unsigned int)(a + 1) > this->size()))
		a = 0;
	if ((b < 0) || ((unsigned int)(b + 1) > this->size()))
		b = this->size() - 1;
	for (int i = a; i <= b; ++i)
		result.push_back(this->at(i));
	return result;
}

template<typename T>
std::string ivector<T>::delimeter = " ";

template<typename T>
ivector<T> _V()
{
	return (ivector<T>());
}

template<typename T>
ivector<T> _V(T x)
{
	return (ivector<T>(), x);
}

template<typename T>
ivector<T> Collect(const ivector<ivector<T> > &in, const unsigned int idx)
{
	ivector<T> result;
	for (unsigned int i = 0; i < in.size(); ++i)
		if (idx < in[i].size())
			result.push_back(in[i][idx]);
	return result;
}

template<typename T>
ivector<T> Flatten(const ivector<ivector<T> > &in)
{
	ivector<T> result;
	for (unsigned int i = 0; i < in.size(); ++i)
		for (unsigned int j = 0; j < in[i].size(); ++j)
			result.push_back(in[i][j]);
	return result;
}

template<typename T2, typename T1>
ivector<T1*> ConvertPtrVector(const ivector<T2*> &in)
{
	ivector<T1*> result;
	for (unsigned int i = 0; i < in.size(); ++i)
		result.push_back((T1*)in[i]);
	return result;
}

template<typename T2, typename T1>
ivector<T1> ConvertVector(const ivector<T2> &in)
{
	ivector<T1> result;
	for (unsigned int i = 0; i < in.size(); ++i)
		result.push_back(T1(in[i]));
	return result;
}

template<typename T>
std::ostream &operator<<(std::ostream &os, const ivector<T> &v)
{
	return os << KappaTools::join(ivector<T>::delimeter, v);
}
