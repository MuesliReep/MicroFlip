#ifndef DISPLAY_H
#define DISPLAY_H

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

#ifdef ISWIN
  bool EnableVTMode();
#endif

  void updateScreen();

  void drawHeader();
  void resetAttributes();
  void clearScreen();
  void drawWorkOrders();

  void drawLog();
  void setForegroundColour(int colour);
  void setBackgroundColour(int colour);
  void getTerminalSize();

public slots:
  void logUpdate  (int workID, QString state);
  void stateUpdate(int workID, QString state);

};

#endif // DISPLAY_H
