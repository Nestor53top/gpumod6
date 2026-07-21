#include "swaptab.h"
#include "gpu_database.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QTimer>
#include <QFile>
#include <QDir>
#include <QCoreApplication>
#include <QTextStream>

SwapTab::SwapTab(QWidget* parent) : QWidget(parent) {
    setupUi();
}

QString SwapTab::currentName() const {
    return m_nameInput->text().trimmed();
}

uint64_t SwapTab::currentVramBytes() const {
    QString sizeStr = m_vramSizeCombo->currentText();
    QString unitStr = m_vramUnitCombo->currentText();

    if (sizeStr.isEmpty() || sizeStr == "0") return 0;

    double value = sizeStr.toDouble();
    if (unitStr == "KB") value *= 1024.0;
    else if (unitStr == "MB") value *= 1024.0 * 1024.0;
    else if (unitStr == "GB") value *= 1024.0 * 1024.0 * 1024.0;
    else if (unitStr == "TB") value *= 1024.0 * 1024.0 * 1024.0 * 1024.0;
    else if (unitStr == "PB") value *= 1024.0 * 1024.0 * 1024.0 * 1024.0 * 1024.0;

    return (uint64_t)value;
}

void SwapTab::setStatusText(const QString& text, bool ok) {
    m_statusLabel->setText(text);
    m_statusLabel->setStyleSheet(QString("color: %1; font-weight: 600; padding: 8px; border-radius: 4px; background: %2;")
        .arg(ok ? "#53d769" : "#e94560")
        .arg(ok ? "#1a3a24" : "#3a1a1a"));
}

void SwapTab::updateStatus(bool taskmgrRunning) {
    if (!m_taskmgrStatusLabel) return;
    m_taskmgrStatusLabel->setText(
        QString("\u041f\u0440\u043e\u0446\u0435\u0441\u0441 Task Manager: %1")
            .arg(taskmgrRunning ? "\u2713 \u0437\u0430\u043f\u0443\u0449\u0435\u043d (real-time \u0430\u043a\u0442\u0438\u0432\u0435\u043d)" :
                                   "\u2717 \u043d\u0435 \u0437\u0430\u043f\u0443\u0449\u0435\u043d")
    );
    m_taskmgrStatusLabel->setStyleSheet(QString("color: %1; font-size: 11px; padding: 4px;")
        .arg(taskmgrRunning ? "#53d769" : "#888888"));
}

void SwapTab::onPresetChanged(const QString& text) {
    if (text.startsWith("--") || text.isEmpty()) return;
    m_nameInput->setText(text);
    int vramMB = getDefaultVramMB(text);
    if (vramMB > 0) {
        if (vramMB >= 1024 && (vramMB % 1024 == 0)) {
            m_vramSizeCombo->setCurrentText(QString::number(vramMB / 1024));
            m_vramUnitCombo->setCurrentText("GB");
        } else {
            m_vramSizeCombo->setCurrentText(QString::number(vramMB));
            m_vramUnitCombo->setCurrentText("MB");
        }
    }
}

void SwapTab::setupUi() {
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(10);
    mainLayout->setContentsMargins(16, 12, 16, 12);

    // Status
    QGroupBox* statusGroup = new QGroupBox("\u0421\u0442\u0430\u0442\u0443\u0441");
    QVBoxLayout* statusLayout = new QVBoxLayout(statusGroup);
    m_taskmgrStatusLabel = new QLabel("\u041f\u0440\u043e\u0432\u0435\u0440\u043a\u0430...");
    statusLayout->addWidget(m_taskmgrStatusLabel);
    m_statusLabel = new QLabel("\u0413\u043e\u0442\u043e\u0432 \u043a \u0440\u0430\u0431\u043e\u0442\u0435");
    m_statusLabel->setStyleSheet("color: #e0e0e0; font-weight: 600;");
    statusLayout->addWidget(m_statusLabel);
    mainLayout->addWidget(statusGroup);

    // GPU Name
    QGroupBox* nameGroup = new QGroupBox("\u0418\u043c\u044f \u0432\u0438\u0434\u0435\u043e\u043a\u0430\u0440\u0442\u044b");
    QVBoxLayout* nameLayout = new QVBoxLayout(nameGroup);
    nameLayout->setSpacing(6);

    QLabel* searchLabel = new QLabel("\u041f\u043e\u0438\u0441\u043a \u043f\u043e \u0431\u0430\u0437\u0435 \u0438\u043b\u0438 \u043d\u0430\u0437\u0432\u0430\u043d\u0438\u044e:");
    nameLayout->addWidget(searchLabel);

    m_presetCombo = new QComboBox();
    m_presetCombo->setInsertPolicy(QComboBox::NoInsert);
    m_presetCombo->setEditable(true);
    m_presetCombo->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    QStringList allGpus = getAllGpuNames();
    allGpus.prepend("-- \u0412\u044b\u0431\u0435\u0440\u0438\u0442\u0435 \u0432\u0438\u0434\u0435\u043e\u043a\u0430\u0440\u0442\u0443 --");
    m_presetCombo->addItems(allGpus);

    m_completer = new QCompleter(allGpus, this);
    m_completer->setCaseSensitivity(Qt::CaseInsensitive);
    m_completer->setFilterMode(Qt::MatchContains);
    m_presetCombo->setCompleter(m_completer);

    connect(m_presetCombo, &QComboBox::currentTextChanged, this, &SwapTab::onPresetChanged);
    connect(m_presetCombo->lineEdit(), &QLineEdit::returnPressed, this, [this]() {
        QString text = m_presetCombo->currentText();
        if (!text.startsWith("--")) {
            m_nameInput->setText(text);
        }
    });
    nameLayout->addWidget(m_presetCombo);

    QLabel* customLabel = new QLabel("\u0421\u0432\u043e\u0451 \u043d\u0430\u0437\u0432\u0430\u043d\u0438\u0435:");
    nameLayout->addWidget(customLabel);

    m_nameInput = new QLineEdit();
    m_nameInput->setPlaceholderText("NVIDIA GeForce RTX 4090 Ti");
    nameLayout->addWidget(m_nameInput);

    mainLayout->addWidget(nameGroup);

    // VRAM
    QGroupBox* vramGroup = new QGroupBox("\u0412\u0438\u0434\u0435\u043e\u043f\u0430\u043c\u044f\u0442\u044c (VRAM)");
    QHBoxLayout* vramLayout = new QHBoxLayout(vramGroup);
    vramLayout->setSpacing(8);

    QLabel* vramSizeLabel = new QLabel("\u041e\u0431\u044a\u0451\u043c:");
    vramLayout->addWidget(vramSizeLabel);

    m_vramSizeCombo = new QComboBox();
    m_vramSizeCombo->setEditable(true);
    m_vramSizeCombo->setFixedWidth(80);
    QStringList sizes = { "0", "1", "2", "3", "4", "6", "8", "10", "12", "14", "16", "18", "20", "24", "32", "48", "64", "96", "128", "256", "512" };
    m_vramSizeCombo->addItems(sizes);
    m_vramSizeCombo->setCurrentText("4");
    vramLayout->addWidget(m_vramSizeCombo);

    m_vramUnitCombo = new QComboBox();
    m_vramUnitCombo->setFixedWidth(70);
    QStringList units = { "GB", "MB", "KB", "TB", "PB" };
    m_vramUnitCombo->addItems(units);
    m_vramUnitCombo->setCurrentText("GB");
    vramLayout->addWidget(m_vramUnitCombo);

    vramLayout->addStretch();
    mainLayout->addWidget(vramGroup);

    // Buttons
    QHBoxLayout* btnLayout = new QHBoxLayout();
    btnLayout->setSpacing(8);
    m_restoreBtn = new QPushButton("\u0412\u043e\u0441\u0441\u0442\u0430\u043d\u043e\u0432\u0438\u0442\u044c");
    m_restoreBtn->setObjectName("restoreBtn");
    connect(m_restoreBtn, &QPushButton::clicked, this, &SwapTab::restoreRequested);
    btnLayout->addWidget(m_restoreBtn);

    m_injectBtn = new QPushButton("\u041f\u0440\u0438\u043c\u0435\u043d\u0438\u0442\u044c");
    m_injectBtn->setObjectName("applyBtn");
    connect(m_injectBtn, &QPushButton::clicked, this, &SwapTab::injectRequested);
    btnLayout->addWidget(m_injectBtn);

    m_restartBtn = new QPushButton("\u041f\u0440\u0438\u043c\u0435\u043d\u0438\u0442\u044c \u0438 \u043f\u0435\u0440\u0435\u0437\u0430\u043f\u0443\u0441\u0442\u0438\u0442\u044c");
    m_restartBtn->setObjectName("applyBtn");
    m_restartBtn->setToolTip("\u0417\u0430\u043f\u0438\u0441\u0430\u0442\u044c \u0438 \u043f\u0435\u0440\u0435\u0437\u0430\u043f\u0443\u0441\u0442\u0438\u0442\u044c Task Manager");
    connect(m_restartBtn, &QPushButton::clicked, this, &SwapTab::restartAndApply);
    btnLayout->addWidget(m_restartBtn);

    mainLayout->addLayout(btnLayout);
    mainLayout->addStretch();
}