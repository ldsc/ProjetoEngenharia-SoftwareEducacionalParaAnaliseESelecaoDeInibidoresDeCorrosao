#include <QApplication>

#include "Estilo.h"
#include "MainWindow.h"

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);
    app.setStyleSheet(ObterFolhaDeEstilos());

    MainWindow janela;
    janela.show();

    return app.exec();
}
