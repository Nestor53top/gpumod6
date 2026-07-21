#pragma once
#include <QWidget>
#include <QPropertyAnimation>
#include <QParallelAnimationGroup>
#include <QPushButton>
#include <QButtonGroup>
#include <QStackedWidget>
#include <QHBoxLayout>
#include <QLabel>

class SideDrawer : public QWidget {
    Q_OBJECT
public:
    explicit SideDrawer(QWidget* parent = nullptr);

    void addPage(const QString& label, const QString& icon, QWidget* page);
    QWidget* page(int index) const;

    void setDrawerWidth(int w) { m_drawerWidth = w; updateLayout(); }

private slots:
    void onButtonClicked(int index);
    void animateToPage(int index);

private:
    void updateLayout();
    void updateActiveButton();

    QWidget* m_drawer;
    QWidget* m_content;
    QStackedWidget* m_stack;
    QVBoxLayout* m_drawerLayout;
    QList<QPushButton*> m_buttons;
    QButtonGroup* m_buttonGroup;
    int m_currentIndex;
    int m_drawerWidth;
    int m_collapsedWidth;
};