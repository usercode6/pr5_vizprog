#pragma once
#include <QGraphicsProxyWidget>
class QPushButton;

enum class ButtonKind { Close, AddClose, RemoveClose };

// Общая оболочка: QPushButton становится полноценным объектом сцены.
class SceneButtonItem : public QGraphicsProxyWidget {
    Q_OBJECT
public:
    SceneButtonItem(ButtonKind kind, int id, int number, const QString &caption, const QString &color);
    ButtonKind kind() const { return m_kind; }
    int id() const { return m_id; }
    QString listName() const;
    QPushButton *button() const { return m_button; }
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr) override;
signals:
    void activated(int id);
private:
    ButtonKind m_kind;
    int m_id;
    int m_number;
    QPushButton *m_button;
};

// Разные типы представлены отдельными классами.
class CloseButtonItem final : public SceneButtonItem {
public:
    CloseButtonItem(int id, int number);
};
class AddCloseButtonItem final : public SceneButtonItem {
public:
    AddCloseButtonItem(int id, int number);
};
class RemoveCloseButtonsItem final : public SceneButtonItem {
public:
    RemoveCloseButtonsItem(int id, int number);
};
