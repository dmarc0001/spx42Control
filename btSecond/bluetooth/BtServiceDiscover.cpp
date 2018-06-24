#include "BtServiceDiscover.hpp"

namespace spx
{
  BtServiceDiscover::BtServiceDiscover( std::shared_ptr< Logger > logger,
                                        QString &dname,
                                        QBluetoothAddress &l_addr,
                                        QBluetoothAddress &r_addr,
                                        QObject *parent )
      : QObject( parent ), lg( logger ), name( dname ), laddr( l_addr ), raddr( r_addr )
  {
    //
    // default Bluetooth adapter
    //
    QBluetoothLocalDevice localDevice;
    QBluetoothAddress adapterAddress = localDevice.address();
    lg->debug( QString( "BtServiceDiscover::BtServiceDiscover: local adapter addr: " ).append( adapterAddress.toString() ) );
    lg->debug( QString( "BtServiceDiscover::BtServiceDiscover: remote adapter addr: " ).append( laddr.toString() ) );
    /*
     * In case of multiple Bluetooth adapters it is possible to
     * set which adapter will be used by providing MAC Address.
     * Example code:
     *
     * QBluetoothAddress adapterAddress("XX:XX:XX:XX:XX:XX");
     * discoveryAgent = new QBluetoothServiceDiscoveryAgent(adapterAddress);
     */
    discoveryAgent = std::unique_ptr< QBluetoothServiceDiscoveryAgent >( new QBluetoothServiceDiscoveryAgent( adapterAddress ) );
    discoveryAgent->setRemoteAddress( raddr );
    //
    // signale mit Slots verbinden
    //
    connect( discoveryAgent.get(), &QBluetoothServiceDiscoveryAgent::serviceDiscovered, this,
             &BtServiceDiscover::slotDiscoveredService );
    connect( discoveryAgent.get(), &QBluetoothServiceDiscoveryAgent::finished, this, [=] { emit sigDiscoverScanFinished( name ); } );
  }

  BtServiceDiscover::~BtServiceDiscover()
  {
    lg->debug( "BtServiceDiscover::~BtServiceDiscover..." );
  }

  void BtServiceDiscover::start( void )
  {
    //
    // starte das suchen nach Services
    //
    discoveryAgent->start();
  }

  int BtServiceDiscover::servicesDiscovered( void )
  {
    return ( servicesCount );
  }

  void BtServiceDiscover::slotDiscoveredService( const QBluetoothServiceInfo &info )
  {
    if ( info.serviceName().isEmpty() )
      return;

    QString line = info.serviceName();
    lg->info( QString( "BtServiceDiscover::slotDiscoveredService: %1 on %2" ).arg( line ).arg( name ) );

    if ( !info.serviceDescription().isEmpty() )
      line.append( " " + info.serviceDescription() );
    if ( !info.serviceProvider().isEmpty() )
      line.append( " " + info.serviceProvider() );
    lg->info( QString( "BtServiceDiscover::slotDiscoveredService: %1 on %2" ).arg( line ).arg( name ) );
    //
    // signalisiere dem interessierten dass ein Service gefunden wurde
    //
    emit sigDiscoveredService( name, info );
    servicesCount++;
  }
}
