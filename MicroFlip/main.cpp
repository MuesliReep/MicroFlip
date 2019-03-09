#include <QCoreApplication>

#include <QThread>

#include "program.h"

int main(int argc, char *argv[]) {

  QCoreApplication a(argc, argv);

  auto *p = new Program();
  auto *programThread = new QThread();

  p->moveToThread(programThread);
  programThread->start();

  return a.exec();
}
