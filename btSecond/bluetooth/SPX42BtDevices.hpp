#ifndef BTDEVICES_HPP
#define BTDEVICES_HPP

#include <QObject>
#include <QPair>
#include <QQueue>
#include <QVector>
#include <memory>
#include "../logging/Logger.hpp"
#include "BtDevicesManager.hpp"
#include "BtServiceDiscover.hpp"

namespace spx
{
  //
  // ein paar Typendefinitionen, um die Sache lesbar zu machen
  //
  using SPXDeviceList = QVector< QBluetoothDeviceInfo >;
  using ToScannedDecice = QPair< QString, QBluetoothAddress >;
  using ToScannedDevicesQueue = QQueue< ToScannedDecice >;

  class SPX42BtDevices : public QObject
  {
    Q_OBJECT
    private:
    std::shared_ptr< Logger > lg;
    bool deviceDiscoverFinished;
    ToScannedDecice currentServiceScanDevice;
    std::unique_ptr< BtDevicesManager > btDevicesManager;
    std::unique_ptr< BtServiceDiscover > btServicesAgent;
    QBluetoothAddress laddr;
    SPXDeviceList discoverdDevices;
    ToScannedDevicesQueue devicesToDiscoverServices;

    public:
    explicit SPX42BtDevices( std::shared_ptr< Logger > logger, QObject *parent = nullptr );
    ~SPX42BtDevices();                                                                         //! Der Destruktor
    void startDiscoverDevices( void );                                                         //! Geräte suchen
    SPXDeviceList getDiscoveredDevices( void ) const;                                          //! gefundene Geräte zurückgeben
    QBluetoothLocalDevice::Pairing getPairingStatus( QBluetoothAddress addr );                 //! pairing für remote gerät
    void setInquiryGeneralUnlimited( bool inquiry );                                           //! set scan mode
    void setHostDiscoverable( bool discoverable );                                             //! Host ist zu finden/nicht zu finden
    void setHostPower( bool powered );                                                         //! host power on/off
    void requestPairing( QBluetoothAddress address, QBluetoothLocalDevice::Pairing pairing );  //! pairing anfordern

    private:
    void startDiscoverServices( void );  //! starte discovering, warte ggf bis ein laufender Prozess abgelaufen ist

    signals:
    void sigDiscoveredDevice( const QBluetoothDeviceInfo &info );
    void sigDevicePairingDone( const QBluetoothAddress &remoteAddr, QBluetoothLocalDevice::Pairing pairing );
    void sigDiscoverScanFinished( void );
    void sigDeviceHostModeStateChanged( QBluetoothLocalDevice::HostMode hostMode );
    void sigDiscoveryServicesFinished( const QString name );
    void sigDiscoveredService( const QString &name, const QBluetoothServiceInfo &info );
    void sigAllScansFinished( void );

    private slots:
    void slotDiscoveredDevice( const QBluetoothDeviceInfo &info );
    void slotDevicePairingDone( const QBluetoothAddress &address, QBluetoothLocalDevice::Pairing pairing );
    void slotDiscoverScanFinished( void );
    void slotDeviceHostModeStateChanged( QBluetoothLocalDevice::HostMode hostMode );
    void slotDiscoveryServicesFinished( const QString name );
    void slotDiscoveredService( const QString &name, const QBluetoothServiceInfo &info );
  };
}
#endif  // BTDEVICES_HPP
