#ifndef BTDISCOVEROBJECT_HPP
#define BTDISCOVEROBJECT_HPP

#include <QBluetoothDeviceDiscoveryAgent>
#include <QBluetoothLocalDevice>
#include <memory>
#include "bluetooth/SPX42BtDevices.hpp"
#include "database/SPX42Database.hpp"
#include "logging/Logger.hpp"

namespace spx
{
  class BtDiscoverObject : public QObject
  {
    Q_OBJECT
    private:
    std::shared_ptr< Logger > lg;                 //! Zeiger auf Logger
    std::unique_ptr< SPX42BtDevices > btDevices;  //! Zeiger auf BT Geräteverwaltung

    public:
    explicit BtDiscoverObject( std::shared_ptr< Logger > logger, QObject *parent = nullptr );
    ~BtDiscoverObject();  //! Destruktor
    void startDiscover( void );
    SPXDeviceList getSPX42Devices( void ) const;  //! Liste aller gefundenen Geräte

    signals:
    void onDiscoveredDeviceSig( const SPXDeviceDescr &deviceInfo );
    void onDiscoverScanFinishedSig( void );

    public slots:
    // void onDevicePairingDoneSlot( const QBluetoothAddress &, QBluetoothLocalDevice::Pairing );

    private slots:
    void onDeviceHostModeStateChangedSlot( QBluetoothLocalDevice::HostMode );
    void onDiscoverScanFinishedSlot( void );
    void onDiscoveredDeviceSlot( const SPXDeviceDescr & );
  };
}
#endif  // BTDISCOVEROBJECT_HPP
