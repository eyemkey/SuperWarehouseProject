#include "utility.h"


QString GET_PROJECT_DIRECTORY(){
    QDir dir(QCoreApplication::applicationDirPath());
    dir.cdUp();
    dir.cdUp();
    dir.cdUp();
    return dir.absolutePath();
}

QString GET_PURCHASES_PROCESSED_DIRECTORY(){
    QString homeDir = GET_PROJECT_DIRECTORY();
    QString purchaseFilesProcessedDir = homeDir
                                        + "/db/purchaseFilesProcessed.txt";

    return purchaseFilesProcessedDir;
}

QString GET_MEMBERS_FILE_DIRECTORY(){
    QString homeDir = GET_PROJECT_DIRECTORY();
    QString membersFileDir = homeDir + "/db/currentMembers.txt";

    return membersFileDir;
}

bool removeIdFromMembersFile(int id){
    QString currentMembersDir = GET_MEMBERS_FILE_DIRECTORY();
    QFile currentMembers(currentMembersDir);

    if(!currentMembers.open(QIODevice::ReadOnly | QIODevice::Text)){
        qDebug() << "Failed to open file for reading";
        return false;
    }

    QVector<QString> updatedLines;
    QTextStream in(&currentMembers);

    while(!in.atEnd()){
        QString line = in.readLine().trimmed();

        if(line.isEmpty()) continue;

        QVector<QString> parts = line.split(", ");

        if(parts.size() >= 1 && parts[0].toInt() != id){
            updatedLines.push_back(line);
        }
    }

    currentMembers.close();

    if(!currentMembers.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate)){
        qDebug() <<"Failed to open file for writing";
        return false;
    }

    QTextStream out(&currentMembers);
    for(auto line: updatedLines){
        out << line << "\n";
    }

    return true;
}

bool addMemberToMembersFile(const Member& member){
    QString currentMembersDir = GET_MEMBERS_FILE_DIRECTORY();
    QFile currentMembers(currentMembersDir);

    QString memberInfo = QString("%1, %2, %3, %4\n")
                             .arg(member.getId())
                             .arg(member.getName())
                             .arg(member.isPreferred() ? "Preferred" : "Basic")
                             .arg(member.getExpiryDate().toString("MM/dd/yyyy"));


    if (currentMembers.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text)) {
        QTextStream out(&currentMembers);
        out << memberInfo;
        currentMembers.close();
        qDebug() << "Data appended successfully.";
    } else {
        qDebug() << "Failed to open file for appending.";
        return false;
    }

    return true;
}
