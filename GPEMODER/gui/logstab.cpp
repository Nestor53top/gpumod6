#include "logstab.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QClipboard>
#include <QGuiApplication>
#include <QFile>
#include <QDir>
#include <QCoreApplication>
#include <QProcess>
#include <QTimer>
#include <QFileInfo>
#include <QDateTime>
#include <QTextStream>
#include <QRegularExpression>
#include <windows.h>

LogsTab::LogsTab(QWidget* parent) : QWidget(parent) {
    m_logPath = QDir::toNativeSeparators(QCoreApplication::applicationDirPath() + "/../gpu_debug.log");

    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(10);
    mainLayout->setContentsMargins(20, 20, 20, 20);

    // Path label
    m_pathLabel = new QLabel("\u041b\u043e\u0433-\u0444\u0430\u0439\u043b: " + m_logPath);
    m_pathLabel->setStyleSheet("color: #888888; font-size: 11px; font-family: Consolas, monospace;");
    m_pathLabel->setWordWrap(true);
    mainLayout->addWidget(m_pathLabel);

    // Options
    QHBoxLayout* optLayout = new QHBoxLayout();
    m_autoScroll = new QCheckBox("\u0410\u0432\u0442\u043e-\u043f\u0440\u043e\u043a\u0440\u0443\u0442\u043a\u0430");
    m_autoScroll->setChecked(true);
    optLayout->addWidget(m_autoScroll);

    m_verboseScan = new QCheckBox("\u041f\u043e\u0434\u0440\u043e\u0431\u043d\u043e\u0435 \u0441\u043a\u0430\u043d\u0438\u0440\u043e\u0432\u0430\u043d\u0438\u0435");
    optLayout->addWidget(m_verboseScan);

    optLayout->addStretch();
    mainLayout->addLayout(optLayout);

    // Log view
    m_logView = new QTextEdit();
    m_logView->setReadOnly(true);
    m_logView->setStyleSheet(
        "QTextEdit {"
        "  background: #0c0c0c;"
        "  color: #e0e0e0;"
        "  border: 1px solid #2a2a2a;"
        "  border-radius: 6px;"
        "  font-family: Consolas, 'Courier New', monospace;"
        "  font-size: 12px;"
        "  padding: 10px;"
        "  selection-background-color: #0078d4;"
        "  selection-color: #ffffff;"
        "}"
    );
    mainLayout->addWidget(m_logView, 1);

    // Buttons
    QHBoxLayout* btnLayout = new QHBoxLayout();

    m_scanBtn = new QPushButton("\u0421\u043a\u0430\u043d\u0438\u0440\u043e\u0432\u0430\u0442\u044c \u043f\u0430\u043c\u044f\u0442\u044c Taskmgr");
    connect(m_scanBtn, &QPushButton::clicked, this, &LogsTab::onScan);
    btnLayout->addWidget(m_scanBtn);

    m_refreshBtn = new QPushButton("\u041e\u0431\u043d\u043e\u0432\u0438\u0442\u044c \u043b\u043e\u0433\u0438");
    connect(m_refreshBtn, &QPushButton::clicked, this, &LogsTab::onRefresh);
    btnLayout->addWidget(m_refreshBtn);

    btnLayout->addStretch();

    m_clearBtn = new QPushButton("\u041e\u0447\u0438\u0441\u0442\u0438\u0442\u044c");
    connect(m_clearBtn, &QPushButton::clicked, this, &LogsTab::onClear);
    btnLayout->addWidget(m_clearBtn);

    m_copyBtn = new QPushButton("\u041a\u043e\u043f\u0438\u0440\u043e\u0432\u0430\u0442\u044c \u0432\u0441\u0435");
    connect(m_copyBtn, &QPushButton::clicked, this, &LogsTab::onCopy);
    btnLayout->addWidget(m_copyBtn);

    mainLayout->addLayout(btnLayout);

    loadLogs();
}

void LogsTab::appendLog(const QString& line) {
    m_logView->append(line);
    if (m_autoScroll->isChecked()) {
        QTextCursor c = m_logView->textCursor();
        c.movePosition(QTextCursor::End);
        m_logView->setTextCursor(c);
    }
}

void LogsTab::loadLogs() {
    m_logView->clear();
    QFileInfo fi(m_logPath);
    if (!fi.exists()) {
        appendLog("\u041b\u043e\u0433-\u0444\u0430\u0439\u043b \u043d\u0435 \u043d\u0430\u0439\u0434\u0435\u043d. \u0417\u0430\u043f\u0443\u0441\u0442\u0438\u0442\u0435 \u0438\u043d\u0436\u0435\u043a\u0446\u0438\u044e \u0434\u043b\u044f \u0441\u043e\u0437\u0434\u0430\u043d\u0438\u044f \u043b\u043e\u0433\u0430.");
        return;
    }

    QFile f(m_logPath);
    if (!f.open(QIODevice::ReadOnly | QIODevice::Text)) {
        appendLog("\u041e\u0448\u0438\u0431\u043a\u0430 \u0447\u0442\u0435\u043d\u0438\u044f \u0444\u0430\u0439\u043b\u0430.");
        return;
    }

    QTextStream ts(&f);
    QString content = ts.readAll();
    f.close();

    QStringList lines = content.split(QRegularExpression("\r\n|\r|\n"));
    for (const QString& line : lines) {
        if (line.isEmpty()) continue;
        if (line.contains("[gpu_hook]")) {
            QString cleaned = line;
            cleaned.replace("[gpu_hook] ", "");
            appendLog(cleaned);
        } else {
            appendLog(line);
        }
    }
}

void LogsTab::onCopy() {
    QGuiApplication::clipboard()->setText(m_logView->toPlainText());
    appendLog("[\u041b\u043e\u0433\u0438] \u0421\u043a\u043e\u043f\u0438\u0440\u043e\u0432\u0430\u043d\u043e \u0432 \u0431\u0443\u0444\u0435\u0440 \u043e\u0431\u043c\u0435\u043d\u0430: " +
              QString::number(m_logView->toPlainText().length()) + " \u0441\u0438\u043c\u0432.");
}

void LogsTab::onClear() {
    QFile f(m_logPath);
    if (f.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        f.close();
    }
    m_logView->clear();
    appendLog("\u041b\u043e\u0433\u0438 \u043e\u0447\u0438\u0449\u0435\u043d\u044b.");
}

void LogsTab::onRefresh() {
    loadLogs();
}

void LogsTab::onScan() {
    appendLog("=== \u0421\u043a\u0430\u043d\u0438\u0440\u043e\u0432\u0430\u043d\u0438\u0435 \u043f\u0430\u043c\u044f\u0442\u0438 Taskmgr ===");

    // Find Taskmgr PID
    QProcess p;
    p.start("tasklist", QStringList() << "/FI" << "IMAGENAME eq Taskmgr.exe" << "/FO" << "CSV" << "/NH");
    p.waitForFinished(3000);
    QString out = p.readAllStandardOutput().trimmed();

    if (out.isEmpty()) {
        appendLog("Taskmgr.exe \u043d\u0435 \u0437\u0430\u043f\u0443\u0449\u0435\u043d.");
        return;
    }

    QString pidStr = out.split(',').value(1).replace("\"", "");
    bool ok;
    DWORD pid = pidStr.toULong(&ok);
    if (!ok) {
        appendLog("\u041d\u0435 \u0443\u0434\u0430\u043b\u043e\u0441\u044c \u043e\u043f\u0440\u0435\u0434\u0435\u043b\u0438\u0442\u044c PID.");
        return;
    }

    appendLog("Taskmgr PID: " + QString::number(pid));

    // Try wmic process info
    QProcess p2;
    p2.start("wmic", QStringList() << "process" << "where" << QString("ProcessId=%1").arg(pid) << "get" << "CommandLine,ExecutablePath,WorkingSetSize" << "/format:list");
    p2.waitForFinished(5000);
    QString info = p2.readAllStandardOutput();
    appendLog("\u0418\u043d\u0444\u043e \u043e \u043f\u0440\u043e\u0446\u0435\u0441\u0441\u0435:");
    QStringList infoLines = info.split(QRegularExpression("\r\n|\r|\n"));
    for (const QString& line : infoLines) {
        if (!line.trimmed().isEmpty()) {
            appendLog("  " + line.trimmed());
        }
    }

    // List loaded modules
    if (m_verboseScan->isChecked()) {
        appendLog("\u0417\u0430\u0433\u0440\u0443\u0436\u0435\u043d\u043d\u044b\u0435 \u043c\u043e\u0434\u0443\u043b\u0438 (related):");
        QProcess p3;
        p3.start("tasklist", QStringList() << "/M" << "/FI" << QString("PID eq %1").arg(pid) << "/FO" << "CSV" << "/NH");
        p3.waitForFinished(5000);
        QString mods = p3.readAllStandardOutput();
        QStringList modLines = mods.split(QRegularExpression("\r\n|\r|\n"));
        int count = 0;
        for (const QString& line : modLines) {
            if (line.contains("gpu_hook") || line.contains("d3d") || line.contains("dxcore") ||
                line.contains("DUI70") || line.contains("DirectUI")) {
                appendLog("  " + line.trimmed());
                count++;
                if (count > 30) {
                    appendLog("  ... (\u043e\u0433\u0440\u0430\u043d\u0438\u0447\u0435\u043d\u043e)");
                    break;
                }
            }
        }

        // Check debug log
        QString altLog = QDir::toNativeSeparators(QCoreApplication::applicationDirPath() + "/../gpu_debug.log");
        QFileInfo fi(altLog);
        if (fi.exists()) {
            appendLog("\u0420\u0430\u0437\u043c\u0435\u0440 \u043b\u043e\u0433-\u0444\u0430\u0439\u043b\u0430: " +
                      QString::number(fi.size()) + " \u0431\u0430\u0439\u0442 (\u0438\u0437\u043c\u0435\u043d\u0451\u043d: " +
                      fi.lastModified().toString("hh:mm:ss") + ")");
        }
    }

    appendLog("=== \u0421\u043a\u0430\u043d\u0438\u0440\u043e\u0432\u0430\u043d\u0438\u0435 \u0437\u0430\u0432\u0435\u0440\u0448\u0435\u043d\u043e ===");
}