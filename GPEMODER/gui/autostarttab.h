#pragma once
#include <QWidget>
#include <QPushButton>
#include <QLabel>
#include <QCheckBox>

class AutostartTab : public QWidget {
    Q_OBJECT
public:
    explicit AutostartTab(QWidget* parent = nullptr);

private slots:
    void onEnable();
    void onDisable();
    void refresh();

private:
    void setupUi();
    bool isAppInitEnabled();
    void setAppInit(bool enable);
    bool isAdmin();

    QLabel* m_statusLabel;
    QLabel* m_pathLabel;
    QPushButton* m_enableBtn;
    QPushButton* m_disableBtn;
    QCheckBox* m_globalCheck;
};