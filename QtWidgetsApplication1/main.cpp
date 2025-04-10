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
            return true; // 已退出
        }
        QThread::msleep(100); // 每100ms检查一次
    }

    qDebug() << QStringLiteral("等待进程退出超时:") << processName;
    return false;
}

// 判断某个进程是否存在
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

// 启动并等待真正的安装进程关闭
bool runInstallerAndWaitForProcessExit(const QString& exePath, const QString& processNameToWaitFor) {
    SHELLEXECUTEINFO sei = { sizeof(SHELLEXECUTEINFO) };
    sei.lpVerb = L"runas"; // 提权
    sei.lpFile = (LPCWSTR)exePath.utf16();
    sei.nShow = SW_SHOWNORMAL;
    sei.fMask = SEE_MASK_NOCLOSEPROCESS;

    if (!ShellExecuteEx(&sei)) {
        qDebug() << QStringLiteral("启动失败：") << exePath;
        return false;
    }

    // 等待目标进程出现
    qDebug() << QStringLiteral("等待进程启动：" )<< processNameToWaitFor;
    // 等待真正的安装进程出现（最多10秒）
    for (int i = 0; i < 100; ++i) {
        if (isProcessRunning(processNameToWaitFor)) break;
        QThread::msleep(100);
    }

    // 等待目标进程退出
    qDebug() << QStringLiteral("等待进程退出：" )<< processNameToWaitFor;
    while (isProcessRunning(processNameToWaitFor)) {
        QThread::sleep(1);
    }

    return waitForProcessToExit(processNameToWaitFor, 20000); // 最多等20秒
}
//bool runInstallerWithAdminAndWait(const QString& exePath) {
//    SHELLEXECUTEINFO sei = { sizeof(SHELLEXECUTEINFO) };
//    sei.lpVerb = L"runas"; // 以管理员权限运行
//    sei.lpFile = (LPCWSTR)exePath.utf16(); // 转换 QString -> LPCWSTR
//    sei.nShow = SW_SHOWNORMAL;
//    sei.fMask = SEE_MASK_NOCLOSEPROCESS;
//
//    if (!ShellExecuteEx(&sei)) {
//        DWORD err = GetLastError();
//        qDebug() << QStringLiteral("ShellExecuteEx 启动失败，错误码:" )<< err;
//        return false;
//    }
//    if (sei.hProcess) {
//        // 等待驱动安装程序的进程真正退出
//        WaitForSingleObject(sei.hProcess, INFINITE);
//        CloseHandle(sei.hProcess);
//        return true;
//    }
//
//    return false;
//    //// 等待进程执行完成
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
//        qDebug() << QStringLiteral("驱动已安装，跳过安装步骤。");
//        return;
//    }
//
//    if (!QFile::exists(exePath)) {
//        qDebug() << QStringLiteral("驱动程序文件不存在！路径：" )<< exePath;
//        return;
//    }
//
//    qDebug() << QStringLiteral("首次运行，准备安装驱动...");
//
//    if (runInstallerWithAdminAndWait(exePath)) {
//        qDebug() << QStringLiteral("驱动安装完成，创建标志文件。");
//
//        QFile flagFile(flagFilePath);
//        if (flagFile.open(QIODevice::WriteOnly)) {
//            flagFile.write("installed");
//            flagFile.close();
//        }
//        else {
//            qDebug() << QStringLiteral("创建标志文件失败！");
//        }
//    }
//    else {
//        qDebug() << QStringLiteral("驱动安装失败或被用户取消。");
//    }
//}
//// 检查并安装两个驱动
//void checkAndInstallDriversIfNeeded() {
//    QString appDir = QCoreApplication::applicationDirPath();
//    QString flagFilePath = appDir + "/drivers_installed.flag";
//
//    // 如果标志文件已存在，跳过安装
//    if (QFile::exists(flagFilePath)) {
//        qDebug() << QStringLiteral("驱动已安装，跳过安装步骤。");
//        return;
//    }
//
//    QString driver1 = appDir + "/1_CH341SER.EXE";
//    QString driver2 = appDir + "/2_MCS_Drivers_V1.exe";
//
//    // 检查文件是否存在
//    if (!QFile::exists(driver1)) {
//        qDebug() << QStringLiteral("驱动 1 文件不存在：") << driver1;
//        return;
//    }
//    if (!QFile::exists(driver2)) {
//        qDebug() << QStringLiteral("驱动 2 文件不存在：") << driver2;
//        return;
//    }
//
//    qDebug() << QStringLiteral("首次运行，开始安装驱动 1：") << driver1;
//    if (!runInstallerWithAdminAndWait(driver1)) {
//        qDebug() << QStringLiteral("驱动 1 安装失败或被取消。");
//        return;
//    }
//
//    qDebug() << QStringLiteral("驱动 1 安装完成，开始安装驱动 2：") << driver2;
//    if (!runInstallerWithAdminAndWait(driver2)) {
//        qDebug() << QStringLiteral("驱动 2 安装失败或被取消。");
//        return;
//    }
//
//    // 安装成功后创建标志文件
//    QFile flagFile(flagFilePath);
//    if (flagFile.open(QIODevice::WriteOnly)) {
//        flagFile.write("drivers_installed");
//        flagFile.close();
//        qDebug() << QStringLiteral("驱动安装完成，标志文件已创建。");
//    }
//    else {
//        qDebug() << QStringLiteral("创建标志文件失败！");
//    }
//}

bool checkAndInstallDriversIfNeeded() {
    QString appDir = QCoreApplication::applicationDirPath();
    QString flagFilePath = appDir + "/drivers_installed.flag";

    if (QFile::exists(flagFilePath)) {
        qDebug() << QStringLiteral("驱动已安装，跳过安装步骤。");
        return true;
    }

    QString driver1Path = appDir + "/1_CH341SER.EXE";
    QString driver2Path = appDir + "/2_MCS_Drivers_V1.exe";

    if (!QFile::exists(driver1Path)) {
        qDebug() << QStringLiteral("串口驱动不存在：") << driver1Path;
        return false;
    }
    if (!QFile::exists(driver2Path)) {
        qDebug() << QStringLiteral("相机驱动 不存在：" )<< driver2Path;
        return false;
    }

    // 注意：进程名区分大小写、要包含扩展名
    if (!runInstallerAndWaitForProcessExit(driver1Path, "CH341SER.EXE")) {
        qDebug() << QStringLiteral("串口驱动安装失败或取消。");
        return false;
    }

    if (!runInstallerAndWaitForProcessExit(driver2Path, "MCS_Drivers_V1.0.exe")) {
        qDebug() << QStringLiteral("相机驱动安装失败或取消。");
        return false;
    }

    QFile flagFile(flagFilePath);
    if (flagFile.open(QIODevice::WriteOnly)) {
        flagFile.write("drivers_installed");
        flagFile.close();
        qDebug() << QStringLiteral("驱动安装完成，标志文件已创建。");
        return true;
    }
    else {
        qDebug() << QStringLiteral("创建标志文件失败。");
        return false;
    }
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    
    //checkAndInstallDriver();  // 放在这里
    //checkAndInstallDriversIfNeeded(); // 安装两个驱动
    if (!checkAndInstallDriversIfNeeded()) {
        QMessageBox::critical(nullptr, QStringLiteral("驱动未安装"), QStringLiteral("请先完成驱动安装后再使用本软件。"));
       // return 0;
    }

    QtWidgetsApplication1 w;
    
    w.show();
    return a.exec();
}
