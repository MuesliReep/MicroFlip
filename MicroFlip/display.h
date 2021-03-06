#ifndef DISPLAY_H
#define DISPLAY_H

#include <QObject>
#include <QList>
#include <QDateTime>

enum consoleColours { BLACK, RED, GREEN, YELLOW, BLUE, MAGENTA, CYAN, WHITE };

class LogItem {

public:

    LogItem(QString time, int workID, QString classID, QString logString, int severity = 1);

    QString getTime      () const;
    int     getWorkID    () const;
    int     getSeverity  () const;
    QString getClassID   () const;
    QString getLogString () const;

private:

    QString time;
    int     workID;
    QString classID;
    QString logString;
    int     severity;
};

class Display : public QObject {

    Q_OBJECT

public:

  Display();

  void setLogLevel(int value);
  void setExchangeName(const QString &value);

private:

  int columns{};
  int lines  {};

  QList<LogItem> logList;
  QList<QString> woList;

  int currentLine{};

  int logLevel{};
  QString exchangeName;

  QString symbol;
  double  lastPrice;
  double  avgPrice;

#ifdef ISWIN
  bool EnableVTMode();
#endif

  void updateScreen    ();
  void drawHeader      ();
  void resetAttributes ();
  void clearScreen     ();
  void drawWorkOrders  ();

  void drawLog();
  void setForegroundColour(int colour, bool bright = false);
  void setBackgroundColour(int colour, bool bright = false);
  bool getTerminalSize();

  void setCursorToPosition(int x, int y);

public slots:

  void stateUpdate(int workID, const QString& state);
  void addToLog   (int workID, QString classID, QString logString, int severity = 1);

  void updateExchangePrices(QString symbol, double lastPrice, double avgPrice);
};

#endif // DISPLAY_H
