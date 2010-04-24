#include <QtGui>
#include <QApplication>

#include "hexgui.h"

int main(int argc, char *argv[])
{
  Q_INIT_RESOURCE(images);

  QApplication a(argc, argv);

  HexGui hexgui;
  hexgui.show();
  return a.exec();
}
