#include "purchase.h"


QString Purchase::getItem() const {
    return item;
}

int Purchase::getQuantity() const {
    return quantity;
}

double Purchase::getPrice() const {
    return price;
}

Date Purchase::getDate() const {
    return date;
}

double Purchase::getTotalPrice() const{
    return price * quantity;
}



