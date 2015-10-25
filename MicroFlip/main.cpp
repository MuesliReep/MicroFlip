//#include "mainwindow.h"
#include <QApplication>

#include "program.h"

int main(int argc, char *argv[])
{
  QApplication a(argc, argv);
//  MainWindow w;
//  w.show();

  Program *p = new Program();

  return a.exec();
}
