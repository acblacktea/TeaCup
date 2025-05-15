#pragma once
#include <list>
#include <map>
#include <optional>
#include <unordered_map>
#include <vector>
enum class Side
{
    BID,
    ASK,
};

class Order
{
public:
    size_t orderID;
    Side side;
    double price;
    double quantity;
};

class Trade
{
public:
    size_t buyOrderID;
    size_t sellOrderID;
    double price;
    double quantity;
};

class OrderBook
{
public:
    bool openOrder(Order & order)
    {
        auto & book = order.side == Side::BID ? bidPriceLevels : askPriceLevels;
        if (orderIDToOrder.contains(order.orderID))
        {
            return false;
        }

        auto & priceLevel = book[order.price];
        priceLevel.emplace_back(order);
        orderIDToOrder[order.orderID] = priceLevel.rbegin().base();
        match(order.orderID);
        return true;
    }

    bool cancelOrder(size_t orderID)
    {
        if (!orderIDToOrder.contains(orderID))
        {
            return false;
        }

        auto & iter = orderIDToOrder[orderID];
        auto & book = iter->side == Side::BID ? bidPriceLevels : askPriceLevels;

        book[iter->price].erase(iter);
        if (book[iter->price].empty())
        {
            book.erase(iter->price);
        }

        orderIDToOrder.erase(orderID);
        return true;
    }

    bool modifyOrder(Order & order)
    {
        // cancel
        bool successful = cancelOrder(order.orderID);
        if (!successful)
        {
            return false;
        }

        // re-insert order again
        openOrder(order);
        match(order.orderID);
        return true;
    }

    std::optional<Order> queryOrder(size_t orderID)
    {
        if (!orderIDToOrder.contains(orderID))
        {
            return std::nullopt;
        }

        return *orderIDToOrder[orderID];
    }

private:
    void match(size_t orderID)
    {
        auto & orderIter = orderIDToOrder[orderID];
        auto & oppositeBook = orderIter->side == Side::BID ? askPriceLevels : bidPriceLevels;
        while (orderIter->quantity != 0 && !oppositeBook.empty())
        {
            auto & orders = oppositeBook.begin()->second;
            if (orderIter->side == Side::BID && orders.begin()->price > orderIter->price)
            {
                break;
            }

            if (orderIter->side == Side::ASK && orders.begin()->price < orderIter->price)
            {
                break;
            }

            while (orders.begin() != orders.end())
            {
                auto iter = orders.begin();
                int quantity = std::min(iter->quantity, orderIter->quantity);
                int price = iter->price;
                int buyOrderID = orderIter->side == Side::BID ? orderIter->orderID : iter->orderID;
                int sellOrderID = orderIter->side == Side::ASK ? orderIter->orderID : iter->orderID;
                historyTrades.emplace_back(buyOrderID, sellOrderID, price, quantity);

                orderIter->quantity -= quantity;
                iter->quantity -= quantity;
                if (iter->quantity == 0)
                {
                    orders.erase(orders.begin());
                }

                if (orderIter->quantity == 0)
                {
                    break;
                }
            }

            if (oppositeBook.begin()->second.empty())
            {
                oppositeBook.erase(oppositeBook.begin());
            }
        }


        if (orderIter->quantity == 0)
        {
            cancelOrder(orderID);
        }
    }

    std::map<double, std::list<Order>> bidPriceLevels; // price -> order list;
    std::map<double, std::list<Order>> askPriceLevels;
    std::unordered_map<size_t, std::list<Order>::iterator> orderIDToOrder;
    std::vector<Trade> historyTrades;
};