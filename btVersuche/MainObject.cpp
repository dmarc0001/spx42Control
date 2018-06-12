#include <QThread>

#include "MainObject.hpp"
#include "bluetooth/BtLocalDevice.hpp"
#include "bluetooth/BtService.hpp"

MainObject::MainObject( int argc, char *argv[] ) : QObject(), isRunnning( true ), btServiceDiscover( nullptr )
{
  lg = std::shared_ptr< Logger >( new Logger() );
  lg->startLogging( LG_DEBUG, "btversuche.log" );
  lg->debug( "MainObject::MainObject" );
  ca = new QCoreApplication( argc, argv );
}

MainObject::~MainObject()
{
  lg->debug( "MainObject::~MainObject" );
  lg->shutdown();
  delete ca;
}

int MainObject::exec( void )
{
  lg->debug( "MainObject::exec...." );
  //
  // erzeuge lokales device, lebensdauer bis exec zuende == Ende Objekt
  //
  BtLocalDevice lbtd( lg, this );
  //
  // verbinde Signale
  //
  connect( &lbtd, &BtLocalDevice::sigFoundDevice, this, &MainObject::slotDeviceDiscovered );
  connect( &lbtd, &BtLocalDevice::sigDiscoverFinished, this, &MainObject::slotDeviceDiscoverFinished );
  //
  // initiiere einen SCAN
  //
  lbtd.startDeviceScan();
  //
  // übergebe an die Eventschleife und
  // warte auf das seelige Ende
  //
  return ca->exec();
}

void MainObject::slotDeviceDiscovered( const QBluetoothDeviceInfo &info )
{
  lg->debug( QString( "MainObject::slotDeviceDiscovered: device: " ).append( info.address().toString() ) );
  //
  // umgehend ein Service discovering auslösen?
  //
  btDevices.append( info );
}

void MainObject::slotDeviceDiscoverFinished( void )
{
  //
  // falls geräte gefunden, finde services
  //
  lg->debug( "MainObject::slotDeviceDiscoverFinished..." );
  if ( !btDevices.isEmpty() )
  {
    lg->debug( "MainObject::slotDeviceDiscoverFinished: found devices try discover services..." );
    // den Iterator auf Start setzten
    itDevice = btDevices.begin();
    delete btServiceDiscover;
    btServiceDiscover = new BtService( lg, itDevice->address(), this );
    connect( btServiceDiscover, &BtService::sigServiceDiscoverStarted, this, &MainObject::slotServiceDiscoverStarted );

    searchServices();
  }
  ca->quit();
}

void MainObject::slotServiceDiscoverStarted( void )
{
  lg->debug( "MainObject::slotServiceDiscoverStarted..." );
}

void MainObject::slotServiceDiscoverCanceled( void )
{
  lg->debug( "MainObject::slotServiceDiscoverCanceled..." );
  mmmmmmmmmmmmmmmmmmmmmmmmmmm
      mmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmm
}

void MainObject::slotServiceDiscoverError( QBluetoothServiceDiscoveryAgent::Error error )
{
  lg->debug( QString( "MainObject::slotServiceDiscoverError: %1" ).arg( error ) );
}

void MainObject::slotServiceDiscovered( const QBluetoothServiceInfo &info )
{
  lg->debug( QString( "MainObject::slotServiceDiscovered: " ).append( info.serviceName() ) );
}

void MainObject::slotServiceDiscoverFinished( void )
{
  lg->debug( "MainObject::slotServiceDiscoverFinished..." );
}

int MainObject::searchServices( void )
{
  int count = 0;
  //
  // Devices aus dem Vector untersuchen
  //
  lg->debug( QString( "MainObject::searchServices..." ) );
  QVector< QBluetoothDeviceInfo >::iterator device;
  for ( device = btDevices.begin(); device != btDevices.end(); ++device )
  {
    if ( device->isValid() )
    {
      count++;
      lg->debug( QString( "MainObject::searchServices: search services on valid device %1" ).arg( device->address().toString() ) );
    }
  }
  return ( count );
}
