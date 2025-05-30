#include <iostream>
#include <string>
#include "MR_with_cache.hpp"
#include "database.hpp"
#include "operator.hpp"
#include "ticket.hpp"
#include "train.hpp"
#include "user.hpp"

using std::cin;
using std::cout;

int main() {
    MR_with_cache<User, 0> userdat;
    userdat.initialise("user.dat", 0, 0);
    BPTdatabase<userstr, int, 0, 50> username_to_index("username_to_index.dat");
    map<userstr, bool> login_users;
    User_Manager usermanager(&userdat, &username_to_index, &login_users);
    MR_with_cache<Train, 0> traindat;
    traindat.initialise("train.dat", 0, 0);
    BPTdatabase<trainstr, int, 0, 50> trainid_to_indexdat("trainid_to_index.dat");
    BPTdatabase<pair<stationstr, stationstr>, int, 0, 50> station_to_stationdat("station_to_station.dat");
    BPTdatabase<stationstr, int, 0, 50> station_to_indexdat("station_to_index.dat");
    Train_Manager trainmanager(&traindat, &trainid_to_indexdat, &station_to_stationdat, &station_to_indexdat);
    MR_with_cache<Order, 0> orderdat;
    orderdat.initialise("order.dat", 0, 0);
    BPTdatabase<pair<int, int>, int, 0, 50> pendingqueuedat("pendingqueue.dat");
    BPTdatabase<int, int, 0, 50> userorderdat("userorder.dat");
    Ticket_Manager ticketmanager(&userdat, &username_to_index, &traindat, &trainid_to_indexdat, &orderdat, &pendingqueuedat, &userorderdat, &login_users);
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
            case _add_train: {
                cout << cmd.showtimestamp() << ' ' << trainmanager.add_train(cmd) << '\n';
                break;
            }
            case _delete_train: {
                cout << cmd.showtimestamp() << ' ' << trainmanager.delete_train(cmd) << '\n';
                break;
            }
            case _release_train: {
                cout << cmd.showtimestamp() << ' ' << trainmanager.releaser_train(cmd) << '\n';
                break;
            }
            case _query_train: {
                // 不用输出换行
                cout << cmd.showtimestamp() << ' ' << trainmanager.query_train(cmd);
                break;
            }
            case _query_ticket: {
                // 不用输出换行
                cout << cmd.showtimestamp() << ' ' << trainmanager.query_ticket(cmd);
                break;
            }
            case _query_transfer: {
                // 不用输出换行
                cout << cmd.showtimestamp() << ' ' << trainmanager.query_transfer(cmd);
                break;
            }
            case _buy_ticket: {
                cout << cmd.showtimestamp() << ' ' << ticketmanager.buy_ticket(cmd) << '\n';
                break;
            }
            case _query_order: {
                // 不用输出换行
                cout << cmd.showtimestamp() << ' ' << ticketmanager.query_order(cmd);
                break;
            }
            case _refund_ticket: {
                cout << cmd.showtimestamp() << ' ' << ticketmanager.refund_ticket(cmd) << '\n';
                break;
            }
            case _clean: {
                userdat.clear("user.dat", 0);
                username_to_index.clear();
                login_users.clear();
                traindat.clear("train.dat", 0);
                trainid_to_indexdat.clear();
                station_to_stationdat.clear();
                station_to_indexdat.clear();
                orderdat.clear("order.dat", 0);
                pendingqueuedat.clear();
                userorderdat.clear();
                cout << cmd.showtimestamp() << " 0\n";
                break;
            }
            case _exit: {
                cout << cmd.showtimestamp() << ' ' << "bye" << '\n';
                return 0;
            }
            // other case: to do
        }
    }
}