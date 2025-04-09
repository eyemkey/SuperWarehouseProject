#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "membermanager.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void addMember();
    void removeMember();
    void searchMember();
    void displayAllMembers();
    // void updateMember(); //!!Remove??
    void calculateRebates();  // 🔹 New function for rebates
    void onUploadFileClicked();
    void generateReport();

    void onItemsSoldReport();
    void onAllPurchaseReport();
    void onTotalPurchaseReport();
    void membershipDueReport();
    void onMembersUpgrade();
    void onMembersDowngrade();
    void onItemsUnitsSold();
    void onGetExpiringMembers();

    void onMemberManagementButton();
    void onSalesReportButton();
    void onHome();


private:
    Ui::MainWindow *ui;
    MemberManager memberManager;


    bool restoreData();
    void changePage(int index);
    void setReportText(const QString& report);

    void getDefaultMemberIfEmpty(QFile& defaultFile);
    QSet<Member::MembershipType> getIncludedTypesSet();
};

#endif // MAINWINDOW_H
