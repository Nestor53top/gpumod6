#pragma once
#include <QMainWindow>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QComboBox>
#include <QProcess>
#include <QTimer>
#include <windows.h>

class SideDrawer;
class SwapTab;
class LogsTab;
class AboutTab;
class AutostartTab;

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

private slots:
    void onSwapClicked();
    void onRestoreClicked();
    void onRestartAndApply();
    void checkTaskmgr();

private:
    void setupUi();
    void applyStyle();
    bool isTaskmgrRunning();
    bool isAdmin();
    QString getBaseDir();
    void writeConfig(const QString& name);
    void killTaskmgr();
    void launchAndInject();
    QString extractDll();
    DWORD findProcess(const wchar_t* name);
    bool injectDll(DWORD pid, const wchar_t* dllPath);

    SideDrawer* m_drawer;
    SwapTab* m_swapTab;
    LogsTab* m_logsTab;
    AboutTab* m_aboutTab;
    AutostartTab* m_autostartTab;
    QTimer* m_checkTimer;
    QString m_dllPath;
};