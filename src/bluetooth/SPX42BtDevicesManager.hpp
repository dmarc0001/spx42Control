#ifndef BTDEVICES_HPP
#define BTDEVICES_HPP

#include <QHash>
#include <QObject>
#include <QPair>
#include <QQueue>
#include <QRegExp>
#include <QVector>
#include <memory>
#include "BtDiscoverRemoteService.hpp"
#include "BtLocalDevicesManager.hpp"
#include "BtTypes.hpp"
#include "config/ProjectConst.hpp"
#include "logging/Logger.hpp"

namespace spx
{
  class SPX42BtDevicesManager : public QObject
  {
    Q_OBJECT
    private:
    std::shared_ptr< Logger > lg;
    bool deviceDiscoverFinished;
    QBluetoothAddress currentServiceScanDevice;
    std::unique_ptr< BtLocalDevicesManager > btDevicesManager;
    std::unique_ptr< BtDiscoverRemoteService > btServicesAgent;
    QBluetoothAddress laddr;
    SPXDeviceList spx42Devices;
    SPXDeviceList discoverdDevices;
    ToScannedDevicesQueue devicesToDiscoverServices;
    QRegExp exp;

    public:
    explicit SPX42BtDevicesManager( std::shared_ptr< Logger > logger, QObject *parent = nullptr );
    ~SPX42BtDevicesManager();
    //! Geräte suchen
    void startDiscoverDevices( void );
    //! Discovering abbrechen
    void cancelDiscoverDevices( void );
    //! gefundene Geräte zurückgeben
    SPXDeviceList getSPX42Devices( void ) const;
    //! pairing für remote gerät
    QBluetoothLocalDevice::Pairing getPairingStatus( const QBluetoothAddress &addr );
    //! set scan mode
    void setInquiryGeneralUnlimited( bool inquiry );
    //! Host ist zu finden/nicht zu finden
    void setHostDiscoverable( bool discoverable );
    //! host power on/off
    void setHostPower( bool powered );
    //! pairing anfordern
    void requestPairing( const QBluetoothAddress &address, QBluetoothLocalDevice::Pairing pairing );

    private:
    void startDiscoverServices( void );

    signals:
    void onDiscoveredDeviceSig( const SPXDeviceDescr &info );
    void onDevicePairingDoneSig( const QBluetoothAddress &remoteAddr, QBluetoothLocalDevice::Pairing pairing );
    void onDiscoverScanFinishedSig( void );
    void onDeviceHostModeStateChangedSig( QBluetoothLocalDevice::HostMode hostMode );
    void onDiscoveryServicesFinishedSig( const QBluetoothAddress &remoteAddr );
    void onAllScansFinishedSig( void );

    private slots:
    void onDiscoveredDeviceSlot( const QBluetoothDeviceInfo &info );
    void onDevicePairingDoneSlot( const QBluetoothAddress &address, QBluetoothLocalDevice::Pairing pairing );
    void onDiscoverScanFinishedSlot( void );
    void onDeviceHostModeStateChangedSlot( QBluetoothLocalDevice::HostMode hostMode );
    void onDiscoveryServicesFinishedSlot( const QBluetoothAddress &remoteAddr );
    void onDiscoveredServiceSlot( const QBluetoothAddress &raddr, const QBluetoothServiceInfo &info );
  };
}  // namespace spx
#endif  // BTDEVICES_HPP
