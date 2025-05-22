#ifndef OPERATOR_HPP
#define OPERATOR_HPP


#include "utility.hpp"
#include "vector.hpp"
#include <cassert>
#include <string>
using std::string;
using sjtu::vector;

// 操作的分类
// 用下划线开头，避免重名
enum operatortype {
    _add_user, _login, _logout, _query_profile, _modify_profile,
    _add_train, _delete_train, _release_train, _query_train, _query_ticket, _query_transfer,
    _buy_ticket, _query_order, _refund_ticket,
    _clean, _exit
};

class OP {
    operatortype type;
    int timestamp;
    string value[26];
    bool is_exist[26];
public:
    OP() = default;
    OP(const string &cmd) {
        for (int i = 0; i < 26; i++) value[i] = "", is_exist[i] = false;
        vector<string> op = split_by_ch(cmd, ' ');
        assert(op.size() > 2 && op.size() % 2 == 0);
        // [timestamp] = op[0]
        for (int i = 1; i + 1 < op[0].size(); i++) timestamp = timestamp * 10 + op[0][i] - '0';
        // cmd = op[1]
        if (op[1] == "add_user") type = _add_user;
        else if (op[1] == "login") type = _login;
        else if (op[1] == "logout") type = _logout;
        else if (op[1] == "query_profile") type = _query_profile;
        else if (op[1] == "modify_profile") type = _modify_profile;
        else if (op[1] == "add_train") type = _add_train;
        else if (op[1] == "delete_train") type = _delete_train;
        else if (op[1] == "release_train") type = _release_train;
        else if (op[1] == "query_train") type = _query_train;
        else if (op[1] == "query_ticket") type = _query_ticket;
        else if (op[1] == "query_transfer") type = _query_transfer;
        else if (op[1] == "buy_ticket") type = _buy_ticket;
        else if (op[1] == "query_order") type = _query_order;
        else if (op[1] == "refund_ticket") type = _refund_ticket;
        else if (op[1] == "clean") type = _clean;
        else if (op[1] == "exit") type = _exit;
        else assert(0); // invalid cmd
        for (int i = 2; i < op.size(); i++) {
            assert(op[i].size() == 2);
            int num = op[i][1] - 'a';
            value[num] = op[i + 1], is_exist[num] = true;
        }
    }
    // 设置 -<ch> <s> 的默认值
    // 如果已经填了，就不修改，否则就设置为默认值
    void setdefault(char ch, string s) {
        int num = ch - 'a';
        if (!is_exist[num]) is_exist[num] = 1, value[num] = s;
    }
    // -ch 是否存在
    bool exist(char ch) const {return is_exist[ch - 'a'];}
    // -ch 对应的参数
    string key(char ch) const {return value[ch - 'a'];}
};

#endif //OPERATOR_HPP