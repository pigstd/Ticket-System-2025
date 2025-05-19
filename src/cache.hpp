// 使用缓存优化
// MemeryRiver 在读取，先看缓存里面是否有存在这个 index 的
// 如果有，而且不为脏，直接从缓存里拿
// 否则加入缓存
// 在修改的时候，如果存在于缓存中，那么标记为脏

#ifndef CACHE_HPP
#define CACHE_HPP

#include <cassert>
#include <cstddef>
#include <functional>
#include <iostream>
#include <ratio>
#include "hashmap.hpp"

template<typename datatype, int MAXSIZE = 128>
class buffer {
public:
    struct data {
        datatype Data;
        int index;
        bool isdirty;
        // if isdirty = true
        // 在把这个内容退出缓存的时候，需要修改文件
        // 否则不用修改
        data(const datatype& _Data, const int &_index) : Data(_Data), index(_index), isdirty(false) {}
        void upddata(const datatype &newdata) {
            Data = newdata;
            isdirty = true;
        }
    };
    struct List {
        data value;
        List *front, *next;
        List(data _value) : value(_value), front(nullptr), next(nullptr) {}
    };
private:
    size_t Size;
    List *_head, *_end;
    hashmap<List> mp;
public:
    buffer() : Size(0), _head(nullptr), _end(nullptr) {}
    ~buffer() {
        while(_head != nullptr) {
            List *nxt = _head->next;
            delete _head;
            _head = nxt;
        }
    }
    // 返回大小
    size_t size() const {return Size;}
    // 返回 index 对应的 List 的指针，若没有返回 nullptr
    List *find(int index) {
        return mp.find(index);
    }
    // 将 ptr 指针设置为 head
    void set_head(List *ptr) {
        assert(_head != nullptr);
        if (ptr == _head) return;
        ptr->front->next = ptr->next;
        if (ptr->next != nullptr)
            ptr->next->front = ptr->front;
        else _end = ptr->front;
        ptr->front = nullptr;
        ptr->next = _head;
        _head->front = ptr;
        _head = ptr;
    }
    List *end() {
        return _end;
    }
    void erase_List(List *p) {
        Size--;
        mp.erase(p->value.index);
        if (p == _head) _head = p->next;
        if (p == _end) _end = p->front;
        if (p->next != nullptr) p->next->front = p->front;
        if (p->front != nullptr) p->front->next = p->next;
        delete p;
    }
    void popend() {
        // Size != 1
        assert(_end != nullptr);
        erase_List(_end);
    }
    void insert(const int &index, const datatype &_data) {
        Size++;
        List *ptr = new List(data(_data, index));
        mp.insert(index, ptr);
        if (_head == nullptr) _head = _end = ptr;
        else {
            _head->front = ptr;
            ptr->next = _head;
            _head = ptr;
        }
    }
};

#endif // CACHE_HPP