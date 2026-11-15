#include "scenecontroller.h"
#include "buttoneventfilter.h"
#include <QGraphicsScene>
#include <QGraphicsView>

SceneController::SceneController(QGraphicsScene *scene, QGraphicsView *view, QObject *parent): QObject(parent), m_scene(scene), m_filter(new ButtonEventFilter(view, this)) {
    connect(m_filter, &ButtonEventFilter::selected, this, &SceneController::select);

    // Выполняем действия после завершения обработки события мыши. Это позволяет безопасно удалить объект без выполнения действия.
    connect(m_filter, &ButtonEventFilter::activated, this, &SceneController::activate, Qt::QueuedConnection);
    connect(m_filter, &ButtonEventFilter::errorRequested, this, &SceneController::errorRequested, Qt::QueuedConnection);
    connect(scene, &QGraphicsScene::selectionChanged, this, [this] {
        int id = -1;
        for (auto *graphicsItem : m_scene->selectedItems()) {
            if (auto *buttonItem = qgraphicsitem_cast<QGraphicsProxyWidget*>(graphicsItem)) {
                if (auto *ownItem = qobject_cast<SceneButtonItem*>(buttonItem)) { id = ownItem->id(); break; }
            }
        }
        if (id != m_lastSelected) { m_lastSelected = id; emit selectedChanged(id); }
    });
}
SceneButtonItem *SceneController::add(ButtonKind kind) {
    const int id = m_nextId++;
    const int number = ++m_numbers[static_cast<int>(kind)];
    SceneButtonItem *created = nullptr;
    switch (kind) {
    case ButtonKind::Close: created = new CloseButtonItem(id, number); break;
    case ButtonKind::AddClose: created = new AddCloseButtonItem(id, number); break;
    case ButtonKind::RemoveClose: created = new RemoveCloseButtonsItem(id, number); break;
    }
    m_scene->addItem(created);
    m_items.insert(id, created);
    m_filter->watch(created);
    connect(created, &SceneButtonItem::activated, this, &SceneController::activate, Qt::QueuedConnection);

    // Сначала заполняем свободные ячейки. Если все заняты, допускаем наложение: количество объектов не ограничено размером сцены или числом ячеек.
    const QRectF area = m_scene->sceneRect();
    const int columns = qMax(1, static_cast<int>((area.width() - 40) / 280));
    const int rows = qMax(1, static_cast<int>((area.height() - 40) / 90));
    QPointF position(30, 30);
    for (int slot = 0; slot < columns * rows; ++slot) {
        position = QPointF(30 + (slot % columns) * 280, 30 + (slot / columns) * 90);
        const QRectF candidate(position, created->size());
        bool occupied = false;
        for (auto *other : m_items)
            if (other != created && other->sceneBoundingRect().intersects(candidate)) { occupied = true; break; }
        if (!occupied) break;
    }
    created->setPos(position);
    created->setZValue(id); // Последний добавленный объект остаётся доступен сверху.
    emit objectsChanged();
    select(id);
    return created;
}
void SceneController::select(int id) {
    auto *selected = item(id);
    if (!selected) { m_scene->clearSelection(); return; }
    if (selected->isSelected() && m_scene->selectedItems().size() == 1) return;
    m_scene->clearSelection();
    selected->setSelected(true);
}
void SceneController::remove(int id) {
    auto *removed = m_items.take(id);
    if (!removed) return;
    m_scene->removeItem(removed);
    removed->deleteLater();
    emit objectsChanged();
}
void SceneController::removeCloseButtons() {
    const auto ids = m_items.keys();
    for (int id : ids) if (item(id)->kind() == ButtonKind::Close) remove(id);
}
void SceneController::activate(int id) {
    auto *source = item(id);
    if (!source) return; // К моменту обработки отложенного сигнала объект мог быть удалён.
    switch (source->kind()) {
    case ButtonKind::Close: emit closeRequested(); break;
    case ButtonKind::AddClose: add(ButtonKind::Close); break;
    case ButtonKind::RemoveClose: removeCloseButtons(); break;
    }
}
