#include "member.h"
#include <iostream>


Member::Member(QString name,
               int id,
               MembershipType type,
               QString expiryDate,
               double totalSpent):
        name(name),
        id(id),
        type(type),
        expiryDate(expiryDate),
        totalSpent(totalSpent) {}

QString Member::getName() const {
    return name;
}

int Member::getId() const {
    return id;
}

QString Member::getExpiryDate() const {
    return expiryDate;
}

double Member::getTotalSpent() const {
    return totalSpent;
}

Member::MembershipType Member::getType() const {
    return type;
}

QString Member::getTypeAsString() const {
    return (type == BASIC) ? "Basic" : "Preferred";
}

double Member::getRebate() const {
    return (type == PREFERRED) ? totalSpent / (1+SALES_TAX) * 0.05 : 0.0;
}

// 🔹 Setters for updating members
void Member::setName(const QString &newName){
    name = newName;
}

void Member::setExpiryDate(const QString &newExpiry) {
    expiryDate = newExpiry;
}

void Member::setType(MembershipType newType) {
    type = newType;
}

void Member::setTotalSpent(double amount) {
    totalSpent = amount;
}

void Member::addPurchase(const Purchase& purchase) {
    Date date = purchase.getDate();
    purchases[date].push_back(purchase);

    totalSpent += purchase.getTotalPrice() * (1+SALES_TAX);
}

QVector<Purchase> Member::getPurchaseOnDate(const Date& date) const{
    return purchases[date];
}


QString Member::toString() const {
    return QString("%1, %2, %3, %4, $%5, Rebate: $%6")
        .arg(name)
        .arg(id)
        .arg(getTypeAsString())
        .arg(expiryDate)
        .arg(totalSpent)
        .arg(getRebate());
}
































