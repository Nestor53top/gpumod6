#include "titlebar.h"
#include <QHBoxLayout>
#include <QPainter>
#include <QMouseEvent>
#include <QStyle>

TitleBar::TitleBar(QWidget* parent) : QWidget(parent) {
    setFixedHeight(32);
    setAttribute(Qt::WA_TranslucentBackground, false);

    QHBoxLayout* layout = new QHBoxLayout(this);
    layout->setContentsMargins(12, 0, 0, 0);
    layout->setSpacing(0);

    m_iconLabel = new QLabel("\u26A1", this);
    m_iconLabel->setStyleSheet("color: #60cdff; font-size: 14px; background: transparent; padding-left: 2px;");
    m_iconLabel->setFixedWidth(20);
    layout->addWidget(m_iconLabel);

    m_titleLabel = new QLabel("GPU Name Swap", this);
    m_titleLabel->setStyleSheet(
        "color: #ffffff;"
        "font-family: 'Segoe UI Variable', 'Segoe UI';"
        "font-size: 12px;"
        "font-weight: 400;"
        "background: transparent;"
        "padding-left: 4px;"
    );
    layout->addWidget(m_titleLabel);

    layout->addStretch();

    QString btnStyle =
        "QPushButton {"
        "  background: transparent;"
        "  border: none;"
        "  color: #b0b0b0;"
        "  font-family: 'Segoe MDL2 Assets', 'Segoe UI Symbol';"
        "  font-size: 10px;"
        "  width: 46px; height: 32px;"
        "}"
        "QPushButton:hover { background: #333333; color: #ffffff; }"
        "QPushButton:pressed { background: #444444; }";

    QString closeBtnStyle =
        "QPushButton {"
        "  background: transparent;"
        "  border: none;"
        "  color: #b0b0b0;"
        "  font-family: 'Segoe MDL2 Assets', 'Segoe UI Symbol';"
        "  font-size: 10px;"
        "  width: 46px; height: 32px;"
        "}"
        "QPushButton:hover { background: #c42b1c; color: #ffffff; }"
        "QPushButton:pressed { background: #a02419; }";

    m_minBtn = new QPushButton("\uE921", this);
    m_minBtn->setStyleSheet(btnStyle);
    connect(m_minBtn, &QPushButton::clicked, this, &TitleBar::minimizeClicked);
    layout->addWidget(m_minBtn);

    m_maxBtn = new QPushButton("\uE922", this);
    m_maxBtn->setStyleSheet(btnStyle);
    connect(m_maxBtn, &QPushButton::clicked, this, &TitleBar::maximizeClicked);
    layout->addWidget(m_maxBtn);

    m_closeBtn = new QPushButton("\uE8BB", this);
    m_closeBtn->setStyleSheet(closeBtnStyle);
    connect(m_closeBtn, &QPushButton::clicked, this, &TitleBar::closeClicked);
    layout->addWidget(m_closeBtn);
}

void TitleBar::setTitle(const QString& title) {
    m_titleLabel->setText(title);
}

void TitleBar::paintEvent(QPaintEvent* event) {
    Q_UNUSED(event);
    QPainter p(this);
    p.fillRect(rect(), QColor(0x1f, 0x1f, 0x1f));
    p.setPen(QColor(0x2a, 0x2a, 0x2a));
    p.drawLine(0, height() - 1, width(), height() - 1);
}

void TitleBar::mousePressEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton) {
        m_dragPos = event->globalPosition().toPoint() - window()->frameGeometry().topLeft();
        event->accept();
    }
}

void TitleBar::mouseMoveEvent(QMouseEvent* event) {
    if (event->buttons() & Qt::LeftButton && !m_dragPos.isNull()) {
        window()->move(event->globalPosition().toPoint() - m_dragPos);
        event->accept();
    }
}

void TitleBar::mouseDoubleClickEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton) {
        emit maximizeClicked();
    }
}