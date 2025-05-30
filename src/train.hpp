#ifndef TRAIN_HPP
#define TRAIN_HPP

#include "MR_with_cache.hpp"
#include "database.hpp"
#include "operator.hpp"
#include "priority_queue.hpp"
#include "utility.hpp"
#include "time.hpp"
#include "vector.hpp"
#include <cassert>
#include <functional>
#include <string>

using std::string;
using sjtu::pair;
using sjtu::vector;
using sjtu::priority_queue;

typedef mystr<20> trainstr;
typedef mystr<30> stationstr;

class Ticket_Manager;
class Train_Manager;

class Train {
    friend class Train_Manager;
    friend class Ticket_Manager;
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
    /*
    找到在 nowDate 当天，在 nowtime 之后，这辆车从 FROMID 到 TOID 站的信息，按照 query_ticket 的格式

    若 can_wait，那么可以等到下一天

    其中，FROMID 和 TOID 都是这个站在这辆车的 id（可以用 findstation 找到）

    1 <= FROMID < TOID <= stationNum
    
    返回的 vector : <trainID> <FROM> <LEAVING_TIME> -> <TO> <ARRIVING_TIME> <PRICE> <SEAT>
    
    如果这辆车时间不满足，返回空的 vector，否则 vector 返回的分别是上述的 9 个参数（LEAVING_TIME 分为日期和时间）
    
    此外，再加上最后的总的字符串以及该趟火车的 dayid
    */
    vector<string> query_ticket(int FROMID, int TOID, date nowDate, Time starttime = Time(), bool can_wait = false) {
        assert(1 <= FROMID && FROMID < TOID && TOID <= stationNum);
        vector<string> res;
        // 先算出从出发到 FROMID 站需要的时间
        Time nowtime = startTime;
        for (int i = 1; i < FROMID; i++) {
            nowtime += travelTimes[i];
            nowtime += stopoverTimes[i + 1];
        }
        // 需要提前 nowtime.day 天出发
        for (int i = 1; i <= nowtime.day; i++) nowDate--;
        // 这辆车到站的时间是 nowtime.hour, nowtime.minute
        // 如果比 starttime 小，那么一定是可以等的情况，并且这个时候要再迟一天
        if (nowtime.hour < starttime.hour ||
            (nowtime.hour == starttime.hour && nowtime.minute < starttime.minute)) {
            assert(can_wait);
            nowDate++;
        }
        // 这个时候的 nowDate 是这辆车应该出发的日期（如果可以等，就是至少出发的日期）
        if (nowDate > saleend) return res;
        if (nowDate < salebegin) {
            if (can_wait) nowDate = salebegin;
            else return res;
        }
        int dayid = int(nowDate) - int(salebegin);
        res.push_back(trainID);
        res.push_back(stations[FROMID]);
        // 再改回在 FROMID 的时候的日期和时间
        while(nowtime.day) nowtime.day--, nowDate++;
        res.push_back(string(nowDate));
        res.push_back(string(nowtime));
        int sumprice = 0, maxseat = seatNum;
        for (int i = FROMID; i < TOID; i++) {
            nowtime += travelTimes[i];
            if (i + 1 != TOID) nowtime += stopoverTimes[i + 1];
            sumprice += prices[i];
            int seat = remain_seat[dayid][i];
            if (seat < maxseat) maxseat = seat;
        }
        while(nowtime.day) nowtime.day--, nowDate++;
        res.push_back(stations[TOID]);
        res.push_back(string(nowDate));
        res.push_back(string(nowtime));
        res.push_back(std::to_string(sumprice));
        res.push_back(std::to_string(maxseat));
        string S = res[0] + " " + res[1] + " " + res[2] + " " + res[3] + " -> "
        + res[4] + " " + res[5] + " " + res[6] + " " + res[7] + res[8] + '\n';
        res.push_back(S);
        res.push_back(std::to_string(dayid));
        return res;
    }
    // 购买第 dayid 天，从 FROMID 到 TOID 的票 num 张
    void buy(int dayid, int num, int FROMID, int TOID) {
        for (int i = FROMID; i < TOID; i++)
            remain_seat[dayid][i] -= num;
    }
    // 判断第 dayid 天是否可以买从 FROMID 到 TOID 的票 num 张
    bool can_buy(int dayid, int num, int FROMID, int TOID) {
        for (int i = FROMID; i < TOID; i++)
            if (remain_seat[dayid][i] < num) return false;
        return true;
    }
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
        return "0";
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
    string query_ticket(OP &cmd) {
        cmd.setdefault('p', "time");
        date Date       = cmd.key('d');
        stationstr FROM = cmd.key('s');
        stationstr TO   = cmd.key('t');
        string method   = cmd.key('p');
        auto trainindex = station_to_stationdat->find_with_vector({FROM, TO});
        // 用于排序的优先队列
        priority_queue<pair<pair<int, trainstr>, string>,
            std::greater<pair<pair<int, trainstr>, string>>> Q;
        for (auto index : trainindex) {
            Train train; traindat->read(train, index);
            int FROMID = train.findstation(FROM);
            int TOID   = train.findstation(TO);
            auto info = train.query_ticket(FROMID, TOID, Date);
            if (info.empty()) continue;
            if (method == "time") {
                date FROMdate = info[2];
                Time FROMtime = info[3];
                date TOdate   = info[5];
                Time TOtime   = info[6];
                int tottime = calc_time_to_begin(TOdate, TOtime)
                            - calc_time_to_begin(FROMdate, FROMtime);
                Q.push({{tottime, train.trainID}, info[9]});
            }
            else {
                int totprice = std::stoi(info[7]);
                Q.push({{totprice, train.trainID}, info[9]});
            }
        }
        string res = std::to_string(Q.size()) + "\n";
        while(!Q.empty()) {
            auto u = Q.top(); Q.pop();
            res += u.second;
        }
        return res;
    }
    string query_transfer(OP &cmd) {
        cmd.setdefault('p', "time");
        date Date       = cmd.key('d');
        stationstr FROM = cmd.key('s');
        stationstr TO   = cmd.key('t');
        string method   = cmd.key('p');
        pair<pair<pair<int, int>, pair<trainstr, trainstr>>, string> ans;
        bool is_find = false;
        auto possibletrain1 = station_to_indexdat->find_with_vector(FROM);
        for (auto train1index : possibletrain1) {
            Train train1; traindat->read(train1, train1index);
            int FROMID = train1.findstation(FROM);
            for (int j = FROMID + 1; j <= train1.stationNum; j++) {
                stationstr MIDstation = train1.stations[j];
                vector<string> info1 = train1.query_ticket(FROMID, j, Date);
                if (info1.empty()) continue;
                date FROMdate = info1[2];
                Time FROMtime = info1[3];
                date Middate   = info1[5];
                Time Midtime   = info1[6];
                int price1  = std::stoi(info1[7]);
                auto possibletrain2 = station_to_stationdat->find_with_vector({MIDstation, TO});
                for (auto train2index : possibletrain2) {
                    if (train2index == train1index) continue;
                    Train train2; traindat->read(train2, train2index);
                    vector<string> info2 = train2.query_ticket(
                        train2.findstation(MIDstation), train2.findstation(TO), 
                        Middate, Midtime, true);
                    if (info2.empty()) continue;
                    date TOdate = info2[5];
                    Time TOtime = info2[6];
                    int price2 = std::stoi(info2[7]);
                    int tottime = calc_time_to_begin(TOdate, TOtime)
                                - calc_time_to_begin(FROMdate, FROMtime);
                    int totprice = price1 + price2;
                    pair<pair<pair<int, int>, pair<trainstr, trainstr>>, string> new_ans;
                    if (method == "time")
                        new_ans = {{{tottime, totprice}, 
                        {train1.trainID, train2.trainID}}, info1[9] + info2[9]};
                    else
                        new_ans = {{{totprice, tottime}, 
                        {train1.trainID, train2.trainID}}, info1[9] + info2[9]};
                    if (!is_find) {
                        is_find = true;
                        ans = new_ans;
                    }
                    else if (new_ans < ans) ans = new_ans;
                }
            }
        }
        if (!is_find) return "0";
        else return ans.second;
    }
};

#endif //TRAIN_HPP