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
    ~SPX42BtDevicesManager();                                                          //! Der Destruktor
    void startDiscoverDevices( void );                                                 //! Geräte suchen
    void cancelDiscoverDevices( void );                                                //! Discovering abbrechen
    SPXDeviceList getSPX42Devices( void ) const;                                       //! gefundene Geräte zurückgeben
    QBluetoothLocalDevice::Pairing getPairingStatus( const QBluetoothAddress &addr );  //! pairing für remote gerät
    void setInquiryGeneralUnlimited( bool inquiry );                                   //! set scan mode
    void setHostDiscoverable( bool discoverable );                                     //! Host ist zu finden/nicht zu finden
    void setHostPower( bool powered );                                                 //! host power on/off
    void requestPairing( const QBluetoothAddress &address, QBluetoothLocalDevice::Pairing pairing );  //! pairing anfordern

    private:
    void startDiscoverServices( void );  //! starte discovering, warte ggf bis ein laufender Prozess abgelaufen ist

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
}
#endif  // BTDEVICES_HPP
