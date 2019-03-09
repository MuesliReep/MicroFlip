#ifndef CONTROL_H
#define CONTROL_H

#include <QString>

class Control {

public:
  Control();

  void setPair         (QString pair          );
  void setMin          (double  min           );
  void setNumWorkOrders(int     numWorkOrders );

};

#endif // CONTROL_H
