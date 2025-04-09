#include "purchase.h"

QMap<Item, int> Purchase::itemList;


Purchase::Purchase(Item item, int quantity, QDate date) :
    item(item), quantity(quantity), date(date) {}



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

QString Purchase::toString() const{
    QString str = QString("%1 %2 %3\n")
                      .arg(item.name)
                      .arg(item.price)
                      .arg(quantity);

    return str;
}
