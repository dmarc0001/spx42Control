#ifndef BTDEVICES_HPP
#define BTDEVICES_HPP

#include <QHash>
#include <QObject>
#include <QPair>
#include <QQueue>
#include <QVector>
#include <memory>
#include "BtDevicesManager.hpp"
#include "BtServiceDiscover.hpp"
#include "config/ProjectConst.hpp"
#include "logging/Logger.hpp"

namespace spx
{
  //
  // ein paar Typendefinitionen, um die Sache lesbar zu machen
  //
  using SPXDeviceList = QHash< QString, QBluetoothDeviceInfo >;  //! device Addr, device Info
  using ToScannedDevicesQueue = QQueue< QBluetoothAddress >;

  class SPX42BtDevices : public QObject
  {
    Q_OBJECT
    private:
    std::shared_ptr< Logger > lg;
    bool deviceDiscoverFinished;
    QBluetoothAddress currentServiceScanDevice;
    std::unique_ptr< BtDevicesManager > btDevicesManager;
    std::unique_ptr< BtServiceDiscover > btServicesAgent;
    QBluetoothAddress laddr;
    SPXDeviceList spx42Devices;
    SPXDeviceList discoverdDevices;
    ToScannedDevicesQueue devicesToDiscoverServices;

    public:
    explicit SPX42BtDevices( std::shared_ptr< Logger > logger, QObject *parent = nullptr );
    ~SPX42BtDevices();                                                                 //! Der Destruktor
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
    void onDiscoveredDeviceSig( const QBluetoothDeviceInfo &info );
    void onDevicePairingDoneSig( const QBluetoothAddress &remoteAddr, QBluetoothLocalDevice::Pairing pairing );
    void onDiscoverScanFinishedSig( void );
    void onDeviceHostModeStateChangedSig( QBluetoothLocalDevice::HostMode hostMode );
    void onDiscoveryServicesFinishedSig( const QBluetoothAddress &remoteAddr );
    void onDiscoveredServiceSig( const QBluetoothAddress &raddr, const QBluetoothServiceInfo &info );
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
