#include <gtest/gtest.h>
#include <Orderbook.hpp>

TEST(OrderBookTest, test1)
{
    OrderBook orderbook;
    std::vector<Order> init;
    init.emplace_back(1, Side::BID, 100, 1);
    init.emplace_back(2, Side::BID, 99, 2);
    init.emplace_back(3, Side::BID, 98, 4);
    init.emplace_back(4, Side::BID, 97, 5);
    init.emplace_back(5, Side::ASK, 102, 1);
    init.emplace_back(6, Side::ASK, 103, 2);
    init.emplace_back(7, Side::ASK, 104, 4);
    init.emplace_back(8, Side::ASK, 105, 5);

    for (auto & order : init)
    {
        orderbook.openOrder(order);
    }

    for (auto & order : init)
    {
        auto ansOrder = orderbook.queryOrder(order.orderID);
        GTEST_ASSERT_EQ(ansOrder.has_value(), true);
        GTEST_ASSERT_EQ(ansOrder.value().orderID, order.orderID);
        GTEST_ASSERT_EQ(ansOrder.value().price, order.price);
        GTEST_ASSERT_EQ(ansOrder.value().quantity, order.quantity);
    }

    orderbook.cancelOrder(1);

    for (auto & order : init)
    {
        auto ansOrder = orderbook.queryOrder(order.orderID);
        if (order.orderID == 1)
        {
            GTEST_ASSERT_EQ(ansOrder.has_value(), false);
        }
        else
        {
            GTEST_ASSERT_EQ(ansOrder.has_value(), true);
            GTEST_ASSERT_EQ(ansOrder.value().orderID, order.orderID);
            GTEST_ASSERT_EQ(ansOrder.value().price, order.price);
            GTEST_ASSERT_EQ(ansOrder.value().quantity, order.quantity);
        }
    }


    auto modifyOrder = Order(2, Side::BID, 100.5, 10);
    orderbook.modifyOrder(modifyOrder);

    std::vector<Order> ans;
    ans.emplace_back(2, Side::BID, 100.5, 10);
    ans.emplace_back(3, Side::BID, 98, 4);
    ans.emplace_back(4, Side::BID, 97, 5);
    ans.emplace_back(5, Side::ASK, 102, 1);
    ans.emplace_back(6, Side::ASK, 103, 2);
    ans.emplace_back(7, Side::ASK, 104, 4);
    ans.emplace_back(8, Side::ASK, 105, 5);

    auto ansOrder = orderbook.queryOrder(1);
    GTEST_ASSERT_EQ(ansOrder.has_value(), false);
    for (auto & order : ans)
    {
        auto ansOrder = orderbook.queryOrder(order.orderID);

        GTEST_ASSERT_EQ(ansOrder.has_value(), true);
        GTEST_ASSERT_EQ(ansOrder.value().orderID, order.orderID);
        GTEST_ASSERT_EQ(ansOrder.value().price, order.price);
        GTEST_ASSERT_EQ(ansOrder.value().quantity, order.quantity);
    }


    modifyOrder = Order(2, Side::BID, 104.5, 10);
    orderbook.modifyOrder(modifyOrder);
    ansOrder = orderbook.queryOrder(5);
    GTEST_ASSERT_EQ(ansOrder.has_value(), false);
    ansOrder = orderbook.queryOrder(6);
    GTEST_ASSERT_EQ(ansOrder.has_value(), false);
    ansOrder = orderbook.queryOrder(7);
    GTEST_ASSERT_EQ(ansOrder.has_value(), false);

    ans.clear();
    ans.emplace_back(2, Side::BID, 104.5, 3);
    ans.emplace_back(3, Side::BID, 98, 4);
    ans.emplace_back(4, Side::BID, 97, 5);
    ans.emplace_back(8, Side::ASK, 105, 5);
    for (auto & order : ans)
    {
        auto ansOrder = orderbook.queryOrder(order.orderID);

        GTEST_ASSERT_EQ(ansOrder.has_value(), true);
        GTEST_ASSERT_EQ(ansOrder.value().orderID, order.orderID);
        GTEST_ASSERT_EQ(ansOrder.value().price, order.price);
        GTEST_ASSERT_EQ(ansOrder.value().quantity, order.quantity);
    }

    modifyOrder = Order(8, Side::ASK, 97, 10);
    orderbook.modifyOrder(modifyOrder);
    ansOrder = orderbook.queryOrder(5);
    GTEST_ASSERT_EQ(ansOrder.has_value(), false);
    ansOrder = orderbook.queryOrder(6);
    GTEST_ASSERT_EQ(ansOrder.has_value(), false);
    ansOrder = orderbook.queryOrder(7);
    GTEST_ASSERT_EQ(ansOrder.has_value(), false);
    ansOrder = orderbook.queryOrder(2);
    GTEST_ASSERT_EQ(ansOrder.has_value(), false);
    ansOrder = orderbook.queryOrder(3);
    GTEST_ASSERT_EQ(ansOrder.has_value(), false);
    ansOrder = orderbook.queryOrder(1);
    GTEST_ASSERT_EQ(ansOrder.has_value(), false);


    ans.clear();
    ans.emplace_back(4, Side::BID, 97, 2);
    for (auto & order : ans)
    {
        auto ansOrder = orderbook.queryOrder(order.orderID);

        GTEST_ASSERT_EQ(ansOrder.has_value(), true);
        GTEST_ASSERT_EQ(ansOrder.value().orderID, order.orderID);
        GTEST_ASSERT_EQ(ansOrder.value().price, order.price);
        GTEST_ASSERT_EQ(ansOrder.value().quantity, order.quantity);
    }

    //GTEST_ASSERT_EQ(ans.size(), 1000)
}