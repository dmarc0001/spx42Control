#include "SPX42BtDevices.hpp"
#include <QTimer>

namespace spx
{
  SPX42BtDevices::SPX42BtDevices( std::shared_ptr< Logger > logger, QObject *parent )
      : QObject( parent ), lg( logger ), deviceDiscoverFinished( true )
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
    btDevicesManager = std::unique_ptr< BtDevicesManager >( new BtDevicesManager( lg, this ) );
    //
    // discovering agent object
    //
    lg->debug( "SPX42BtDevices::SPX42BtDevices: connect signals..." );
    connect( btDevicesManager.get(), &BtDevicesManager::sigDiscoveredDevice, this, &SPX42BtDevices::slotDiscoveredDevice );
    connect( btDevicesManager.get(), &BtDevicesManager::sigDiscoverScanFinished, this, &SPX42BtDevices::slotDiscoverScanFinished );
    connect( btDevicesManager.get(), &BtDevicesManager::sigDeviceHostModeStateChanged, this,
             &SPX42BtDevices::slotDeviceHostModeStateChanged );
    connect( btDevicesManager.get(), &BtDevicesManager::sigDevicePairingDone, this, &SPX42BtDevices::slotDevicePairingDone );
    //
    lg->debug( "SPX42BtDevices::SPX42BtDevices: connect signals...OK" );
    //
    // initialisierung durchführen
    //
    btDevicesManager->init();
  }

  SPX42BtDevices::~SPX42BtDevices()
  {
    lg->debug( "SPX42BtDevices::~SPX42BtDevices" );
  }

  void SPX42BtDevices::startDiscoverDevices( void )
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

  SPXDeviceList SPX42BtDevices::getSPX42Devices( void ) const
  {
    return ( spx42Devices );
  }

  QBluetoothLocalDevice::Pairing SPX42BtDevices::getPairingStatus( QBluetoothAddress addr )
  {
    return ( btDevicesManager->getLocalDevice()->pairingStatus( addr ) );
  }

  void SPX42BtDevices::setInquiryGeneralUnlimited( bool inquiry )
  {
    btDevicesManager->setInquiryGeneralUnlimited( inquiry );
  }

  void SPX42BtDevices::setHostDiscoverable( bool discoverable )
  {
    btDevicesManager->setHostDiscoverable( discoverable );
  }

  void SPX42BtDevices::setHostPower( bool powered )
  {
    btDevicesManager->setHostPower( powered );
  }

  void SPX42BtDevices::requestPairing( QBluetoothAddress address, QBluetoothLocalDevice::Pairing pairing )
  {
    btDevicesManager->requestPairing( address, pairing );
  }

  void SPX42BtDevices::slotDiscoveredDevice( const QBluetoothDeviceInfo &info )
  {
    QString device( QString( "addr: <%1>, name: <%2>" ).arg( info.address().toString() ).arg( info.name() ) );
    //
    // ist das schon vorhanden?
    //
    if ( discoverdDevices.contains( info.address().toString() ) )
    {
      lg->debug( QString( "SPX42BtDevices::slotDiscoveredDevice: device %1 always discovered. ignore." ).arg( device ) );
    }
    else if ( info.address().isNull() )
    {
      lg->debug( "SPX42BtDevices::slotDiscoveredDevice: device has no address. Ignore." );
    }
    else
    {
      lg->debug( QString( "SPX42BtDevices::slotDiscoveredDevice: device %1 in local list inserted." ).arg( device ) );
      // zufügen zu den gefundenen Geräten
      discoverdDevices.insert( info.address().toString(), info );
      // in die queue zum service finden
      devicesToDiscoverServices.enqueue( info.address() );
      // starte (auch verzögertes) discovering der Services
      startDiscoverServices();
      // signalisiere, dass Gerät gefunden wurde
      emit sigDiscoveredDevice( info );
    }
  }

  void SPX42BtDevices::slotDevicePairingDone( const QBluetoothAddress &address, QBluetoothLocalDevice::Pairing pairing )
  {
    lg->debug( QString( "SPX42BtDevices::slotDevicePairingDone: device: %1" ).arg( address.toString() ) );
    lg->info( QString( "pairing done: device: %1" ).arg( address.toString() ) );
    emit sigDevicePairingDone( address, pairing );
  }

  void SPX42BtDevices::slotDiscoverScanFinished( void )
  {
    lg->debug( "SPX42BtDevices::slotDiscoverScanFinished..." );
    lg->info( "device discovering finished..." );
    deviceDiscoverFinished = true;
    emit sigDiscoverScanFinished();
  }

  void SPX42BtDevices::slotDeviceHostModeStateChanged( QBluetoothLocalDevice::HostMode hostMode )
  {
    lg->debug( QString( "SPX42BtDevices::slotDeviceHostModeStateChanged to %1" ).arg( hostMode ) );
    emit sigDeviceHostModeStateChanged( hostMode );
  }

  void SPX42BtDevices::startDiscoverServices( void )
  {
    lg->debug( "SPX42BtDevices::startDiscoverServices:..." );
    if ( devicesToDiscoverServices.isEmpty() && deviceDiscoverFinished )
    {
      emit sigAllScansFinished();
      return;
    }
    if ( currentServiceScanDevice.isNull() )
    {
      // Die Adresse ist nicht leer, scanne!
      currentServiceScanDevice = devicesToDiscoverServices.dequeue();
      lg->debug( "SPX42BtDevices::startDiscoverServices: remote adapter addr is valid. scan..." );
      btServicesAgent = std::unique_ptr< BtServiceDiscover >( new BtServiceDiscover( lg, laddr, currentServiceScanDevice, this ) );
      //
      // Filter um nur die richtigen Geräte zu finden
      //
      btServicesAgent->setServiceFilter( "SPX42|SPP" );
      lg->debug( QString( "SPX42BtDevices::startDiscoverServices: local adapter addr: " ).append( laddr.toString() ) );
      lg->debug(
          QString( "SPX42BtDevices::startDiscoverServices: remote adapter addr: " ).append( currentServiceScanDevice.toString() ) );
      lg->debug( "SPX42BtDevices::startDiscoverServices: connect signals and slots..." );
      connect( btServicesAgent.get(), &BtServiceDiscover::sigDiscoveredService, this, &SPX42BtDevices::slotDiscoveredService );
      connect( btServicesAgent.get(), &BtServiceDiscover::sigDiscoverScanFinished, this,
               &SPX42BtDevices::slotDiscoveryServicesFinished );
      //
      // starte das suchen nach Services
      //
      btServicesAgent->start();
    }
    else
    {
      //
      // starte einen Timer, der nachher noch einmal versucht die Services zu erkunden
      // das sollte soll ange wiederholt werden, bis das Objekt vernichtet oder
      // di Queue leer ist
      //
      lg->debug( QString( "SPX42BtDevices::startDiscoverServices: service for device <%1> is in progress, wait for ending..." )
                     .arg( currentServiceScanDevice.toString() ) );
      QTimer::singleShot( 600, this, &SPX42BtDevices::startDiscoverServices );
    }
  }

  void SPX42BtDevices::slotDiscoveryServicesFinished( const QBluetoothAddress &remoteAddr )
  {
    lg->debug( "SPX42BtDevices::slotDiscoveryServicesFinished..." );
    // freigeben für nächsten scan
    currentServiceScanDevice.clear();
    emit sigDiscoveryServicesFinished( remoteAddr );
    //
    // sind alle geräte gescannt und ist kein serviescan offen
    //
    if ( deviceDiscoverFinished && devicesToDiscoverServices.isEmpty() )
    {
      //
      // sende das endgültige ENDe Signal
      //
      emit sigAllScansFinished();
    }
  }

  void SPX42BtDevices::slotDiscoveredService( const QBluetoothAddress &raddr, const QBluetoothServiceInfo &info )
  {
    //
    // ist das gefundene SPX42 Teil schon in der Liste?
    //
    QString raddrStr = raddr.toString();
    //
    if ( spx42Devices.contains( raddrStr ) )
    {
      lg->debug( QString( "SPX42BtDevices::slotDiscoveryServicesFinished: device: %1, service: %2 always present. Ignore." )
                     .arg( raddr.toString() )
                     .arg( info.serviceName() ) );
    }
    else
    {
      if ( discoverdDevices.contains( raddrStr ) )
      {
        lg->info( QString( "SPX42BtDevices::slotDiscoveryServicesFinished: device: %1, service: %2 add..." )
                      .arg( raddr.toString() )
                      .arg( info.serviceName() ) );
        spx42Devices.insert( raddrStr, discoverdDevices.value( raddrStr ) );
      }
      else
      {
        lg->crit(
            QString( "SPX42BtDevices::slotDiscoveryServicesFinished: device: %1 not in discovered list..." ).arg( raddr.toString() ) );
      }
    }
    emit sigDiscoveredService( raddr, info );
  }
}
