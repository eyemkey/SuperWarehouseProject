#include "date.h"

#include <iostream>

Date::Date() : day(1), month(1), year(2000) {}

Date::Date(int day, int month, int year) :
    day(day), month(month), year(year) {}

Date::Date(const QString& date){
    Date(parseDateStringToDate(date));
}


int Date::getDay() const{
    return day;
}

int Date::getMonth() const {
    return month;
}

int Date::getYear() const {
    return year;
}

void Date::setDate(int day, int month, int year) {
    this->day = day;
    this->month = month;
    this->year = year;
}

QString Date::toString() const {
    QString dateToString = "";

    if(month < 10){
        dateToString += "0";
    }
    dateToString += std::to_string(month);
    dateToString += "-";

    if(day < 10){
        dateToString += "0";
    }
    dateToString += std::to_string(day);
    dateToString += "-";

    dateToString += std::to_string(year);

    return dateToString;
}


bool Date::isLeapYear() const {
    return (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
}

bool Date::isValidDate() const {
    if (month < 1 || month > 12 || day < 1 || year < 1) return false;

    int daysInMonth[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    if(isLeapYear()){
        daysInMonth[1] = 29;
    }

    return day <= daysInMonth[month - 1];
}

bool Date::operator==(const Date& other) const {
    return (day == other.day && month == other.month && year == other.year);
}

bool Date::operator!=(const Date& other) const {
    return !(*this == other);
}

bool Date::operator<(const Date& other) const {
    if (year != other.year) return year < other.year;
    if (month != other.month) return month < other.month;
    return day < other.day;
}

bool Date::operator>(const Date& other) const {
    return other < *this;
}

bool Date::operator<=(const Date& other) const {
    return !(*this > other);
}

bool Date::operator>=(const Date& other) const {
    return !(*this < other);
}

Date Date::parseDateStringToDate(const QString& dateStr) {
    QVector<QString> parts = dateStr.split('/');

    if (parts.size() != 3) {
        qDebug() << "Invalid date format. Expected mm/dd/yyyy.";
        return Date();  // Return default date
    }
    bool ok1, ok2, ok3;
    int month = parts[0].toInt(&ok1);
    int day = parts[1].toInt(&ok2);
    int year = parts[2].toInt(&ok3);

    if (!ok1 || !ok2 || !ok3) {
        qDebug() << "Conversion error: One or more parts of the date are not valid numbers.";
        return Date();
    }

    Date d(day, month, year);
    if (!d.isValidDate()) {
        qDebug() << "Invalid date values.";
        return Date();  // Optionally return an error flag
    }

    return d;
}
