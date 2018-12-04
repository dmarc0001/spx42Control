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
    std::shared_ptr< Logger > lg;                        //! Zeiger auf Logger
    std::unique_ptr< SPX42BtDevicesManager > btDevices;  //! Zeiger auf BT Geräteverwaltung

    public:
    explicit BtDiscoverRemoteDevice( std::shared_ptr< Logger > logger, QObject *parent = nullptr );
    ~BtDiscoverRemoteDevice();  //! Destruktor
    void startDiscover( void );
    void stopDiscover( void );
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
#endif  // BTDISCOVERREMOTEDEVICE_HPP
