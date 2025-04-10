#include "QtWidgetsApplication1.h"
#include <QtWidgets/QApplication>
#include <QCoreApplication>
#include <QFile>
#include <QDir>
#include <QProcess>
#include <QDebug>
#include <QMessageBox>
#include <windows.h>
#include <shellapi.h>
#include <QThread>
#include <tlhelp32.h>
#include <QElapsedTimer>


bool isProcessRunning(const QString& processName);
bool waitForProcessToExit(const QString& processName, int timeoutMs = 30000) {
    QElapsedTimer timer;
    timer.start();

    while (timer.elapsed() < timeoutMs) {
        if (!isProcessRunning(processName)) {
            return true; // ���˳�
        }
        QThread::msleep(100); // ÿ100ms���һ��
    }

    qDebug() << QStringLiteral("�ȴ������˳���ʱ:") << processName;
    return false;
}

// �ж�ĳ�������Ƿ����
bool isProcessRunning(const QString& processName) {
    HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnap == INVALID_HANDLE_VALUE) return false;

    PROCESSENTRY32 pe;
    pe.dwSize = sizeof(PROCESSENTRY32);
    if (Process32First(hSnap, &pe)) {
        do {
            if (QString::fromWCharArray(pe.szExeFile).compare(processName, Qt::CaseInsensitive) == 0) {
                CloseHandle(hSnap);
                return true;
            }
        } while (Process32Next(hSnap, &pe));
    }

    CloseHandle(hSnap);
    return false;
}

// �������ȴ������İ�װ���̹ر�
bool runInstallerAndWaitForProcessExit(const QString& exePath, const QString& processNameToWaitFor) {
    SHELLEXECUTEINFO sei = { sizeof(SHELLEXECUTEINFO) };
    sei.lpVerb = L"runas"; // ��Ȩ
    sei.lpFile = (LPCWSTR)exePath.utf16();
    sei.nShow = SW_SHOWNORMAL;
    sei.fMask = SEE_MASK_NOCLOSEPROCESS;

    if (!ShellExecuteEx(&sei)) {
        qDebug() << QStringLiteral("����ʧ�ܣ�") << exePath;
        return false;
    }

    // �ȴ�Ŀ����̳���
    qDebug() << QStringLiteral("�ȴ�����������" )<< processNameToWaitFor;
    // �ȴ������İ�װ���̳��֣����10�룩
    for (int i = 0; i < 100; ++i) {
        if (isProcessRunning(processNameToWaitFor)) break;
        QThread::msleep(100);
    }

    // �ȴ�Ŀ������˳�
    qDebug() << QStringLiteral("�ȴ������˳���" )<< processNameToWaitFor;
    while (isProcessRunning(processNameToWaitFor)) {
        QThread::sleep(1);
    }

    return waitForProcessToExit(processNameToWaitFor, 20000); // ����20��
}
//bool runInstallerWithAdminAndWait(const QString& exePath) {
//    SHELLEXECUTEINFO sei = { sizeof(SHELLEXECUTEINFO) };
//    sei.lpVerb = L"runas"; // �Թ���ԱȨ������
//    sei.lpFile = (LPCWSTR)exePath.utf16(); // ת�� QString -> LPCWSTR
//    sei.nShow = SW_SHOWNORMAL;
//    sei.fMask = SEE_MASK_NOCLOSEPROCESS;
//
//    if (!ShellExecuteEx(&sei)) {
//        DWORD err = GetLastError();
//        qDebug() << QStringLiteral("ShellExecuteEx ����ʧ�ܣ�������:" )<< err;
//        return false;
//    }
//    if (sei.hProcess) {
//        // �ȴ�������װ����Ľ��������˳�
//        WaitForSingleObject(sei.hProcess, INFINITE);
//        CloseHandle(sei.hProcess);
//        return true;
//    }
//
//    return false;
//    //// �ȴ�����ִ�����
//    //WaitForSingleObject(sei.hProcess, INFINITE);
//    //CloseHandle(sei.hProcess);
//
//    //return true;
//}


//void checkAndInstallDriver() {
//    QString appDir = QCoreApplication::applicationDirPath();
//    QString flagFilePath = appDir + "/driver_installed.flag";
//    QString exePath = appDir + "/2_MCS_Drivers_V1.exe";
//
//    if (QFile::exists(flagFilePath)) {
//        qDebug() << QStringLiteral("�����Ѱ�װ��������װ���衣");
//        return;
//    }
//
//    if (!QFile::exists(exePath)) {
//        qDebug() << QStringLiteral("���������ļ������ڣ�·����" )<< exePath;
//        return;
//    }
//
//    qDebug() << QStringLiteral("�״����У�׼����װ����...");
//
//    if (runInstallerWithAdminAndWait(exePath)) {
//        qDebug() << QStringLiteral("������װ��ɣ�������־�ļ���");
//
//        QFile flagFile(flagFilePath);
//        if (flagFile.open(QIODevice::WriteOnly)) {
//            flagFile.write("installed");
//            flagFile.close();
//        }
//        else {
//            qDebug() << QStringLiteral("������־�ļ�ʧ�ܣ�");
//        }
//    }
//    else {
//        qDebug() << QStringLiteral("������װʧ�ܻ��û�ȡ����");
//    }
//}
//// ��鲢��װ��������
//void checkAndInstallDriversIfNeeded() {
//    QString appDir = QCoreApplication::applicationDirPath();
//    QString flagFilePath = appDir + "/drivers_installed.flag";
//
//    // �����־�ļ��Ѵ��ڣ�������װ
//    if (QFile::exists(flagFilePath)) {
//        qDebug() << QStringLiteral("�����Ѱ�װ��������װ���衣");
//        return;
//    }
//
//    QString driver1 = appDir + "/1_CH341SER.EXE";
//    QString driver2 = appDir + "/2_MCS_Drivers_V1.exe";
//
//    // ����ļ��Ƿ����
//    if (!QFile::exists(driver1)) {
//        qDebug() << QStringLiteral("���� 1 �ļ������ڣ�") << driver1;
//        return;
//    }
//    if (!QFile::exists(driver2)) {
//        qDebug() << QStringLiteral("���� 2 �ļ������ڣ�") << driver2;
//        return;
//    }
//
//    qDebug() << QStringLiteral("�״����У���ʼ��װ���� 1��") << driver1;
//    if (!runInstallerWithAdminAndWait(driver1)) {
//        qDebug() << QStringLiteral("���� 1 ��װʧ�ܻ�ȡ����");
//        return;
//    }
//
//    qDebug() << QStringLiteral("���� 1 ��װ��ɣ���ʼ��װ���� 2��") << driver2;
//    if (!runInstallerWithAdminAndWait(driver2)) {
//        qDebug() << QStringLiteral("���� 2 ��װʧ�ܻ�ȡ����");
//        return;
//    }
//
//    // ��װ�ɹ��󴴽���־�ļ�
//    QFile flagFile(flagFilePath);
//    if (flagFile.open(QIODevice::WriteOnly)) {
//        flagFile.write("drivers_installed");
//        flagFile.close();
//        qDebug() << QStringLiteral("������װ��ɣ���־�ļ��Ѵ�����");
//    }
//    else {
//        qDebug() << QStringLiteral("������־�ļ�ʧ�ܣ�");
//    }
//}

bool checkAndInstallDriversIfNeeded() {
    QString appDir = QCoreApplication::applicationDirPath();
    QString flagFilePath = appDir + "/drivers_installed.flag";

    if (QFile::exists(flagFilePath)) {
        qDebug() << QStringLiteral("�����Ѱ�װ��������װ���衣");
        return true;
    }

    QString driver1Path = appDir + "/1_CH341SER.EXE";
    QString driver2Path = appDir + "/2_MCS_Drivers_V1.exe";

    if (!QFile::exists(driver1Path)) {
        qDebug() << QStringLiteral("�������������ڣ�") << driver1Path;
        return false;
    }
    if (!QFile::exists(driver2Path)) {
        qDebug() << QStringLiteral("������� �����ڣ�" )<< driver2Path;
        return false;
    }

    // ע�⣺���������ִ�Сд��Ҫ������չ��
    if (!runInstallerAndWaitForProcessExit(driver1Path, "CH341SER.EXE")) {
        qDebug() << QStringLiteral("����������װʧ�ܻ�ȡ����");
        return false;
    }

    if (!runInstallerAndWaitForProcessExit(driver2Path, "MCS_Drivers_V1.0.exe")) {
        qDebug() << QStringLiteral("���������װʧ�ܻ�ȡ����");
        return false;
    }

    QFile flagFile(flagFilePath);
    if (flagFile.open(QIODevice::WriteOnly)) {
        flagFile.write("drivers_installed");
        flagFile.close();
        qDebug() << QStringLiteral("������װ��ɣ���־�ļ��Ѵ�����");
        return true;
    }
    else {
        qDebug() << QStringLiteral("������־�ļ�ʧ�ܡ�");
        return false;
    }
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    
    //checkAndInstallDriver();  // ��������
    //checkAndInstallDriversIfNeeded(); // ��װ��������
    if (!checkAndInstallDriversIfNeeded()) {
        QMessageBox::critical(nullptr, QStringLiteral("����δ��װ"), QStringLiteral("�������������װ����ʹ�ñ������"));
       // return 0;
    }

    QtWidgetsApplication1 w;
    
    w.show();
    return a.exec();
}
