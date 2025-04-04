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

    Purchase::itemList[purchase.getItem()] += purchase.getQuantity();
}

QVector<Purchase> Member::getPurchaseOnDate(const Date& date) const{
    return purchases[date];
}


QVector<Purchase> Member::getPurchaseOnYear(int year) const {
    Date startingDate(1, 1, year);
    Date endingDate(31, 12, year);
    QVector<Purchase> purchasesInYear;
    for(const Date& date : purchases.keys()){
        if(date >= startingDate && date <= endingDate){
            for(auto e : purchases[date]){
                purchasesInYear.push_back(e);
            }
        }
    }
    return purchasesInYear;
}

QVector<Purchase> Member::getAllPurchases() const {
    QVector<Purchase> allPurchases;
    for(auto it = purchases.constBegin(); it != purchases.constEnd(); ++it){
        for(auto purchase : it.value()){
            allPurchases.push_back(purchase);
        }
    }
    return allPurchases;
}

QString Member::toString() const {/*
    QString n = name.leftJustified(20, ' ');
    QString id = QString(id).leftJustified(10, ' ');
    QString type = getTypeAsString().leftJustified(12, ' ');
    QString date = expiryDate.leftJustified(12, ' ');

    return n + id + type + date + QString::number(getRebate());*/

    return QString("%1, %2, %3, %4, $%5, Rebate: $%6")
        .arg(name)
        .arg(id)
        .arg(getTypeAsString())
        .arg(expiryDate)
        .arg(totalSpent)
        .arg(getRebate());
}
































