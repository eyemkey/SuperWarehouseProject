#ifndef MEMBER_H
#define MEMBER_H

#include <QString>
#include <QMap>
#include <QVector>
#include "date.h"
#include "purchase.h"

const double SALES_TAX = 0.0875;
const double REBATE_RATE = 0.05;
const double PREFERRED_PRICE = 75.00;
const double BASIC_PRICE = 60.00;

class Member {
public:
    enum MembershipType { BASIC, PREFERRED };

    Member() : id(0), type(BASIC), totalSpent(0.0) {}

    Member(QString name,
           int id,
           MembershipType type,
           Date expiryDate,
           double totalSpent = 0.0);

    QString getName() const;
    int getId() const;
    MembershipType getType() const;
    QString getTypeAsString() const;
    Date getExpiryDate() const;

    bool isPreferred() const;
    double getTotalSpent() const;
    double getRebate() const;
    double getSavings() const;
    double getDues() const;


    // 🔹 Setters for updating member details
    void setName(const QString &newName);
    void setExpiryDate(const Date &newExpiry);
    void setType(MembershipType newType);
    void setTotalSpent(double amount);


    void addPurchase(const Purchase& purchase);
    QVector<Purchase> getPurchaseOnDate(const Date& date) const;
    QVector<Purchase> getPurchaseOnYear(int year) const;
    QVector<Purchase> getAllPurchases() const;
    QString toString() const;

private:
    QString name;
    int id;
    MembershipType type;
    Date expiryDate;
    double totalSpent;

    QMap<Date, QVector<Purchase>> purchases;
};

#endif // MEMBER_H
