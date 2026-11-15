#pragma once
#include <QMainWindow>
#include <QPointer>
class QComboBox;
class QListWidget;
class QPushButton;
class QGraphicsScene;
class QGraphicsView;
class QMessageBox;
class SceneController;

// Главное окно отвечает за компоновку и синхронизацию списка со сценой.
class MainWindow final : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;
private:
    void rebuildList();
    void syncSelection(int id);
    void showClickError();
    QGraphicsScene *m_scene;
    QGraphicsView *m_view;
    SceneController *m_controller;
    QComboBox *m_type;
    QListWidget *m_list;
    QPushButton *m_remove;
    QPointer<QMessageBox> m_errorDialog;
};
