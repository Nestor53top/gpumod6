#include "abouttab.h"
#include <QVBoxLayout>
#include <QGroupBox>
#include <QCoreApplication>
#include <QDir>
#include <QDesktopServices>
#include <QUrl>

AboutTab::AboutTab(QWidget* parent) : QWidget(parent) {
    setupUi();
}

void AboutTab::setupUi() {
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(14);
    mainLayout->setContentsMargins(20, 20, 20, 20);

    QLabel* header = new QLabel("\u26A1 GPU Name Swap");
    header->setStyleSheet(
        "color: #60cdff;"
        "font-size: 28px;"
        "font-weight: 700;"
        "font-family: 'Segoe UI Variable', 'Segoe UI';"
    );
    mainLayout->addWidget(header);

    QLabel* sub = new QLabel("\u041f\u043e\u0434\u043c\u0435\u043d\u0430 \u0438\u043c\u0435\u043d\u0438 GPU \u0432 \u0414\u0438\u0441\u043f\u0435\u0442\u0447\u0435\u0440\u0435 \u0437\u0430\u0434\u0430\u0447");
    sub->setStyleSheet("color: #b0b0b0; font-size: 14px;");
    mainLayout->addWidget(sub);

    QGroupBox* infoGroup = new QGroupBox("\u0418\u043d\u0444\u043e\u0440\u043c\u0430\u0446\u0438\u044f");
    QVBoxLayout* infoLayout = new QVBoxLayout(infoGroup);

    QString baseDir = QDir::toNativeSeparators(QCoreApplication::applicationDirPath());
    m_infoLabel = new QLabel(
        QString("\u0420\u0430\u0441\u043f\u043e\u043b\u043e\u0436\u0435\u043d\u0438\u0435: %1\n\n"
                "\u041a\u0430\u043a \u0440\u0430\u0431\u043e\u0442\u0430\u0435\u0442:\n"
                "  \u2022 DLL \u0438\u043d\u0436\u0435\u043a\u0442\u0438\u0442\u0441\u044f \u0432 Taskmgr.exe\n"
                "  \u2022 \u041f\u0435\u0440\u0435\u0445\u0432\u0430\u0442\u044b\u0432\u0430\u0435\u0442 DirectUI::Element::SetContentString\n"
                "  \u2022 \u041f\u0440\u0438 \u0432\u0441\u0442\u0440\u0435\u0447\u0435 \u043d\u0430\u0437\u0432\u0430\u043d\u0438\u044f GPU \u0437\u0430\u043c\u0435\u043d\u044f\u0435\u0442 \u043d\u0430 \u0432\u0430\u0448\u0435\n\n"
                "\u0424\u0430\u0439\u043b\u044b:\n"
                "  gpu_hook.dll \u2014 \u0445\u0443\u043a DLL\n"
                "  inject.exe \u2014 \u0438\u043d\u0436\u0435\u043a\u0442\u043e\u0440\n"
                "  gpu_swap.json \u2014 \u043a\u043e\u043d\u0444\u0438\u0433\u0443\u0440\u0430\u0446\u0438\u044f")
            .arg(baseDir)
    );
    m_infoLabel->setStyleSheet(
        "color: #d0d0d0;"
        "font-family: 'Segoe UI', sans-serif;"
        "font-size: 12px;"
        "line-height: 1.5;"
    );
    m_infoLabel->setWordWrap(true);
    infoLayout->addWidget(m_infoLabel);

    mainLayout->addWidget(infoGroup);

    QPushButton* openBtn = new QPushButton("\u041e\u0442\u043a\u0440\u044b\u0442\u044c \u043f\u0430\u043f\u043a\u0443");
    connect(openBtn, &QPushButton::clicked, this, &AboutTab::onOpenBaseDir);
    mainLayout->addWidget(openBtn);

    mainLayout->addStretch();

    QLabel* footer = new QLabel("v1.0 \u2022 Qt6 GUI");
    footer->setStyleSheet("color: #555555; font-size: 11px;");
    mainLayout->addWidget(footer);
}

void AboutTab::onOpenBaseDir() {
    QString path = QDir::toNativeSeparators(QCoreApplication::applicationDirPath());
    QDesktopServices::openUrl(QUrl::fromLocalFile(path));
}