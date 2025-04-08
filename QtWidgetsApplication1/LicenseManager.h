#pragma once
#ifndef LICENSEMANAGER_H
#define LICENSEMANAGER_H

#include <QString>
#include <QDate>

class LicenseManager {
public:
    LicenseManager(const QString& licenseFilePath);

    bool isLicenseValid();
    bool loadLicense();
    bool saveLicense(const QDate& startDate, int validDays);
    bool activateWithKey(const QString& key);

    QString simpleEncrypt(const QString& data);
    QString simpleDecrypt(const QString& data);

    QString generateActivationKey(int days);

private:
    QString licenseFile;
    QDate startDate;
    int validDays;

    
};

#endif // LICENSEMANAGER_H