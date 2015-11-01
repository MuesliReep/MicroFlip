#ifndef DISPLAY_H
#define DISPLAY_H

#ifndef ISWIN
#include <sys/ioctl.h>
#endif

#include <stdio.h>
#include <unistd.h>
#include <iostream>

#include <QObject>
#include <QList>
#include <QDateTime>

enum consoleColours { BLACK, RED, GREEN, YELLOW, BLUE, MAGENTA, CYAN, WHITE };

class Display : public QObject
{
  Q_OBJECT
public:
  Display();

private:

  int columns;
  int lines;

  QList<QString> logList;
  QList<QString> woList;

  int currentLine;

  void updateScreen();

  void drawHeader();
  void resetAttributes();
  void clearScreen();
  void drawWorkOrders();

  void drawLog();
  void setForegroundColour(int colour);
  void setBackgroundColour(int colour);
public slots:
  void logUpdate(int workID, QString state);
  void stateUpdate(int workID, QString state);
};

#endif // DISPLAY_H
