#ifndef DIFFERENCE_H
#define DIFFERENCE_H

#include <QObject>
#include "../tools.h"

class Imagedifference : public QObject
{
  Q_OBJECT
 public:
  Imagedifference();

 public slots:
  void onUpdate();
};

#endif // DIFFERENCE_H
