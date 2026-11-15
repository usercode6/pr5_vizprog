#pragma once
#include <QObject>
#include <QPointer>
#include <QPointF>
#include "scenebutton.h"
class QGraphicsView;

// Фильтр установлен на каждую встроенную QPushButton. Он различает щелчок и перенос, чтобы перенос не выполнял действие кнопки.
class ButtonEventFilter final : public QObject {
    Q_OBJECT
public:
    explicit ButtonEventFilter(QGraphicsView *view, QObject *parent = nullptr);
    void watch(SceneButtonItem *item);
protected:
    bool eventFilter(QObject *watched, QEvent *event) override;
signals:
    void selected(int id);
    void activated(int id);
    void errorRequested();
private:
    QPointF scenePosition(const QPoint &globalPosition) const;
    QGraphicsView *m_view;
    QPointer<SceneButtonItem> m_dragItem;
    QPoint m_pressGlobal;
    QPointF m_pressScene;
    QPointF m_startPosition;
    Qt::MouseButton m_pressedButton = Qt::NoButton;
    bool m_dragging = false;
};
