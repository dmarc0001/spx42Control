#include "BtDiscoverRemoteDevice.hpp"

namespace spx
{
  BtDiscoverRemoteDevice::BtDiscoverRemoteDevice( std::shared_ptr< Logger > logger, QObject *parent ) : QObject( parent ), lg( logger )
  {
    //
    // Geräte Discovering Objekt erschaffen
    //
    lg->debug( " BtDiscoverDialog::BtDiscoverDialog -> create device discovering object..." );
    btDevices = std::unique_ptr< SPX42BtDevicesManager >( new SPX42BtDevicesManager( lg, this ) );
    //
    // discovering agent object
    //
    connect( btDevices.get(), &SPX42BtDevicesManager::onDiscoveredDeviceSig, this, &BtDiscoverRemoteDevice::onDiscoveredDeviceSlot );
    connect( btDevices.get(), &SPX42BtDevicesManager::onDiscoverScanFinishedSig, this,
             &BtDiscoverRemoteDevice::onDiscoverScanFinishedSlot );
    connect( btDevices.get(), &SPX42BtDevicesManager::onDeviceHostModeStateChangedSig, this,
             &BtDiscoverRemoteDevice::onDeviceHostModeStateChangedSlot );
    // connect( btDevices.get(), &SPX42BtDevices::onDevicePairingDoneSig, this, &BtDiscoverObject::onDevicePairingDoneSlot );
  }

  BtDiscoverRemoteDevice::~BtDiscoverRemoteDevice()
  {
    lg->debug( "BtDiscoverObject::~BtDiscoverObject..." );
  }

  void BtDiscoverRemoteDevice::startDiscover()
  {
    lg->debug( "BtDiscoverObject::onGuiStartScanSlot..." );
    btDevices->setHostDiscoverable( true );
    btDevices->setHostPower( true );
    btDevices->setInquiryGeneralUnlimited( true );
    btDevices->startDiscoverDevices();
    lg->debug( "BtDiscoverObject::onGuiStartScanSlot...OK" );
  }

  void BtDiscoverRemoteDevice::stopDiscover()
  {
    lg->debug( "BtDiscoverObject::stopDiscover..." );
    btDevices->cancelDiscoverDevices();
    lg->debug( "BtDiscoverObject::stopDiscover...OK" );
  }

  SPXDeviceList BtDiscoverRemoteDevice::getSPX42Devices() const
  {
    return ( btDevices->getSPX42Devices() );
  }

  void BtDiscoverRemoteDevice::onDeviceHostModeStateChangedSlot( QBluetoothLocalDevice::HostMode mode )
  {
    if ( mode != QBluetoothLocalDevice::HostPoweredOff )
    {
      lg->info( QString( "BtDiscoverObject::onDeviceHostModeStateChangedSlot -> host power off: true" ) );
    }
    else
    {
      lg->info( QString( "BtDiscoverObject::onDeviceHostModeStateChangedSlot -> host power off: false" ) );
    }

    if ( mode == QBluetoothLocalDevice::HostDiscoverable )
    {
      lg->info( QString( "BtDiscoverObject::onDeviceHostModeStateChangedSlot -> host discoverable: true" ) );
    }
    else
    {
      lg->info( QString( "BtDiscoverObject::onDeviceHostModeStateChangedSlot -> host discoverable: false" ) );
    }
  }

  void BtDiscoverRemoteDevice::onDiscoverScanFinishedSlot()
  {
    lg->debug( "BtDiscoverOject::onDiscoverScanFinishedSlot..." );
    emit onDiscoverScanFinishedSig();
  }

  void BtDiscoverRemoteDevice::onDiscoveredDeviceSlot( const SPXDeviceDescr &deviceInfo )
  {
    // TODO: durch lambda ersetzten
    emit onDiscoveredDeviceSig( deviceInfo );
  }
}
