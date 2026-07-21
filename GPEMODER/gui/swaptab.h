#pragma once
#include <QWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QComboBox>
#include <QCompleter>
#include <cstdint>

class SwapTab : public QWidget {
    Q_OBJECT
public:
    explicit SwapTab(QWidget* parent = nullptr);

    QString currentName() const;
    uint64_t currentVramBytes() const;
    void setStatusText(const QString& text, bool ok);
    void updateStatus(bool taskmgrRunning);

signals:
    void injectRequested();
    void restoreRequested();
    void restartAndApply();

private:
    void setupUi();
    void onPresetChanged(const QString& text);

    QComboBox* m_presetCombo;
    QLineEdit* m_nameInput;
    QComboBox* m_vramSizeCombo;
    QComboBox* m_vramUnitCombo;
    QPushButton* m_injectBtn;
    QPushButton* m_restoreBtn;
    QPushButton* m_restartBtn;
    QLabel* m_statusLabel;
    QLabel* m_taskmgrStatusLabel;
    QCompleter* m_completer;
};