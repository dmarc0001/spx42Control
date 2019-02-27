#ifndef BTDISCOVERREMOTEDEVICE_HPP
#define BTDISCOVERREMOTEDEVICE_HPP

#include <QBluetoothDeviceDiscoveryAgent>
#include <QBluetoothLocalDevice>
#include <memory>
#include "bluetooth/SPX42BtDevicesManager.hpp"
#include "database/SPX42Database.hpp"
#include "logging/Logger.hpp"

namespace spx
{
  class BtDiscoverRemoteDevice : public QObject
  {
    Q_OBJECT
    private:
    //! Zeiger auf Logger
    std::shared_ptr< Logger > lg;
    //! Zeiger auf BT Geräteverwaltung
    std::unique_ptr< SPX42BtDevicesManager > btDevices;

    public:
    explicit BtDiscoverRemoteDevice( std::shared_ptr< Logger > logger, QObject *parent = nullptr );
    //! Destruktor
    ~BtDiscoverRemoteDevice();
    void startDiscover( void );
    void stopDiscover( void );
    //! Liste aller gefundenen Geräte
    SPXDeviceList getSPX42Devices( void ) const;

    signals:
    void onDiscoveredDeviceSig( const SPXDeviceDescr &deviceInfo );
    void onDiscoverScanFinishedSig( void );

    private slots:
    void onDeviceHostModeStateChangedSlot( QBluetoothLocalDevice::HostMode );
    void onDiscoverScanFinishedSlot( void );
    void onDiscoveredDeviceSlot( const SPXDeviceDescr & );
  };
}  // namespace spx
#endif  // BTDISCOVERREMOTEDEVICE_HPP
