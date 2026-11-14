#include "BillingQueue.h"

BillingQueue::BillingQueue() {
    frontNode = nullptr;
    rearNode = nullptr;
    count = 0;
}

void BillingQueue::enqueue(const BillRequest& request) {
    QueueNode* node = new QueueNode;
    node->data = request;
    node->next = nullptr;

    if (rearNode == nullptr) {
        frontNode = node;
        rearNode = node;
    } else {
        rearNode->next = node;
        rearNode = node;
    }
    count++;
}

bool BillingQueue::dequeue(BillRequest& request) {
    if (frontNode == nullptr) return false;

    QueueNode* node = frontNode;
    request = node->data;
    frontNode = node->next;
    if (frontNode == nullptr) {
        rearNode = nullptr;
    }
    delete node;
    count--;
    return true;
}

bool BillingQueue::front(BillRequest& request) const {
    if (frontNode == nullptr) return false;
    request = frontNode->data;
    return true;
}

bool BillingQueue::empty() const {
    return frontNode == nullptr;
}

int BillingQueue::size() const {
    return count;
}

void BillingQueue::clear() {
    BillRequest ignored;
    while (dequeue(ignored)) {
    }
}

int BillingQueue::getRequestedQuantity(int productId) const {
    int total = 0;
    for (QueueNode* n = frontNode; n != nullptr; n = n->next) {
        if (n->data.productId == productId) {
            total = total + n->data.quantity;
        }
    }
    return total;
}

bool BillingQueue::getItem(int index, BillRequest& request) const {
    if (index < 0) return false;
    QueueNode* n = frontNode;
    for (int i = 0; i < index && n != nullptr; i++) {
        n = n->next;
    }
    if (n == nullptr) return false;
    request = n->data;
    return true;
}
