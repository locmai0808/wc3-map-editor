#include "mainwindow.h"
#include <QApplication>
#include "dialogs/dgprojectselect.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QCoreApplication::setOrganizationName("ECS160ToolsW17");
    QCoreApplication::setApplicationName("MapEditor");

    DgProjectSelect p;
    if(p.exec() == QDialog::Rejected) return 0;

    MainWindow w;

    if(p.shouldLoad == true)
    {
        w.loadFile(p.filePath);
    }
    else
    {
        w.skipNewFile = false;
        w.newFile();
    }

    w.show();
    w.raise();
    w.activateWindow();

    return a.exec();
}
