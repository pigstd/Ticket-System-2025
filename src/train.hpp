#ifndef TRAIN_HPP
#define TRAIN_HPP

#include "MR_with_cache.hpp"
#include "database.hpp"
#include "operator.hpp"
#include "utility.hpp"
#include "time.hpp"
#include <string>

using std::string;
using sjtu::pair;

typedef mystr<20> trainstr;
typedef mystr<30> stationstr;

class Train_Manager;

class Train {
    friend class Train_Manager;
    static constexpr int STATION_MAX= 105;
    static constexpr int DAY_MAX = 100;
    trainstr trainID;
    int stationNum, seatNum;
    stationstr stations[STATION_MAX];
    int prices[STATION_MAX];
    Time startTime;
    int travelTimes[STATION_MAX];
    int stopoverTimes[STATION_MAX];
    date salebegin, saleend;
    char type;
    bool is_release;
    // 距离开始日期的第 i 天的第 j 到 j + 1 个站的车票剩余量（i 是从 0 开始）
    int remain_seat[DAY_MAX][STATION_MAX];
public:
    Train() = default;
    Train(string _trainID, string _stationNum, string _seatNum, string _stations, string _prices,
          string _startTime, string _travelTimes, string _stopoverTimes, string _saleDate, string _type) :
        trainID(_trainID), stationNum(std::stoi(_stationNum)), seatNum(std::stoi(_seatNum)),
        startTime(_startTime), type(_type[0]), is_release(false) {
        auto stationsvec = split_by_ch(_stations, '|');
        for (int i = 1; i <= stationNum; i++) stations[i] = stationsvec[i - 1];
        auto pricevec = split_by_ch(_prices, '|');
        for (int i = 1; i < stationNum; i++) prices[i] = std::stoi(pricevec[i - 1]);
        auto travelTimesvec = split_by_ch(_travelTimes, '|');
        for (int i = 1; i < stationNum; i++) travelTimes[i] = std::stoi(travelTimesvec[i - 1]);
        if (stationNum > 2) {
            auto stopoverTimesvec = split_by_ch(_stopoverTimes, '|');
            for (int i = 2; i < stationNum; i++) stopoverTimes[i] = std::stoi(stopoverTimesvec[i - 2]);
        }
        auto dates = split_by_ch(_saleDate, '|');
        salebegin = dates[0], saleend = dates[1];
        int totday = int(saleend) - int(salebegin);
        for (int i = 0; i <= totday; i++) for (int j = 1; j < stationNum; j++) remain_seat[i][j] = seatNum;
    }
    // 发布该车
    // 传入两个数据库的指针和 index 作为参数
    void release(BPTdatabase<pair<stationstr, stationstr>, int, 0, 50> *station_to_stationdat,
    BPTdatabase<stationstr, int, 0, 50> *station_to_indexdat, int index) {
        is_release = true;
        for (int i = 1; i <= stationNum; i++)
            for (int j = i + 1; j <= stationNum; j++)
                station_to_stationdat->insert({stations[i], stations[j]}, index);
        for (int i = 1; i <= stationNum; i++)
            station_to_indexdat->insert(stations[i], index);
    }
    // query train，返回第 Date 天的 train 的信息
    string query_train(date Date) {
        // dayindex: 用于座位的处理
        int dayindex = int(Date) - int(salebegin);
        string res = string(trainID) + ' ' + type + '\n';
        Time nowtime = startTime;
        int totprice = 0;
        for (int i = 1; i <= stationNum; i++) {
            res += string(stations[i]) + ' ';
            if (i == 1) res += "xx-xx xx:xx";
            else res += string(Date) + " " + string(nowtime);
            res += " -> ";
            if (i != 1) {
                totprice += prices[i - 1];
                if (i != stationNum) nowtime += stopoverTimes[i];
                // 超过一天的处理
                while(nowtime.day != 0) nowtime.day--, Date++;
            }
            if (i != stationNum) res += string(Date) + " " + string(nowtime);
            else res += "xx-xx xx:xx";
            res += " " + std::to_string(totprice) + " ";
            if (i != stationNum) res += std::to_string(remain_seat[dayindex][i]);
            else res += "x";
            res += '\n';
        }
        return res;
    }
    // 找到站名对应的是第几站
    int findstation(stationstr station) {
        for (int i = 1; i <= stationNum; i++)
            if (stations[i] == station) return i;
        return -1;
    }
    // TO DO
};

class Train_Manager {
    MR_with_cache<Train, 0> *traindat;
    BPTdatabase<trainstr, int, 0, 50> *trainid_to_indexdat;
    BPTdatabase<pair<stationstr, stationstr>, int, 0, 50> *station_to_stationdat;
    BPTdatabase<stationstr, int, 0, 50> *station_to_indexdat;
    // 找到 trainID 对应的 index
    // 若不存在，返回 -1
    int findindex(trainstr trainID) {
        auto vec = trainid_to_indexdat->find_with_vector(trainID);
        if (vec.empty()) return -1;
        else return vec[0];
    }
public:
    Train_Manager() = default;
    Train_Manager(MR_with_cache<Train, 0> *_traindat, BPTdatabase<trainstr, int, 0, 50> *_trainid_to_indexdat,
    BPTdatabase<pair<stationstr, stationstr>, int, 0, 50> *_station_to_stationdat, BPTdatabase<stationstr, int, 0, 50> *_station_to_indexdat) :
    traindat(_traindat), trainid_to_indexdat(_trainid_to_indexdat), station_to_stationdat(_station_to_stationdat),
    station_to_indexdat(_station_to_indexdat) {}
    string add_train(const OP &cmd) {
        string trainID = cmd.key('i');
        if (findindex(trainID) != -1) return "-1";
        string stationNum    = cmd.key('n');
        string seatNum       = cmd.key('m');
        string stations      = cmd.key('s');
        string prices        = cmd.key('p');
        string startTime     = cmd.key('x');
        string travelTimes   = cmd.key('t');
        string stopoverTimes = cmd.key('o');
        string saleDate      = cmd.key('d');
        string type          = cmd.key('y');
        Train new_train(trainID, stationNum, seatNum, stations, prices,
            startTime, travelTimes, stopoverTimes, saleDate, type);
        int index = traindat->write(new_train);
        trainid_to_indexdat->insert(trainID, index);
        return "0";
    }
    string delete_train(const OP &cmd) {
        string trainID = cmd.key('i');
        int index = findindex(trainID);
        if (index == -1) return "-1";
        Train train; traindat->read(train, index);
        if (train.is_release == true) return "-1";
        traindat->Delete(index);
        trainid_to_indexdat->del(trainID, index);
        return "0";
    }
    string releaser_train(const OP &cmd) {
        string trainID = cmd.key('i');
        int index = findindex(trainID);
        if (index == -1) return "-1";
        Train train; traindat->read(train, index);
        if (train.is_release == true) return "-1";
        train.release(station_to_stationdat, station_to_indexdat, index);
        traindat->update(train, index);
    }
    string query_train(const OP &cmd) {
        string trainID = cmd.key('i');
        date Date      = cmd.key('d');
        int index = findindex(trainID);
        if (index == -1) return "-1";
        Train train; traindat->read(train, index);
        if (!(train.salebegin <= Date && Date <= train.saleend)) return "-1";
        return train.query_train(Date);
    }
    string query_ticket(const OP &cmd) {
        // TO DO!!
    }
    string query_transfer(const OP &cmd) {
        // TO DO!!
    }
    // TO DO
};

#endif //TRAIN_HPP