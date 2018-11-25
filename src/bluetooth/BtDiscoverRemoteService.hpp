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
  class BtDiscoverRemoteService : public QObject
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
    explicit BtDiscoverRemoteService( std::shared_ptr< Logger > logger,
                                      QBluetoothAddress &l_addr,
                                      QBluetoothAddress &r_addr,
                                      QObject *parent = nullptr );
    ~BtDiscoverRemoteService();
    bool setServiceFilter( const QString &expr );
    void resetServiceFilter( void );
    int servicesDiscovered( void );
    void start( void );
    void cancelDiscover( void );

    signals:
    void onDiscoveredServiceSig( const QBluetoothAddress &raddr, const QBluetoothServiceInfo &info );
    void onDiscoverScanFinishedSig( const QBluetoothAddress &raddr );

    private slots:
    void onDiscoveredServiceSlot( const QBluetoothServiceInfo &info );
  };
}
#endif  // BTSERVICEDISCOVER_HPP
