#ifndef LOGDETAILWRITER_HPP
#define LOGDETAILWRITER_HPP

#include <QtCore/QObject>

class LogDetailWriter : public QObject
{
    Q_OBJECT
  public:
    explicit LogDetailWriter(QObject *parent = nullptr);

  signals:

  public slots:
};

#endif // LOGDETAILWRITER_HPP