#ifndef PURCHASE_H
#define PURCHASE_H

#include <QString>
#include <QDate>

struct Item{
    QString name;
    double price;

    Item(const QString& name = "", double price = 0) : name(name), price(price)
    {}

    bool operator<(const Item& other) const{
        return name < other.name;
    }
};



class Purchase
{
public:
    static QMap<Item, int> itemList;

    Purchase(Item item, int quantity, QDate date) :
        item(item), quantity(quantity), date(date) {}

    Item getItem() const;
    int getQuantity() const;
    QDate getDate() const;
    double getTotalPrice() const;

private:
    Item item;
    int quantity;
    QDate date;
};

#endif // PURCHASE_H
