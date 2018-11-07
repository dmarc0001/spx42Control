#include "BtServiceDiscover.hpp"

#include <utility>

#include <memory>

namespace spx
{
  BtServiceDiscover::BtServiceDiscover( std::shared_ptr< Logger > logger,
                                        QBluetoothAddress &l_addr,
                                        QBluetoothAddress &r_addr,
                                        QObject *parent )
      : QObject( parent ), lg(std::move( logger )), laddr( l_addr ), raddr( r_addr ), servicesCount( 0 ), expression( ".*" )
  {
    //
    // default Bluetooth adapter
    //
    QBluetoothLocalDevice localDevice;
    QBluetoothAddress adapterAddress = localDevice.address();
    lg->debug( QString( "BtServiceDiscover::BtServiceDiscover: local adapter addr: " ).append( adapterAddress.toString() ) );
    lg->debug( QString( "BtServiceDiscover::BtServiceDiscover: remote adapter addr: " ).append( raddr.toString() ) );
    /*
     * In case of multiple Bluetooth adapters it is possible to
     * set which adapter will be used by providing MAC Address.
     * Example code:
     *
     * QBlueto{}othAddress adapterAddress("XX:XX:XX:XX:XX:XX");
     * discoveryAgent = new QBluetoothServiceDiscoveryAgent(adapterAddress);
     */
    discoveryAgent = std::make_unique< QBluetoothServiceDiscoveryAgent >( adapterAddress );
    discoveryAgent->setRemoteAddress( raddr );
    //
    // signale mit Slots verbinden
    //
    connect( discoveryAgent.get(), &QBluetoothServiceDiscoveryAgent::serviceDiscovered, this,
             &BtServiceDiscover::onDiscoveredServiceSlot );
    connect( discoveryAgent.get(), &QBluetoothServiceDiscoveryAgent::finished, this,
             [=] { emit onDiscoverScanFinishedSig( raddr ); } );
  }

  BtServiceDiscover::~BtServiceDiscover()
  {
    lg->debug( "BtServiceDiscover::~BtServiceDiscover..." );
  }

  bool BtServiceDiscover::setServiceFilter( const QString &expr )
  {
    expression.setPattern( expr );
    if ( expression.isValid() )
      return ( true );
    expression.setPattern( ".*" );
    return ( false );
  }

  void BtServiceDiscover::resetServiceFilter( )
  {
    expression.setPattern( ".*" );
  }

  void BtServiceDiscover::start( )
  {
    //
    // starte das suchen nach Services
    //
    discoveryAgent->start();
  }

  void BtServiceDiscover::cancelDiscover( )
  {
    discoveryAgent->stop();
  }

  int BtServiceDiscover::servicesDiscovered( )
  {
    return ( servicesCount );
  }

  void BtServiceDiscover::onDiscoveredServiceSlot( const QBluetoothServiceInfo &info )
  {
    if ( info.serviceName().isEmpty() )
      return;
    QString line = info.serviceName();
    lg->info( QString( "BtServiceDiscover::onDiscoveredServiceSlot: %1 on %2" ).arg( line ).arg( raddr.toString() ) );
    //
    // ist das ein gesuchter service
    //
    if ( expression.isValid() )
    {
      if ( expression.indexIn( line ) < 0 )
      {
        lg->debug( QString( "BtServiceDiscover::onDiscoveredServiceSlot: service %1 is not matching. ignore" ).arg( line ) );
        return;
      }
    }
    //
    if ( !info.serviceDescription().isEmpty() )
      line.append( " " + info.serviceDescription() );
    if ( !info.serviceProvider().isEmpty() )
      line.append( " " + info.serviceProvider() );
    lg->info(
        QString( "BtServiceDiscover::onDiscoveredServiceSlot: %1 on %2 signal to list..." ).arg( line ).arg( raddr.toString() ) );
    //
    // signalisiere dem interessierten dass ein Service gefunden wurde
    //
    emit onDiscoveredServiceSig( raddr, info );
    servicesCount++;
  }
}
