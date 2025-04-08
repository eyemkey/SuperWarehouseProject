#include "membermanager.h"
#include <QMessageBox>
#include <QDebug>
#include <iostream>
#include <QDir>
#include <QCoreApplication>
#include <tuple>
#include "utility.h"

QSet<QString> MemberManager::processedFiles;

void MemberManager::addMember(const Member &member) {
    members[member.getId()] = member;
}

bool MemberManager::removeMember(int id) {
    return members.remove(id) > 0;
}

bool MemberManager::containsMember(int id){
    return members.contains(id);
}

Member* MemberManager::searchMember(int id, QSet<Member::MembershipType> includedTypes) {
    if (members.contains(id)) {
        if(includedTypes.contains(members[id].getType())){
            return &members[id];
        }
    }
    return nullptr;
}

Member* MemberManager::searchMember(const QString& name, QSet<Member::MembershipType> includedTypes) const {
    for(auto member: members){
        if(!includedTypes.contains(member.getType())){
            continue;
        }

        if(member.getName() == name){
            return &member;
        }
    }
    return nullptr;
}

QVector<Member> MemberManager::getAllMembers(QSet<Member::MembershipType> includedTypes) const {
    QVector<Member> allMembers;
    for(auto member: members){
        if(includedTypes.contains(member.getType())){
            allMembers.push_back(member);
        }
    }
    return allMembers;
}

void MemberManager::saveToFile(const QString &filename) {
    QFile file(filename);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qDebug() << "Error: Cannot open file for writing!";
        return;
    }

    QTextStream out(&file);
    out << "Name, ID, Type, Expiry Date, Total Spent\n";

    for (const Member &m : members) {
        out << m.getName() << ", " << m.getId() << ", " << m.getTypeAsString() << ", "
            << m.getExpiryDate().toString() << ", " << m.getTotalSpent() << "\n";
    }
    file.close();
}

void MemberManager::loadFromFile(const QString &filename) {
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "Error: Cannot open file for reading!";
        return;
    }

    QTextStream in(&file);
    members.clear();
    if (!in.atEnd()) in.readLine(); // Skip header row

    while (!in.atEnd()) {
        QString line = in.readLine();
        QStringList parts = line.split(",");
        if (parts.size() < 5) continue;

        QString name = parts[0];
        int id = parts[1].toInt();
        Member::MembershipType type = (parts[2] == "Basic") ? Member::BASIC : Member::PREFERRED;
        QString expiryDate = parts[3];
        double spent = parts[4].toDouble();

        members.insert(id, Member(name, id, type, QDate::fromString(expiryDate, "MM/dd/yyyy"), spent));

        // 🔹 Remove redundant initialization:
        // totalSpent[id] = 0.0;
        totalSpent[id] = spent;  // Correctly store total spent
    }
    file.close();
}


// 🔹 Load warehouse shoppers data
void MemberManager::loadShoppersFile(QFile& file) {

    QTextStream in(&file);

    while (!in.atEnd()) {
        QString name = in.readLine();
        int id = in.readLine().toInt();
        QString typeStr = in.readLine();
        QString expiry = in.readLine();

        Member::MembershipType type = (typeStr == "Preferred") ? Member::PREFERRED : Member::BASIC;
        members[id] = Member(name, id, type, QDate::fromString(expiry, "MM/dd/yyyy"));
        totalSpent[id] = 0.0; // Initialize spending to 0
    }

}

void MemberManager::loadShoppersFile(const QString& filename) {
    QFile file(filename);

    if(!file.open(QIODevice::ReadOnly | QIODevice::Text)){
        qDebug() << "Error: Cannot open file for reading!";
        return;
    }

    loadShoppersFile(file);
}

void MemberManager::loadShoppersFileFormatted() {
    QString currentMembersDir = GET_MEMBERS_FILE_DIRECTORY();
    QFile currentMembers(currentMembersDir);

    if(!currentMembers.open(QIODevice::ReadOnly | QIODevice::Text)){
        qDebug() << "Failed to open current Members file";
        return;
    }

    QTextStream in(&currentMembers);

    while(!in.atEnd()){
        QString line = in.readLine().trimmed();

        if(line.isEmpty()) continue;

        QVector<QString> parts = line.split(", ");

        if(parts.size() != 4){
            qDebug() <<"Invalid line format, skipping:"<<line;
            continue;
        }
        std::cout<<line.toStdString()<<std::endl;
        std::cout<<parts[3].toStdString()<<std::endl;

        int id = parts[0].toInt();
        QString name = parts[1];
        QString typeStr = parts[2];
        QString expiryStr = parts[3];

        QDate date = QDate::fromString(expiryStr, "MM/dd/yyyy");
        std::cout<<date.toString("MM/dd/yyyy").toStdString()<<std::endl;

        Member::MembershipType type = (typeStr == "Preferred") ? Member::PREFERRED : Member::BASIC;
        Member m(name, id, type, QDate::fromString(expiryStr, "MM/dd/yyyy"));

        members[id] = m;
        totalSpent[id] = 0;
    }
}


// 🔹 Process purchases from day1.txt
void MemberManager::processSalesFile(const QString &filename) {

    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "Error: Cannot open sales file!";
        return;
    }

    QTextStream in(&file);
    while (!in.atEnd()) {
        QString date = in.readLine();  // Read transaction date

        int id = in.readLine().toInt(); // Read customer ID

        QString itemName = in.readLine();
        QStringList priceQuantity = in.readLine().split("\t");

        if (priceQuantity.size() < 2) continue;

        double price = priceQuantity[0].toDouble();
        int quantity = priceQuantity[1].toInt();

        if(containsMember(id)){
            QDate d = QDate::fromString(date, "MM/dd/yyyy");
            Purchase purchase(Item(itemName, price), quantity, QDate::fromString(date, "MM/dd/yyyy"));
            members[id].addPurchase(purchase);
        }
    }
    file.close();
}

// 🔹 Calculate 5% rebate for Preferred Members
void MemberManager::calculateRebates() {
    for (auto id : members.keys()) {
        if (members[id].getType() == Member::PREFERRED) {
            double rebate = totalSpent[id] * 0.05;  // 5% rebate
            members[id].setTotalSpent(totalSpent[id]); // Store total spent
            qDebug() << "Member:" << members[id].getName() << "Rebate: $" << rebate;
        }
    }
}

// 🔹 Display rebate results in a Qt Message Box
// void MemberManager::displayRebates(QWidget *parent) {
//     QString result = "Rebate Summary for Preferred Members:\n";

//     for (auto id : members.keys()) {
//         if (members[id].getType() == Member::PREFERRED) {
//             double rebate = totalSpent[id] * 0.05;
//             result += QString("%1 (ID: %2): $%3\n")
//                           .arg(members[id].getName())
//                           .arg(id)
//                           .arg(rebate, 0, 'f', 2);
//         }
//     }

//     QMessageBox::information(parent, "Rebate Report", result);
// }

QString MemberManager::getRebates() const{
    QString result = "Rebate Summary for Preferred members:\n";

    for(auto id : members.keys()){
        if(members[id].getType() == Member::PREFERRED){
            double rebate = totalSpent[id] * 0.05;
            result += QString("%1 (ID: %2): $%3\n")
                                      .arg(members[id].getName())
                                      .arg(id)
                                      .arg(rebate, 0, 'f', 2);
        }
    }

    return result;
}

QString MemberManager::generateDailyReport(const QDate& date, QSet<Member::MembershipType> includedTypes) const{
    int prefferedCount = 0, basicCount = 0;
    QMap<Item, int> itemList;
    QVector<QString> customerNames;
    double totalRevenue = 0;

    for(auto member: members){
        if(!includedTypes.contains(member.getType())){
            continue;
        }

        QVector<Purchase> memberPurchase = member.getPurchaseOnDate(date);
        if(!memberPurchase.empty()){
            customerNames.push_back(member.getName());
            if(member.getType() == Member::PREFERRED){
                prefferedCount++;
            }else{
                basicCount++;
            }
        }
        for(auto purchase : memberPurchase){
            itemList[purchase.getItem()] += purchase.getQuantity();
            totalRevenue += purchase.getTotalPrice();
        }
    }


    QString reportString = "";

    for(auto item : itemList.keys()){
        reportString += QString("%1%2\n\n")
                    .arg(item.name, -40)
                    .arg(itemList[item], 10);
    }

    reportString += "Customers that Shopped: \n";

    for(auto e: customerNames){
        reportString += QString("%1\n").arg(e);
    }
    reportString += QString("\nPreferred Customers: %1\n").arg(prefferedCount);
    reportString += QString("Basic Customers: %1\n").arg(basicCount);

    return reportString;
}

QString MemberManager::generateYearReport(int year, QSet<Member::MembershipType> includedTypes) const {
    double totalRevenue = 0;
    QMap<Item, int> itemList;
    QPair<Item, int> bestSelling(Item(), 0);
    QPair<Item, int> worstSelling(Item(), 0);
    
    for(auto member : members){
        if(!includedTypes.contains(member.getType())){
            continue;
        }

        QVector<Purchase> purchasesInYear = member.getPurchaseOnYear(year);

        for(Purchase purchase : purchasesInYear){
            itemList[purchase.getItem()] += purchase.getQuantity();

            int currentItemQnt = itemList[purchase.getItem()];

            if(currentItemQnt> bestSelling.second){
                bestSelling.first = purchase.getItem();
                bestSelling.second = currentItemQnt;
            }

            if(worstSelling.second == 0 ||
               currentItemQnt < worstSelling.second)
            {
               worstSelling.first = purchase.getItem();
               worstSelling.second = currentItemQnt;
            }

           totalRevenue += purchase.getTotalPrice();
        }
    }
    
    //Display ItemList
    
    for(const Item& item : itemList.keys()){
        std::cout<<item.name.toStdString()<<" "<<itemList[item]<<std::endl;
    }

    QString reportText = "";

    for(auto it = itemList.constBegin(); it != itemList.constEnd(); ++it){
        reportText += QString("%1 %2 \n\n")
            .arg(it.key().name, -25)
            .arg(it.value(), 10);
    }
    
    reportText += QString("Total Revenue: $%1\n\n").arg(totalRevenue);
    
    reportText += QString("Best Selling Item: %1 %2 count\n\n")
                      .arg(bestSelling.first.name)
                      .arg(bestSelling.second);
    
    reportText += QString("Worst Selling Item: %1 %2 count\n\n")
                      .arg(worstSelling.first.name)
                      .arg(worstSelling.second);

    return reportText;

}

QString MemberManager::generateTotalPurchaseReport(QSet<Member::MembershipType> includedTypes) const {

    QString reportText = "";

    double totalSpending = 0;
    for(auto member: members){

        if(!includedTypes.contains(member.getType())){
            continue;
        }

        reportText += "-----------------------------------------------------------\n";
        reportText += QString("\nMember: %1 \nID: %2 \n")
                          .arg(member.getName())
                          .arg(member.getId());


        QVector<Purchase> memberPurchases = member.getAllPurchases();
        QDate currentPurchaseDate;
        double totalMemberSpending = 0;
        for(auto purchase : memberPurchases){
            if(purchase.getDate() != currentPurchaseDate){
                currentPurchaseDate = purchase.getDate();

                reportText += QString("\n%1: \n")
                                  .arg(currentPurchaseDate.toString());
            }
            totalMemberSpending += purchase.getTotalPrice();

            reportText += QString("  %1 %2 %3 \n")
                            .arg(purchase.getItem().name, -25)
                            .arg(purchase.getQuantity(), 10)
                            .arg(purchase.getTotalPrice(), 10);
        }
        reportText += QString("\nTotal Spending for %1: $%2 \n")
                        .arg(member.getName())
                        .arg(totalMemberSpending);
        reportText += "-----------------------------------------------------------\n";
        totalSpending += totalMemberSpending;
    }

    reportText += QString("\nGrand Total: $%1").arg(totalSpending);

    return reportText;
}



QString MemberManager::generateYearlyDuesReport(QSet<Member::MembershipType> includedTypes) const{
    QVector<Member> preferredMembers;
    QVector<Member> basicMembers;

    for(auto member: members){
        if(!includedTypes.contains(member.getType())){
            continue;
        }

        if(member.getType() == Member::PREFERRED){
            preferredMembers.push_back(member);
        }else{
            basicMembers.push_back(member);
        }
    }

    std::sort(preferredMembers.begin(),
              preferredMembers.end(),
              [](const Member& a, const Member& b){
                return a.getName() < b.getName();
              }
    );

    std::sort(basicMembers.begin(),
              basicMembers.end(),
              [](const Member& a, const Member& b){
                  return a.getName() < b.getName();
              }
    );

    QString report = "";


    double basicMemberDues = !basicMembers.isEmpty() ? 0 :  basicMembers.size() * basicMembers[0].getDues();;
    double preferredMemberDues = !preferredMembers.isEmpty() ? 0 : preferredMembers.size() * preferredMembers[0].getDues();;


    if(includedTypes.contains(Member::BASIC)){
        report += "Basic Members: \n";

        for(auto member : basicMembers){
            report += QString("%1Dues: $%2\n")
            .arg(member.getName(), -30)
                .arg(member.getDues());
        }


        report += QString("Total Dues from basic members: $%1\n\n")
                      .arg(basicMemberDues);
    }

    if(includedTypes.contains(Member::PREFERRED)){
        report += "Preferred members: \n";
        for(auto member: preferredMembers){
            report += QString("%1Dues: $%2\n")
            .arg(member.getName(), -30)
                .arg(member.getDues());
        }


        report += QString("Total Dues from preferred members: $%1\n\n")
                      .arg(preferredMemberDues);
    }



    report += QString("Total Dues from all Members: $%1")
                  .arg(basicMemberDues + preferredMemberDues);

    return report;
}

QVector<Member> MemberManager::getMembersShouldUpgrade() const{
    QVector<Member> membersShouldUpgrade;

    for(auto member: members){
        if(member.getType() == Member::PREFERRED)
            continue;

        double savings = member.getSavings();

        if(savings > 0){
            membersShouldUpgrade.push_back(member);
        }
    }

    return membersShouldUpgrade;
}

QVector<Member> MemberManager::getMembersShouldDowngrade() const{
    QVector<Member> membersShouldDowngrade;

    for(auto member: members){
        if(member.getType() == Member::PREFERRED){
            double savings = member.getSavings();

            if(savings < 0){
                membersShouldDowngrade.push_back(member);
            }
        }
    }

    return membersShouldDowngrade;
}

QVector<Member> MemberManager::getExpiringMembers(int month, int year, QSet<Member::MembershipType> includedTypes) const {
    QVector<Member> expiringMembers;

    for(auto member: members){
        if(!includedTypes.contains(member.getType())){
            continue;
        }
        if(member.getExpiryDate().month() == month
            && member.getExpiryDate().year() == year)
        {
            expiringMembers.push_back(member);
        }
    }

    return expiringMembers;
}













