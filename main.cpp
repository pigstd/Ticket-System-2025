#include <iostream>
#include <string>
#include "MR_with_cache.hpp"
#include "database.hpp"
#include "operator.hpp"
#include "user.hpp"

using std::cin;
using std::cout;

int main() {
    MR_with_cache<User, 0> userdat;
    userdat.initialise("user.dat");
    BPTdatabase<userstr, int, 0, 50> username_to_index("username_to_index.dat");
    User_Manager usermanager(&userdat, &username_to_index);
    string in;
    while(std::getline(cin, in)) {
        OP cmd(in);
        switch (cmd.type) {
            case _add_user: {
                cout << cmd.showtimestamp() << ' ' << usermanager.add_user(cmd) << '\n';
                break;
            }
            case _login: {
                cout << cmd.showtimestamp() << ' ' << usermanager.login(cmd) << '\n';
                break;
            }
            case _logout: {
                cout << cmd.showtimestamp() << ' ' << usermanager.logout(cmd) << '\n';
                break;
            }
            case _query_profile: {
                cout << cmd.showtimestamp() << ' ' << usermanager.query_profile(cmd) << '\n';
                break;
            }
            case _modify_profile: {
                cout << cmd.showtimestamp() << ' ' << usermanager.modify_profile(cmd) << '\n';
                break;
            }
            // other case: to do
        }
    }
}