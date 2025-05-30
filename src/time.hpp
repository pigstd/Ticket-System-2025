#ifndef TIME_HPP
#define TIME_HPP

#include "utility.hpp"
#include <cassert>
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
        if (month != 1 && day == 1) month--, day = day_per_month[month];
        else day--;
        return *this;
    }
    date operator--(int x) {
        date tmp = *this;
        --(*this);
        return tmp;
    }
    // 距离 1 月 1 日的日期
    operator int() const {
        int res = day - 1;
        for (int i = 1; i < month; i++) res += day_per_month[i];
        return res;
    }
    // 转换为一个字符串
    operator string() const {
        return to_string_len2(month) + "-" + to_string_len2(day);
    }
};

class Time {
    static constexpr int minutelim = 60;
    static constexpr int hourlim = 24;
    void check_valid() {
        assert(hour >= 0 && hour < hourlim && minute >= 0 && minute < minutelim);
    }
public:
    int day, hour, minute;
    Time() : day(0), hour(0), minute(0) {}
    // 从 int 的构造函数：00:mm
    Time(int mm) : day(0), hour(0), minute(mm) {
        // mm 可能比较大
        if (minute >= minutelim) {
            hour = minute / minutelim;
            minute -= hour * minutelim;
            if (hour >= hourlim) {
                day = hour / hourlim;
                hour -= day * hourlim;
            }
        }
    }
    // hh:mm 的构造函数
    Time(string s) : day(0) {
        auto vec = split_by_ch(s, ':');
        hour = std::stoi(vec[0]), minute = std::stoi(vec[1]);
        check_valid();
    }
    Time& operator +=(const Time &b) {
        day += b.day, hour += b.hour, minute += b.minute;
        if (minute >= minutelim) minute -= minutelim, hour++;
        if (hour >= hourlim) hour -= hourlim, day++;
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
    // 转化为 int，表示用分钟表示的时间
    operator int() const {
        return day * hourlim * minutelim + hour * minutelim + minute;
    }
    // 转换为一个字符串
    operator string() const {
        return to_string_len2(hour) + ":" + to_string_len2(minute);
    }
};

// 计算第 _date 天 _time 时刻到 1 月 1 号 0 点 0 分的时间（返回分数）
inline int calc_time_to_begin(const date &_date, const Time &_time) {
    int day = int(_date);
    return day * 24 * 60 + int(_time);
}

#endif