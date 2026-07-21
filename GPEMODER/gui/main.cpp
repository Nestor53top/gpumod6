#include <QApplication>
#include <QMessageBox>
#include <QProcess>
#include <QStandardPaths>
#include <QDir>
#include <QCoreApplication>
#include <QSysInfo>
#include "mainwindow.h"
#include <windows.h>
#include <shellapi.h>

static bool isElevated() {
    BOOL elevated = FALSE;
    HANDLE token = nullptr;
    if (OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &token)) {
        TOKEN_ELEVATION elev;
        DWORD size;
        if (GetTokenInformation(token, TokenElevation, &elev, sizeof(elev), &size)) {
            elevated = elev.TokenIsElevated;
        }
        CloseHandle(token);
    }
    return elevated != FALSE;
}

static bool relaunchElevated(int argc, char* argv[]) {
    wchar_t exePath[MAX_PATH];
    GetModuleFileNameW(nullptr, exePath, MAX_PATH);

    QString args;
    for (int i = 1; i < argc; i++) {
        args += QString(" \"%1\"").arg(QString::fromLocal8Bit(argv[i]));
    }

    HINSTANCE r = ShellExecuteW(nullptr, L"runas", exePath,
                                reinterpret_cast<LPCWSTR>(args.utf16()),
                                nullptr, SW_SHOWNORMAL);
    return reinterpret_cast<INT_PTR>(r) > 32;
}

int main(int argc, char* argv[]) {
    // Check elevation FIRST (before creating QApplication)
    bool wasElevated = isElevated();

    if (!wasElevated) {
        // Try to relaunch elevated
        if (relaunchElevated(argc, argv)) {
            // Successfully launched elevated instance — exit this one
            return 0;
        }
        // User denied UAC — continue running without elevation
    }

    // We're either already elevated or user denied UAC
    QApplication app(argc, argv);
    app.setApplicationName("GPU Name Swap");
    app.setOrganizationName("GPUSwap");
    app.setStyle("Fusion");

    MainWindow w;
    w.show();

    return app.exec();
}