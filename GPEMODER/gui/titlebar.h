#pragma once
#include <QWidget>
#include <QPushButton>
#include <QLabel>

class TitleBar : public QWidget {
    Q_OBJECT
public:
    explicit TitleBar(QWidget* parent = nullptr);

    void setTitle(const QString& title);
    QPushButton* minButton() const { return m_minBtn; }
    QPushButton* maxButton() const { return m_maxBtn; }
    QPushButton* closeButton() const { return m_closeBtn; }

signals:
    void minimizeClicked();
    void maximizeClicked();
    void closeClicked();

protected:
    void paintEvent(QPaintEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseDoubleClickEvent(QMouseEvent* event) override;

private:
    QLabel* m_iconLabel;
    QLabel* m_titleLabel;
    QPushButton* m_minBtn;
    QPushButton* m_maxBtn;
    QPushButton* m_closeBtn;
    QPoint m_dragPos;
};