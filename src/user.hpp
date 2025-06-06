#ifndef USER_H
#define USER_H

#include "map.hpp"
#include "operator.hpp"
#include "utility.hpp"
#include "MR_with_cache.hpp"
#include "database.hpp"
#include "vector.hpp"
#include <cassert>
#include <iostream>
#include <string>

using sjtu::map;
using sjtu::vector;
using std::string;


typedef mystr<20> userstr;
typedef mystr<15> namestr;
typedef mystr<30> pswdstr;
typedef mystr<30> mailstr;


class Ticket_Manager;
class User_Manager;

class User {
    friend class User_Manager;
    friend class Ticket_Manager;
    userstr username;
    pswdstr password;
    namestr name;
    mailstr mailAddr;
    int privilege;
public:
    User() = default;
    User(string _username, string _password, string _name, string _mailAddr, string _privilege) :
    username(_username), password(_password), name(_name), mailAddr(_mailAddr), privilege(std::stoi(_privilege)) {}
    // 检验密码是否正确
    bool checkpswd(string s) const {
        return password == pswdstr(s);
    }
    // 用户信息的信息，返回一个 string
    string query_profile() const {
        return string(username) + " " + string(name) + " " + string(mailAddr) + " " + std::to_string(privilege);
    }
};

class User_Manager {
    MR_with_cache<User, 0> *userdat;
    BPTdatabase<userstr, int, 0, 50> *username_to_indexdat;
    map<userstr, bool> *login_users;
    // 找到 username 对应的 index
    // 若不存在，返回 -1
    int findindex(userstr username) {
        auto vec = username_to_indexdat->find_with_vector(username);
        if (vec.empty()) return -1;
        else return vec[0];
    }
public:
    User_Manager() = delete;
    User_Manager(MR_with_cache<User, 0> *_userdat, BPTdatabase<userstr, int, 0, 50> *_username_to_indexdat,
                map<userstr, bool> *_login_users) :
        userdat(_userdat), username_to_indexdat(_username_to_indexdat), login_users(_login_users) {}
    string add_user(const OP &cmd) {
        userstr user_name = cmd.key('u');
        // 第一个用户
        if (username_to_indexdat->empty()) {
            User user(user_name, cmd.key('p'), cmd.key('n'), cmd.key('m'), "10");
            int index = userdat->write(user);
            username_to_indexdat->insert(user_name, index);
            return "0";
        }
        int cur_index = findindex(cmd.key('c'));
        if (cur_index == -1) return "-1";
        User cur_user; userdat->read(cur_user, cur_index);
        if (login_users->find(cmd.key('c')) == login_users->end() || cur_user.privilege <= std::stoi(cmd.key('g')))
            return "-1";
        if (findindex(user_name) != -1) return "-1";
        User new_user(user_name, cmd.key('p'), cmd.key('n'), cmd.key('m'), cmd.key('g'));
        int index = userdat->write(new_user);
        username_to_indexdat->insert(user_name, index);
        return "0";
    }
    string login(const OP &cmd) {
        userstr user_name = cmd.key('u');
        int index = findindex(user_name);
        if (index == -1) return "-1";
        User user; userdat->read(user, index);
        if (login_users->find(user_name) != login_users->end() || !user.checkpswd(cmd.key('p'))) return "-1";
        (*login_users)[user_name] = 1;
        return "0";
    }
    string logout(const OP &cmd) {
        userstr user_name = cmd.key('u');
        if (login_users->find(user_name) == login_users->end()) return "-1";
        login_users->erase(login_users->find(user_name));
        return "0";
    }
    string query_profile(const OP &cmd) {
        userstr cur_username = cmd.key('c');
        userstr user_name = cmd.key('u');
        int cur_index = findindex(cur_username);
        int index = findindex(user_name);
        if (cur_index == -1 || index == -1) return "-1";
        User cur_user, user;
        userdat->read(cur_user, cur_index);
        if (login_users->find(cur_username) == login_users->end()) return "-1";
        userdat->read(user, index);
        if (index == cur_index || cur_user.privilege > user.privilege) return user.query_profile();
        else return "-1";
    }
    string modify_profile(const OP &cmd) {
        userstr cur_username = cmd.key('c');
        userstr user_name = cmd.key('u');
        int cur_index = findindex(cur_username);
        int index = findindex(user_name);
        if (cur_index == -1 || index == -1) return "-1";
        User cur_user, user;
        userdat->read(cur_user, cur_index);
        if (login_users->find(cur_username) == login_users->end()) return "-1";
        userdat->read(user, index);
        if (index != cur_index && cur_user.privilege <= user.privilege) return "-1";
        // 如果修改权限 
        if (cmd.exist('g')) {
            int new_privilege = std::stoi(cmd.key('g'));
            if (new_privilege >= cur_user.privilege) return "-1";
            user.privilege = new_privilege;
        }
        if (cmd.exist('p')) user.password = cmd.key('p');
        if (cmd.exist('n')) user.name = cmd.key('n');
        if (cmd.exist('m')) user.mailAddr = cmd.key('m');
        userdat->update(user, index);
        return user.query_profile();
    }
};

#endif // USER_H