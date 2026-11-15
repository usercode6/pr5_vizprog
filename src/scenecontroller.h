#pragma once
#include <QObject>
#include <QMap>
#include "scenebutton.h"
class QGraphicsScene;
class QGraphicsView;
class ButtonEventFilter;

// Управляет объектами, но не зависит от списка и панели главного окна.
class SceneController final : public QObject {
    Q_OBJECT
public:
    SceneController(QGraphicsScene *scene, QGraphicsView *view, QObject *parent = nullptr);
    SceneButtonItem *add(ButtonKind kind);
    void select(int id);
    void remove(int id);
    void activate(int id);
    void removeCloseButtons();
    int count() const { return m_items.size(); }
    SceneButtonItem *item(int id) const { return m_items.value(id, nullptr); }
    const QMap<int, SceneButtonItem*> &items() const { return m_items; }
    ButtonEventFilter *filter() const { return m_filter; }
signals:
    void objectsChanged();
    void selectedChanged(int id);
    void closeRequested();
    void errorRequested();
private:
    QGraphicsScene *m_scene;
    ButtonEventFilter *m_filter;
    QMap<int, SceneButtonItem*> m_items;
    int m_nextId = 1;
    int m_numbers[3] = {0, 0, 0};
    int m_lastSelected = -1;
};
