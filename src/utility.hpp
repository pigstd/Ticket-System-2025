#ifndef UTILITY_HPP
#define UTILITY_HPP

#include <cstring>
#include <iostream>
#include <string>
#include "vector.hpp"

using std::string;
using sjtu::vector;

namespace sjtu {

template<class T1, class T2>
class pair {
public:
	T1 first;
	T2 second;
	constexpr pair() : first(), second() {}
	pair(const pair &other) = default;
	pair(pair &&other) = default;
	pair(const T1 &x, const T2 &y) : first(x), second(y) {}
	template<class U1, class U2>
	pair(U1 &&x, U2 &&y) : first(x), second(y) {}
	template<class U1, class U2>
	pair(const pair<U1, U2> &other) : first(other.first), second(other.second) {}
	template<class U1, class U2>
	pair(pair<U1, U2> &&other) : first(other.first), second(other.second) {}
    bool operator < (const pair<T1, T2> &b) const {
        return first < b.first || (first == b.first && second < b.second);
    }
    bool operator <= (const pair<T1, T2> &b) const {
        return first < b.first || (first == b.first && second <= b.second);
    }
    bool operator == (const pair<T1, T2> &b) const {
        return first == b.first && second == b.second;
    }
    bool operator != (const pair<T1, T2> &b) const {
        return first != b.first || second != b.second;
    }
    pair<T1, T2>& operator = (const pair<T1, T2> &b) {
        first = b.first, second = b.second;
        return *this;
    }
};

}

template<int MAX_LEN>
class mystr {
template<int MAX__Len>
    friend bool operator < (const mystr<MAX__Len> &a, const mystr<MAX__Len> &b);
template<int MAX__Len>
    friend bool operator == (const mystr<MAX__Len> &a, const mystr<MAX__Len> &b);
template<int MAX__Len>
    friend bool operator <= (const mystr<MAX__Len> &a, const mystr<MAX__Len> &b);
template<int MAX__Len>
    friend bool operator != (const mystr<MAX__Len> &a, const mystr<MAX__Len> &b);
template<int MAX__Len>
    friend std::ostream& operator << (std::ostream &os, const mystr<MAX__Len> &b);
template<int MAX__Len>
    friend std::istream& operator >> (std::istream &is, mystr<MAX__Len> &b);

    char ch[MAX_LEN + 1];
public:
    mystr(string s = "") {
        for (int i = 0; i < s.size(); i++) ch[i] = s[i];
        ch[s.size()] = '\0';
    }
	mystr(const mystr &other) = default;
    // 到 string 的转换
    operator string() const {
        string res = "";
        for (int i = 0; i <= MAX_LEN; i++)
            if (ch[i] != '\0') res += ch[i];
            else break;
        return res;
    }
};

template<int MAX__Len>
bool operator < (const mystr<MAX__Len> &a, const mystr<MAX__Len> &b) {
    return strcmp(a.ch, b.ch) < 0;
}
template<int MAX__Len>
bool operator == (const mystr<MAX__Len> &a, const mystr<MAX__Len> &b) {
    return strcmp(a.ch, b.ch) == 0;
}
template<int MAX__Len>
bool operator <= (const mystr<MAX__Len> &a, const mystr<MAX__Len> &b) {
    return strcmp(a.ch, b.ch) <= 0;
}
template<int MAX__Len>
bool operator != (const mystr<MAX__Len> &a, const mystr<MAX__Len> &b) {
    return strcmp(a.ch, b.ch) != 0;
}

template<int MAX__Len>
std::ostream& operator << (std::ostream &os, const mystr<MAX__Len> &b) {
    os << b.ch;
    return os;
}
template<int MAX__Len>
std::istream& operator >> (std::istream &is, mystr<MAX__Len> &b) {
    is >> b.ch;
    return is;
}

// 将字符串 s 根据字符 ch 划分成若干字符串，每个用空格隔开，不包含空字符串
inline vector<string> split_by_ch(string s, char ch) {
    vector<string> res;
    string now = "";
    for (auto c : s) {
        if (c == ch) {
            if (now != "") res.push_back(now);
            now = "";
        }
        else now += c;
    }
    if (now != "") res.push_back(now);
    return res;
}


#endif // UTILITY_HPP