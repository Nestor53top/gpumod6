#include "sidedrawer.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPainter>
#include <QPropertyAnimation>
#include <QParallelAnimationGroup>
#include <QEasingCurve>

SideDrawer::SideDrawer(QWidget* parent)
    : QWidget(parent), m_currentIndex(0), m_drawerWidth(160), m_collapsedWidth(56)
{
    setMinimumHeight(400);

    QHBoxLayout* mainLayout = new QHBoxLayout(this);
    mainLayout->setSpacing(0);
    mainLayout->setContentsMargins(0, 0, 0, 0);

    // Drawer (left side)
    m_drawer = new QWidget(this);
    m_drawer->setObjectName("drawerPanel");
    m_drawer->setFixedWidth(m_drawerWidth);
    m_drawer->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);

    m_drawerLayout = new QVBoxLayout(m_drawer);
    m_drawerLayout->setSpacing(1);
    m_drawerLayout->setContentsMargins(4, 4, 4, 4);

    m_buttonGroup = new QButtonGroup(this);
    m_buttonGroup->setExclusive(true);

    // Content (right side)
    m_content = new QWidget(this);
    m_content->setObjectName("contentPanel");
    m_content->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    QVBoxLayout* contentLayout = new QVBoxLayout(m_content);
    contentLayout->setSpacing(0);
    contentLayout->setContentsMargins(0, 0, 0, 0);

    m_stack = new QStackedWidget(this);
    contentLayout->addWidget(m_stack);

    mainLayout->addWidget(m_drawer);
    mainLayout->addWidget(m_content, 1);
}

void SideDrawer::addPage(const QString& label, const QString& icon, QWidget* page) {
    int index = m_stack->addWidget(page);

    QPushButton* btn = new QPushButton(this);
    btn->setCheckable(true);
    btn->setProperty("pageIndex", index);
    btn->setMinimumHeight(28);
    btn->setCursor(Qt::PointingHandCursor);

    QString btnText = QString("\u00A0\u00A0") + icon + QString("\u00A0\u00A0") + label;
    btn->setText(btnText);

    m_buttonGroup->addButton(btn, index);
    m_drawerLayout->addWidget(btn);
    m_buttons.append(btn);

    connect(btn, &QPushButton::toggled, this, [this, index](bool checked) {
        if (checked) {
            animateToPage(index);
        }
    });

    if (m_buttons.size() == 1) {
        btn->setChecked(true);
        m_currentIndex = 0;
    }
}

QWidget* SideDrawer::page(int index) const {
    return m_stack->widget(index);
}

void SideDrawer::onButtonClicked(int index) {
    animateToPage(index);
}

void SideDrawer::animateToPage(int index) {
    if (index < 0 || index >= m_stack->count()) return;
    m_currentIndex = index;

    QPropertyAnimation* anim = new QPropertyAnimation(m_stack, "currentIndex");
    anim->setDuration(200);
    anim->setStartValue(m_stack->currentIndex());
    anim->setEndValue(index);
    anim->setEasingCurve(QEasingCurve::InOutQuad);

    m_stack->setCurrentIndex(index);
    anim->start(QAbstractAnimation::DeleteWhenStopped);
    updateActiveButton();
}

void SideDrawer::updateLayout() {
    m_drawer->setFixedWidth(m_drawerWidth);
}

void SideDrawer::updateActiveButton() {
    for (int i = 0; i < m_buttons.size(); i++) {
        m_buttons[i]->setChecked(i == m_currentIndex);
    }
}