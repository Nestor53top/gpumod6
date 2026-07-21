#include "mainwindow.h"
#include "sidedrawer.h"
#include "swaptab.h"
#include "logstab.h"
#include "abouttab.h"
#include "autostarttab.h"
#include <QPainter>
#include <QVBoxLayout>
#include <QApplication>
#include <QProcess>
#include <QDir>
#include <QCoreApplication>
#include <QSettings>
#include <QMessageBox>
#include <QTimer>
#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <QThread>
#include <QTemporaryFile>
#include <windows.h>
#include <tlhelp32.h>
#include <dwmapi.h>

void qDebugMsgHandler(QtMsgType type, const QMessageLogContext& ctx, const QString& msg) {
    if (msg.contains("QFont::setPointSize")) return;
    fprintf(stderr, "%s\n", msg.toLocal8Bit().constData());
}

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent) {
    qInstallMessageHandler(qDebugMsgHandler);
    setWindowTitle("GPU Name Swap");
    setMinimumSize(680, 520);
    resize(720, 560);

    m_dllPath = extractDll();

    setupUi();

    HWND hwnd = (HWND)this->winId();
    BOOL darkMode = TRUE;
    DwmSetWindowAttribute(hwnd, 20 /*DWMWA_USE_IMMERSIVE_DARK_MODE*/, &darkMode, sizeof(darkMode));
    MARGINS margins = {0, 0, 0, 1};
    DwmExtendFrameIntoClientArea(hwnd, &margins);

    m_checkTimer = new QTimer(this);
    connect(m_checkTimer, &QTimer::timeout, this, &MainWindow::checkTaskmgr);
    m_checkTimer->start(2000);
    checkTaskmgr();
}

MainWindow::~MainWindow() {}

QString MainWindow::extractDll() {
    QFile res(":/gpu_hook.dll");
    if (!res.exists()) {
        QMessageBox::critical(this, "Error", "DLL resource not found in EXE!");
        return "";
    }

    QString dllDir = getBaseDir();
    QString dllPath = dllDir + "/gpu_hook.dll";

    if (res.open(QIODevice::ReadOnly)) {
        QByteArray data = res.readAll();
        QFile out(dllPath);
        if (out.open(QIODevice::WriteOnly)) {
            out.write(data);
            out.close();
            res.close();
            return dllPath;
        }
        res.close();
    }

    QMessageBox::critical(this, "Error", "Failed to extract DLL!");
    return "";
}

void MainWindow::setupUi() {
    applyStyle();

    QWidget* central = new QWidget(this);
    setCentralWidget(central);
    central->setObjectName("centralWidget");

    QVBoxLayout* mainLayout = new QVBoxLayout(central);
    mainLayout->setSpacing(0);
    mainLayout->setContentsMargins(0, 0, 0, 0);

    m_drawer = new SideDrawer(this);

    m_swapTab = new SwapTab(this);
    m_logsTab = new LogsTab(this);
    m_aboutTab = new AboutTab(this);
    m_autostartTab = new AutostartTab(this);

    m_drawer->addPage("\u041f\u043e\u0434\u043c\u0435\u043d\u0430", "\u26A1", m_swapTab);
    m_drawer->addPage("\u041b\u043e\u0433\u0438", "\u2630", m_logsTab);
    m_drawer->addPage("\u0410\u0432\u0442\u043e\u0437\u0430\u0433\u0440\u0443\u043a\u0430", "\u2699", m_autostartTab);
    m_drawer->addPage("\u041e \u043f\u0440\u043e\u0433\u0440\u0430\u043c\u043c\u0435", "\u2139", m_aboutTab);

    mainLayout->addWidget(m_drawer, 1);

    connect(m_swapTab, &SwapTab::injectRequested, this, &MainWindow::onSwapClicked);
    connect(m_swapTab, &SwapTab::restoreRequested, this, &MainWindow::onRestoreClicked);
    connect(m_swapTab, &SwapTab::restartAndApply, this, &MainWindow::onRestartAndApply);
}

void MainWindow::applyStyle() {
    setStyleSheet(
        "QMainWindow { background: #202020; }"
        "QWidget#centralWidget { background: #202020; }"

        "QWidget#drawerPanel {"
        "  background: #1c1c1c;"
        "  border-right: 1px solid #2a2a2a;"
        "}"

        "QWidget#contentPanel {"
        "  background: #202020;"
        "}"

        "QPushButton {"
        "  text-align: left;"
        "  padding: 7px 12px;"
        "  border: none;"
        "  border-radius: 4px;"
        "  background: transparent;"
        "  color: #a0a0a0;"
        "  font-family: 'Segoe UI', sans-serif;"
        "  font-size: 12px;"
        "  font-weight: 600;"
        "}"
        "QPushButton:hover {"
        "  background: #2a2a2a;"
        "  color: #ffffff;"
        "}"
        "QPushButton:checked {"
        "  background: qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0 #60cdff, stop:1 #0078d4);"
        "  color: #ffffff;"
        "}"

        "QGroupBox {"
        "  border: 1px solid #2a2a2a;"
        "  border-radius: 6px;"
        "  margin-top: 14px;"
        "  padding: 14px 12px 12px 12px;"
        "  font-family: 'Segoe UI', sans-serif;"
        "  font-size: 12px;"
        "  font-weight: 600;"
        "  color: #60cdff;"
        "}"
        "QGroupBox::title {"
        "  subcontrol-origin: margin;"
        "  subcontrol-position: top left;"
        "  left: 12px;"
        "  padding: 0 6px;"
        "  background: #202020;"
        "}"
        "QLabel { color: #e0e0e0; font-family: 'Segoe UI', sans-serif; font-size: 12px; }"
        "QLineEdit, QComboBox {"
        "  background: #2d2d2d; border: 1px solid #3a3a3a; border-radius: 4px;"
        "  color: #ffffff; padding: 6px 8px; font-family: 'Segoe UI', sans-serif; font-size: 12px;"
        "  selection-background-color: #0078d4;"
        "}"
        "QLineEdit:focus, QComboBox:focus { border: 1px solid #60cdff; }"
        "QComboBox::drop-down { border: none; width: 24px; }"
        "QComboBox QAbstractItemView {"
        "  background: #2d2d2d; color: #ffffff; selection-background-color: #0078d4;"
        "  selection-color: #ffffff; border: 1px solid #3a3a3a; outline: 0; padding: 4px;"
        "}"

        "QPushButton#applyBtn {"
        "  background: qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0 #60cdff, stop:1 #0078d4);"
        "  color: white; padding: 8px 20px; font-weight: 700; border-radius: 4px;"
        "  border: none; text-align: center;"
        "}"
        "QPushButton#applyBtn:hover { background: qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0 #80d8ff, stop:1 #1a8ae0); }"
        "QPushButton#applyBtn:pressed { background: #005a9e; }"

        "QPushButton#restoreBtn {"
        "  background: #2d2d2d; color: #e0e0e0; padding: 8px 20px; font-weight: 600;"
        "  border: 1px solid #3a3a3a; border-radius: 4px; text-align: center;"
        "}"
        "QPushButton#restoreBtn:hover { background: #353535; border: 1px solid #4a4a4a; }"

        "QCheckBox {"
        "  color: #e0e0e0; font-family: 'Segoe UI', sans-serif; font-size: 12px; spacing: 6px;"
        "}"
        "QCheckBox::indicator {"
        "  width: 16px; height: 16px; border: 1px solid #4a4a4a; border-radius: 3px; background: #2d2d2d;"
        "}"
        "QCheckBox::indicator:hover { border: 1px solid #60cdff; }"
        "QCheckBox::indicator:checked { background: #60cdff; border: 1px solid #60cdff; }"

        "QScrollBar:vertical { background: #202020; width: 12px; border: none; }"
        "QScrollBar::handle:vertical { background: #3a3a3a; border-radius: 4px; min-height: 30px; margin: 2px; }"
        "QScrollBar::handle:vertical:hover { background: #4a4a4a; }"
        "QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical { height: 0; }"
    );
}

bool MainWindow::isTaskmgrRunning() {
    QProcess p;
    p.start("tasklist", QStringList() << "/FI" << "IMAGENAME eq Taskmgr.exe" << "/NH");
    p.waitForFinished(2000);
    return p.readAllStandardOutput().contains("Taskmgr.exe");
}

bool MainWindow::isAdmin() {
    QProcess p;
    p.start("net", QStringList() << "session");
    p.waitForFinished(2000);
    return p.exitCode() == 0;
}

void MainWindow::checkTaskmgr() {
    bool running = isTaskmgrRunning();
    if (m_swapTab) m_swapTab->updateStatus(running);
}

QString MainWindow::getBaseDir() {
    return QDir::toNativeSeparators(QCoreApplication::applicationDirPath());
}

void MainWindow::writeConfig(const QString& name) {
    SIZE_T vram = m_swapTab->currentVramBytes();
    QString configPath = getBaseDir() + "/gpu_swap.json";
    QFile f(configPath);
    if (f.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream ts(&f);
        ts << "{\n  \"mode\": \"rename_all\",\n  \"rename_to\": \"" << name << "\",\n  \"vram\": " << vram << ",\n  \"enabled\": true\n}\n";
        f.close();
    }
}

DWORD MainWindow::findProcess(const wchar_t* name) {
    HANDLE snap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (snap == INVALID_HANDLE_VALUE) return 0;

    PROCESSENTRY32W pe = {};
    pe.dwSize = sizeof(pe);

    if (Process32FirstW(snap, &pe)) {
        do {
            if (_wcsicmp(pe.szExeFile, name) == 0) {
                CloseHandle(snap);
                return pe.th32ProcessID;
            }
        } while (Process32NextW(snap, &pe));
    }
    CloseHandle(snap);
    return 0;
}

bool MainWindow::injectDll(DWORD pid, const wchar_t* dllPath) {
    HANDLE hProc = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
    if (!hProc) {
        m_logsTab->appendLog(QString("Cannot open process %1 (error %2)").arg(pid).arg(GetLastError()));
        return false;
    }

    size_t pathSize = (wcslen(dllPath) + 1) * sizeof(wchar_t);
    LPVOID remoteMem = VirtualAllocEx(hProc, nullptr, pathSize, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    if (!remoteMem) {
        m_logsTab->appendLog("VirtualAllocEx failed");
        CloseHandle(hProc);
        return false;
    }

    if (!WriteProcessMemory(hProc, remoteMem, dllPath, pathSize, nullptr)) {
        m_logsTab->appendLog("WriteProcessMemory failed");
        VirtualFreeEx(hProc, remoteMem, 0, MEM_RELEASE);
        CloseHandle(hProc);
        return false;
    }

    HMODULE hKernel32 = GetModuleHandleW(L"kernel32.dll");
    FARPROC pLoadLibraryW = GetProcAddress(hKernel32, "LoadLibraryW");

    HANDLE hThread = CreateRemoteThread(hProc, nullptr, 0,
        (LPTHREAD_START_ROUTINE)pLoadLibraryW, remoteMem, 0, nullptr);

    if (!hThread) {
        m_logsTab->appendLog("CreateRemoteThread failed");
        VirtualFreeEx(hProc, remoteMem, 0, MEM_RELEASE);
        CloseHandle(hProc);
        return false;
    }

    WaitForSingleObject(hThread, 5000);

    DWORD exitCode = 0;
    GetExitCodeThread(hThread, &exitCode);

    CloseHandle(hThread);
    VirtualFreeEx(hProc, remoteMem, 0, MEM_RELEASE);
    CloseHandle(hProc);

    if (exitCode == 0) {
        m_logsTab->appendLog("LoadLibraryW returned NULL - DLL load failed");
        return false;
    }

    m_logsTab->appendLog(QString("DLL injected! Module: 0x%1").arg((quintptr)exitCode, 0, 16));
    return true;
}

void MainWindow::killTaskmgr() {
    QProcess::execute("taskkill", QStringList() << "/F" << "/IM" << "Taskmgr.exe");
    QApplication::processEvents();
    QThread::msleep(500);
}

void MainWindow::launchAndInject() {
    if (m_dllPath.isEmpty()) return;

    DWORD pid = findProcess(L"Taskmgr.exe");

    if (pid == 0) {
        m_logsTab->appendLog("Launching Taskmgr.exe...");
        STARTUPINFOW si = { sizeof(si) };
        PROCESS_INFORMATION pi = {};
        wchar_t cmdLine[] = L"Taskmgr.exe";
        if (CreateProcessW(nullptr, cmdLine, nullptr, nullptr, FALSE, 0, nullptr, nullptr, &si, &pi)) {
            pid = pi.dwProcessId;
            CloseHandle(pi.hProcess);
            CloseHandle(pi.hThread);
            m_logsTab->appendLog(QString("Taskmgr started, PID %1").arg(pid));
            QThread::msleep(500);
        } else {
            m_logsTab->appendLog(QString("Failed to launch Taskmgr (error %1)").arg(GetLastError()));
            return;
        }
    }

    if (pid == 0) {
        m_logsTab->appendLog("Taskmgr not found");
        return;
    }

    m_logsTab->appendLog(QString("Injecting into PID %1...").arg(pid));

    std::wstring dllW = m_dllPath.toStdWString();
    if (injectDll(pid, dllW.c_str())) {
        m_logsTab->appendLog("SUCCESS! GPU name will be swapped.");
    } else {
        m_logsTab->appendLog("FAILED to inject DLL.");
    }
}

void MainWindow::onSwapClicked() {
    QString name = m_swapTab->currentName();
    if (name.isEmpty()) name = "NVIDIA GeForce RTX 4090 Ti";

    writeConfig(name);
    m_logsTab->appendLog(QString("Written to gpu_swap.json: '%1'").arg(name));

    if (isTaskmgrRunning()) {
        m_logsTab->appendLog("Task Manager running - name updates via monitor thread");
        m_swapTab->setStatusText("Applied: " + name + " (real-time)", true);
    } else {
        m_logsTab->appendLog("Task Manager not running. Launching...");
        launchAndInject();
        m_swapTab->setStatusText("Launched with: " + name, true);
    }
}

void MainWindow::onRestoreClicked() {
    QString name = "NVIDIA GeForce GTX 1650";
    QString configPath = getBaseDir() + "/gpu_swap.json";
    QFile f(configPath);
    if (f.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream ts(&f);
        ts << "{\n  \"mode\": \"rename_all\",\n  \"rename_to\": \"" << name << "\",\n  \"vram\": 0,\n  \"enabled\": true\n}\n";
        f.close();
    }
    m_logsTab->appendLog(QString("Restoring original: '%1' (VRAM: auto)").arg(name));

    if (isTaskmgrRunning()) {
        m_logsTab->appendLog("Real-time: name restored in ~1 second");
        m_swapTab->setStatusText("Restored: " + name, true);
    }
}

void MainWindow::onRestartAndApply() {
    QString name = m_swapTab->currentName();
    if (name.isEmpty()) name = "NVIDIA GeForce RTX 4090 Ti";

    m_logsTab->appendLog("Restarting Task Manager...");
    m_swapTab->setStatusText("Restarting...", true);
    QApplication::processEvents();

    writeConfig(name);

    if (isTaskmgrRunning()) {
        killTaskmgr();
        m_logsTab->appendLog("Task Manager stopped.");
    }

    launchAndInject();
    m_swapTab->setStatusText("Ready: " + name, true);
}