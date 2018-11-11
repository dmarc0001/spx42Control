#include "BtDiscoverObject.hpp"

namespace spx
{
  BtDiscoverObject::BtDiscoverObject( std::shared_ptr< Logger > logger, QObject *parent ) : QObject( parent ), lg( logger )
  {
    //
    // Geräte Discovering Objekt erschaffen
    //
    lg->debug( " BtDiscoverDialog::BtDiscoverDialog -> create device discovering object..." );
    btDevices = std::unique_ptr< SPX42BtDevices >( new SPX42BtDevices( lg, this ) );
    //
    // discovering agent object
    //
    connect( btDevices.get(), &SPX42BtDevices::onDiscoveredDeviceSig, this, &BtDiscoverObject::onDiscoveredDeviceSlot );
    connect( btDevices.get(), &SPX42BtDevices::onDiscoverScanFinishedSig, this, &BtDiscoverObject::onDiscoverScanFinishedSlot );
    connect( btDevices.get(), &SPX42BtDevices::onDeviceHostModeStateChangedSig, this,
             &BtDiscoverObject::onDeviceHostModeStateChangedSlot );
    // connect( btDevices.get(), &SPX42BtDevices::onDevicePairingDoneSig, this, &BtDiscoverObject::onDevicePairingDoneSlot );
  }

  BtDiscoverObject::~BtDiscoverObject()
  {
    lg->debug( "BtDiscoverObject::~BtDiscoverObject..." );
  }

  void BtDiscoverObject::startDiscover( void )
  {
    lg->debug( "BtDiscoverObject::onGuiStartScanSlot..." );
    btDevices->setHostDiscoverable( true );
    btDevices->setHostPower( true );
    btDevices->setInquiryGeneralUnlimited( true );
    btDevices->startDiscoverDevices();
    lg->debug( "BtDiscoverObject::onGuiStartScanSlot...OK" );
  }

  SPXDeviceList BtDiscoverObject::getSPX42Devices() const
  {
    return ( btDevices->getSPX42Devices() );
  }

  void BtDiscoverObject::onDeviceHostModeStateChangedSlot( QBluetoothLocalDevice::HostMode mode )
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

  void BtDiscoverObject::onDiscoverScanFinishedSlot()
  {
    lg->debug( "BtDiscoverOject::onDiscoverScanFinishedSlot..." );
    emit onDiscoverScanFinishedSig();
  }

  void BtDiscoverObject::onDiscoveredDeviceSlot( const SPXDeviceDescr &deviceInfo )
  {
    // TODO: durch lambda ersetzten
    emit onDiscoveredDeviceSig( deviceInfo );
  }
}
