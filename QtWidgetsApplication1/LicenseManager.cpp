// LicenseManager.cpp
#include "LicenseManager.h"
#include <QFile>
#include <QTextStream>
#include <QDate>
#include <QDebug>

LicenseManager::LicenseManager(const QString& licensePath)
    : licenseFile(licensePath), validDays(0) {}

QString LicenseManager::simpleEncrypt(const QString& data) {
    QByteArray array = data.toUtf8();
    for (int i = 0; i < array.size(); ++i)
        array[i] = array[i] + 3; // 简单 Caesar 加密
    return QString(array.toBase64());
}

QString LicenseManager::simpleDecrypt(const QString& data) {
    QByteArray array = QByteArray::fromBase64(data.toUtf8());
    for (int i = 0; i < array.size(); ++i)
        array[i] = array[i] - 3;
    return QString(array);
}

bool LicenseManager::saveLicense(const QDate& start, int days) {
    startDate = start;
    validDays = days;
    QString licenseInfo = start.toString("yyyy-MM-dd") + "|" + QString::number(days);
    QString encrypted = simpleEncrypt(licenseInfo);
    QFile file(licenseFile);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) return false;
    QTextStream out(&file);
    out << encrypted;
    file.close();
    return true;
}

bool LicenseManager::loadLicense() {
    QFile file(licenseFile);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) return false;
    QTextStream in(&file);
    QString encrypted;
    in >> encrypted;
    file.close();
    QString decrypted = simpleDecrypt(encrypted);
    QStringList parts = decrypted.split("|");
    if (parts.size() != 2) return false;
    startDate = QDate::fromString(parts[0], "yyyy-MM-dd");
    validDays = parts[1].toInt();
    return true;
}

bool LicenseManager::isLicenseValid() {
    if (!loadLicense()) return false;
    QDate today = QDate::currentDate();
    QDate endDate = startDate.addDays(validDays);
    return today >= startDate && today <= endDate;
}

bool LicenseManager::activateWithKey(const QString& key) {
    if (key.startsWith("RENEW-")) {
        bool ok;
        int days = key.mid(6).toInt(&ok);
        if (ok) {
            return saveLicense(QDate::currentDate(), days);
        }
    }
    return false;
}

QString LicenseManager::generateActivationKey(int days) {
    //QString raw = "RENEW-" + QString::number(days);
    QString dateStr = QDate::currentDate().toString("yyyyMMdd");
    QString raw = dateStr + "-" + QString::number(days);  // 如：20250408-30
    return simpleEncrypt(raw);  // 返回加密后的激活码
}
