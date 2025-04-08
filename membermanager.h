#ifndef MEMBERMANAGER_H
#define MEMBERMANAGER_H

#include "member.h"
#include <QMap>
#include <QFile>
#include <QTextStream>
#include <QVector>

class MemberManager {
public:
    static QSet<QString> processedFiles;

    void addMember(const Member &member);
    bool removeMember(int id);
    bool containsMember(int id);

    Member* searchMember(int id, QSet<Member::MembershipType> includedTypes);
    Member* searchMember(const QString& name, QSet<Member::MembershipType> includedTypes) const;


    QVector<Member> getAllMembers(QSet<Member::MembershipType> includedTypes) const;


    void saveToFile(const QString &filename);
    void loadFromFile(const QString &filename);

    // 🔹 New methods for processing sales and rebates
    void loadShoppersFile(QFile& file);
    void loadShoppersFile(const QString& filename);
    void loadShoppersFileFormatted();

    void processSalesFile(const QString &filename);
    void calculateRebates();
    // void displayRebates(QWidget *parent);
    QString getRebates() const;

    QString generateYearReport(int year, QSet<Member::MembershipType> includedTypes) const;
    QString generateDailyReport(const QDate& date, QSet<Member::MembershipType> includedTypes) const;
    QString generateTotalPurchaseReport(QSet<Member::MembershipType> includedTypes) const;
    QString generateYearlyDuesReport(QSet<Member::MembershipType> includedTypes) const;
    QVector<Member> getMembersShouldUpgrade() const;
    QVector<Member> getMembersShouldDowngrade() const;
    QVector<Member> getExpiringMembers(int month, int year, QSet<Member::MembershipType> includedTypes) const;


private:
    QMap<int, Member> members;  // Use QMap for quick lookups by ID
    QMap<int, double> totalSpent; // Track total spending per member

};



#endif // MEMBERMANAGER_H
