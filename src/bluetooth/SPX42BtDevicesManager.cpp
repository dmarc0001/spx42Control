#include "SPX42BtDevicesManager.hpp"
#include <QTimer>

namespace spx
{
  SPX42BtDevicesManager::SPX42BtDevicesManager( std::shared_ptr< Logger > logger, QObject *parent )
      : QObject( parent ), lg( logger ), deviceDiscoverFinished( true ), exp( ProjectConst::searchedServiceRegex )
  {
    *lg << LDEBUG << "SPX42BtDevicesManager::SPX42BtDevicesManager..." << Qt::endl;
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
    *lg << LDEBUG << "SPX42BtDevicesManager::SPX42BtDevicesManager -> create device discovering object..." << Qt::endl;
    btDevicesManager = std::unique_ptr< BtLocalDevicesManager >( new BtLocalDevicesManager( lg, this ) );
    //
    // discovering agent object
    //
    *lg << LDEBUG << "SPX42BtDevicesManager::SPX42BtDevicesManager -> connect signals..." << Qt::endl;
    connect( btDevicesManager.get(), &BtLocalDevicesManager::onDiscoveredDeviceSig, this,
             &SPX42BtDevicesManager::onDiscoveredDeviceSlot );
    connect( btDevicesManager.get(), &BtLocalDevicesManager::onDiscoverScanFinishedSig, this,
             &SPX42BtDevicesManager::onDiscoverScanFinishedSlot );
    connect( btDevicesManager.get(), &BtLocalDevicesManager::onDeviceHostModeStateChangedSig, this,
             &SPX42BtDevicesManager::onDeviceHostModeStateChangedSlot );
    connect( btDevicesManager.get(), &BtLocalDevicesManager::onDevicePairingDoneSig, this,
             &SPX42BtDevicesManager::onDevicePairingDoneSlot );
    //
    *lg << LDEBUG << "SPX42BtDevicesManager::SPX42BtDevicesManager -> connect signals...OK" << Qt::endl;
    //
    // initialisierung durchführen
    //
    btDevicesManager->init();
  }

  SPX42BtDevicesManager::~SPX42BtDevicesManager()
  {
    *lg << LDEBUG << "SPX42BtDevicesManager::~SPX42BtDevicesManager" << Qt::endl;
  }

  void SPX42BtDevicesManager::startDiscoverDevices()
  {
    *lg << LDEBUG << "SPX42BtDevicesManager::startDiscoverDevices... " << Qt::endl;
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
      *lg << LDEBUG << "SPX42BtDevicesManager::onDiscoveredDeviceSlot -> device " << device << " always discovered. ignore."
          << Qt::endl;
    }
    else if ( info.address().isNull() )
    {
      *lg << LDEBUG << "SPX42BtDevicesManager::onDiscoveredDeviceSlot ->  device has no address. Ignore." << Qt::endl;
    }
    else
    {
      *lg << LDEBUG << "SPX42BtDevicesManager::onDiscoveredDeviceSlot -> device " << device << " in local list inserted." << Qt::endl;
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
    *lg << LDEBUG << "SPX42BtDevicesManager::onDevicePairingDoneSlot -> device: " << address.toString() << Qt::endl;
    *lg << LINFO << "pairing done. device: " << address.toString() << Qt::endl;
    emit onDevicePairingDoneSig( address, pairing );
  }

  void SPX42BtDevicesManager::onDiscoverScanFinishedSlot()
  {
    *lg << LDEBUG << "SPX42BtDevicesManager::onDiscoverScanFinishedSlot..." << Qt::endl;
    *lg << LINFO << "device discovering finished..." << Qt::endl;
    deviceDiscoverFinished = true;
    emit onDiscoverScanFinishedSig();
  }

  void SPX42BtDevicesManager::onDeviceHostModeStateChangedSlot( QBluetoothLocalDevice::HostMode hostMode )
  {
    *lg << LDEBUG << "SPX42BtDevicesManager::onDeviceHostModeStateChangedSlot -> change to hostmode " << hostMode << Qt::endl;
    emit onDeviceHostModeStateChangedSig( hostMode );
  }

  void SPX42BtDevicesManager::startDiscoverServices()
  {
    *lg << LDEBUG << "SPX42BtDevicesManager::startDiscoverServices..." << Qt::endl;
    if ( devicesToDiscoverServices.isEmpty() && deviceDiscoverFinished )
    {
      emit onAllScansFinishedSig();
      return;
    }
    if ( currentServiceScanDevice.isNull() )
    {
      // Die Adresse ist nicht leer, scanne!
      currentServiceScanDevice = devicesToDiscoverServices.dequeue();
      *lg << LDEBUG << "SPX42BtDevicesManager::startDiscoverServices -> remote adapter addr is valid. scan..." << Qt::endl;
      btServicesAgent =
          std::unique_ptr< BtDiscoverRemoteService >( new BtDiscoverRemoteService( lg, laddr, currentServiceScanDevice, this ) );
      //
      // Filter um nur die richtigen Geräte zu finden
      //
      btServicesAgent->setServiceFilter( ProjectConst::searchedServiceRegex );
      *lg << LDEBUG << "SPX42BtDevicesManager::startDiscoverServices -> local adapter addr: " << laddr.toString() << Qt::endl;
      *lg << LDEBUG << "SPX42BtDevicesManager::startDiscoverServices -> remote adapter addr: " << currentServiceScanDevice.toString()
          << Qt::endl;
      *lg << LDEBUG << "SPX42BtDevicesManager::startDiscoverServices -> connect signals and slots..." << Qt::endl;
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
      *lg << LDEBUG << "SPX42BtDevicesManager::startDiscoverServices -> service for device <" << currentServiceScanDevice.toString()
          << "> is in progress, wait for ending..." << Qt::endl;
      QTimer::singleShot( 600, this, &SPX42BtDevicesManager::startDiscoverServices );
    }
  }

  void SPX42BtDevicesManager::onDiscoveryServicesFinishedSlot( const QBluetoothAddress &remoteAddr )
  {
    *lg << LDEBUG << "SPX42BtDevicesManager::onDiscoveryServicesFinishedSlot..." << Qt::endl;
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
      *lg << LDEBUG << "SPX42BtDevicesManager::onDiscoveredServiceSlot -> device: " << raddr.toString()
          << ", with submatix spx42 service discovered" << Qt::endl;
      //
      // mal schauen ob das Gerät schon in der Liste der bereits erforschten Geräte ist
      //
      if ( spx42Devices.contains( raddrStr ) )
      {
        //
        // Das Gerät ist bereits vorhanden, dann ignoriere es
        //
        *lg << LDEBUG << "SPX42BtDevicesManager::onDiscoveredServiceSlot -> device: " << raddr.toString() << " always present. Ignore."
            << Qt::endl;
      }
      else
      {
        //
        // ist das Gerät schon gefunden aber der Service noch nicht?
        //
        if ( discoverdDevices.contains( raddrStr ) )
        {
          SPXDeviceDescr newDevice( discoverdDevices.take( raddrStr ) );
          *lg << LINFO << "SPX42BtDevicesManager::onDiscoveredServiceSlot -> device: " << raddrStr << " add... " << Qt::endl;
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
          //
          *lg << LCRIT << "SPX42BtDevicesManager::onDiscoveredServiceSlot -> device: " << raddr.toString()
              << "  not in discovered list..." << Qt::endl;
        }
      }
    }
  }
}  // namespace spx
