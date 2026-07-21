#pragma once
#include <QWidget>
#include <QTextEdit>
#include <QPushButton>
#include <QCheckBox>
#include <QLabel>

class LogsTab : public QWidget {
    Q_OBJECT
public:
    explicit LogsTab(QWidget* parent = nullptr);

public slots:
    void appendLog(const QString& line);
    void loadLogs();

private slots:
    void onCopy();
    void onClear();
    void onScan();
    void onRefresh();

private:
    QTextEdit* m_logView;
    QPushButton* m_copyBtn;
    QPushButton* m_clearBtn;
    QPushButton* m_scanBtn;
    QPushButton* m_refreshBtn;
    QCheckBox* m_autoScroll;
    QCheckBox* m_verboseScan;
    QLabel* m_pathLabel;
    QString m_logPath;
};