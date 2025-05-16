#include <iostream>
#include <optional>
#include <unordered_map>
#include <vector>
#include <gtest/gtest.h>
using namespace std;
/*
class OrderBook {
public:
    std::unordered_map<int, std::pair<int, int>> orderIDToPriceRef;//
    std::map<int, std::unordered_map<int, int>, std::greater<int>> priceToRefCount;
};
*/

const size_t MAXCOUNT = 1000007;

class Node
{
public:
    bool hasOrder = false;
    int frontID = -1;
    int hasOnlyOneFrontID = false;
    std::unordered_map<int, int> fronts;
};


class SegmentTree
{
public:
    SegmentTree()
    {
        nodes = new Node[MAXCOUNT * 3];
        std::cout << "successfully" << std::endl;
        build(1, 1, MAXCOUNT);
        std::cout << "successfully" << std::endl;
    }

    ~SegmentTree() { delete[] nodes; }

    void build(int nodeIndex, int l, int r)
    {
        if (l == r)
        {
            price2NodeIndex[l] = nodeIndex;
            return;
        }

        auto mid = (l + r) / 2;
        build(nodeIndex * 2, l, mid);
        build(nodeIndex * 2 + 1, mid + 1, r);
    }

    void addOrder(int orderID, int price, int front)
    {
        orderId2Price[orderID] = price;
        orderId2Front[orderID] = front;

        auto nodeIndex = price2NodeIndex[price];
        ++nodes[nodeIndex].fronts[front];
        nodes[nodeIndex].hasOrder = true;

        if (nodes[nodeIndex].fronts.size() == 1)
        {
            nodes[nodeIndex].hasOnlyOneFrontID = true;
            nodes[nodeIndex].frontID = front;
        }
        else if (nodes[nodeIndex].fronts.size() > 1)
        {
            nodes[nodeIndex].hasOnlyOneFrontID = false;
        }

        pushUp(nodeIndex / 2);
    }

    void cancelOrders(int orderID)
    {
        if (orderId2Price.find(orderID) == orderId2Price.end())
        {
            return;
        }

        int price = orderId2Price[orderID];
        orderId2Price.erase(orderID);
        int front = orderId2Front[orderID];
        orderId2Front.erase(orderID);

        auto nodeIndex = price2NodeIndex[price];
        --nodes[nodeIndex].fronts[front];
        if (nodes[nodeIndex].fronts[front] == 0)
        {
            nodes[nodeIndex].fronts.erase(front);
        }

        nodes[nodeIndex].hasOrder = true;
        if (nodes[nodeIndex].fronts.size() == 1)
        {
            nodes[nodeIndex].hasOnlyOneFrontID = true;
            nodes[nodeIndex].frontID = front;
        }
        else if (nodes[nodeIndex].fronts.size() == 0)
        {
            nodes[nodeIndex].hasOnlyOneFrontID = false;
            nodes[nodeIndex].hasOrder = false;
            nodes[nodeIndex].frontID = -1;
        }

        pushUp(nodeIndex / 2);
    }


    int query(int nodeIndex, int l, int r, int left, int right)
    {
        // std::cout << l << " " << r << " " << left << " " << right << std::endl;
        if (l == left && r == right)
        {
            if (!nodes[nodeIndex].hasOrder)
            {
                return -1;
            }
            else if (nodes[nodeIndex].hasOnlyOneFrontID)
            {
                return nodes[nodeIndex].frontID;
            }
            else
            {
                return 0;
            }
        }

        auto mid = (left + right) / 2;
        if (l >= mid + 1)
        {
            return query(nodeIndex * 2 + 1, l, r, mid + 1, right);
        }
        if (r <= mid)
        {
            return query(nodeIndex * 2, l, r, left, mid);
        }
        else
        {
            auto leftValue = query(nodeIndex * 2, l, mid, left, mid);
            auto rightValue = query(nodeIndex * 2 + 1, mid + 1, r, mid + 1, right);
            if (leftValue == rightValue)
            {
                return leftValue;
            }

            if (leftValue == -1 && rightValue != 0)
            {
                return rightValue;
            }

            if (rightValue == -1 && leftValue != 0)
            {
                return leftValue;
            }


            return 0;
        }
    }

    void pushUp(int nodeIndex)
    {
        if (nodeIndex <= 0)
        {
            return;
        }

        auto leftChildIndex = nodeIndex * 2;
        auto rightChildIndex = nodeIndex * 2 + 1;
        if (nodes[leftChildIndex].hasOnlyOneFrontID && nodes[rightChildIndex].hasOnlyOneFrontID
            && nodes[leftChildIndex].frontID == nodes[rightChildIndex].frontID)
        {
            nodes[nodeIndex].hasOnlyOneFrontID = true;
            nodes[nodeIndex].frontID = nodes[rightChildIndex].frontID;
        }

        if (nodes[leftChildIndex].hasOnlyOneFrontID && !nodes[rightChildIndex].hasOrder)
        {
            nodes[nodeIndex].hasOnlyOneFrontID = true;
            nodes[nodeIndex].frontID = nodes[leftChildIndex].frontID;
        }

        if (nodes[rightChildIndex].hasOnlyOneFrontID && !nodes[leftChildIndex].hasOrder)
        {
            nodes[nodeIndex].hasOnlyOneFrontID = true;
            nodes[nodeIndex].frontID = nodes[rightChildIndex].frontID;
        }

        nodes[nodeIndex].hasOrder = nodes[leftChildIndex].hasOrder || nodes[leftChildIndex].hasOrder;

        pushUp(nodeIndex / 2);
    }

    Node * nodes = nullptr;
    std::unordered_map<int, int> orderId2Price;
    std::unordered_map<int, int> orderId2Front;
    std::unordered_map<int, int> price2NodeIndex;
};

class Solution
{
public:
    /**
     * 代码中的类名、方法名、参数名已经指定，请勿修改，直接返回方法规定的值即可
     *
     * 
     * @param quote_orders int整型vector<vector<>> 每个挂单用一个长度为4的vec表示[time,ref,price,front]
     * @param cancel_orders int整型vector<vector<>> 每个撤单用一个长度为2的vec表示[time, ref]
     * @param query int整型vector<vector<>> 每个查询用一个长度为2的vec表示 [time, price]
     * @return bool布尔型vector
     */
    vector<bool> solve(vector<vector<int>> & quote_orders, vector<vector<int>> & cancel_orders, vector<vector<int>> & query)
    {
        std::vector<bool> ans;
        int quoteIndex = 0;
        int cancelOrdersIndex = 0;
        int queryIndex = 0;

        SegmentTree tree;
        while (quoteIndex < quote_orders.size() || cancelOrdersIndex < cancel_orders.size() || queryIndex < query.size())
        {
            std::cout << "start" << std::endl;
            int quoteTime = quoteIndex >= static_cast<int>(quote_orders.size()) ? 10000000 : quote_orders[quoteIndex][0];
            int cancelTime = cancelOrdersIndex >= static_cast<int>(cancel_orders.size()) ? 10000000 : cancel_orders[cancelOrdersIndex][0];
            int queryTime = queryIndex >= static_cast<int>(query.size()) ? 10000000 : query[queryIndex][0];

            std::cout << quoteTime << " " << cancelTime << " " << queryTime << std::endl;
            if (quoteTime != -1 && quoteTime <= cancelTime && quoteTime <= queryTime)
            {
                std::cout << "add order" << std::endl;
                tree.addOrder(quote_orders[quoteIndex][1], quote_orders[quoteIndex][2], quote_orders[quoteIndex][3]);
                ++quoteIndex;
            }
            else if (cancelTime != -1 && cancelTime <= quoteTime && cancelTime <= queryTime)
            {
                std::cout << "cancel order" << std::endl;
                tree.cancelOrders(cancel_orders[cancelOrdersIndex][1]);
                ++cancelOrdersIndex;
            }
            else if (queryTime != -1 && queryTime <= cancelTime && queryTime <= quoteTime)
            {
                std::cout << "query order" << std::endl;
                auto value = tree.query(1, 1, query[queryIndex][1], 1, MAXCOUNT);
                if (value != 0)
                {
                    ans.emplace_back(true);
                }
                else
                {
                    ans.emplace_back(false);
                }

                ++queryIndex;
            }
        }

        return ans;
        // write code here
    }

private:
};

TEST(interviewTest, test1)
{
    Solution sol;
    //[[1,1,1,1], [3,2,2,2]],[],[[2,2],[4,2]]
    std::vector<std::vector<int>> quote = {{1, 1, 1, 1}, {3, 2, 2, 2}};
    std::vector<std::vector<int>> cancel = {};
    std::vector<std::vector<int>> query = {{2, 2}, {4, 2}};
    sol.solve(quote, cancel, query);
}