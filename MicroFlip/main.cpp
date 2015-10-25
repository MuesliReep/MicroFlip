//#include "mainwindow.h"
#include <QCoreApplication>

#include "program.h"

int main(int argc, char *argv[])
{
  QCoreApplication a(argc, argv);
//  MainWindow w;
//  w.show();

  Program *p = new Program();

  return a.exec();
}
