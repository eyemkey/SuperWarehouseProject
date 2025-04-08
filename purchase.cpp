#include "purchase.h"

QMap<Item, int> Purchase::itemList;

Item Purchase::getItem() const {
    return item;
}

int Purchase::getQuantity() const {
    return quantity;
}


QDate Purchase::getDate() const {
    return date;
}

double Purchase::getTotalPrice() const{
    return item.price * quantity;
}



