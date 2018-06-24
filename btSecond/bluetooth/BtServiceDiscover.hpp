#ifndef BT_SERVICE_DISCOVER_HPP
#define BT_SERVICE_DISCOVER_HPP

#include <QBluetoothLocalDevice>
#include <QBluetoothServiceDiscoveryAgent>
#include <QBluetoothServiceInfo>
#include <QObject>
#include <memory>
#include "logging/Logger.hpp"

namespace spx
{
  class BtServiceDiscover : public QObject
  {
    Q_OBJECT
    private:
    private:
    std::shared_ptr< Logger > lg;
    QString name;
    QBluetoothAddress &laddr;
    QBluetoothAddress &raddr;
    std::unique_ptr< QBluetoothServiceDiscoveryAgent > discoveryAgent;
    int servicesCount;

    public:
    explicit BtServiceDiscover( std::shared_ptr< Logger > logger,
                                QString &dname,
                                QBluetoothAddress &l_addr,
                                QBluetoothAddress &r_addr,
                                QObject *parent = nullptr );
    ~BtServiceDiscover();
    int servicesDiscovered( void );
    void start( void );

    signals:
    void sigDiscoveredService( const QString &name, const QBluetoothServiceInfo &info );
    void sigDiscoverScanFinished( const QString &name );

    private slots:
    void slotDiscoveredService( const QBluetoothServiceInfo &info );
  };
}
#endif  // BTSERVICEDISCOVER_HPP
