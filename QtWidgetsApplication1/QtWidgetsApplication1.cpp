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
        QDate customStartDate(2021, 4, 1); // ��, ��, ��
       // licenseManager.saveLicense(QDate::currentDate(),30); // Ĭ����Ч��Ϊ30��
        licenseManager.saveLicense(customStartDate, 30); // Ĭ����Ч��Ϊ30��
    }
    if (licenseManager.isLicenseValid()) {
        ui.statusLabel->setText(QStringLiteral("License ��Ч����ӭʹ�������"));
    }
    else {
        ui.statusLabel->setText(QStringLiteral("License ��Ч���ѹ��ڣ������뼤���롣"));
    }

    //connect(ui.activateButton, SIGNAL(clicked()), this, SLOT(on_activateButton_clicked()));
    //connect(ui.plaintextButton, SIGNAL(clicked()), this, SLOT(on_plaintextButton_clicked()));
}

QtWidgetsApplication1::~QtWidgetsApplication1()
{

}

void QtWidgetsApplication1::on_activateButton_clicked() {
    QString key = ui.keyInput->text();
    QString decryptedKey = licenseManager.simpleDecrypt(key);  // ����
    QStringList parts = decryptedKey.split("-");
    if (parts.size() != 2) {
        QMessageBox::warning(this, QStringLiteral("����"), QStringLiteral( "�������ʽ��Ч��"));
        return;
    }

    QDate keyDate = QDate::fromString(parts[0], "yyyyMMdd");
    bool ok;
    int days = parts[1].toInt(&ok);

    if (!keyDate.isValid() || !ok) {
        QMessageBox::warning(this, QStringLiteral("����"), QStringLiteral( "��������Ч�����𻵣�"));
        return;
    }

    if (licenseManager.saveLicense(keyDate, days)) {
        QMessageBox::information(this, QStringLiteral("�ɹ�"), QStringLiteral("����ɹ���"));
        ui.statusLabel->setText("License ��Ч��");
    }
    else {
        QMessageBox::warning(this, QStringLiteral("ʧ��"), QStringLiteral("��Ȩ����ʧ�ܡ�"));
    }

    //if (decryptedKey.startsWith("RENEW-")) {
    //    bool ok;
    //    int days = decryptedKey.mid(6).toInt(&ok);  // ��ȡ����
    //    if (ok) {
    //        if (licenseManager.saveLicense(QDate::currentDate(), days)) {
    //            QMessageBox::information(this, QStringLiteral("�ɹ�"), QStringLiteral("����ɹ���"));
    //            ui.statusLabel->setText(QStringLiteral("License ��Ч��"));
    //        }
    //        else {
    //            QMessageBox::warning(this, QStringLiteral("ʧ��"), ("������Ȩʧ�ܡ�"));
    //        }
    //    }
    //    else {
    //        QMessageBox::warning(this, QStringLiteral("ʧ��"), QStringLiteral("��������Ч��"));
    //    }
    //}
    //else {
    //    QMessageBox::warning(this, QStringLiteral("ʧ��"), QStringLiteral("�������ʽ����"));
    //}
   /* if (licenseManager.activateWithKey(key)) {
        QMessageBox::information(this, QStringLiteral("�ɹ�"), QStringLiteral("����ɹ���"));
        ui.statusLabel->setText(QStringLiteral("License ��Ч��"));
    }
    else {
        QMessageBox::warning(this, QStringLiteral("ʧ��"), QStringLiteral("��������Ч��"));
    }*/
}

void QtWidgetsApplication1::on_plaintextButton_clicked()
{
    QString str_days = ui.keyInput_plaintext->text();
    bool ok;
    int days = str_days.toInt(&ok);
    if (!ok) {
        QMessageBox::warning(this, QStringLiteral("������Ч"), QStringLiteral("��������Ч��������"));
        return;  // ���������������ֹ��������
    }
    QString enstring=licenseManager.generateActivationKey(days);

    ui.keyOutput->setText(enstring);

}
