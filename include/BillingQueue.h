#pragma once

struct BillRequest {
    int productId;
    int quantity;
};

struct QueueNode {
    BillRequest data;
    QueueNode* next;
};

class BillingQueue {
private:
    QueueNode* frontNode;
    QueueNode* rearNode;
    int count;

public:
    BillingQueue();

    void enqueue(const BillRequest& request);
    bool dequeue(BillRequest& request);
    bool front(BillRequest& request) const;
    bool empty() const;
    int size() const;
    void clear();

    int getRequestedQuantity(int productId) const;
    bool getItem(int index, BillRequest& request) const;
};
