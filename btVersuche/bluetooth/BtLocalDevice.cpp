#include "BtLocalDevice.hpp"

/**
 * @brief BtLocalDevice::BtLocalDevice
 */
BtLocalDevice::BtLocalDevice( std::shared_ptr< Logger > logger, QObject *parent )
    : QObject( parent ), lg( logger ), localDevice( new QBluetoothLocalDevice )
{
  //
  // neuen Device Discovering Agenten machen
  //
  lg->debug( "BtLocalDevice::BtLocalDevice: create device discovery agent..." );
  discoveryAgent = new QBluetoothDeviceDiscoveryAgent();
  //
  // Signale mit Slots verbinden
  //
  lg->debug( "BtLocalDevice::BtLocalDevice: connect signals..." );
  connect( localDevice, &QBluetoothLocalDevice::pairingFinished, this, &BtLocalDevice::slotDevicePairingDone );
  connect( localDevice, &QBluetoothLocalDevice::hostModeStateChanged, this, &BtLocalDevice::slotDeviceHostModeStateChanged );
  connect( localDevice, &QBluetoothLocalDevice::deviceConnected, this, &BtLocalDevice::slotDeviceConnected );
  connect( localDevice, &QBluetoothLocalDevice::deviceDisconnected, this, &BtLocalDevice::slotDeviceDisconnected );
  connect( localDevice, &QBluetoothLocalDevice::error, this, &BtLocalDevice::slotDeviceError );
  connect( localDevice, &QBluetoothLocalDevice::pairingDisplayConfirmation, this,
           &BtLocalDevice::slotDevicePairingDisplayConfirmation );
  connect( localDevice, &QBluetoothLocalDevice::pairingDisplayPinCode, this, &BtLocalDevice::slotDevicePairingDisplayPinCode );
  //
  connect( discoveryAgent, &QBluetoothDeviceDiscoveryAgent::deviceDiscovered, this, &BtLocalDevice::slotDiscoveredDevice );
  connect( discoveryAgent, &QBluetoothDeviceDiscoveryAgent::canceled, this, &BtLocalDevice::slotDiscoverCanceled );
  connect( discoveryAgent, &QBluetoothDeviceDiscoveryAgent::finished, this, &BtLocalDevice::slotDiscoverFinished );
  connect( discoveryAgent, QOverload< QBluetoothDeviceDiscoveryAgent::Error >::of( &QBluetoothDeviceDiscoveryAgent::error ), this,
           &BtLocalDevice::slotDiscoverError );
  //
  // den richtigen Hostmode einstellen (via slot)
  //
  slotDeviceHostModeStateChanged( localDevice->hostMode() );
}

/**
 * @brief BtLocalDevice::~BtLocalDevice
 */
BtLocalDevice::~BtLocalDevice()
{
  lg->debug( "BtLocalDevice::~BtLocalDevice..." );
  delete discoveryAgent;
}

void BtLocalDevice::startDeviceScan( void )
{
  lg->info( "BtLocalDevice::startDeviceScan: start device scanning..." );
  // QTimer::singleShot(200, this, SLOT(updateCaption()));
  QTimer::singleShot( BtLocalDevice::discoverTimeout, this, &BtLocalDevice::slotTimeout );
  discoveryAgent->start();
  emit sigDiscoverStarted();
}

void BtLocalDevice::slotDiscoverFinished( void )
{
  lg->info( "BtLocalDevice::slotDiscoverFinished: device scanning finished..." );
  emit sigDiscoverFinished();
}

void BtLocalDevice::slotDiscoverCanceled( void )
{
  lg->info( "BtLocalDevice::slotDiscoverCanceled: device scanning canceled..." );
  emit sigDiscoverFinished();
}

void BtLocalDevice::slotDiscoveredDevice( const QBluetoothDeviceInfo &info )
{
  bool paired;  // ist device gepaart
  // pairing status abfragen
  QBluetoothLocalDevice::Pairing pairingStatus = localDevice->pairingStatus( info.address() );
  if ( pairingStatus == QBluetoothLocalDevice::Paired || pairingStatus == QBluetoothLocalDevice::AuthorizedPaired )
  {
    paired = true;
  }
  else
  {
    paired = false;
  }
  lg->debug( QString( "BtLocalDevice::addDevice: %1, %2 gepaart: %3 ..." )
                 .arg( info.address().toString() )
                 .arg( info.name() )
                 .arg( paired ) );
  emit sigDiscoveredDevice( info );
}

void BtLocalDevice::slotDeviceHostModeStateChanged( QBluetoothLocalDevice::HostMode mode )
{
  switch ( mode )
  {
    case QBluetoothLocalDevice::HostPoweredOff:
      lg->debug( "BtLocalDevice::hostModeStateChanged: host powered off" );
      break;
    case QBluetoothLocalDevice::HostConnectable:
      lg->debug( "BtLocalDevice::hostModeStateChanged: host is connectable" );
      break;
    case QBluetoothLocalDevice::HostDiscoverable:
      lg->debug( "BtLocalDevice::hostModeStateChanged: host is discoverable" );
      break;
    case QBluetoothLocalDevice::HostDiscoverableLimitedInquiry:
      lg->debug( "BtLocalDevice::hostModeStateChanged: host is limited inquiryable " );
  }
  // TODO: Signal über Änderung senden?
}

void BtLocalDevice::slotDevicePairingDone( const QBluetoothAddress &addr, QBluetoothLocalDevice::Pairing pairing )
{
  lg->info( QString( "BtLocalDevice::pairingDone for %1, return: %2..." ).arg( addr.toString() ).arg( pairing ) );
  emit sigDevicePaitingDone( addr, pairing );
  // TODO: signal über Pairing senden
}

void BtLocalDevice::slotDiscoverError( QBluetoothDeviceDiscoveryAgent::Error error )
{
  lg->crit( QString( "BtLocalDevice::slotDiscoverError: %1" ).arg( error ) );
  emit sigDiscoverError( error );
}

void BtLocalDevice::slotDeviceConnected( const QBluetoothAddress &address )
{
  lg->debug( QString( "BtLocalDevice::slotDeviceConnected, addr: %1" ).arg( address.toString() ) );
}

void BtLocalDevice::slotDeviceDisconnected( const QBluetoothAddress &address )
{
  lg->debug( QString( "BtLocalDevice::slotDeviceDisonnected from addr: %1" ).arg( address.toString() ) );
}

void BtLocalDevice::slotDeviceError( QBluetoothLocalDevice::Error error )
{
  lg->debug( QString( "BtLocalDevice::slotDeviceError: %1" ).arg( error ) );
}

void BtLocalDevice::slotDevicePairingDisplayConfirmation( const QBluetoothAddress &address, QString pin )
{
  lg->debug( QString( "BtLocalDevice::slotPairingDisplayConfirmation, addr: %1, pin: %2" ).arg( address.toString() ).arg( pin ) );
}

void BtLocalDevice::slotDevicePairingDisplayPinCode( const QBluetoothAddress &address, QString pin )
{
  lg->debug( QString( "BtLocalDevice::slotPairingDisplayPinCode, addr: %1, pin: %2" ).arg( address.toString() ).arg( pin ) );
}

void BtLocalDevice::slotTimeout( void )
{
  lg->debug( "discover timeout reached..." );
  if ( discoveryAgent && discoveryAgent->isActive() )
  {
    discoveryAgent->stop();
  }
}
