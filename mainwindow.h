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
    void updateMember();
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
private:
    Ui::MainWindow *ui;
    MemberManager memberManager;

    void setReportText(const QString& report);
};

#endif // MAINWINDOW_H
