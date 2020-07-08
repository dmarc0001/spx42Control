#ifndef LOGDETAILDELETER_HPP
#define LOGDETAILDELETER_HPP

#include <QObject>

class LogDetailDeleter : public QThread
{
  Q_OBJECT
  public:
  LogDetailDeleter();
};

#endif // LOGDETAILDELETER_HPP
