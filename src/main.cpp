#include <QApplication>
#include <QFont>
#include <QPalette>
#include "mainwindow.h"
int main(int argc, char *argv[]) {

    // QApplication управляет виджетами и общей очередью событий.
    QApplication app(argc, argv);
    app.setApplicationName("SceneButtons");
    app.setApplicationVersion("1.0");
    app.setStyle("Fusion");

    // Фиксируем светлую палитру с тёмным текстом.
    QPalette palette;

    // Одинаково читаемые цвета для активного, неактивного и отключённого состояния.
    for (auto group : {QPalette::Active, QPalette::Inactive, QPalette::Disabled}) {
        palette.setColor(group, QPalette::Window, QColor("#f3f5f8"));
        palette.setColor(group, QPalette::WindowText, QColor("#17212f"));
        palette.setColor(group, QPalette::Base, QColor("#ffffff"));
        palette.setColor(group, QPalette::AlternateBase, QColor("#f1f5fa"));
        palette.setColor(group, QPalette::Text, QColor("#17212f"));
        palette.setColor(group, QPalette::Button, QColor("#e6ebf2"));
        palette.setColor(group, QPalette::ButtonText, QColor("#17212f"));
        palette.setColor(group, QPalette::Highlight, QColor("#c7e0ff"));
        palette.setColor(group, QPalette::HighlightedText, QColor("#10243b"));
        palette.setColor(group, QPalette::ToolTipBase, QColor("#fff7dc"));
        palette.setColor(group, QPalette::ToolTipText, QColor("#17212f"));
        palette.setColor(group, QPalette::PlaceholderText, QColor("#526173"));
    }
    palette.setColor(QPalette::Disabled, QPalette::Base, QColor("#edf0f4"));
    palette.setColor(QPalette::Disabled, QPalette::Button, QColor("#edf0f4"));
    app.setPalette(palette);
    QFont font = app.font(); font.setPointSize(10); app.setFont(font);
    MainWindow window; window.show();

    // Цикл событий обрабатывает нажатия кнопок, перерисовку и сигналы таймера.
    return app.exec();
}
