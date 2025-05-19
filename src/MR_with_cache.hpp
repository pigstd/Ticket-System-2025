#ifndef MR_WITH_CACHE
#define MR_WITH_CACHE

// 将 MemoryRiver 和 cache 相结合
#include "MemoryRiver.hpp"
#include "cache.hpp"
#include <iostream>
#include <string>

using std::string;

template<class T, int info_len = 2, int MAXSIZE = 128>
class MR_with_cache {
    MemoryRiver<T, info_len> MR;
    buffer<T, MAXSIZE> Buffer;
    string filename;
    // 删去 buffer 中的最后一个数据
    // 若 isdirty，则需要修改 MR 中存储的数据
    void buffer_popend() {
        auto p = Buffer.end();
        if (p->value.isdirty) {
            // std::cerr << "upd " << p->value.index << '\n';
            MR.update(p->value.Data, p->value.index);
        }
        Buffer.popend();
    }
public:
    MR_with_cache(const string& file_name = "") : filename(file_name), Buffer() {}
    ~MR_with_cache() {
        // std::cerr << "buffer size = " << Buffer.size() << '\n';
        while (Buffer.size()) buffer_popend();
        // std::cerr << "buffer size = " << Buffer.size() << '\n';
    }
    void initialise(string FN = "", int num = 0, int is_cover = 1) {
        MR.initialise(FN, num, is_cover);
    }
    void get_info(int &tmp, int n) {MR.get_info(tmp, n);}
    void write_info(int tmp, int n) {MR.write_info(tmp, n);}
    int write(T &t) {
        int index = MR.write(t);
        if (Buffer.size() == MAXSIZE) buffer_popend();
        Buffer.insert(index, t);
        return index;
    }
    void update(T &t, const int index) {
        auto p = Buffer.find(index);
        if (p != nullptr) {
            p->value.upddata(t);
            Buffer.set_head(p);
        }
        else {
            MR.update(t, index);
            if (Buffer.size() == MAXSIZE) buffer_popend();
            Buffer.insert(index, t);
        }
    }
    void read(T &t, const int index) {
        auto p = Buffer.find(index);
        if (p != nullptr) {
            t = p->value.Data;
            Buffer.set_head(p);
        }
        else {
            MR.read(t, index);
            if (Buffer.size() == MAXSIZE) buffer_popend();
            Buffer.insert(index, t);
        }
    }
    void Delete(int index) {
        auto p = Buffer.find(index);
        if (p == nullptr) {
            MR.Delete(index);
        }
        else {
            Buffer.erase_List(p);
            MR.Delete(index);
        }
    }
};

#endif