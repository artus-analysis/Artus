#ifndef TBMEMORY_H
#define TBMEMORY_H

#include <vector>

template<typename T>
class MemoryPool
{
public:
	MemoryPool(size_t reserve = 1);
	~MemoryPool();
	const T &AddObject(const T item);
	const T &AddObject(T *ptr);
	void Cleanup();
private:
	std::vector<T*> list;
};

template <typename T>
inline void mydelete(T* &ptr);

template <typename T>
inline void mydelete_array(T* &ptr);

template <typename T>
inline void mydelete_vector(std::vector<T*> &vec);

#include "Memory.hxx"

#endif
