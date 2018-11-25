#include "SPX42BtDevicesManager.hpp"
#include <QTimer>

namespace spx
{
  SPX42BtDevicesManager::SPX42BtDevicesManager( std::shared_ptr< Logger > logger, QObject *parent )
      : QObject( parent ), lg( logger ), deviceDiscoverFinished( true ), exp( ProjectConst::searchedServiceRegex )
  {
    lg->debug( "SPX42BtDevices::SPX42BtDevices..." );
    currentServiceScanDevice.clear();
    /*
     * In case of multiple Bluetooth adapters it is possible to set adapter
     * which will be used. Example code:
     *
     * QBluetoothAddress address("XX:XX:XX:XX:XX:XX");
     * discoveryAgent = new QBluetoothDeviceDiscoveryAgent(address);
     *
     **/
    //
    // Geräte Discovering Objekt erschaffen
    //
    lg->debug( "SPX42BtDevices::SPX42BtDevices: create device discovering object..." );
    btDevicesManager = std::unique_ptr< BtLocalDevicesManager >( new BtLocalDevicesManager( lg, this ) );
    //
    // discovering agent object
    //
    lg->debug( "SPX42BtDevices::SPX42BtDevices: connect signals..." );
    connect( btDevicesManager.get(), &BtLocalDevicesManager::onDiscoveredDeviceSig, this,
             &SPX42BtDevicesManager::onDiscoveredDeviceSlot );
    connect( btDevicesManager.get(), &BtLocalDevicesManager::onDiscoverScanFinishedSig, this,
             &SPX42BtDevicesManager::onDiscoverScanFinishedSlot );
    connect( btDevicesManager.get(), &BtLocalDevicesManager::onDeviceHostModeStateChangedSig, this,
             &SPX42BtDevicesManager::onDeviceHostModeStateChangedSlot );
    connect( btDevicesManager.get(), &BtLocalDevicesManager::onDevicePairingDoneSig, this,
             &SPX42BtDevicesManager::onDevicePairingDoneSlot );
    //
    lg->debug( "SPX42BtDevices::SPX42BtDevices: connect signals...OK" );
    //
    // initialisierung durchführen
    //
    btDevicesManager->init();
  }

  SPX42BtDevicesManager::~SPX42BtDevicesManager()
  {
    lg->debug( "SPX42BtDevices::~SPX42BtDevices" );
  }

  void SPX42BtDevicesManager::startDiscoverDevices()
  {
    lg->debug( "SPX42BtDevices::startDiscover..." );
    //
    // die Liste leeren
    //
    discoverdDevices.clear();
    spx42Devices.clear();
    deviceDiscoverFinished = false;
    btDevicesManager->setInquiryGeneralUnlimited( true );
    btDevicesManager->startDiscoverDevices();
  }

  /**
   * @brief SPX42BtDevices::cancelDiscovering
   */
  void SPX42BtDevicesManager::cancelDiscoverDevices()
  {
    //
    // brich das ab
    //
    btDevicesManager->cancelDiscoverDevices();
    btServicesAgent->cancelDiscover();
  }

  SPXDeviceList SPX42BtDevicesManager::getSPX42Devices() const
  {
    return ( spx42Devices );
  }

  QBluetoothLocalDevice::Pairing SPX42BtDevicesManager::getPairingStatus( const QBluetoothAddress &addr )
  {
    return ( btDevicesManager->getLocalDevice()->pairingStatus( addr ) );
  }

  void SPX42BtDevicesManager::setInquiryGeneralUnlimited( bool inquiry )
  {
    btDevicesManager->setInquiryGeneralUnlimited( inquiry );
  }

  void SPX42BtDevicesManager::setHostDiscoverable( bool discoverable )
  {
    btDevicesManager->setHostDiscoverable( discoverable );
  }

  void SPX42BtDevicesManager::setHostPower( bool powered )
  {
    btDevicesManager->setHostPower( powered );
  }

  void SPX42BtDevicesManager::requestPairing( const QBluetoothAddress &address, QBluetoothLocalDevice::Pairing pairing )
  {
    btDevicesManager->requestPairing( address, pairing );
  }

  void SPX42BtDevicesManager::onDiscoveredDeviceSlot( const QBluetoothDeviceInfo &info )
  {
    QString device( QString( "addr: <%1>, name: <%2>" ).arg( info.address().toString() ).arg( info.name() ) );
    //
    // ist das schon vorhanden?
    //
    if ( discoverdDevices.contains( info.address().toString() ) )
    {
      lg->debug( QString( "SPX42BtDevices::onDiscoveredDeviceSlot: device %1 always discovered. ignore." ).arg( device ) );
    }
    else if ( info.address().isNull() )
    {
      lg->debug( "SPX42BtDevices::onDiscoveredDeviceSlot: device has no address. Ignore." );
    }
    else
    {
      lg->debug( QString( "SPX42BtDevices::onDiscoveredDeviceSlot: device %1 in local list inserted." ).arg( device ) );
      // zufügen zu den gefundenen Geräten
      discoverdDevices.insert( info.address().toString(), SPXDeviceDescr( info.address().toString(), info.name() ) );
      // in die queue zum service finden
      devicesToDiscoverServices.enqueue( info.address() );
      // starte (auch verzögertes) discovering der Services
      startDiscoverServices();
    }
  }

  void SPX42BtDevicesManager::onDevicePairingDoneSlot( const QBluetoothAddress &address, QBluetoothLocalDevice::Pairing pairing )
  {
    lg->debug( QString( "SPX42BtDevices::onDevicePairingDoneSlot: device: %1" ).arg( address.toString() ) );
    lg->info( QString( "pairing done: device: %1" ).arg( address.toString() ) );
    emit onDevicePairingDoneSig( address, pairing );
  }

  void SPX42BtDevicesManager::onDiscoverScanFinishedSlot()
  {
    lg->debug( "SPX42BtDevices::onDiscoverScanFinishedSlot..." );
    lg->info( "device discovering finished..." );
    deviceDiscoverFinished = true;
    emit onDiscoverScanFinishedSig();
  }

  void SPX42BtDevicesManager::onDeviceHostModeStateChangedSlot( QBluetoothLocalDevice::HostMode hostMode )
  {
    lg->debug( QString( "SPX42BtDevices::onDeviceHostModeStateChangedSlot to %1" ).arg( hostMode ) );
    emit onDeviceHostModeStateChangedSig( hostMode );
  }

  void SPX42BtDevicesManager::startDiscoverServices()
  {
    lg->debug( "SPX42BtDevices::startDiscoverServices:..." );
    if ( devicesToDiscoverServices.isEmpty() && deviceDiscoverFinished )
    {
      emit onAllScansFinishedSig();
      return;
    }
    if ( currentServiceScanDevice.isNull() )
    {
      // Die Adresse ist nicht leer, scanne!
      currentServiceScanDevice = devicesToDiscoverServices.dequeue();
      lg->debug( "SPX42BtDevices::startDiscoverServices: remote adapter addr is valid. scan..." );
      btServicesAgent =
          std::unique_ptr< BtDiscoverRemoteService >( new BtDiscoverRemoteService( lg, laddr, currentServiceScanDevice, this ) );
      //
      // Filter um nur die richtigen Geräte zu finden
      //
      btServicesAgent->setServiceFilter( ProjectConst::searchedServiceRegex );
      lg->debug( QString( "SPX42BtDevices::startDiscoverServices: local adapter addr: " ).append( laddr.toString() ) );
      lg->debug(
          QString( "SPX42BtDevices::startDiscoverServices: remote adapter addr: " ).append( currentServiceScanDevice.toString() ) );
      lg->debug( "SPX42BtDevices::startDiscoverServices: connect signals and slots..." );
      connect( btServicesAgent.get(), &BtDiscoverRemoteService::onDiscoveredServiceSig, this,
               &SPX42BtDevicesManager::onDiscoveredServiceSlot );
      connect( btServicesAgent.get(), &BtDiscoverRemoteService::onDiscoverScanFinishedSig, this,
               &SPX42BtDevicesManager::onDiscoveryServicesFinishedSlot );
      //
      // starte das suchen nach Services
      //
      btServicesAgent->start();
    }
    else
    {
      //
      // starte einen Timer, der nachher noch einmal versucht die Services zu erkunden
      // das sollte soll solange wiederholt werden, bis das Objekt vernichtet oder
      // di Queue leer ist
      //
      lg->debug( QString( "SPX42BtDevices::startDiscoverServices: service for device <%1> is in progress, wait for ending..." )
                     .arg( currentServiceScanDevice.toString() ) );
      QTimer::singleShot( 600, this, &SPX42BtDevicesManager::startDiscoverServices );
    }
  }

  void SPX42BtDevicesManager::onDiscoveryServicesFinishedSlot( const QBluetoothAddress &remoteAddr )
  {
    lg->debug( "SPX42BtDevices::onDiscoveryServicesFinishedSlot..." );
    // freigeben für nächsten scan
    currentServiceScanDevice.clear();
    emit onDiscoveryServicesFinishedSig( remoteAddr );
    //
    // sind alle geräte gescannt und ist kein serviescan offen
    //
    if ( deviceDiscoverFinished && devicesToDiscoverServices.isEmpty() )
    {
      //
      // sende das endgültige ENDe Signal
      //
      emit onAllScansFinishedSig();
    }
  }

  void SPX42BtDevicesManager::onDiscoveredServiceSlot( const QBluetoothAddress &raddr, const QBluetoothServiceInfo &info )
  {
    //
    // ist das gefundene SPX42 Teil schon in der Liste?
    //
    QString raddrStr = raddr.toString();
    //

    if ( ProjectConst::SPX42ServiceUuid == info.serviceUuid() || ( exp.indexIn( info.serviceName() ) > -1 ) )
    {
      //
      // also erst mal ist das Gerät schon mal mit dem passenden Service vorhanden
      //
      lg->info( QString( "SPX42BtDevices::onDiscoveredServiceSlot: device: %1, with submatix spx42 service discovered" )
                    .arg( raddr.toString() ) );
      //
      // mal schauen ob das Gerät schon in der Liste der bereits erforschten Geräte ist
      //
      if ( spx42Devices.contains( raddrStr ) )
      {
        //
        // Das Gerät ist bereits vorhanden, dann ignoriere es
        //
        lg->debug( QString( "SPX42BtDevices::onDiscoveredServiceSlot: device: %1 always present. Ignore." ).arg( raddr.toString() ) );
      }
      else
      {
        //
        // ist das Gerät schon gefunden aber der Service noch nicht?
        //
        if ( discoverdDevices.contains( raddrStr ) )
        {
          SPXDeviceDescr newDevice( discoverdDevices.take( raddrStr ) );
          lg->info( QString( "SPX42BtDevices::onDiscoveredServiceSlot: device: %1 add..." ).arg( raddrStr ) );
          spx42Devices.insert( raddrStr, newDevice );
          //
          // das neue SPX42 Device melden!
          //
          emit onDiscoveredDeviceSig( newDevice );
        }
        else
        {
          //
          // das sollte nicht passieren, das gerät sollte in der Liste der gefundenen Geräte vorhanden sien :-(
          lg->crit(
              QString( "SPX42BtDevices::onDiscoveredServiceSlot: device: %1 not in discovered list..." ).arg( raddr.toString() ) );
        }
      }
    }
  }
}
