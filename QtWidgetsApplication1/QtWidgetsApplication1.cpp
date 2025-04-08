#include "QtWidgetsApplication1.h"
#include <QMessageBox>

QtWidgetsApplication1::QtWidgetsApplication1(QWidget *parent)
    : QMainWindow(parent)
    , licenseManager("license.dat")
{
    ui.setupUi(this);
    if (!licenseManager.loadLicense()) {
       
        QString dateString = "2025-04-01";
        QDate customDate = QDate::fromString(dateString, "yyyy-MM-dd");
        QDate customStartDate(2021, 4, 1); // 年, 月, 日
       // licenseManager.saveLicense(QDate::currentDate(),30); // 默认有效期为30天
        licenseManager.saveLicense(customStartDate, 30); // 默认有效期为30天
    }
    if (licenseManager.isLicenseValid()) {
        ui.statusLabel->setText(QStringLiteral("License 有效，欢迎使用软件。"));
    }
    else {
        ui.statusLabel->setText(QStringLiteral("License 无效或已过期，请输入激活码。"));
    }

    //connect(ui.activateButton, SIGNAL(clicked()), this, SLOT(on_activateButton_clicked()));
    //connect(ui.plaintextButton, SIGNAL(clicked()), this, SLOT(on_plaintextButton_clicked()));
}

QtWidgetsApplication1::~QtWidgetsApplication1()
{

}

void QtWidgetsApplication1::on_activateButton_clicked() {
    QString key = ui.keyInput->text();
    QString decryptedKey = licenseManager.simpleDecrypt(key);  // 解密
    QStringList parts = decryptedKey.split("-");
    if (parts.size() != 2) {
        QMessageBox::warning(this, QStringLiteral("错误"), QStringLiteral( "激活码格式无效！"));
        return;
    }

    QDate keyDate = QDate::fromString(parts[0], "yyyyMMdd");
    bool ok;
    int days = parts[1].toInt(&ok);

    if (!keyDate.isValid() || !ok) {
        QMessageBox::warning(this, QStringLiteral("错误"), QStringLiteral( "激活码无效或已损坏！"));
        return;
    }

    if (licenseManager.saveLicense(keyDate, days)) {
        QMessageBox::information(this, QStringLiteral("成功"), QStringLiteral("激活成功！"));
        ui.statusLabel->setText("License 有效。");
    }
    else {
        QMessageBox::warning(this, QStringLiteral("失败"), QStringLiteral("授权保存失败。"));
    }

    //if (decryptedKey.startsWith("RENEW-")) {
    //    bool ok;
    //    int days = decryptedKey.mid(6).toInt(&ok);  // 提取天数
    //    if (ok) {
    //        if (licenseManager.saveLicense(QDate::currentDate(), days)) {
    //            QMessageBox::information(this, QStringLiteral("成功"), QStringLiteral("激活成功！"));
    //            ui.statusLabel->setText(QStringLiteral("License 有效。"));
    //        }
    //        else {
    //            QMessageBox::warning(this, QStringLiteral("失败"), ("保存授权失败。"));
    //        }
    //    }
    //    else {
    //        QMessageBox::warning(this, QStringLiteral("失败"), QStringLiteral("激活码无效。"));
    //    }
    //}
    //else {
    //    QMessageBox::warning(this, QStringLiteral("失败"), QStringLiteral("激活码格式错误。"));
    //}
   /* if (licenseManager.activateWithKey(key)) {
        QMessageBox::information(this, QStringLiteral("成功"), QStringLiteral("激活成功！"));
        ui.statusLabel->setText(QStringLiteral("License 有效。"));
    }
    else {
        QMessageBox::warning(this, QStringLiteral("失败"), QStringLiteral("激活码无效。"));
    }*/
}

void QtWidgetsApplication1::on_plaintextButton_clicked()
{
    QString str_days = ui.keyInput_plaintext->text();
    bool ok;
    int days = str_days.toInt(&ok);
    if (!ok) {
        QMessageBox::warning(this, QStringLiteral("输入无效"), QStringLiteral("请输入有效的整数！"));
        return;  // 如果不是整数，终止后续处理
    }
    QString enstring=licenseManager.generateActivationKey(days);

    ui.keyOutput->setText(enstring);

}
