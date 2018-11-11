#ifndef BTDISCOVEROBJECT_HPP
#define BTDISCOVEROBJECT_HPP

#include <QtCore/QObject>

class BtDiscoverObject : public QObject
{
    Q_OBJECT
  public:
    explicit BtDiscoverObject(QObject *parent = nullptr);

  signals:

  public slots:
};

#endif // BTDISCOVEROBJECT_HPP