#include "BtLocalDevicesManager.hpp"

namespace spx
{
  /**
   * @brief BtDevices::BtDevices
   * @param logger
   * @param parent
   */
  BtLocalDevicesManager::BtLocalDevicesManager( std::shared_ptr< Logger > logger, QObject *parent ) : QObject( parent ), lg( logger )
  {
    localDevice = std::unique_ptr< QBluetoothLocalDevice >( new QBluetoothLocalDevice );
    /*
     * In case of multiple Bluetooth adapters it is possible to set adapter
     * which will be used. Example code:
     *
     * QBluetoothAddress address("XX:XX:XX:XX:XX:XX");
     * discoveryAgent = new QBluetoothDeviceDiscoveryAgent(address);
     *
     **/
    //
    // Geräte Discovering Objekt instanzieren
    //
    lg->debug( "BtDevices::BtDevices: create QBluetoothDeviceDiscoveryAgent..." );
    discoveryAgent = std::unique_ptr< QBluetoothDeviceDiscoveryAgent >( new QBluetoothDeviceDiscoveryAgent() );
    lg->debug( "BtDevices::BtDevices: connect signals..." );
    //
    // discovering agent
    //
    connect( discoveryAgent.get(), &QBluetoothDeviceDiscoveryAgent::deviceDiscovered, this,
             [=]( const QBluetoothDeviceInfo &info ) { emit sigDiscoveredDevice( info ); } );
    connect( discoveryAgent.get(), &QBluetoothDeviceDiscoveryAgent::finished, this, [=] { emit sigDiscoverScanFinished(); } );
    //
    // local device
    //
    connect( localDevice.get(), &QBluetoothLocalDevice::hostModeStateChanged, this,
             [=]( QBluetoothLocalDevice::HostMode mode ) { emit sigDeviceHostModeStateChanged( mode ); } );
    connect(
        localDevice.get(), &QBluetoothLocalDevice::pairingFinished, this,
        [=]( const QBluetoothAddress &addr, QBluetoothLocalDevice::Pairing pairing ) { emit sigDevicePairingDone( addr, pairing ); } );
    //
    lg->debug( "BtDevices::BtDevices: connect signals...OK" );
  }

  /**
   * @brief BtDevices::~BtDevices
   */
  BtLocalDevicesManager::~BtLocalDevicesManager()
  {
  }

  void BtLocalDevicesManager::init( void )
  {
    //
    // ereignis für hostmode einmal ausführen
    //
    emit sigDeviceHostModeStateChanged( localDevice->hostMode() );
  }

  void BtLocalDevicesManager::startDiscoverDevices( void )
  {
    lg->debug( "BtDevices::startDiscoverDevices..." );
    discoveryAgent->start();
  }

  const QBluetoothLocalDevice *BtLocalDevicesManager::getLocalDevice( void )
  {
    return ( localDevice.get() );
  }

  void BtLocalDevicesManager::setInquiryGeneralUnlimited( bool unlimited )
  {
    if ( unlimited )
      discoveryAgent->setInquiryType( QBluetoothDeviceDiscoveryAgent::GeneralUnlimitedInquiry );
    else
      discoveryAgent->setInquiryType( QBluetoothDeviceDiscoveryAgent::LimitedInquiry );
  }

  void BtLocalDevicesManager::setHostDiscoverable( bool discoverable )
  {
    if ( discoverable )
      localDevice->setHostMode( QBluetoothLocalDevice::HostDiscoverable );
    else
      localDevice->setHostMode( QBluetoothLocalDevice::HostConnectable );
  }

  void BtLocalDevicesManager::setHostPower( bool on )
  {
    if ( on )
      localDevice->powerOn();
    else
      localDevice->setHostMode( QBluetoothLocalDevice::HostPoweredOff );
  }

  void BtLocalDevicesManager::requestPairing( const QBluetoothAddress &address, QBluetoothLocalDevice::Pairing pairing )
  {
    localDevice->requestPairing( address, pairing );
  }
}
