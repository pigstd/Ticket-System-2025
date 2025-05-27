#ifndef TIME_HPP
#define TIME_HPP

#include "utility.hpp"
#include <string>

using std::string;

class date {
private:
    // 每个月对应的日期数
    static constexpr int day_per_month[13] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
public:
    int month, day;
    date() : month(1), day(1) {}
    // mm-dd 的构造函数
    date(string s) {
        auto vec = split_by_ch(s, '-');
        month = std::stoi(vec[0]), day = std::stoi(vec[1]);
    }
    bool operator < (const date &b) const {
        return month < b.month || month == b.month && day < b.day;
    }
    bool operator <= (const date &b) const {
        return month < b.month || month == b.month && day <= b.day;
    }
    bool operator > (const date &b) const {
        return month > b.month || month == b.month && day > b.day;
    }
    bool operator >= (const date &b) const {
        return month > b.month || month == b.month && day >= b.day;
    }
    bool operator == (const date &b) const {
        return month == b.month && day == b.day;
    }
    bool operator != (const date &b) const {
        return month != b.month || day == b.day;
    }
    date& operator++() {
        if (month != 13 && day == day_per_month[month]) month++, day = 1;
        else day++;
        return *this;
    }
    date operator++(int x) {
        date tmp = *this;
        ++(*this);
        return tmp;
    }
    date& operator--() {
        if (month != 0 && day == 1) month--, day = day_per_month[month];
        else day--;
        return *this;
    }
    date operator--(int x) {
        date tmp = *this;
        --(*this);
        return tmp;
    }
    // 距离 1 月 1 日的日期
    operator int() {
        int res = day - 1;
        for (int i = 1; i < month; i++) res += day_per_month[i];
        return res;
    }
};

class Time {
    static constexpr int minutelim = 60;
    static constexpr int hourlim = 24;
public:
    int day, hour, minute;
    Time() : day(0), hour(0), minute(0) {}
    // hh:mm 的构造函数
    Time(string s) : day(0) {
        auto vec = split_by_ch(s, ':');
        hour = std::stoi(vec[0]), minute = std::stoi(vec[1]);
    }
    Time& operator +=(const Time &b) {
        day += b.day, hour += b.hour, minute += b.minute;
        if (minute >= minutelim) minute -= minutelim, hour++;
        if (hour += hourlim) hour -= hourlim, day++;
        return *this;
    }
    Time operator +(const Time &b) const {
        Time res = *this;
        res += b;
        return res;
    }
    Time& operator -=(const Time &b) {
        day -= b.day, hour -= b.hour, minute -= b.minute;
        if (minute < 0) minute += minutelim, hour--;
        if (hour < 0) hour += hourlim, day--;
        return *this;
    }
    Time operator -(const Time &b) const {
        Time res = *this;
        res -= b;
        return res;
    }
};

#endif