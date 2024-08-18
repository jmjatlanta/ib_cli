#include <iostream>
#include <vector>
#include <string>

#include "IBConnector.hpp"

std::vector<std::string> availableAccounts;
std::vector<std::string> accounts;
std::vector<std::string> tickers;
std::string host = "10.8.0.1";
int port = 4007;

bool parse_command_line(int argc, char** argv)
{
    // after all processing, do we have what we need?
    return false;
}

int main(int argc, char** argv)
{
    // parse command line
    if (!parse_command_line(argc, argv))
        exit(1);
    // connect to IB
    ib_helper::IBConnector conn(host, port, 0);
    size_t timeout = 0;
    while(!conn.IsConnected() && timeout <= 5000)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        timeout += 100;
    }
    if (conn.IsConnected())
        std::cout << "Connected!\n";
    else
        std::cout << "Not connected\n";
    // wait for accounts
    // perform the action
    std::cout << "Hello, world!\n";
}
