#ifndef NEWDATABASE
#define NEWDATABASE

// Using B+ tree as database

#include "MemoryRiver.hpp"
#include "MR_with_cache.hpp"
#include "vector.hpp"
#include <cstddef>
#include <cassert>
#include <iostream>
#include <iterator>
using sjtu::vector;

template <typename Key, typename Value, int is_cover = 0, int MAXB = 4>
class BPTdatabase {
    struct data {
        Key key; Value value;
        data() = default;
        data(Key _key, Value _value) : key(_key), value(_value) {}
        bool operator <=(const data &b) const {
            return key < b.key || (key == b.key && value <= b.value);
        }
        bool operator <(const data &b) const {
            return key < b.key || (key == b.key && value < b.value);
        }
        bool operator == (const data &b) const {
            return key == b.key && value == b.value;
        }
    };
    struct tree {
        bool is_leaf;
        size_t size;
        data key[MAXB];
        int ptr[MAXB + 1];
        // int parent;
        tree() : is_leaf(false), size(0) {
            for (int i = 0; i < MAXB; i++) key[i] = data();
            for (int i = 0; i <= MAXB; i++) ptr[i] = 0;
        }
        // 加入 key = _data, 对应的 ptr 为 nptr 的节点
        void insertdata(data _data, int nptr) {
            int pos = size;
            for (int i = 0; i < size; i++)
                if (_data < key[i]) {
                    pos = i;
                    break;
                }
            for (int i = size - 1; i >= pos; i--)
                key[i + 1] = key[i], ptr[i + 2] = ptr[i + 1];
            key[pos] = _data; ptr[pos + 1] = nptr;
            size++;
        }
        // 在叶子处加入 key = _data 的节点
        void insertdata_leaf(data _data) {
            int pos = size;
            for (int i = 0; i < size; i++)
                if (_data < key[i]) {
                    pos = i;
                    break;
                }
            for (int i = size - 1; i >= pos; i--)
                key[i + 1] = key[i];
            key[pos] = _data;
            ptr[size + 1] = ptr[size];
            ptr[size] = 0;
            size++;
        }
        // 加入 key = _data, 对应的 ptr 为 nptr 的节点插入到最后
        void appenddata(data _data, int nptr = 0) {
            key[size] = _data; ptr[size + 1] = nptr;
            size++;
        }
        // 加入 key = _data, 对应的 ptr 为 nptr 的节点插入到最前面
        void push_frontdata(data _data, int nptr = 0) {
            for (int i = size; i >= 1; i--) key[i] = key[i - 1];
            for (int i = size + 1; i >= 1; i--) ptr[i] = ptr[i - 1];
            key[0] = _data, ptr[0] = nptr;
            size++;
        }
        // 删掉第 posk 个 key 和第 posp 个 ptr
        void erase(int posk, int posp) {
            for (int i = posk; i + 1 < size; i++)
                key[i] = key[i + 1];
            for (int i = posp; i + 1 <= size; i++)
                ptr[i] = ptr[i + 1];
            ptr[size] = 0, key[size - 1] = data();
            size--;
        }
    };
    // 从根到节点的指针的 vector
    vector<int> vecptr;
    // 从根到节点的 tree
    vector<tree> vectree;
    MR_with_cache<tree, 1> file;
    int rt;
    // 获得根
    int getrt() {
        // std::cerr << "rt " << rt << '\n';
        // int rt;
        // file.get_info(rt, 1);
        return rt;
    }
    // 修改根
    void updrt(int rt) {
        this->rt = rt;
        // file.write_info(rt, 1);
    }
    // 修改 ptr 位置的 parent
    // void upd_parent(int ptr, int parent) {
    //     tree son; file.read(son, ptr);
    //     son.parent = parent;
    //     file.update(son, ptr);
    // }
    // ptr 加入 (data, nptr)
    void insertInternal(int ptr, data _data, int nptr, int dep) {
        // std::cerr << "start insert internal\n";
        tree cursor = vectree[dep - 1];
        if (cursor.size + 1 < MAXB) {
            cursor.insertdata(_data, nptr);
            file.update(cursor, ptr);
            return;
        }
        tree Ls, Rs;
        Ls.is_leaf = Rs.is_leaf = false;
        int ptrR = file.write(Rs);
        cursor.insertdata(_data, nptr);
        // new key size = MAXB
        Ls.size = MAXB / 2, Rs.size = MAXB - 1 - MAXB / 2;
        data newkey = cursor.key[Ls.size];
        for (int i = 0; i < Ls.size; i++) Ls.key[i] = cursor.key[i], Ls.ptr[i] = cursor.ptr[i];
        Ls.ptr[Ls.size] = cursor.ptr[Ls.size];
        for (int i = Ls.size + 1, j = 0; i < MAXB; i++, j++) {
            Rs.key[j] = cursor.key[i], Rs.ptr[j] = cursor.ptr[i];
        }
        Rs.ptr[Rs.size] = cursor.ptr[cursor.size];
        if (dep == 1) {
            // cursor is root
            tree newroot; newroot.is_leaf = false;
            newroot.size = 1, newroot.key[0] = newkey;
            newroot.ptr[0] = ptr, newroot.ptr[1] = ptrR;
            int rt = file.write(newroot);
            updrt(rt);
            file.update(Ls, ptr), file.update(Rs, ptrR);
            return;
        }
        file.update(Ls, ptr), file.update(Rs, ptrR);
        insertInternal(vecptr[dep - 1], newkey, ptrR, dep - 1);
    }
    void split(int ptr, tree cursor, data _data, int dep) {
        tree Ls, Rs;
        Ls.is_leaf = Rs.is_leaf = true;
        int ptrR = file.write(Rs);
        cursor.insertdata_leaf(_data);
        // std::cerr << "!! split!\n";
        // for (int i = 0; i <= cursor.size; i++) std::cerr << cursor.ptr[i] << ' ';
        // std::cerr << '\n';
        Ls.size = MAXB / 2, Rs.size = MAXB - MAXB / 2;
        for (int i = 0; i < MAXB; i++)
            if (i < Ls.size) Ls.key[i] = cursor.key[i];
            else Rs.key[i - Ls.size] = cursor.key[i];
        Ls.ptr[Ls.size] = ptrR; Rs.ptr[Rs.size] = cursor.ptr[cursor.size];
        if (dep == 1) {
            // cursor is root
            tree newroot;
            newroot.is_leaf = false;
            newroot.size = 1;
            newroot.key[0] = Rs.key[0];
            newroot.ptr[0] = ptr, newroot.ptr[1] = ptrR;
            int rt = file.write(newroot);
            updrt(rt);
            file.update(Ls, ptr);
            file.update(Rs, ptrR);
            return;
        }
        file.update(Ls, ptr), file.update(Rs, ptrR);
        insertInternal(vecptr[dep - 1], Rs.key[0], ptrR, dep - 1);
    }
    // 给 ptr 这个位置删去第 id 个指针(id >= 1) 和第 id-1 个 key
    void deleteInternal(int ptr, int id, int dep) {
        tree cursor = vectree[dep - 1];
        if (dep == 1) {
            // cursor is root
            if (cursor.size == 0) {
                file.Delete(ptr);
                updrt(-1);
                return;
            }
            cursor.erase(id - 1, id);
            file.update(cursor, ptr);
            if (cursor.size == 0) {
                // 如果根节点只有一个儿子，直接把儿子变成新的根节点
                int sonptr = cursor.ptr[0];
                file.Delete(ptr);
                // upd_parent(sonptr, -1);
                updrt(sonptr);
            }
            return;
        }
        // std::cerr << "delInternal " << ptr << ' ' << id << '\n';
        cursor.erase(id - 1, id);
        if (cursor.size >= (MAXB - 1) / 2) {
            file.update(cursor, ptr);
            return;
        }
        int parentptr = vecptr[dep - 1];
        tree parent = vectree[dep - 2];
        int sonid = -1;
        for (int i = 0; i <= parent.size; i++)
            if (parent.ptr[i] == ptr) sonid = i;
        assert(sonid != -1);
        if (sonid == 0) {
            tree Rsibling; file.read(Rsibling, parent.ptr[sonid + 1]);
            if (Rsibling.size >= (MAXB - 1) / 2 + 1) {
                data firstkey = Rsibling.key[0];
                int firstptr = Rsibling.ptr[0];
                data head = parent.key[0];
                Rsibling.erase(0, 0);
                // upd_parent(firstptr, ptr);
                cursor.appenddata(head, firstptr);
                parent.key[0] = firstkey;
                file.update(Rsibling, parent.ptr[sonid + 1]);
                file.update(cursor, ptr);
                file.update(parent, parentptr);
                return;
            }
            else {
                data head = parent.key[0];
                cursor.appenddata(head, Rsibling.ptr[0]);
                // upd_parent(Rsibling.ptr[0], ptr);
                for (int i = 0; i < Rsibling.size; i++)
                    cursor.appenddata(Rsibling.key[i], Rsibling.ptr[i + 1]);
                    // upd_parent(Rsibling.ptr[i + 1], ptr);
                file.Delete(parent.ptr[sonid + 1]);
                file.update(cursor, ptr);
                deleteInternal(parentptr, sonid + 1, dep - 1);
            }
        }
        else {
            tree Lsibling; file.read(Lsibling, parent.ptr[sonid - 1]);
            if (Lsibling.size >= (MAXB - 1) / 2 + 1) {
                data lastkey = Lsibling.key[Lsibling.size - 1];
                int lastptr = Lsibling.ptr[Lsibling.size];
                data head = parent.key[sonid - 1];
                Lsibling.erase(Lsibling.size - 1, Lsibling.size);
                // upd_parent(lastptr, ptr);
                cursor.push_frontdata(head, lastptr);
                parent.key[sonid - 1] = lastkey;
                file.update(Lsibling, parent.ptr[sonid - 1]);
                file.update(cursor, ptr);
                file.update(parent, parentptr);
                return;
            }
            else {
                data head = parent.key[sonid - 1];
                Lsibling.appenddata(head, cursor.ptr[0]);
                // upd_parent(cursor.ptr[0], parent.ptr[sonid - 1]);
                for (int i = 0; i < cursor.size; i++)
                    Lsibling.appenddata(cursor.key[i], cursor.ptr[i + 1]);
                    // upd_parent(cursor.ptr[i + 1], parent.ptr[sonid - 1]);
                file.Delete(ptr);
                file.update(Lsibling, parent.ptr[sonid - 1]);
                deleteInternal(parentptr, sonid, dep - 1);
            }
        }
    }
public:
    BPTdatabase(string filename = "") {
        file.initialise(filename, -1, is_cover);
        file.get_info(rt, 1);
    }
    ~BPTdatabase() {
        file.write_info(rt, 1);
    }
    // 判断是否为空
    bool empty() {
        // 若根是 -1 则为空，否则不为空
        return getrt() == -1;
    }
    void insert(const Key &key, const Value &value) {
        int rt = getrt();
        data _data(key, value);
        if (rt == -1) {
            tree newtree;
            newtree.is_leaf = 1;
            newtree.size = 1;
            // newtree.parent = -1;
            newtree.key[0] = _data;
            rt = file.write(newtree);
            updrt(rt);
            return;
        }
        vecptr.clear(), vectree.clear();
        vecptr.push_back(-1);
        tree cursor; file.read(cursor, rt);
        int ptr = rt;
        while(!cursor.is_leaf) {
            vecptr.push_back(ptr), vectree.push_back(cursor);
            int pos = cursor.size;
            for (int i = 0; i < cursor.size; i++) {
                if (_data < cursor.key[i]) {
                    pos = i;
                    break;
                }
            }
            ptr = cursor.ptr[pos];
            file.read(cursor, ptr);
        }
        if (cursor.size < MAXB - 1) {
            cursor.insertdata_leaf(_data);
            file.update(cursor, ptr);
            return;
        }
        split(ptr, cursor, _data, vecptr.size());
    }
    void del(const Key &key, const Value &value) {
        int rt = getrt();
        data _data(key, value);
        if (rt == -1) return;
        vecptr.clear(), vectree.clear();
        vecptr.push_back(-1);
        tree cursor; file.read(cursor, rt);
        int ptr = rt;
        while(!cursor.is_leaf) {
            vecptr.push_back(ptr), vectree.push_back(cursor);
            int pos = cursor.size;
            for (int i = 0; i < cursor.size; i++)
                if (_data < cursor.key[i]) {
                    pos = i;
                    break;
                }
            ptr = cursor.ptr[pos];
            file.read(cursor, ptr);
        }
        int pos = -1;
        for (int i = 0; i < cursor.size; i++)
            if (cursor.key[i] == _data) pos = i;
        if (pos == -1) return; // not found
        cursor.erase(pos, pos);
        if (vecptr.size() == 1) {
            if (cursor.size == 0) {
                file.Delete(rt);
                rt = -1;
                updrt(rt);
            }
            else file.update(cursor, rt);
            return;
        }
        if (cursor.size >= (MAXB - 1) / 2) {
            file.update(cursor, ptr);
            return;
        }
        tree parent; file.read(parent, vecptr.back());
        int sonid = -1;
        for (int i = 0; i <= parent.size; i++)
            if (parent.ptr[i] == ptr) sonid = i;
        assert(sonid != -1);
        // std::cerr << "del, sonid = " << sonid << '\n';
        // std::cerr << cursor.size << ' ' << cursor.ptr[0] << '\n';
        if (sonid == 0) {
            tree Rsibling; file.read(Rsibling, parent.ptr[sonid + 1]);
            if (Rsibling.size >= (MAXB - 1) / 2 + 1) {
                data first = Rsibling.key[0];
                Rsibling.erase(0, 0);
                // std::cerr << "Case!! " << cursor.size << ' ' << cursor.ptr[cursor.size] << '\n';
                int appendptr = cursor.ptr[cursor.size];
                cursor.ptr[cursor.size] = 0;
                cursor.appenddata(first, appendptr);
                parent.key[sonid] = Rsibling.key[0];
                file.update(Rsibling, parent.ptr[sonid + 1]);
                file.update(cursor, ptr);
                file.update(parent, vecptr.back());
                return;
            }
            else {
                for (int j = 0, i = cursor.size; j < Rsibling.size; j++, i++)
                    cursor.key[i] = Rsibling.key[j];
                cursor.ptr[cursor.size] = 0;
                cursor.size += Rsibling.size;
                cursor.ptr[cursor.size] = Rsibling.ptr[Rsibling.size];
                file.Delete(parent.ptr[sonid + 1]);
                file.update(cursor, ptr);
                deleteInternal(vecptr.back(), sonid + 1, vecptr.size() - 1);
            }
        }
        else {
            tree Lsibling; file.read(Lsibling, parent.ptr[sonid - 1]);
            if (Lsibling.size >= (MAXB - 1) / 2 + 1) {
                data last = Lsibling.key[Lsibling.size - 1];
                Lsibling.erase(Lsibling.size - 1, Lsibling.size - 1);
                cursor.push_frontdata(last);
                parent.key[sonid - 1] = cursor.key[0];
                file.update(Lsibling, parent.ptr[sonid - 1]);
                file.update(cursor, ptr);
                file.update(parent, vecptr.back());
                return;
            }
            else {
                for (int j = 0, i = Lsibling.size; j < cursor.size; j++, i++)
                    Lsibling.key[i] = cursor.key[j];
                Lsibling.ptr[Lsibling.size] = 0;
                Lsibling.size += cursor.size;
                Lsibling.ptr[Lsibling.size] = cursor.ptr[cursor.size];
                file.Delete(ptr);
                file.update(Lsibling, parent.ptr[sonid - 1]);
                deleteInternal(vecptr.back(), sonid, vecptr.size() - 1);
            }
        }
    }
    vector<Value> find_with_vector(const Key &key) {
        vector<Value> res;
        int rt = getrt();
        if (rt == -1) return res;
        tree cursor; file.read(cursor, rt);
        int ptr = rt;
        while(!cursor.is_leaf) {
            int pos = cursor.size;
            for (int i = 0; i < cursor.size; i++)
                if (key <= cursor.key[i].key) {
                    pos = i;
                    break;
                }
            ptr = cursor.ptr[pos];
            file.read(cursor, ptr);
        }
        for (int i = 0; i < cursor.size; i++)
            if (key == cursor.key[i].key) res.push_back(cursor.key[i].value);
        ptr = cursor.ptr[cursor.size];
        while(ptr != 0) {
            file.read(cursor, ptr);
            bool is_valid = true;
            for (int i = 0; i < cursor.size; i++)
                if (key == cursor.key[i].key) res.push_back(cursor.key[i].value);
                else {is_valid = false; break;}
            if (!is_valid) break;
            ptr = cursor.ptr[cursor.size];
        }
        return res;
    }
    // debug 的时候使用，输出整个树的结构
    void print_tree(int now, int _parent = -1) {
        if (now == -1) return;
        assert(now != 0);
        tree t; file.read(t, now);
        // assert(t.parent == _parent);
        assert(t.size < MAXB);
        std::cerr << "ptr = " << now << 
        ", is_leaf = " << t.is_leaf << ", size = " << t.size << '\n';
        std::cerr << "key: \n";
        for (int i = 0; i < t.size; i++)
            std::cerr << "(" << std::string(t.key[i].key) << ", " << t.key[i].value << ") |";
        std::cerr << "\n";
        std::cerr << "ptr: \n";
        for (int i = 0; i <= t.size; i++)
            std::cerr << t.ptr[i] << ' ';
        std::cerr << "\n";
        if (!t.is_leaf) {
            for (int i = 0; i <= t.size; i++)
                print_tree(t.ptr[i], now);
        }
    }
    // debug 的时候使用，输出整个树的结构
    void print_tree() {
        int rt = getrt();
        print_tree(rt);
    }
};

#endif