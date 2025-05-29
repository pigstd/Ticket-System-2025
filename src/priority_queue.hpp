#ifndef SJTU_PRIORITY_QUEUE_HPP
#define SJTU_PRIORITY_QUEUE_HPP

#include <array>
#include <cstddef>
#include <functional>
#include <tuple>
#include <type_traits>
#include <iostream>
#include <assert.h>

namespace sjtu {
/**
 * @brief a container like std::priority_queue which is a heap internal.
 * **Exception Safety**: The `Compare` operation might throw exceptions for certain data.
 * In such cases, any ongoing operation should be terminated, and the priority queue should be restored to its original state before the operation began.
 */
// 实现：斜堆 skew heap
template<typename T, class Compare = std::less<T>>
class priority_queue {
private:
	struct node {
		T val;
		node *Leftson, *Rightson;
		node(T _val) : val(_val), Leftson(nullptr), Rightson(nullptr) {}
		~node() {
			if (Leftson != nullptr) delete Leftson;
			if (Rightson != nullptr) delete Rightson;
		}
	};
	size_t Size;
	node *Topnode;
	// copy: 拷贝复制一个和 src 一样的树，并且返回指针
	node *copy(node *src) {
		if (src == nullptr) return nullptr;
		node *res = new node(src->val);
		res->Leftson = copy(src->Leftson);
		res->Rightson = copy(src->Rightson);
		return res;
	}
	node *merge(node *L, node *R) {
		if (L == nullptr) return R;
		if (R == nullptr) return L;
		bool isflip = false;
		if (Compare()(L->val, R->val))
			std::swap(L, R), isflip = true;
		try {
			L->Rightson = merge(L->Rightson, R);
		}
		catch(...) {
			if (isflip)
				std::swap(L, R);
			throw;
		}
		std::swap(L->Leftson, L->Rightson);
		return L;
	}
public:
	/**
	 * @brief default constructor
	 */
	priority_queue() : Size(0), Topnode(nullptr) {}

	/**
	 * @brief copy constructor
	 * @param other the priority_queue to be copied
	 */
	priority_queue(const priority_queue &other) : Size(other.Size) {
		Topnode = copy(other.Topnode);
	}

	/**
	 * @brief deconstructor
	 */
	~priority_queue() {
		delete Topnode;
	}

	/**
	 * @brief Assignment operator
	 * @param other the priority_queue to be assigned from
	 * @return a reference to this priority_queue after assignment
	 */
	priority_queue &operator=(const priority_queue &other) {
		if (this == &other) return *this;
		Size = other.Size;
		delete Topnode;
		Topnode = copy(other.Topnode);
		return *this;
	}

	/**
	 * @brief get the top element of the priority queue.
	 * @return a reference of the top element.
	 * @throws container_is_empty if empty() returns true
	 */
	const T & top() const {
		return Topnode->val;
	}

	/**
	 * @brief push new element to the priority queue.
	 * @param e the element to be pushed
	 */
	void push(const T &e) {
		node *newnode = new node(e);
		try {
			Topnode = merge(Topnode, newnode);
		}
		catch(...) {
			delete newnode;
			throw;
		}
		Size++;
	}

	/**
	 * @brief delete the top element from the priority queue.
	 * @throws container_is_empty if empty() returns true
	 */
	void pop() {
		node *Ls = Topnode->Leftson, *Rs = Topnode->Rightson;
		node *tmp = Topnode;
		Topnode = merge(Ls, Rs);
		tmp->Leftson = tmp->Rightson = nullptr;
		delete tmp;
		Size--;
	}

	/**
	 * @brief return the number of elements in the priority queue.
	 * @return the number of elements.
	 */
	size_t size() const {return Size;}

	/**
	 * @brief check if the container is empty.
	 * @return true if it is empty, false otherwise.
	 */
	bool empty() const {return Size == 0;}

	/**
	 * @brief merge another priority_queue into this one.
	 * The other priority_queue will be cleared after merging.
	 * The complexity is at most O(logn).
	 * @param other the priority_queue to be merged.
	 */
	void merge(priority_queue &other) {
		Topnode = merge(Topnode, other.Topnode);
		Size += other.Size;
		other.Size = 0;
		other.Topnode = nullptr;
	}
};

}

#endif