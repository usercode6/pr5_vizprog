#include "scenebutton.h"
#include <QPainter>
#include <QPushButton>
#include <QStyleOptionGraphicsItem>

SceneButtonItem::SceneButtonItem(ButtonKind kind, int id, int number, const QString &caption, const QString &color) : m_kind(kind), m_id(id), m_number(number), m_button(new QPushButton(caption)) {
    m_button->setFixedSize(250, 58);
    m_button->setCursor(Qt::OpenHandCursor);
    m_button->setStyleSheet(QString(
        "QPushButton { background: %1; color: #17212f; border: 1px solid #8796ab;"
        "border-radius: 6px; padding: 8px; font-size: 13px; }"
        "QPushButton:hover { border: 2px solid #3979bb; }"
        "QPushButton:pressed { background: #b9d6f5; }").arg(color));
    m_button->setToolTip(kind == ButtonKind::AddClose? tr("ЛКМ – добавить кнопку выхода. Перетаскивание ЛКМ – перемещение."): tr("ЛКМ – сообщение об ошибке. ПКМ – действие. Перетаскивание ЛКМ – перемещение."));

    // Сцена владеет оболочкой, а оболочка – встроенной кнопкой.
    setWidget(m_button);
    setFlag(QGraphicsItem::ItemIsSelectable);
    setCacheMode(QGraphicsItem::DeviceCoordinateCache);

    // Обычный clicked используется только у второго типа.
    if (kind == ButtonKind::AddClose)
        connect(m_button, &QPushButton::clicked, this, [this] { emit activated(m_id); });
}
QString SceneButtonItem::listName() const {
    QString name;
    switch (m_kind) {
    case ButtonKind::Close: name = tr("Выход"); break;
    case ButtonKind::AddClose: name = tr("Добавление"); break;
    case ButtonKind::RemoveClose: name = tr("Удаление"); break;
    }
    return tr("%1 №%2").arg(name).arg(m_number);
}
void SceneButtonItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
    QGraphicsProxyWidget::paint(painter, option, widget);
    if (isSelected()) {
        // Рамка рисуется внутри границ объекта и не меняет его геометрию.
        painter->save();
        painter->setRenderHint(QPainter::Antialiasing);
        painter->setPen(QPen(QColor("#1769b5"), 3, Qt::DashLine));
        painter->setBrush(Qt::NoBrush);
        painter->drawRoundedRect(rect().adjusted(2, 2, -2, -2), 5, 5);
        painter->restore();
    }
}
CloseButtonItem::CloseButtonItem(int id, int number): SceneButtonItem(ButtonKind::Close, id, number, tr("Закрыть программу"), "#f8d9d9") {}
AddCloseButtonItem::AddCloseButtonItem(int id, int number): SceneButtonItem(ButtonKind::AddClose, id, number, tr("Добавить кнопку выхода"), "#d9eddf") {}
RemoveCloseButtonsItem::RemoveCloseButtonsItem(int id, int number): SceneButtonItem(ButtonKind::RemoveClose, id, number, tr("Удалить все кнопки выхода"), "#ffedc9") {}
