#ifndef MEMBERMANAGER_H
#define MEMBERMANAGER_H

#include "member.h"
#include <QMap>
#include <QFile>
#include <QTextStream>
#include <QVector>

class MemberManager {
public:
    void addMember(const Member &member);
    bool removeMember(int id);

    Member* searchMember(int id);
    Member* searchMember(const QString& name) const;
    QVector<Member> getAllMembers() const;

    void saveToFile(const QString &filename);
    void loadFromFile(const QString &filename);

    // 🔹 New methods for processing sales and rebates
    void loadShoppersFile(const QString &filename);
    void processSalesFile(const QString &filename);
    void calculateRebates();
    // void displayRebates(QWidget *parent);
    QString getRebates() const;

    QString generateYearReport(int year) const;
    QString generateDailyReport(const Date& date) const;
    QString generateTotalPurchaseReport() const;
    QString generateYearlyDuesReport() const;
    QVector<Member> getMembersShouldUpgrade() const;
    QVector<Member> getMembersShouldDowngrade() const;

private:
    QMap<int, Member> members;  // Use QMap for quick lookups by ID
    QMap<int, double> totalSpent; // Track total spending per member
};



#endif // MEMBERMANAGER_H
