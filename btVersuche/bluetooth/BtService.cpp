#include "BtService.hpp"

BtService::BtService( std::shared_ptr< Logger > logger, const QBluetoothAddress &address, QObject *parent )
    : QObject( parent ), lg( logger )
{
  lg->debug( "BtServices::BtServices..." );
  // Using default Bluetooth adapter
  QBluetoothLocalDevice localDevice;
  QBluetoothAddress adapterAddress = localDevice.address();
  /*
   * In case of multiple Bluetooth adapters it is possible to
   * set which adapter will be used by providing MAC Address.
   * Example code:
   *
   * QBluetoothAddress adapterAddress("XX:XX:XX:XX:XX:XX");
   * discoveryAgent = new QBluetoothServiceDiscoveryAgent(adapterAddress);
   */
  //
  // Service discovery agenten machen
  //
  discoveryAgent = new QBluetoothServiceDiscoveryAgent( adapterAddress );
  discoveryAgent->setRemoteAddress( address );
  //
  // die Signale des Agent mit den Slots verbinden
  //
  connect( discoveryAgent, &QBluetoothServiceDiscoveryAgent::canceled, this, &BtService::slotDiscoverCanceled );
  connect( discoveryAgent, &QBluetoothServiceDiscoveryAgent::serviceDiscovered, this, &BtService::slotDiscoveredService );
  connect( discoveryAgent, &QBluetoothServiceDiscoveryAgent::finished, this, &BtService::slotDiscoverFinished );
  connect( discoveryAgent, QOverload< QBluetoothServiceDiscoveryAgent::Error >::of( &QBluetoothServiceDiscoveryAgent::error ), this,
           &BtService::slotDiscoverError );
  //
  // Agenten starten
  //
  emit sigServiceDiscoverStarted();
  discoveryAgent->clear();
  discoveryAgent->start();
}

BtService::~BtService()
{
  delete discoveryAgent;
}

void BtService::slotDiscoveredService( const QBluetoothServiceInfo &info )
{
  if ( info.serviceName().isEmpty() )
  {
    lg->warn( "BtServices::addService: service name is empty..." );
    return;
  }
  QString line = info.serviceName();

  if ( !info.serviceDescription().isEmpty() )
    line.append( "\n\t" + info.serviceDescription() );
  if ( !info.serviceProvider().isEmpty() )
    line.append( "\n\t" + info.serviceProvider() );
  lg->info( QString( "BtServices::addService: " ).append( line ) );
  emit sigServiceDiscovered( info );
}

void BtService::slotDiscoverFinished( void )
{
  lg->debug( "BtServices::discoverFinished..." );
  emit sigServiceDiscoverFinished();
}

void BtService::slotDiscoverCanceled( void )
{
  lg->debug( "BtService::slotDiscoverCanceled" );
}

void BtService::slotDiscoverError( QBluetoothServiceDiscoveryAgent::Error error )
{
  lg->crit( QString( "BtService::slotDiscoverError: %1" ).arg( error ) );
}
