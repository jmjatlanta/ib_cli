#include <iostream>
#include <vector>
#include <string>

#include "IBConnector.hpp"

std::vector<std::string> availableAccounts;
std::vector<std::string> accounts;
std::vector<std::string> tickers;
std::string host = "localhost";
int port = 4007;
int orderId = -1;
bool allOrders = false;
std::string action = "";

bool parse_command_line(int argc, char** argv)
{
    for(int i = 1; i < argc; ++i)
    {
        std::string param = argv[i];
        if (param[0] == '-')
        {
            bool valid = false;
            if (param == "-ticker")
            {
                tickers.push_back(argv[i+1]);
                valid = true;
                i++;
            }
            if (param == "-id")
            {
                orderId = strtol(argv[i+1], nullptr, 10);
                valid = true;
                i++;
            }
            if (param == "-all")
            {
                allOrders = true;
                valid = true;
            }
            if (param == "-account")
            {
                accounts.push_back(argv[i+1]);
                valid = true;
                i++;
            }
            if (param == "-port")
            {
                port = strtol(argv[i+1], nullptr, 10);
                valid = true;
                i++;
            }
            if (param == "-host")
            {
                host = argv[i+1];
                valid = true;
                i++;
            }
            if (!valid)
            {
                std::cerr << "Unknown parameter: " + param << "\n";
                return false;
            }
        }
        else // no dash prefix
        {
            if (param == "show"
                    || param == "cancel"
                    || param == "flatten")
                action = param;
            else
            {
                std::cerr << "Unknown action: " << param << "\n";
                return false;
            }
        }
    }
    // after all processing, do we have what we need?
    return true;
}

void print_syntax(int argc, char** argv)
{
    std::cout << "Syntax: " << argv[0] << " show|cancel|flatten [-ticker ticker] [-id orderId] [-all] [-account U12345] [-port 4001] [-host 192.168.0.1]\n";
}

class AccountMonitor : public ib_helper::AccountHandler
{
public:
    AccountMonitor() : ib_helper::AccountHandler()
    {
    }
    void OnAccountValueUpdate(const std::string& key, const std::string& value, const std::string& currency,
            const std::string& accountName) override
    {
        std::cout << "Account " << accountName << " value: " << value << " " << currency << "\n";
    }
    void OnPortfolioUpdate(const Contract& contract, Decimal position, double marketPrice, double marketValue,
            double averageCost, double unrealizedPNL, double realizedPNL, const std::string& accountName) override
    {
        std::cout << decimalToDouble(position) << " " << contract.symbol << " on account " << accountName << "\n";
    }
    void OnUpdateAccountTime(const std::string& timestamp) override
    {
    }
    void OnAccountDownloadEnd(const std::string& accountName) override
    {
        std::cout << "Account downloade ended for " << accountName << "\n";
    }
    void OnPosition(const std::string& account, const Contract& contract, Decimal pos, double averageCost) override
    {
        std::cout << account << " " << decimalToDouble(pos) << " " << contract.symbol << "\n";
    }
    void OnPositionEnd() override
    {
        std::cout << "End of positions\n";
        accountComplete = true;
    }
    void OnCurrentTime(long currentTime) override
    {
    }
    void OnError(int id, int errorCode, const std::string& errorMessage, 
            const std::string& advancedOrderRejectJson) override
    {
        std::cerr << "AccountMonitor received an error: " << id << " " << errorCode << " " << errorMessage << " " << advancedOrderRejectJson << "\n";
    }
    bool accountComplete = false;
};

class OrderMonitor : public ib_helper::OrderHandler
{
public:
    OrderMonitor() : ib_helper::OrderHandler()
    {
    }
    void OnOpenOrder(int orderId, const Contract& contract, const ::Order& order, 
            const OrderState& orderState) override
    {
        std::cout << "Id: " << orderId << " " << contract.symbol 
                << "Total: " << decimalToDouble(order.totalQuantity)
                << "Filed: " << decimalToDouble(order.filledQuantity)
                << "Status: " << orderState.status
                << "\n";
    }
    void OnOrderStatus(int orderId, const std::string& status, Decimal filled, Decimal remaining,
            double avgFillPrice, int permId, int parentId, double lastFillPrice, int clientId,
            const std::string& whyHeld, double mktCapPrice) override
    {
    }
    void OnOpenOrderEnd() override
    {
        std::cout << "End of orders\n";
        orderComplete = true;
    }
    void OnOrderBound(long orderId, int apiClientId, int apiOrderId) override
    {
    }
    bool orderComplete = false;
};

void perform_action(ib_helper::IBConnector& conn)
{
    if (action == "show")
    {
        std::cout << "Default account: " << conn.GetDefaultAccount() << "\n";
        std::cout << "Positions:\n";
        AccountMonitor accountMonitor;
        conn.AddAccountHandler(&accountMonitor);
        conn.RequestPositions();
        size_t timeout = 0;
        while(!accountMonitor.accountComplete && timeout < 3000)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            timeout += 100;
        }
        conn.RemoveAccountHandler(&accountMonitor);
        OrderMonitor orderMonitor;
        conn.AddOrderHandler(&orderMonitor);
        conn.RequestOpenOrders();
        timeout = 0;
        while(!orderMonitor.orderComplete && timeout < 3000)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            timeout += 100;
        }
        conn.RemoveOrderHandler(&orderMonitor);
    }
}

int main(int argc, char** argv)
{
    // parse command line
    if (!parse_command_line(argc, argv))
    {
        print_syntax(argc, argv);
        exit(1);
    }
    // connect to IB
    ib_helper::IBConnector conn(host, port, 0);
    size_t timeout = 0;
    while(!conn.IsConnected() && timeout <= 5000)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        timeout += 100;
    }
    if (conn.IsConnected())
        perform_action(conn);
    else
        std::cout << "Not connected\n";
}
