#ifndef DATE_H
#define DATE_H

#include <QString>
#include <QVector>
#include <QDebug>

class Date
{
public:
    Date();
    Date(int day, int month, int year);
    Date(const QString& date);
    // Date(const Date& other);

    int getDay() const;
    int getMonth() const;
    int getYear() const;

    void setDate(int day, int month, int year);

    QString toString() const;
    bool isLeapYear() const;
    bool isValidDate() const;

    bool operator==(const Date& other) const;
    bool operator!=(const Date& other) const;
    bool operator<(const Date& other) const;
    bool operator>(const Date& other) const;
    bool operator>= (const Date& other) const;
    bool operator<= (const Date& other) const;

    static Date parseDateStringToDate(const QString& dateStr);
private:
    int day;
    int month;
    int year;
};

#endif // DATE_H
