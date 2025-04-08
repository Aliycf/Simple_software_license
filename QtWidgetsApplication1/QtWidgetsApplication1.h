#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_QtWidgetsApplication1.h"
#include "LicenseManager.h"

class QtWidgetsApplication1 : public QMainWindow
{
    Q_OBJECT

public:
    QtWidgetsApplication1(QWidget *parent = nullptr);
    ~QtWidgetsApplication1();

private slots:
    void on_activateButton_clicked();
    void on_plaintextButton_clicked();

private:
    Ui::QtWidgetsApplication1Class ui;
    LicenseManager licenseManager;
};
