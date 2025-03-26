#ifndef PURCHASE_H
#define PURCHASE_H

#include <QString>
#include "date.h"

class Purchase
{
public:
    Purchase(QString item, int quantity, double price, Date date) :
        item(item), quantity(quantity), price(price), date(date) {}

    QString getItem() const;
    int getQuantity() const;
    double getPrice() const;
    Date getDate() const;
    double getTotalPrice() const;

private:
    QString item;
    int quantity;
    double price;
    Date date;
};

#endif // PURCHASE_H
