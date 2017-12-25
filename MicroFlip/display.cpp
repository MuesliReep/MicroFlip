#include "display.h"

Display::Display()
{
    getTerminalSize();

    printf ("lines %d\n", lines);
    printf ("columns %d\n", columns);

    std::cout << "\x1b[31m" << std::endl;
    std::cout << "Test"     << std::endl;
}

void Display::getTerminalSize() {

  struct winsize w;
  ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);

  columns = (int)w.ws_col;
  lines   = (int)w.ws_row;

  // Minus 1 so there is room for 1 qDebug line
  lines--;
}

void Display::updateScreen()
{
  getTerminalSize();
  clearScreen();
  drawHeader();
  drawWorkOrders();
  drawLog();
}

void Display::logUpdate(int workID, QString log) {

  QString time = QDateTime::currentDateTime().toString("hh:mm:ss");
  QString logLine = "[" + time + "]" + " ID:" + QString::number(workID) + " " + log;

  logList.prepend(logLine);

  // Clean up log
  while(logList.size() > 50){
    logList.removeLast();
  }

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

  std::cout << "  MicroFlip";

  // Draw header
  for(int i=11; i<columns; i++) { // MicroFlip text is 11 characters
    std::cout << ' ';
  }

  currentLine = 2;
}

void Display::drawWorkOrders() {
  resetAttributes();

  for(int i = 0; i < woList.size(); i++) {

    if(currentLine > lines)
      break;

    QString wo = woList.at(i);

    if(wo.contains("ERROR"))
      setForegroundColour(RED);
    else if(wo.contains("COMPLETE"))
      setForegroundColour(GREEN);
    else
      setForegroundColour(WHITE);

    std::cout << wo.toStdString() << std::endl;
    currentLine++;
  }
}

void Display::drawLog() {
  resetAttributes();

  std::cout << std::endl;
  currentLine++;

  for(int i = 0; i < logList.size(); i++) {

    if(currentLine > lines)
      break;

    QString log = logList.at(i);

    std::cout << log.toStdString() << std::endl;
    currentLine++;
  }
}

void Display::resetAttributes() {
  std::cout << "\x1b[0m";
}

void Display::clearScreen() {
  std::cout << "\x1b[1J";
  std::cout << "\x1b[1;1f";
}

void Display::setForegroundColour(int colour) {

  QString code = "\x1b[3" + QString::number(colour) + "m";

  std::cout << code.toStdString();
}

void Display::setBackgroundColour(int colour) {

  QString code = "\x1b[4" + QString::number(colour) + "m";

  std::cout << code.toStdString();
}
