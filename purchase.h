#ifndef PURCHASE_H
#define PURCHASE_H

#include <QString>
#include <QDate>

struct Item{
    QString name;
    double price;

    Item(const QString& name = "", double price = 0) : name(name), price(price)
    {}

    Item(const Item& other) {
        this->name = other.name;
        this->price = other.price;
    }

    bool operator<(const Item& other) const{
        return name < other.name;
    }
};


class Purchase
{
public:
    static QMap<Item, int> itemList;

    Purchase(Item item, int quantity, QDate date);

    Item getItem() const;
    int getQuantity() const;
    QDate getDate() const;
    double getTotalPrice() const;
    QString toString() const;

private:
    Item item;
    int quantity;
    QDate date;
};

#endif // PURCHASE_H
