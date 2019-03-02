#include <QCoreApplication>

#include <QThread>

#include "program.h"

int main(int argc, char *argv[])
{
  QCoreApplication a(argc, argv);

  Program *p = new Program();
  QThread *programThread = new QThread();

  p->moveToThread(programThread);
  programThread->start();

  return a.exec();
}
