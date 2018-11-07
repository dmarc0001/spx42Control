#ifndef SPX42REMOTBTDEVICE_HPP
#define SPX42REMOTBTDEVICE_HPP

#include <QtCore/QObject>

class SPX42RemotBtDevice : public QObject
{
    Q_OBJECT
  public:
    explicit SPX42RemotBtDevice(QObject *parent = nullptr);

  signals:

  public slots:
};

#endif // SPX42REMOTBTDEVICE_HPP