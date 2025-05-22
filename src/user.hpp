#ifndef USER_H
#define USER_H

#include "operator.hpp"
#include "utility.hpp"
#include "MR_with_cache.hpp"
#include "database.hpp"

typedef mystr<20> userstr;
typedef mystr<10> namestr;
typedef mystr<30> pswdstr;
typedef mystr<30> mailstr;

class User {
    userstr username;
    pswdstr password;
    namestr name;
    mailstr mailAddr;
    int privilege;
public:
    // to do
};

class User_Manager {
    MR_with_cache<User> *userdat;
    BPTdatabase<userstr, int> *username_to_indexdat;
public:
    string add_user(const OP &cmd) {
        // to do
    }
    string login(const OP &cmd) {
        // to do
    }
    string logout(const OP &cmd) {
        // to do
    }
    string query_profile(const OP &cmd) {
        // to do
    }
    string modify_profile(const OP &cmd) {
        // to do
    }
};

#endif // USER_H