#ifndef BPT_MEMORYRIVER_HPP
#define BPT_MEMORYRIVER_HPP

// #include <cassert>
#include <fstream>
#include <filesystem>
#include <iostream>
// #include <map>

using std::string;
using std::fstream;
using std::ifstream;
using std::ofstream;

// using std::map;

//sizeof(T) >= sizeof(int)!!
template<class T, int info_len = 2>
class MemoryRiver {
private:
    // map<int, int> mp;
    /* your code here */
    fstream file;
    string file_name;
    int a[info_len + 1];
    // int sizeofT = sizeof(T);
public:
    MemoryRiver() = default;
    MemoryRiver(const string& file_name) : file_name(file_name) {}
    // num 初始填入的数字 is_cover 是否强制覆盖
    void initialise(string FN = "", int num = 0, int is_cover = 1) {
        if (FN != "") file_name = FN;
        // 如果存在文件，就不修改，视情况选择是否强制覆盖
        // is_cover = 1 强制覆盖
        if (is_cover == 0 && std::filesystem::exists(file_name)) {
            file.open(file_name, std::ios::in);
            for (int i = 0; i <= info_len; i++)
                file.seekg(i * sizeof(int), std::ios::beg),
                file.read(reinterpret_cast<char *>(&a[i]), sizeof(int));
            file.close();
            return;
        }
        // std::cerr << "cover: " << file_name << '\n';
        file.open(file_name, std::ios::out);
        int _head = 0;
        file.write(reinterpret_cast<char *>(&_head), sizeof(int));
        a[0] = 0;
        for (int i = 0; i < info_len; ++i)
            file.write(reinterpret_cast<char *>(&num), sizeof(int)), a[i + 1] = num;
        file.close();
    }

    //读出第n个int的值赋给tmp，1_base
    void get_info(int &tmp, int n) {
        if (n > info_len) return;
        tmp = a[n];
        // file.open(file_name, std::ios::in);
        // file.seekg(n * sizeof(int), std::ios::beg);
        // file.read(reinterpret_cast<char *>(&tmp), sizeof(int));
        // file.close();
    }

    //将tmp写入第n个int的位置，1_base
    void write_info(int tmp, int n) {
        if (n > info_len) return;
        a[n] = tmp;
        // file.open(file_name, std::ios::out | std::ios::in);
        // file.seekp(n * sizeof(int), std::ios::beg);
        // file.write(reinterpret_cast<char *>(&tmp), sizeof(int));
        // file.close();
    }

    //在文件合适位置写入类对象t，并返回写入的位置索引index
    //位置索引意味着当输入正确的位置索引index，在以下三个函数中都能顺利的找到目标对象进行操作
    //位置索引index可以取为对象写入的起始位置
    int write(T &t) {
        file.open(file_name, std::ios::out | std::ios::in);
        int head; get_info(head, 0);
        if (head == 0) file.seekp(0, std::ios::end);
        else {
            file.seekg(head, std::ios::beg);
            int nxtpos; file.read(reinterpret_cast<char *>(&nxtpos), sizeof(int));
            write_info(nxtpos, 0);
            file.seekp(head, std::ios::beg);
        }
        int index = file.tellp();
        file.write(reinterpret_cast<char *>(&t), sizeof(T));
        file.close();
        // mp[index] = 1;
        return index;
    }
    //用t的值更新位置索引index对应的对象，保证调用的index都是由write函数产生
    void update(T &t, const int index) {
        file.open(file_name, std::ios::out | std::ios::in);
        file.seekp(index, std::ios::beg);
        file.write(reinterpret_cast<char *>(&t), sizeof(T));
        file.close();
    }

    //读出位置索引index对应的T对象的值并赋值给t，保证调用的index都是由write函数产生
    void read(T &t, const int index) {
        // if (mp.find(index) == mp.end()) {
        //     std::cerr << "error index: " << index << '\n';
        //     assert(0);
        // }
        file.open(file_name, std::ios::in);
        file.seekg(index,std::ios::beg);
        file.read(reinterpret_cast<char *>(&t), sizeof(T));
        file.close();
    }

    //删除位置索引index对应的对象(不涉及空间回收时，可忽略此函数)，保证调用的index都是由write函数产生
    void Delete(int index) {
        // if (mp.find(index) == mp.end()) assert(0);
        // mp.erase(mp.find(index));
        file.open(file_name, std::ios::out | std::ios::in);
        int head; get_info(head, 0);
        write_info(index, 0);
        file.seekp(index, std::ios::beg);
        file.write(reinterpret_cast<char *>(&head), sizeof(int));
        file.close();
    }
    ~MemoryRiver() {
        file.open(file_name, std::ios::out | std::ios::in);
        for (int i = 0; i <= info_len; i++)
            file.seekp(i * sizeof(int), std::ios::beg),
            file.write(reinterpret_cast<char *>(&a[i]), sizeof(int));
        file.close();
    }
};


#endif //BPT_MEMORYRIVER_HPP