#ifndef HASHMAP_HPP
#define HASHMAP_HPP

// hash map
// int -> (valuename*) 的映射
// 直接使用 vector，将 int 对 mod 取模

#include "vector.hpp"
#include <cassert>

using sjtu::vector;

template <typename valuename, int mod = 131>
class hashmap {
private:
    struct data {
        int index;
        valuename* value;
        data(const int &_index, valuename *_value) : index(_index), value(_value) {}
    };
    vector<data> a[mod];
public:
    hashmap() {
        for (int i = 0; i < mod; i++) a[i] = vector<data>();
    }
    valuename *find(int index) {
        int _index = index % mod;
        for (auto p = a[_index].begin(); p != a[_index].end(); p++) {
            if ((*p).index == index) return (*p).value;
        }
        return nullptr;
    }
    void erase(int index) {
        int _index = index % mod;
        for (auto p = a[_index].begin(); p != a[_index].end(); p++) {
            if ((*p).index == index) {
                a[_index].erase(p);
                return;
            }
        }
        assert(0);
    }
    void insert(int index, valuename *ptr) {
        int _index = index % mod;
        a[_index].push_back(data(index, ptr));
    }
};

#endif //HASHMP_HPP