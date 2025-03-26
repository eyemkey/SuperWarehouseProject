#ifndef MEMBERMANAGER_H
#define MEMBERMANAGER_H

#include "member.h"
#include <QMap>
#include <QFile>
#include <QTextStream>

class MemberManager {
public:
    void addMember(const Member &member);
    bool removeMember(int id);
    Member* searchMember(int id);
    QList<Member> getAllMembers() const;

    void saveToFile(const QString &filename);
    void loadFromFile(const QString &filename);

    // 🔹 New methods for processing sales and rebates
    void loadShoppersFile(const QString &filename);
    void processSalesFile(const QString &filename);
    void calculateRebates();
    void displayRebates(QWidget *parent);

    // void generateYearReport(int year) const;
    void generateDailyReport(const Date& date) const;

private:
    QMap<int, Member> members;  // Use QMap for quick lookups by ID
    QMap<int, double> totalSpent; // Track total spending per member
};



#endif // MEMBERMANAGER_H
