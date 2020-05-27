#include "BtDiscoverRemoteDevice.hpp"

namespace spx
{
  BtDiscoverRemoteDevice::BtDiscoverRemoteDevice( std::shared_ptr< Logger > logger, QObject *parent ) : QObject( parent ), lg( logger )
  {
    //
    // Geräte Discovering Objekt erschaffen
    //
    *lg << LDEBUG << " BtDiscoverRemoteDevice::BtDiscoverRemoteDevice -> create device discovering object..." << Qt::endl;
    btDevices = std::unique_ptr< SPX42BtDevicesManager >( new SPX42BtDevicesManager( lg, this ) );
    //
    // discovering agent object
    //
    connect( btDevices.get(), &SPX42BtDevicesManager::onDiscoveredDeviceSig, this, &BtDiscoverRemoteDevice::onDiscoveredDeviceSlot );
    connect( btDevices.get(), &SPX42BtDevicesManager::onDiscoverScanFinishedSig, this,
             &BtDiscoverRemoteDevice::onDiscoverScanFinishedSlot );
    connect( btDevices.get(), &SPX42BtDevicesManager::onDeviceHostModeStateChangedSig, this,
             &BtDiscoverRemoteDevice::onDeviceHostModeStateChangedSlot );
    // connect( btDevices.get(), &SPX42BtDevices::onDevicePairingDoneSig, this, &BtDiscoverRemoteDevice::onDevicePairingDoneSlot );
  }

  BtDiscoverRemoteDevice::~BtDiscoverRemoteDevice()
  {
    *lg << LDEBUG << "BtDiscoverRemoteDevice::~BtDiscoverRemoteDevice..." << Qt::endl;
  }

  void BtDiscoverRemoteDevice::startDiscover()
  {
    *lg << LDEBUG << "BtDiscoverRemoteDevice::onGuiStartScanSlot..." << Qt::endl;
    btDevices->setHostDiscoverable( true );
    btDevices->setHostPower( true );
    btDevices->setInquiryGeneralUnlimited( true );
    btDevices->startDiscoverDevices();
    *lg << LDEBUG << "BtDiscoverRemoteDevice::onGuiStartScanSlot...OK" << Qt::endl;
  }

  void BtDiscoverRemoteDevice::stopDiscover()
  {
    *lg << LDEBUG << "BtDiscoverRemoteDevice::stopDiscover..." << Qt::endl;
    btDevices->cancelDiscoverDevices();
    *lg << LDEBUG << "BtDiscoverRemoteDevice::stopDiscover...OK" << Qt::endl;
  }

  SPXDeviceList BtDiscoverRemoteDevice::getSPX42Devices() const
  {
    return ( btDevices->getSPX42Devices() );
  }

  void BtDiscoverRemoteDevice::onDeviceHostModeStateChangedSlot( QBluetoothLocalDevice::HostMode mode )
  {
    if ( mode != QBluetoothLocalDevice::HostPoweredOff )
    {
      *lg << LINFO << "BtDiscoverRemoteDevice::onDeviceHostModeStateChangedSlot -> host power off: true" << Qt::endl;
    }
    else
    {
      *lg << LINFO << "BtDiscoverRemoteDevice::onDeviceHostModeStateChangedSlot -> host power off: false" << Qt::endl;
    }

    if ( mode == QBluetoothLocalDevice::HostDiscoverable )
    {
      *lg << LINFO << "BtDiscoverRemoteDevice::onDeviceHostModeStateChangedSlot -> host discoverable: true" << Qt::endl;
    }
    else
    {
      *lg << LINFO << "BtDiscoverRemoteDevice::onDeviceHostModeStateChangedSlot -> host discoverable: false" << Qt::endl;
    }
  }

  void BtDiscoverRemoteDevice::onDiscoverScanFinishedSlot()
  {
    *lg << LINFO << "BtDiscoverOject::onDiscoverScanFinishedSlot..." << Qt::endl;
    emit onDiscoverScanFinishedSig();
  }

  void BtDiscoverRemoteDevice::onDiscoveredDeviceSlot( const SPXDeviceDescr &deviceInfo )
  {
    // TODO: durch lambda ersetzten
    emit onDiscoveredDeviceSig( deviceInfo );
  }
}  // namespace spx
