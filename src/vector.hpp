#ifndef SJTU_VECTOR_HPP
#define SJTU_VECTOR_HPP

// #include "exceptions.hpp"

#include <climits>
#include <cstddef>
#include <cstring>
#include <iostream>

namespace sjtu
{
/**
 * a data container like std::vector
 * store data in a successive memory and support random access.
 */
template<typename T>
class vector
{
private:
	void *MemoryData;
	int Size, Capability;
	// 扩大 Capability
	void extent() {
		int newCapability = Capability * 2;
		if (Capability == 0) newCapability = 1;
		void *newMemoryData = operator new (newCapability * sizeof(T));
		for (int i = 0; i < Size; i++)
			new(static_cast<T*>(newMemoryData) + i) T(*(static_cast<T*>(MemoryData) + i)),
			(static_cast<T*>(MemoryData) + i)->~T();
		if (MemoryData != nullptr) operator delete(MemoryData);
		MemoryData = newMemoryData;
		Capability = newCapability;
	}
public:
	/**
	 * TODO
	 * a type for actions of the elements of a vector, and you should write
	 *   a class named const_iterator with same interfaces.
	 */
	/**
	 * you can see RandomAccessIterator at CppReference for help.
	 */
	/**
	 * TODO
	 * has same function as iterator, just for a const object.
	 */
	 class const_iterator
	 {
	 public:
		 using difference_type = std::ptrdiff_t;
		 using value_type = T;
		 using pointer = T*;
		 using reference = T&;
		 using iterator_category = std::output_iterator_tag;
 
	 private:
		 void *MemoryData;
		 int index;
	 public:
		 const_iterator(void *_MemoryData = nullptr, int index_ = 0) : MemoryData(_MemoryData), index(index_) {}
		 const_iterator operator+(const int &n) const {
			 return {MemoryData, index + n};
		 }
		 const_iterator operator-(const int &n) const {
			 return {MemoryData, index - n};
		 }
		 // return the distance between two iterators,
		 // if these two iterators point to different vectors, throw invaild_iterator.
		 int operator-(const const_iterator &rhs) const {
			//  if (rhs.MemoryData != MemoryData) throw invalid_iterator();
			 return index - rhs.index;
		 }
		 const_iterator& operator+=(const int &n) {
			 index += n;
			 return *this;
		 }
		 const_iterator& operator-=(const int &n) {
			 index -= n;
			 return *this;
		 }
		 const_iterator operator++(int) {
			 const_iterator tmp = *this;
			 ++index;
			 return tmp;
		 }
		 const_iterator& operator++() {
			 ++index;
			 return *this;
		 }
		 const_iterator operator--(int) {
			 iterator tmp = *this;
			 --index;
			 return tmp;
		 }
		 const_iterator& operator--() {
			 --index;
			 return *this;
		 }
		 const T& operator*() const {
			 return *(static_cast<T*>(MemoryData) + index);
		 }
		 /**
		  * a operator to check whether two iterators are same (pointing to the same memory address).
		  */
		 bool operator==(const const_iterator &rhs) const {
			 return MemoryData == rhs.MemoryData && index == rhs.index;
		 }
		 /**
		  * some other operator for iterator.
		  */
		 bool operator!=(const const_iterator &rhs) const {
			 return MemoryData != rhs.MemoryData || index != rhs.index;
		 }
	 };
	class iterator
	{
	// The following code is written for the C++ type_traits library.
	// Type traits is a C++ feature for describing certain properties of a type.
	// For instance, for an iterator, iterator::value_type is the type that the
	// iterator points to.
	// STL algorithms and containers may use these type_traits (e.g. the following
	// typedef) to work properly. In particular, without the following code,
	// @code{std::sort(iter, iter1);} would not compile.
	// See these websites for more information:
	// https://en.cppreference.com/w/cpp/header/type_traits
	// About value_type: https://blog.csdn.net/u014299153/article/details/72419713
	// About iterator_category: https://en.cppreference.com/w/cpp/iterator
	public:
		using difference_type = std::ptrdiff_t;
		using value_type = T;
		using pointer = T*;
		using reference = T&;
		using iterator_category = std::output_iterator_tag;

	private:
		void *MemoryData;
		int index;
	public:
		iterator(void *_MemoryData = nullptr, int index_ = 0) : MemoryData(_MemoryData), index(index_) {}
		/**
		 * return a new iterator which pointer n-next elements
		 * as well as operator-
		 */
		iterator operator+(const int &n) const {
			return {MemoryData, index + n};
		}
		iterator operator-(const int &n) const {
			return {MemoryData, index - n};
		}
		// return the distance between two iterators,
		// if these two iterators point to different vectors, throw invaild_iterator.
		int operator-(const iterator &rhs) const {
			// if (rhs.MemoryData != MemoryData) throw invalid_iterator();
			return index - rhs.index;
		}
		iterator& operator+=(const int &n) {
			index += n;
			return *this;
		}
		iterator& operator-=(const int &n) {
			index -= n;
			return *this;
		}
		iterator operator++(int) {
			iterator tmp = *this;
			++index;
			return tmp;
		}
		iterator& operator++() {
			++index;
			return *this;
		}
		iterator operator--(int) {
			iterator tmp = *this;
			--index;
			return tmp;
		}
		/**
		 * TODO --iter
		 */
		iterator& operator--() {
			--index;
			return *this;
		}
		/**
		 * TODO *it
		 */
		T& operator*() const{
			return *(static_cast<T*>(MemoryData) + index);
		}
		/**
		 * a operator to check whether two iterators are same (pointing to the same memory address).
		 */
		bool operator==(const iterator &rhs) const {
			return MemoryData == rhs.MemoryData && index == rhs.index;
		}
		// bool operator==(const const_iterator &rhs) const;
		/**
		 * some other operator for iterator.
		 */
		bool operator!=(const iterator &rhs) const {
			return MemoryData != rhs.MemoryData || index != rhs.index;
		}
		// bool operator!=(const const_iterator &rhs) const;

		// 只需要支持隐式转换 const_iterator，就不需要上面那两个东西了
		// 隐式转换为 const_iterator
		operator const_iterator() {
			return const_iterator(MemoryData, index);
		}
	};
	/**
	 * TODO Constructs
	 * At least two: default constructor, copy constructor
	 */
	vector() : Size(0), Capability(0), MemoryData(nullptr) {}
	vector(const vector &other) : Size(other.Size), Capability(other.Size), MemoryData(nullptr) {
		MemoryData = operator new (Capability * sizeof(T));
		for (int i = 0; i < Size; i++)
			new (static_cast<T*>(MemoryData) + i) T(other[i]);
	}
	/**
	 * TODO Destructor
	 */
	~vector() {
		for (int i = 0; i < Size; i++)
			this->operator[](i).~T();
		operator delete(MemoryData);
	}
	/**
	 * TODO Assignment operator
	 */
	vector &operator=(const vector &other) {
		if (this == &other) return *this;
		
		for (int i = 0; i < Size; i++)
			this->operator[](i).~T();
		operator delete(MemoryData);

		Size = other.Size, Capability = other.Size;
		MemoryData = operator new(sizeof(T) * Capability);
		for (int i = 0; i < Size; i++)
			new (static_cast<T*>(MemoryData) + i) T(other[i]);
		return *this;
	}
	/**
	 * assigns specified element with bounds checking
	 * throw index_out_of_bound if pos is not in [0, size)
	 */
	T & at(const size_t &pos) {
		// if (pos < 0 || pos >= Size) throw index_out_of_bound();
		return *(static_cast<T*>(MemoryData) + pos);
	}
	const T & at(const size_t &pos) const {
		// if (pos < 0 || pos >= Size) throw index_out_of_bound();
		return *(static_cast<T*>(MemoryData) + pos);
	}
	/**
	 * assigns specified element with bounds checking
	 * throw index_out_of_bound if pos is not in [0, size)
	 * !!! Pay attentions
	 *   In STL this operator does not check the boundary but I want you to do.
	 */
	T & operator[](const size_t &pos) {
		return at(pos);
	}
	const T & operator[](const size_t &pos) const {
		return at(pos);
	}
	/**
	 * access the first element.
	 * throw container_is_empty if size == 0
	 */
	const T & front() const {
		// if (Size == 0) throw container_is_empty();
		return at(0);
	}
	/**
	 * access the last element.
	 * throw container_is_empty if size == 0
	 */
	const T & back() const {
		// if (Size == 0) throw container_is_empty();
		return at(Size - 1);
	}
	/**
	 * returns an iterator to the beginning.
	 */
	iterator begin() {return iterator(MemoryData, 0);}
	const_iterator begin() const {return const_iterator(MemoryData, 0);}
	const_iterator cbegin() const {return const_iterator(MemoryData, 0);}
	/**
	 * returns an iterator to the end.
	 */
	iterator end() {return iterator(MemoryData, Size);}
	const_iterator end() const {return const_iterator(MemoryData, Size);}
	const_iterator cend() const {return const_iterator(MemoryData, Size);}
	/**
	 * checks whether the container is empty
	 */
	bool empty() const {return Size == 0;}
	/**
	 * returns the number of elements
	 */
	size_t size() const {return Size;}
	/**
	 * clears the contents
	 */
	void clear() {
		for (int i = 0; i < Size; i++)
			this->operator[](i).~T();
		operator delete(MemoryData);

		MemoryData = nullptr;
		Size = Capability = 0;
	}
	/**
	 * inserts value at index ind.
	 * after inserting, this->at(ind) == value
	 * returns an iterator pointing to the inserted value.
	 * throw index_out_of_bound if ind > size (in this situation ind can be size because after inserting the size will increase 1.)
	 */
	iterator insert(const size_t &ind, const T &value) {
		// std::cerr << "ins: " << ind << ' ' << Size << '\n';
		// if (ind > Size) throw index_out_of_bound();
		if (Size == Capability) extent();
		new(static_cast<T*>(MemoryData) + Size) T(at(Size - 1));
		for (int i = Size - 1; i > ind; i--)
			at(i) = at(i - 1);
		Size++;
		at(ind) = value;
		return iterator(MemoryData, ind);
	}
	/**
	 * inserts value before pos
	 * returns an iterator pointing to the inserted value.
	 */
	iterator insert(iterator pos, const T &value) {
		int index = pos - begin();
		return insert(index, value);
	}
	
	/**
	 * removes the element with index ind.
	 * return an iterator pointing to the following element.
	 * throw index_out_of_bound if ind >= size
	 */
	iterator erase(const size_t &ind) {
		// if (ind >= Size) throw index_out_of_bound();
		at(ind).~T();
		for (int i = ind; i + 1 < Size; i++)
			at(i) = at(i + 1);
		Size--;
		return begin() + ind;
	}
	/**
	 * removes the element at pos.
	 * return an iterator pointing to the following element.
	 * If the iterator pos refers the last element, the end() iterator is returned.
	 */
	iterator erase(iterator pos) {
		return erase(pos - begin());
	}
	/**
	 * adds an element to the end.
	 */
	void push_back(const T &value) {
		if (Size == Capability) extent();
		new (static_cast<T*>(MemoryData) + Size) T(value);
		Size++;
	}
	/**
	 * remove the last element from the end.
	 * throw container_is_empty if size() == 0
	 */
	void pop_back() {
		// if (Size == 0) throw container_is_empty();
		at(Size - 1).~T();
		Size--;
	}
};


}

#endif
