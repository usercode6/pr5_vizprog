#include "buttoneventfilter.h"
#include <QApplication>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QMouseEvent>
#include <QPushButton>

ButtonEventFilter::ButtonEventFilter(QGraphicsView *view, QObject *parent): QObject(parent), m_view(view) {}
void ButtonEventFilter::watch(SceneButtonItem *item) {
    item->button()->installEventFilter(this);
    item->button()->setProperty("sceneItem", QVariant::fromValue(static_cast<QObject*>(item))); // Связь хранится на кнопке и не требует отдельной таблицы указателей.
}
QPointF ButtonEventFilter::scenePosition(const QPoint &globalPosition) const {
    return m_view->mapToScene(m_view->viewport()->mapFromGlobal(globalPosition));
}
bool ButtonEventFilter::eventFilter(QObject *watched, QEvent *event) {
    auto *button = qobject_cast<QPushButton*>(watched);
    if (!button) return QObject::eventFilter(watched, event);
    auto *item = qobject_cast<SceneButtonItem*>(button->property("sceneItem").value<QObject*>());
    if (!item) return QObject::eventFilter(watched, event);

    if (event->type() == QEvent::ContextMenu) return true;
    if (event->type() == QEvent::MouseButtonPress || event->type() == QEvent::MouseButtonDblClick) {
        auto *mouse = static_cast<QMouseEvent*>(event);
        if (mouse->button() != Qt::LeftButton && mouse->button() != Qt::RightButton) return false;
        emit selected(item->id());

        // ПКМ второго типа не должна выполнять основную функцию.
        if (mouse->button() == Qt::RightButton && item->kind() == ButtonKind::AddClose) {
            mouse->accept(); return true;
        }
        m_dragItem = item;
        m_pressedButton = mouse->button();
        m_pressGlobal = mouse->globalPosition().toPoint();
        m_pressScene = scenePosition(m_pressGlobal);
        m_startPosition = item->pos();
        m_dragging = false;

        // Второму типу оставляем штатную обработку нажатия QPushButton.
        if (item->kind() == ButtonKind::AddClose && m_pressedButton == Qt::LeftButton) return false;
        button->setDown(true);
        mouse->accept(); return true;
    }
    if (event->type() == QEvent::MouseMove && m_dragItem == item && m_pressedButton == Qt::LeftButton) {
        auto *mouse = static_cast<QMouseEvent*>(event);
        const QPoint global = mouse->globalPosition().toPoint();
        if (!m_dragging && (global - m_pressGlobal).manhattanLength() >= QApplication::startDragDistance())
            m_dragging = true;
        if (m_dragging) {
            button->setDown(false);
            button->setCursor(Qt::ClosedHandCursor);
            const QPointF position = m_startPosition + scenePosition(global) - m_pressScene;
            const QRectF area = item->scene()->sceneRect();

            // Оставляем объект целиком внутри рабочей области.
            item->setPos(qBound(area.left(), position.x(), area.right() - item->rect().width()), qBound(area.top(), position.y(), area.bottom() - item->rect().height()));
            mouse->accept(); return true;
        }
    }
    if (event->type() == QEvent::MouseButtonRelease && m_dragItem == item) {
        auto *mouse = static_cast<QMouseEvent*>(event);
        if (mouse->button() != m_pressedButton) return false;
        const bool dragged = m_dragging;
        const auto pressed = m_pressedButton;
        const bool inside = button->rect().contains(mouse->position().toPoint());
        m_dragItem.clear(); m_pressedButton = Qt::NoButton; m_dragging = false;
        button->setCursor(Qt::OpenHandCursor);
        if (!dragged && item->kind() == ButtonKind::AddClose && pressed == Qt::LeftButton)
            return false;
        button->setDown(false);
        if (!dragged && inside) {
            if (pressed == Qt::RightButton) emit activated(item->id());
            else emit errorRequested();
        }
        mouse->accept(); return true;
    }
    return QObject::eventFilter(watched, event);
}
