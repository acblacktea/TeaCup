#pragma once
#include <iostream>
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
    OrderBook() { orderIDToOrder.reserve(1000000); }

    bool openOrder(Order & order)
    {
        if (orderIDToOrder.contains(order.orderID))
        {
            return false;
        }

        match(order);

        // taker
        if (order.quantity == 0)
        {
            return true;
        }

        // maker
        auto & book = order.side == Side::BID ? bidPriceLevels : askPriceLevels;
        auto & priceLevel = book[order.price];
        priceLevel.emplace_back(order);
        orderIDToOrder[order.orderID] = --priceLevel.end();
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

        auto price = iter->price;
        book[price].erase(iter);
        if (book[price].empty())
        {
            book.erase(price);
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
    void match(Order & order)
    {
        auto & oppositeBook = order.side == Side::BID ? askPriceLevels : bidPriceLevels;
        while (order.quantity != 0 && !oppositeBook.empty())
        {
            auto oppositeBookIter = order.side == Side::ASK ? --oppositeBook.end() : oppositeBook.begin();
            auto & orders = oppositeBookIter->second;

            if ((order.side == Side::BID && orders.begin()->price > order.price)
                || (order.side == Side::ASK && orders.begin()->price < order.price))
            {
                break;
            }

            while (orders.begin() != orders.end())
            {
                auto iter = orders.begin();
                int quantity = std::min(iter->quantity, order.quantity);
                int price = iter->price;
                int buyOrderID = order.side == Side::BID ? order.orderID : iter->orderID;
                int sellOrderID = order.side == Side::ASK ? order.orderID : iter->orderID;
                historyTrades.emplace_back(buyOrderID, sellOrderID, price, quantity);
                order.quantity -= quantity;
                iter->quantity -= quantity;
                if (iter->quantity == 0)
                {
                    orderIDToOrder.erase(iter->orderID);
                    orders.erase(iter);
                }

                if (order.quantity == 0)
                {
                    break;
                }
            }


            if (orders.empty())
            {
                oppositeBook.erase(oppositeBookIter);
            }
        }
    }

    std::map<double, std::list<Order>> bidPriceLevels; // price -> order list;
    std::map<double, std::list<Order>> askPriceLevels;
    std::unordered_map<size_t, std::list<Order>::iterator> orderIDToOrder;
    std::vector<Trade> historyTrades;
};