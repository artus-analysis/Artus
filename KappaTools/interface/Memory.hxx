template<typename T>
MemoryPool<T>::MemoryPool(size_t reserve)
{
	list.reserve(reserve);
}

template<typename T>
MemoryPool<T>::~MemoryPool()
{
	Cleanup();
}

template<typename T>
const T &MemoryPool<T>::AddObject(const T item)
{
	return AddObject(new T(item));
}

template<typename T>
const T &MemoryPool<T>::AddObject(T *ptr)
{
	list.push_back(ptr);
	return *ptr;
}

template<typename T>
void MemoryPool<T>::Cleanup()
{
	for (typename std::vector<T*>::iterator it = list.begin(); it < list.end(); ++it)
		mydelete<T>(*it);
	list.clear();
}

template <typename T>
inline void mydelete(T *&ptr)
{
	if (ptr)
	{
		delete ptr;
		ptr = 0;
	}
}

template <typename T>
inline void mydelete_array(T *&ptr)
{
	if (ptr)
	{
		delete [] ptr;
		ptr = 0;
	}
}

template <typename T>
inline void mydelete_vector(std::vector<T*> &vec)
{
	for (unsigned int i = 0; i < vec.size(); ++i)
		mydelete<T>(vec[i]);
	vec.clear();
}
