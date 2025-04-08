#ifndef UTILITY_H
#define UTILITY_H

#include <QString>
#include <QDir>
#include <QCoreApplication>
#include "member.h"


QString GET_PROJECT_DIRECTORY();
QString GET_PURCHASES_PROCESSED_DIRECTORY();
QString GET_MEMBERS_FILE_DIRECTORY();
bool removeIdFromMembersFile(int id);
bool addMemberToMembersFile(const Member& member);

#endif // UTILITY_H
