#include "mainwindow.h"
#include "scenecontroller.h"
#include <QComboBox>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QListWidget>
#include <QMessageBox>
#include <QPushButton>
#include <QSignalBlocker>
#include <QSplitter>
#include <QStatusBar>
#include <QVBoxLayout>

// Конструктор главного окна.
MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    setWindowTitle(tr("Графическая сцена"));
    resize(1180, 760);
    setMinimumSize(950, 600);
    auto *central = new QWidget(this); setCentralWidget(central);
    auto *layout = new QVBoxLayout(central);
    layout->setContentsMargins(16, 16, 16, 12); layout->setSpacing(14);
    auto *toolbar = new QHBoxLayout;
    toolbar->addWidget(new QLabel(tr("Тип объекта:"), central));
    m_type = new QComboBox(central); m_type->setObjectName("objectType");

    // Добавление на созданный Layout объектов кнопок.
    m_type->addItem(tr("1. Закрыть программу"), static_cast<int>(ButtonKind::Close));
    m_type->addItem(tr("2. Добавить кнопку выхода"), static_cast<int>(ButtonKind::AddClose));
    m_type->addItem(tr("3. Удалить все кнопки выхода"), static_cast<int>(ButtonKind::RemoveClose));
    toolbar->addWidget(m_type, 1);
    auto *add = new QPushButton(tr("Добавить на сцену"), central); add->setObjectName("addObject");
    toolbar->addWidget(add); layout->addLayout(toolbar);

    // Создание разделения между прошлым Layout и основным виджетом сцены.
    auto *splitter = new QSplitter(Qt::Horizontal, central);
    auto *side = new QGroupBox(tr("Объекты сцены"), splitter);
    auto *sideLayout = new QVBoxLayout(side);

    // Создание списка добавленных объектов.
    m_list = new QListWidget(side); m_list->setObjectName("objectList");
    m_list->setAlternatingRowColors(true); sideLayout->addWidget(m_list);
    m_remove = new QPushButton(tr("Удалить выбранный"), side);
    m_remove->setObjectName("removeObject"); sideLayout->addWidget(m_remove);
    m_scene = new QGraphicsScene(this);
    m_scene->setSceneRect(0, 0, 900, 850);
    m_scene->setBackgroundBrush(QColor("#f4f7fc"));
    m_view = new QGraphicsView(m_scene, splitter); m_view->setObjectName("sceneView");
    m_view->setRenderHint(QPainter::Antialiasing);
    m_view->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    splitter->setSizes({250, 900}); splitter->setStretchFactor(0, 0); splitter->setStretchFactor(1, 1);
    layout->addWidget(splitter, 1);
    auto *hint = new QLabel(tr("Перетаскивание ЛКМ – переместить объект. Щелчок: типы 1 и 3 – ПКМ, тип 2 – ЛКМ."), central);
    hint->setWordWrap(true); layout->addWidget(hint);
    m_controller = new SceneController(m_scene, m_view, this);

    // Создание связи между сигналами от кнопок и слотами контроллера.
    connect(add, &QPushButton::clicked, this, [this] {
        auto *created = m_controller->add(static_cast<ButtonKind>(m_type->currentData().toInt()));
        m_view->ensureVisible(created, 30, 30);
    });
    connect(m_remove, &QPushButton::clicked, this, [this] {
        if (auto *row = m_list->currentItem()) m_controller->remove(row->data(Qt::UserRole).toInt());
    });
    connect(m_list, &QListWidget::currentItemChanged, this, [this](QListWidgetItem *row) {
        const int id = row ? row->data(Qt::UserRole).toInt() : -1;
        m_controller->select(id); m_remove->setEnabled(row != nullptr);
        if (auto *selected = m_controller->item(id)) m_view->ensureVisible(selected, 30, 30);
    });

    // Создание связей между сигналами контроллера и слотами основного виджета окна.
    connect(m_controller, &SceneController::objectsChanged, this, &MainWindow::rebuildList);
    connect(m_controller, &SceneController::selectedChanged, this, &MainWindow::syncSelection);
    connect(m_controller, &SceneController::closeRequested, this, &QWidget::close);
    connect(m_controller, &SceneController::errorRequested, this, &MainWindow::showClickError);
    setStyleSheet(
        "QWidget { color: #17212f; } QMainWindow { background: #eef2f7; }"
        "QGroupBox { border: 1px solid #cad3df; border-radius: 6px; margin-top: 10px; padding-top: 12px; }"
        "QGroupBox::title { subcontrol-origin: margin; left: 10px; padding: 0 4px; }"
        "QPushButton { color: #17212f; background: #e0eaf7; padding: 9px; border: 1px solid #9cadc2; border-radius: 4px; }"
        "QPushButton:disabled { color: #566475; background: #e5e9ef; }"
        "QPushButton:hover { background: #cee2fa; }"
        "QComboBox, QListWidget { color: #17212f; background: white; }"
        "QComboBox { padding: 7px; } QListWidget::item { padding: 10px; }"
        "QListWidget::item:selected { color: #10243b; background: #c7e0ff; }"
        "QToolTip { color: #17212f; background: #fff7dc; border: 1px solid #c5b879; }");
    rebuildList(); // Начальная сцена пуста; пользователь сам добавляет объекты.
}
MainWindow::~MainWindow() {
    // При закрытии окна виджеты уничтожаются раньше сцены. Отключаем связи, чтобы снятие выделения при удалении объектов не обращалось к списку.
    disconnect(m_scene, nullptr, m_controller, nullptr);
    disconnect(m_controller, nullptr, this, nullptr);
    disconnect(m_list, nullptr, this, nullptr);
}
// Функция добавления созданных объектов на сцену и изменения их количества.
void MainWindow::rebuildList() {
    int selectedId = -1;
    for (auto *item : m_controller->items()) if (item->isSelected()) selectedId = item->id();
    const QSignalBlocker blocker(m_list);
    m_list->clear();
    for (auto *item : m_controller->items()) {
        auto *row = new QListWidgetItem(item->listName(), m_list);
        row->setData(Qt::UserRole, item->id());
        row->setToolTip(item->button()->toolTip());
        if (item->id() == selectedId) m_list->setCurrentItem(row);
    }
    m_remove->setEnabled(m_list->currentItem() != nullptr);
    statusBar()->showMessage(tr("Объектов на сцене: %1").arg(m_controller->count()));
}

void MainWindow::syncSelection(int id) {
    const QSignalBlocker blocker(m_list); // Блокировка предотвращает повторное изменение выделения через сигнал списка.
    m_list->clearSelection(); m_list->setCurrentRow(-1);
    for (int row = 0; row < m_list->count(); ++row)
        if (m_list->item(row)->data(Qt::UserRole).toInt() == id) { m_list->setCurrentRow(row); break; }
    m_remove->setEnabled(m_list->currentItem() != nullptr);
}

//Ошибка при неверном нажатии.
void MainWindow::showClickError() {
    if (m_errorDialog) { m_errorDialog->raise(); return; }
    m_errorDialog = new QMessageBox(QMessageBox::Critical, tr("Ошибка"),
        tr("Для выполнения действия этой кнопки используйте правую кнопку мыши.\n" "Для перемещения удерживайте левую кнопку мыши и перетаскивайте объект."), QMessageBox::Ok, this);
    m_errorDialog->setAttribute(Qt::WA_DeleteOnClose);
    m_errorDialog->open();
}
