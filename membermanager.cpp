#include "membermanager.h"
#include "date.h"
#include <QMessageBox>
#include <QDebug>
#include <iostream>
#include <QDir>
#include <QCoreApplication>
#include <tuple>

void MemberManager::addMember(const Member &member) {
    members[member.getId()] = member;
}

bool MemberManager::removeMember(int id) {
    return members.remove(id) > 0;
}

Member* MemberManager::searchMember(int id) {
    if (members.contains(id)) {
        return &members[id];
    }
    return nullptr;
}

QList<Member> MemberManager::getAllMembers() const {
    return members.values();
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
            << m.getExpiryDate() << ", " << m.getTotalSpent() << "\n";
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

        members.insert(id, Member(name, id, type, expiryDate, spent));

        // 🔹 Remove redundant initialization:
        // totalSpent[id] = 0.0;
        totalSpent[id] = spent;  // Correctly store total spent
    }
    file.close();
}


// 🔹 Load warehouse shoppers data
void MemberManager::loadShoppersFile(const QString &filename) {
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "Error: Cannot open shoppers file!";
        return;
    }

    QTextStream in(&file);
    while (!in.atEnd()) {
        QString name = in.readLine();
        int id = in.readLine().toInt();
        QString typeStr = in.readLine();
        QString expiry = in.readLine();

        Member::MembershipType type = (typeStr == "Preferred") ? Member::PREFERRED : Member::BASIC;
        members[id] = Member(name, id, type, expiry);
        totalSpent[id] = 0.0; // Initialize spending to 0
    }
    file.close();
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

        Date d = Date::parseDateStringToDate(date);
        Purchase purchase(Item(itemName, price), quantity, Date::parseDateStringToDate(date));
        members[id].addPurchase(purchase);
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
void MemberManager::displayRebates(QWidget *parent) {
    QString result = "Rebate Summary for Preferred Members:\n";

    for (auto id : members.keys()) {
        if (members[id].getType() == Member::PREFERRED) {
            double rebate = totalSpent[id] * 0.05;
            result += QString("%1 (ID: %2): $%3\n")
                          .arg(members[id].getName())
                          .arg(id)
                          .arg(rebate, 0, 'f', 2);
        }
    }

    QMessageBox::information(parent, "Rebate Report", result);
}


void MemberManager::generateDailyReport(const Date& date) const{
    int prefferedCount = 0, basicCount = 0;
    QMap<Item, int> itemList;
    QVector<QString> customerNames;
    double totalRevenue = 0;

    for(auto member: members){
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

    QString projectRoot = QCoreApplication::applicationDirPath() + "/..";
    QDir rootDir(projectRoot);
    rootDir.cdUp();
    rootDir.cdUp();

    QDir reportsDir(rootDir.filePath("reports"));
    if(!reportsDir.exists()){
        reportsDir.mkpath(".");
    }

    QString filename = "DailySalesReport-" + date.toString()+".txt";
    QString fullPath = reportsDir.filePath(filename);


    QFile file(fullPath);

    if(!file.open(QIODevice::WriteOnly | QIODevice::Text)){
        return;
    }

    QTextStream out(&file);

    for(auto item : itemList.keys()){

        out.setFieldWidth(30);
        out<<item.name + " ";


        out.setFieldWidth(5);
        out<<itemList[item];

        out<<"\n\n";
    }

    out<<"Customers that Shopped: \n";

    for(auto e: customerNames){
        out<<e<<"\n";
    }

    out<<"\n";
    out.setFieldWidth(20);
    out<<"Preferred Customers: "<<prefferedCount<<"\n";

    out.setFieldWidth(20);
    out<<"Basic Customers: "<<basicCount<<"\n";

}

void MemberManager::generateYearReport(int year) const {
    double totalRevenue = 0;
    QMap<Item, int> itemList;
    QPair<Item, int> bestSelling(Item(), 0);
    QPair<Item, int> worstSelling(Item(), 0);

    for(auto member : members){

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

    QString projectRoot = QCoreApplication::applicationDirPath() + "/..";
    QDir rootDir(projectRoot);
    rootDir.cdUp();
    rootDir.cdUp();

    QDir reportsDir(rootDir.filePath("reports"));
    if(!reportsDir.exists()){
        reportsDir.mkpath(".");
    }

    QString filename = "YearlySalesReport-" + QString::number(year) +  ".txt";
    QString fullPath = reportsDir.filePath(filename);

    QFile file(fullPath);

    if(!file.open(QIODevice::WriteOnly | QIODevice::Text)){
        return;
    }

    QTextStream out(&file);

    for(auto it = itemList.constBegin(); it != itemList.constEnd(); ++it){
        out << QString("%1 %2 \n\n")
                .arg(it.key().name, -25)
                .arg(it.value(), 10);
    }

    out<<"Total Revenue: "<<QString::number(totalRevenue) + "\n\n";

    out<<"Best Selling Item: "<<bestSelling.first.name;
    out<<" "<<QString::number(bestSelling.second)<<" count \n\n";

    out<<"Worst Selling Item: "<<worstSelling.first.name;
    out<<" "<<QString::number(worstSelling.second)<<" count \n\n";


}

void MemberManager::generateTotalPurchaseReport() const {

    QString projectRoot = QCoreApplication::applicationDirPath() + "/..";
    QDir rootDir(projectRoot);
    rootDir.cdUp();
    rootDir.cdUp();

    QDir reportsDir(rootDir.filePath("reports"));
    if(!reportsDir.exists()){
        reportsDir.mkpath(".");
    }

    QString filename = "All Members - Total Purchase Report.txt";
    QString fullPath = reportsDir.filePath(filename);

    QFile file(fullPath);

    if(!file.open(QIODevice::WriteOnly | QIODevice::Text)){
        return;
    }

    QTextStream out(&file);


    double totalSpending = 0;
    for(auto member: members){
        out<<"-----------------------------------------------------------\n";
        out<<QString("\nMember: %1 \nID: %2 \n")
                   .arg(member.getName())
                   .arg(member.getId());
        QVector<Purchase> memberPurchases = member.getAllPurchases();
        Date currentPurchaseDate;
        double totalMemberSpending = 0;
        for(auto purchase : memberPurchases){
            if(purchase.getDate() != currentPurchaseDate){
                currentPurchaseDate = purchase.getDate();

                out<<"\n"<<currentPurchaseDate.toString()<<": \n";
            }
            totalMemberSpending += purchase.getTotalPrice();

            out<<QString("  %1 %2 %3 \n")
                       .arg(purchase.getItem().name, -25)
                       .arg(purchase.getQuantity(), 10)
                       .arg(purchase.getTotalPrice(), 10);
        }
        out<<QString("\nTotal Spending for %1: $%2 \n")
                   .arg(member.getName())
                   .arg(totalMemberSpending);
        out<<"-----------------------------------------------------------\n";
        totalSpending += totalMemberSpending;
    }

    out<<QString("\nGrand Total: $%1").arg(totalSpending);
}




















