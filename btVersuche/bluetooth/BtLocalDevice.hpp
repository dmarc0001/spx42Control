#ifndef BTLOCALDEVICE_HPP
#define BTLOCALDEVICE_HPP

#include <QBluetoothDeviceDiscoveryAgent>
#include <QBluetoothDeviceInfo>
#include <QBluetoothLocalDevice>
#include <QObject>
#include <QTimer>
#include <logging/Logger.hpp>

// QT_FORWARD_DECLARE_CLASS( QBluetoothDeviceDiscoveryAgent )
// QT_FORWARD_DECLARE_CLASS( QBluetoothDeviceInfo )

QT_USE_NAMESPACE

class BtLocalDevice : public QObject
{
  Q_OBJECT

  private:
  std::shared_ptr< Logger > lg;
  constexpr static int discoverTimeout = 5000;
  constexpr static char testDevice[] = "00:80:25:41:88:31";

  public:
  explicit BtLocalDevice( std::shared_ptr< Logger > logger, QObject *parent = nullptr );
  ~BtLocalDevice();
  void startDeviceScan( void );

  signals:
  void sigDiscoverStarted( void );
  void sigDiscoveredDevice( const QBluetoothDeviceInfo &info );
  void sigDevicePaitingDone( const QBluetoothAddress &addr, QBluetoothLocalDevice::Pairing pairing );
  void sigDeviceHostModeStateChanged( QBluetoothLocalDevice::HostMode );
  void sigDiscoverCanceled( void );
  void sigDiscoverFinished( void );
  void sigDiscoverError( QBluetoothDeviceDiscoveryAgent::Error error );

  private slots:
  void slotDiscoveredDevice( const QBluetoothDeviceInfo &info );
  void slotDevicePairingDone( const QBluetoothAddress &addr, QBluetoothLocalDevice::Pairing pairing );
  void slotDeviceHostModeStateChanged( QBluetoothLocalDevice::HostMode );
  void slotDiscoverCanceled( void );
  void slotDiscoverFinished( void );
  void slotDiscoverError( QBluetoothDeviceDiscoveryAgent::Error error );
  void slotDeviceConnected( const QBluetoothAddress &address );
  void slotDeviceDisconnected( const QBluetoothAddress &address );
  void slotDeviceError( QBluetoothLocalDevice::Error error );
  void slotDevicePairingDisplayConfirmation( const QBluetoothAddress &address, QString pin );
  void slotDevicePairingDisplayPinCode( const QBluetoothAddress &address, QString pin );
  void slotTimeout( void );

  private:
  QBluetoothDeviceDiscoveryAgent *discoveryAgent;
  QBluetoothLocalDevice *localDevice;
};

#endif  // BTLOCALDEVICE_HPP
