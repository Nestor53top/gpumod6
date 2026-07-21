#pragma once
#include <QWidget>
#include <QLabel>
#include <QPushButton>

class AboutTab : public QWidget {
    Q_OBJECT
public:
    explicit AboutTab(QWidget* parent = nullptr);

private slots:
    void onOpenBaseDir();

private:
    void setupUi();
    QLabel* m_infoLabel;
};