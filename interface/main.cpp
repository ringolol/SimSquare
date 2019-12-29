#include "gr_mainwin.h"
#include <QApplication>
#include "./logger/gr_logger.h"

//#include <QQuickView>
//#include "QtAndroidTools/QtAndroidTools.h"

int main(int argc, char *argv[])
{
    Gr_Logger().Get()<<"Start...";
    QApplication a(argc, argv);
    MainWindow w(&a);

    bool FullFlg=1;
    if(FullFlg)
    {
        Gr_Logger().Get()<<"Set fullscreen mode";
        w.showFullScreen();
    }
    else
    {
        Gr_Logger().Get()<<"Set window mode";
        w.show();
    }

    Gr_Logger().Get()<<"Return main";

    return a.exec();


    /*QQuickView view;
    view.setSource(QUrl::fromLocalFile(":/qml/interface/banner_test.qml"));
    view.setWidth(320);
    view.setHeight(50);
    QObject *object = (QObject*)view.rootObject();
    object->setProperty("width", 320);
    object->setProperty("height", 50);
    //QObject *banner1 = object->findChild<QObject*>("col")->findChild<QObject*>("rect")->findChild<QObject*>("banner1");
    QMetaObject::invokeMethod(object, "show");*/
    //view.showNormal();
}
