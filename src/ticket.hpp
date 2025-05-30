#ifndef TICKET_HPP
#define TICKET_HPP

#include "MR_with_cache.hpp"
#include "database.hpp"
#include "operator.hpp"
#include "time.hpp"
#include "train.hpp"
#include "user.hpp"
#include <iostream>
#include <math.h>
#include <string>
class Ticket_Manager;

class Order {
    friend class Ticket_Manager;
    enum ordertype {SUCCESS, PENDING, REFUNDED};
    ordertype status;
    userstr username;
    trainstr trainID;
    // dayid : 火车开始的那个日期的 id（距离 salebegin 的日期）
    int dayid;
    stationstr FROM, TO;
    date FROMdate, TOdate;
    Time FROMtime, TOtime;
    int price, buy_num, trainindex;
public:
    Order() = default;
    Order(string _username, string _trainID, int _dayid, string _FROM, string _TO, string _FROMdate,
          string _TOdate, string _FROMtime, string _TOtime, int _price, int _buy_num, int _trainindex) :
    username(_username), trainID(_trainID), dayid(_dayid), FROM(_FROM), TO(_TO),
    FROMdate(_FROMdate), TOdate(_TOdate), FROMtime(_FROMtime), TOtime(_TOtime), price(_price), buy_num(_buy_num), trainindex(_trainindex) {}
    void set_satue(ordertype new_status) {
        status = new_status;
    }
    // 返回这个订单的信息
    string query() {
        string res = "";
        switch (status) {
            case SUCCESS:
                res = "[success]";
                break;
            case PENDING:
                res = "[pending]";
                break;
            case REFUNDED:
                res = "[refunded]";
                break;
        }
        res += ' ' + string(trainID) + ' ' + string(FROM) + ' ' + string(FROMdate) + ' ' + string(FROMtime)
        + " -> " + string(TO) + ' ' + string(TOdate) + ' ' + string(TOtime) + ' ' + std::to_string(price) + ' ' + std::to_string(buy_num) + '\n';
        return res;
    }
    // TO DO!!!
};

class Ticket_Manager {
    MR_with_cache<User, 0> *userdat;
    BPTdatabase<userstr, int, 0, 50> *username_to_indexdat;
    MR_with_cache<Train, 0> *traindat;
    BPTdatabase<trainstr, int, 0, 50> *trainid_to_indexdat;
    MR_with_cache<Order, 0> *orderdat;
    BPTdatabase<pair<int, int>, int, 0, 50> *pendingqueuedat;
    BPTdatabase<int, int, 0, 50> *userorderat;
    map<userstr, bool> *login_users;
    // 找到 username 对应的 userindex
    int finduserindex(userstr username) {
        auto vec = username_to_indexdat->find_with_vector(username);
        if (vec.empty()) return -1;
        else return vec[0];
    }
    // 找到 trainID 对应的 trainindex
    int findtrainindex(trainstr trainID) {
        auto vec = trainid_to_indexdat->find_with_vector(trainID);
        if (vec.empty()) return -1;
        else return vec[0];
    }
public:
    Ticket_Manager() = default;
    Ticket_Manager(MR_with_cache<User, 0> *_userdat,
    BPTdatabase<userstr, int, 0, 50> *_username_to_indexdat,
    MR_with_cache<Train, 0> *_traindat,
    BPTdatabase<trainstr, int, 0, 50> *_trainid_to_indexdat,
    MR_with_cache<Order, 0> *_orderdat,
    BPTdatabase<pair<int, int>, int, 0, 50> *_pendingqueuedat,
    BPTdatabase<int, int, 0, 50> *_userorderat,
    map<userstr, bool> *_login_users) : userdat(_userdat), traindat(_traindat), username_to_indexdat(_username_to_indexdat), 
    trainid_to_indexdat(_trainid_to_indexdat), orderdat(_orderdat), pendingqueuedat(_pendingqueuedat), userorderat(_userorderat), login_users(_login_users) {}
    string buy_ticket(OP &cmd) {
        cmd.setdefault('q', "false");
        string username = cmd.key('u');
        string trainID  = cmd.key('i');
        string Date     = cmd.key('d');
        string FROM     = cmd.key('f');
        string TO       = cmd.key('t');
        string num_str  = cmd.key('n');
        string can_wait = cmd.key('q');
        if (login_users->find(username) == login_users->end()) return "-1";
        int userindex = finduserindex(username);
        User user; userdat->read(user, userindex);
        int trainindex = findtrainindex(trainID);
        if (trainindex == -1) return "-1";
        Train train; traindat->read(train, trainindex);
        if (!train.is_release) return "-1";
        // 以上：找到了对应的用户和火车
        // 如果票数大于火车的总票数，返回 -1
        int buynum    = std::stoi(num_str);
        if (train.seatNum < buynum) return "-1";
        int FROMID = train.findstation(FROM);
        int TOID   = train.findstation(TO);
        if (TOID <= FROMID || TOID == -1 || FROMID == -1) return "-1";
        auto info = train.query_ticket(FROMID, TOID, Date);
        if (info.empty()) return "-1";
        date FROMdate = info[2];
        Time FROMtime = info[3];
        date TOdate   = info[5];
        Time TOtime   = info[6];
        int totprice  = std::stoi(info[7]);
        int max_seat  = std::stoi(info[8]);
        int dayid     = std::stoi(info[10]);
        Order order(username, trainID, dayid, FROM, TO, FROMdate,
            TOdate, FROMtime, TOtime, totprice, buynum, trainindex);
        if (buynum <= max_seat) {
            // 可以购买
            order.set_satue(Order::SUCCESS);
            int orderindex = orderdat->write(order);
            userorderat->insert(userindex, orderindex);
            train.buy(dayid, buynum, FROMID, TOID);
            traindat->update(train, trainindex);
            long long sum_price = (long long)totprice * buynum;
            return std::to_string(sum_price);
        }
        else if (can_wait == "true") {
            // 加入候补队列
            order.set_satue(Order::PENDING);
            int orderindex = orderdat->write(order);
            userorderat->insert(userindex, orderindex);
            pendingqueuedat->insert({trainindex, dayid}, orderindex);
            return "queue";
        }
        else return "-1";
    }
    string query_order(const OP &cmd)  {
        string username = cmd.key('u');
        if (login_users->find(username) == login_users->end()) return "-1\n";
        int userindex = finduserindex(username);
        auto ordervec = userorderat->find_with_vector(userindex);
        string res = std::to_string(ordervec.size()) +  '\n';
        // 需要反着来
        for (int i = (int)(ordervec.size()) - 1; i >= 0; i--) {
            int orderindex = ordervec[i];
            Order order; orderdat->read(order, orderindex);
            res += order.query();
        }
        return res;
    }
    string refund_ticket(OP &cmd) {
        cmd.setdefault('n', "1");
        string username = cmd.key('u');
        int num = std::stoi(cmd.key('n'));
        if (login_users->find(username) == login_users->end()) return "-1";
        int userindex = finduserindex(username);
        // 先找到对应的那个订单
        auto ordervec = userorderat->find_with_vector(userindex);
        if (ordervec.size() < num) return "-1";
        int refund_orderinex = ordervec[ordervec.size() - num];
        Order refund_order; orderdat->read(refund_order, refund_orderinex);
        if (refund_order.status == Order::REFUNDED) return "-1";
        if (refund_order.status == Order::PENDING) {
            // 退款在 pending 的 order
            refund_order.set_satue(Order::REFUNDED);
            orderdat->update(refund_order, refund_orderinex);
            pendingqueuedat->del({refund_order.trainindex, refund_order.dayid}, refund_orderinex);
            return "0";
        }
        // status == SUCCESS
        // 退款已经 SUCCESS 的 order
        refund_order.set_satue(Order::REFUNDED);
        orderdat->update(refund_order, refund_orderinex);
        int trainindex = refund_order.trainindex;
        Train train; traindat->read(train, trainindex);
        int dayid  = refund_order.dayid;
        int FROMID = train.findstation(refund_order.FROM);
        int TOID   = train.findstation(refund_order.TO);
        int refund_num = refund_order.buy_num;
        // 相当于买 -refund_num 张票
        train.buy(dayid, -refund_num, FROMID, TOID);
        // 接下来检查候补队列
        auto pendingque = pendingqueuedat->find_with_vector({trainindex, dayid});
        for (auto pending_orderindex : pendingque) {
            Order pending_order; orderdat->read(pending_order, pending_orderindex);
            // pending 订单的 FROMID 和 TOID
            int FROMID = train.findstation(pending_order.FROM);
            int TOID   = train.findstation(pending_order.TO);
            if (train.can_buy(dayid, pending_order.buy_num, FROMID, TOID)) {
                // 这个订单成功购买
                train.buy(dayid, pending_order.buy_num, FROMID, TOID);
                pending_order.set_satue(Order::SUCCESS);
                pendingqueuedat->del({trainindex, dayid}, pending_orderindex);
                orderdat->update(pending_order, pending_orderindex);
            }
        }
        traindat->update(train, trainindex);
        return "0";
    }
};

#endif //TICKET_HPP