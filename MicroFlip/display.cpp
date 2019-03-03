#include "display.h"

#include "common.h"

#ifdef ISWIN
#include <conio.h>
#include <windows.h>
#include <cwchar>
#else
#include <sys/ioctl.h>
#endif

#include <cstdio>
#include <unistd.h>
#include <iostream>

#define ESC "\x1b"
#define CSI "\x1b["

Display::Display() {

#ifdef ISWIN
    EnableVTMode();
#endif

    getTerminalSize();

    std::cout << "lines "   << lines   << std::endl;
    std::cout << "columns " << columns << std::endl;

    std::cout << "\x1b[31m" << std::endl;
    std::cout << "Test"     << std::endl;

    //std::cout << "\x1b[35m" << std::endl;
    std::cout << CSI << "35m";
    std::cout << std::endl;

    exchangeName = "";
    lastPrice    = 0.0;
    avgPrice     = 0.0;
}

#ifdef ISWIN
bool Display::EnableVTMode() {

    // Set output mode to handle virtual terminal sequences
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hOut == INVALID_HANDLE_VALUE)
    {
        return false;
    }

    DWORD dwMode = 0;
    if (!GetConsoleMode(hOut, &dwMode))
    {
        return false;
    }

    dwMode |= 0x0004;//ENABLE_VIRTUAL_TERMINAL_PROCESSING;
    if (!SetConsoleMode(hOut, dwMode))
    {
        return false;
    }
    return true;
}
#endif

bool Display::getTerminalSize() {

    int currentColumns = this->columns;
    int currentLines   = this->lines;

    bool windowChanged = false;

#ifdef ISWIN

    CONSOLE_SCREEN_BUFFER_INFO csbi;

    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
    this->columns = csbi.srWindow.Right - csbi.srWindow.Left + 1;
    this->lines   = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;

#else

    struct winsize w;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);

    this->columns = (int)w.ws_col;
    this->lines   = (int)w.ws_row;

#endif

    if((this->columns != currentColumns) || (this->lines != currentLines)) {
        windowChanged = true;
    }

    // Minus 1 so there is room for 1 qDebug line
    lines-=2;

    return windowChanged;
}

void Display::updateScreen() {

    getTerminalSize();
    clearScreen();
    drawHeader();
    drawWorkOrders();
    drawLog();
}

void Display::addToLog(int workID, QString classID, QString logString, int severity) {

    // Dont add to log if loglevel is too low
    if(severity < logLevel) {
        return;
    }

    QString time = QDateTime::currentDateTime().toString("hh:mm:ss");

    logList.prepend(LogItem(time, workID, classID, logString, severity));

    // Clean up log
    while(logList.size() > 50){
      logList.removeLast();
    }

    updateScreen();
}

void Display::updateExchangePrices(QString symbol, double lastPrice, double avgPrice) {

    this->symbol    = symbol;
    this->lastPrice = lastPrice;
    this->avgPrice  = avgPrice;

    updateScreen();
}

void Display::stateUpdate(int workID, QString state) {

  int listID = -1;

  QString woLine = "ID:" + QString::number(workID) + " \t" + state;

  // Check if this workID already exists
  for(int i = 0; i < woList.size(); i++) {
    QString check = "ID:" + QString::number(workID) + " ";
    if(woList.at(i).contains(check)) {
      listID = i;
      break;
    }
  }

  // If already exists replace, else append
  if(listID > -1) {
    woList.replace(listID, woLine);
  } else {
    woList.append(woLine);
  }

  updateScreen();
}

void Display::drawHeader() {
  resetAttributes();

  // Set background to blue
//  std::cout << "\x1b[44m";
  setBackgroundColour(BLUE);

  // Set foreground to white
  std::cout << "\x1b[37m";

  QString header = " ";

  static const QString programName = "MicroFlip";
  static const QString average     = "Avg:";
  static const QString last        = "Last:";

  header.append(programName + "       " + exchangeName + "       " + last + QString::number(lastPrice,'f',2) + "  " + average + QString::number(avgPrice,'f',2));

  // Draw header
  if(header < columns) {
      header.resize(header.length() + (columns - header.length()), ' ');
  } else {
      header.truncate(columns);
  }

  std::cout << header.toStdString();

  currentLine = 2;
}

void Display::drawWorkOrders() {
  resetAttributes();

  for(const auto &wo : woList) {

    if(currentLine > lines) {
        break;
    }

    if(wo.contains("ERROR")) {
        setForegroundColour(RED);
    }
    else if(wo.contains("COMPLETE")) {
        setForegroundColour(GREEN);
    }
    else {
        setForegroundColour(WHITE);
    }

    std::cout << wo.toStdString() << std::endl;
    currentLine++;
  }
}

void Display::drawLog() {

    resetAttributes();

    std::cout << std::endl;
    currentLine++;

    for(const auto &logItem : logList) {

        if(currentLine > lines) {
          break;
        }

        if(logItem.getSeverity() < this->logLevel) {
            continue;
        }

        // Set colour according to log severity
        switch(logItem.getSeverity()) {
        case logSeverity::LOG_DEBUG:
            setForegroundColour(MAGENTA);
            break;
        case logSeverity::LOG_INFO:
            setForegroundColour(BLUE, true);
            break;
        case logSeverity::LOG_WARNING:
            setForegroundColour(YELLOW, true);
            break;
        case logSeverity::LOG_CRITICAL:
            setForegroundColour(RED, true);
            break;
        case logSeverity::LOG_FATAL:
            setForegroundColour(YELLOW);
            setBackgroundColour(RED);
            break;
        default:
            break;
        }

        QString logLine;

        // Time
        logLine.append("[" + logItem.getTime() + "]");

        // Work Order ID
        logLine.append("[ID " + QString::number(logItem.getWorkID()) + "]");

        // Class origin
        logLine.append("[" + logItem.getClassID() + "]");

        // Log line
        logLine.append(" " + logItem.getLogString());

        // Truncate if line is too long for window
        if(logLine.length() > columns) {
            logLine.truncate(columns);
        }

        //
        std::cout << logLine.toStdString();

        // Go to next line
        std::cout << std::endl;
        currentLine++;
    }

    resetAttributes();
}

void Display::resetAttributes() {
  std::cout << "\x1b[0m";
}

void Display::clearScreen() {
  std::cout << "\x1b[1J";
  std::cout << "\x1b[1;1f";
}

void Display::setCursorToPosition(int x, int y) {

    // Do not place cursor outside of bounds
    if((x > columns) || (y > lines)) {
        return;
    }
}

void Display::setForegroundColour(int colour, bool bright) {

    QString colourPrefix = bright ? "9" : "3";
    QString colourSuffix = "m";

    QString code = "\x1b[" + colourPrefix + QString::number(colour) + colourSuffix;

    std::cout << code.toStdString();
}

void Display::setBackgroundColour(int colour, bool bright) {

    QString colourPrefix = bright ? "10" : "4";
    QString colourSuffix = "m";

    QString code = "\x1b[" + colourPrefix + QString::number(colour) + colourSuffix;

    std::cout << code.toStdString();
}

LogItem::LogItem(QString time, int workID, QString classID, QString logString, int severity) {

    this->time      = time;
    this->workID    = workID;
    this->classID   = classID;
    this->logString = logString;
    this->severity  = severity;
}

void Display::setLogLevel    (int   value)          { logLevel     = value; }
void Display::setExchangeName(const QString &value) { exchangeName = value; }

int     LogItem::getSeverity  () const { return severity ; }
QString LogItem::getClassID   () const { return classID  ; }
QString LogItem::getLogString () const { return logString; }
QString LogItem::getTime      () const { return time     ; }
int     LogItem::getWorkID    () const { return workID   ; }
