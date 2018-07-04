#ifndef BT_SERVICE_DISCOVER_HPP
#define BT_SERVICE_DISCOVER_HPP

#include <QBluetoothLocalDevice>
#include <QBluetoothServiceDiscoveryAgent>
#include <QBluetoothServiceInfo>
#include <QObject>
#include <QRegExp>
#include <memory>
#include "logging/Logger.hpp"

namespace spx
{
  class BtServiceDiscover : public QObject
  {
    Q_OBJECT
    private:
    std::shared_ptr< Logger > lg;
    QBluetoothAddress &laddr;
    QBluetoothAddress &raddr;
    std::unique_ptr< QBluetoothServiceDiscoveryAgent > discoveryAgent;
    int servicesCount;
    QRegExp expression;

    public:
    explicit BtServiceDiscover( std::shared_ptr< Logger > logger,
                                QBluetoothAddress &l_addr,
                                QBluetoothAddress &r_addr,
                                QObject *parent = nullptr );
    ~BtServiceDiscover();
    bool setServiceFilter( const QString &expr );
    void resetServiceFilter( void );
    int servicesDiscovered( void );
    void start( void );

    signals:
    void sigDiscoveredService( const QBluetoothAddress &raddr, const QBluetoothServiceInfo &info );
    void sigDiscoverScanFinished( const QBluetoothAddress &raddr );

    private slots:
    void slotDiscoveredService( const QBluetoothServiceInfo &info );
  };
}
#endif  // BTSERVICEDISCOVER_HPP
