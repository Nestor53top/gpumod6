#include "autostarttab.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QMessageBox>
#include <QProcess>
#include <QSettings>
#include <QDir>
#include <QCoreApplication>

AutostartTab::AutostartTab(QWidget* parent) : QWidget(parent) {
    setupUi();
    refresh();
}

void AutostartTab::setupUi() {
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(20, 20, 20, 20);
    mainLayout->setSpacing(12);

    QGroupBox* group = new QGroupBox("\u0410\u0432\u0442\u043e\u0437\u0430\u0433\u0440\u0443\u0437\u043a\u0430 (AppInit_DLLs)");
    QVBoxLayout* groupLayout = new QVBoxLayout(group);
    groupLayout->setSpacing(10);

    QLabel* descLabel = new QLabel(
        "\u041f\u0440\u0438 \u0432\u043a\u043b\u044e\u0447\u0451\u043d\u043d\u043e\u0439 \u0430\u0432\u0442\u043e\u0437\u0430\u0433\u0440\u0443\u0437\u043a\u0435 DLL \u0437\u0430\u0433\u0440\u0443\u0436\u0430\u0435\u0442\u0441\u044f \u0430\u0432\u0442\u043e\u043c\u0430\u0442\u0438\u0447\u0435\u0441\u043a\u0438 \u043f\u0440\u0438 \u0437\u0430\u043f\u0443\u0441\u043a\u0435 \u043b\u044e\u0431\u043e\u0433\u043e \u043f\u0440\u0438\u043b\u043e\u0436\u0435\u043d\u0438\u044f, \u043a\u043e\u0442\u043e\u0440\u043e\u0435 \u0438\u0441\u043f\u043e\u043b\u044c\u0437\u0443\u0435\u0442 user32.dll.\n"
        "\u041f\u0440\u0438 \u043f\u0435\u0440\u0432\u043e\u043c \u0437\u0430\u043f\u0443\u0441\u043a\u0435 Task Manager \u043f\u043e\u0441\u043b\u0435 \u044d\u0442\u043e\u0439 \u043d\u0430\u0441\u0442\u0440\u043e\u0439\u043a\u0438 DLL \u043f\u043e\u0434\u0433\u0440\u0443\u0436\u0430\u0435\u0442\u0441\u044f \u0438 \u0445\u0443\u043a \u043d\u0430\u0441\u0442\u0440\u0430\u0438\u0432\u0430\u0435\u0442\u0441\u044f."
    );
    descLabel->setWordWrap(true);
    descLabel->setStyleSheet("color: #a0a0a0; font-size: 11px; line-height: 1.4;");
    groupLayout->addWidget(descLabel);

    QHBoxLayout* statusLayout = new QHBoxLayout();
    QLabel* statusTitle = new QLabel("\u0421\u0442\u0430\u0442\u0443\u0441:");
    statusTitle->setStyleSheet("color: #a0a0a0; font-weight: 600;");
    m_statusLabel = new QLabel();
    statusLayout->addWidget(statusTitle);
    statusLayout->addWidget(m_statusLabel);
    statusLayout->addStretch();
    groupLayout->addLayout(statusLayout);

    QHBoxLayout* pathLayout = new QHBoxLayout();
    QLabel* pathTitle = new QLabel("DLL:");
    pathTitle->setStyleSheet("color: #a0a0a0; font-weight: 600;");
    m_pathLabel = new QLabel();
    m_pathLabel->setStyleSheet("color: #60cdff; font-family: Consolas; font-size: 11px;");
    pathLayout->addWidget(pathTitle);
    pathLayout->addWidget(m_pathLabel, 1);
    groupLayout->addLayout(pathLayout);

    QHBoxLayout* btnLayout = new QHBoxLayout();
    btnLayout->setSpacing(10);

    m_enableBtn = new QPushButton("\u0412\u043a\u043b\u044e\u0447\u0438\u0442\u044c");
    m_enableBtn->setObjectName("applyBtn");
    m_enableBtn->setFixedHeight(36);
    connect(m_enableBtn, &QPushButton::clicked, this, &AutostartTab::onEnable);
    btnLayout->addWidget(m_enableBtn);

    m_disableBtn = new QPushButton("\u0412\u044b\u043a\u043b\u044e\u0447\u0438\u0442\u044c");
    m_disableBtn->setObjectName("restoreBtn");
    m_disableBtn->setFixedHeight(36);
    connect(m_disableBtn, &QPushButton::clicked, this, &AutostartTab::onDisable);
    btnLayout->addWidget(m_disableBtn);

    btnLayout->addStretch();
    groupLayout->addLayout(btnLayout);

    groupLayout->addStretch();
    mainLayout->addWidget(group);
    mainLayout->addStretch();
}

bool AutostartTab::isAppInitEnabled() {
    QSettings reg("HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\Windows",
                  QSettings::NativeFormat);
    return reg.value("LoadAppInit_DLLs", 0).toInt() == 1;
}

void AutostartTab::setAppInit(bool enable) {
    QString dllPath = QDir::toNativeSeparators(
        QCoreApplication::applicationDirPath() + "/../gpu_hook.dll");

    QSettings reg("HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\Windows",
                  QSettings::NativeFormat);

    if (enable) {
        reg.setValue("LoadAppInit_DLLs", 1);
        reg.setValue("AppInit_DLLs", dllPath);
        reg.setValue("RequireSignedAppInit_DLLs", 0);
    } else {
        reg.setValue("LoadAppInit_DLLs", 0);
        reg.setValue("AppInit_DLLs", "");
    }

    reg.sync();
}

void AutostartTab::refresh() {
    bool enabled = isAppInitEnabled();
    QString dllPath = QDir::toNativeSeparators(
        QCoreApplication::applicationDirPath() + "/../gpu_hook.dll");

    m_statusLabel->setText(enabled ? "\u0412\u043a\u043b\u044e\u0447\u0435\u043d\u043e" : "\u0412\u044b\u043a\u043b\u044e\u0447\u0435\u043d\u043e");
    m_statusLabel->setStyleSheet(enabled ? "color: #4caf50; font-weight: 700;" : "color: #e0e0e0; font-weight: 700;");
    m_pathLabel->setText(dllPath);
    m_enableBtn->setEnabled(!enabled);
    m_disableBtn->setEnabled(enabled);
}

void AutostartTab::onEnable() {
    if (!isAdmin()) {
        QMessageBox::warning(this, "\u041e\u0448\u0438\u0431\u043a\u0430",
            "\u0414\u043b\u044f \u043d\u0430\u0441\u0442\u0440\u043e\u0439\u043a\u0438 AppInit_DLLs \u043d\u0435\u043e\u0431\u0445\u043e\u0434\u0438\u043c\u044b \u043f\u0440\u0430\u0432\u0430 \u0430\u0434\u043c\u0438\u043d\u0438\u0441\u0442\u0440\u0430\u0442\u043e\u0440\u0430.\n\u041f\u0435\u0440\u0435\u0437\u0430\u043f\u0443\u0441\u0442\u0438\u0442\u0435 \u043f\u0440\u0438\u043b\u043e\u0436\u0435\u043d\u0438\u0435 \u043e\u0442 \u0438\u043c\u0435\u043d\u0438 \u0430\u0434\u043c\u0438\u043d\u0438\u0441\u0442\u0440\u0430\u0442\u043e\u0440\u0430.");
        return;
    }
    setAppInit(true);
    QMessageBox::information(this, "\u0413\u043e\u0442\u043e\u0432\u043e",
        "\u0410\u0432\u0442\u043e\u0437\u0430\u0433\u0440\u0443\u0437\u043a\u0430 \u0432\u043a\u043b\u044e\u0447\u0435\u043d\u0430.\nDLL \u0431\u0443\u0434\u0435\u0442 \u0437\u0430\u0433\u0440\u0443\u0436\u0430\u0442\u044c\u0441\u044f \u043f\u0440\u0438 \u043a\u0430\u0436\u0434\u043e\u043c \u0437\u0430\u043f\u0443\u0441\u043a\u0435 Task Manager.");
    refresh();
}

void AutostartTab::onDisable() {
    if (!isAdmin()) {
        QMessageBox::warning(this, "\u041e\u0448\u0438\u0431\u043a\u0430",
            "\u0414\u043b\u044f \u043e\u0442\u043a\u043b\u044e\u0447\u0435\u043d\u0438\u044f AppInit_DLLs \u043d\u0435\u043e\u0431\u0445\u043e\u0434\u0438\u043c\u044b \u043f\u0440\u0430\u0432\u0430 \u0430\u0434\u043c\u0438\u043d\u0438\u0441\u0442\u0440\u0430\u0442\u043e\u0440\u0430.\n\u041f\u0435\u0440\u0435\u0437\u0430\u043f\u0443\u0441\u0442\u0438\u0442\u0435 \u043f\u0440\u0438\u043b\u043e\u0436\u0435\u043d\u0438\u0435 \u043e\u0442 \u0438\u043c\u0435\u043d\u0438 \u0430\u0434\u043c\u0438\u043d\u0438\u0441\u0442\u0440\u0430\u0442\u043e\u0440\u0430.");
        return;
    }
    setAppInit(false);
    QMessageBox::information(this, "\u0413\u043e\u0442\u043e\u0432\u043e",
        "\u0410\u0432\u0442\u043e\u0437\u0430\u0433\u0440\u0443\u0437\u043a\u0430 \u043e\u0442\u043a\u043b\u044e\u0447\u0435\u043d\u0430.\n\u041f\u043e\u0441\u043b\u0435 \u043f\u0435\u0440\u0435\u0437\u0430\u043f\u0443\u0441\u043a\u0430 Task Manager DLL \u043d\u0435 \u0431\u0443\u0434\u0435\u0442 \u0437\u0430\u0433\u0440\u0443\u0436\u0430\u0442\u044c\u0441\u044f.");
    refresh();
}

bool AutostartTab::isAdmin() {
    QProcess p;
    p.start("net", QStringList() << "session");
    p.waitForFinished(2000);
    return p.exitCode() == 0;
}