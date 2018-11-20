#include "BtDiscoverRemoteService.hpp"

#include <utility>

#include <memory>

namespace spx
{
  BtDiscoverRemoteService::BtDiscoverRemoteService( std::shared_ptr< Logger > logger,
                                                    QBluetoothAddress &l_addr,
                                                    QBluetoothAddress &r_addr,
                                                    QObject *parent )
      : QObject( parent ), lg( std::move( logger ) ), laddr( l_addr ), raddr( r_addr ), servicesCount( 0 ), expression( ".*" )
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
             &BtDiscoverRemoteService::onDiscoveredServiceSlot );
    connect( discoveryAgent.get(), &QBluetoothServiceDiscoveryAgent::finished, this,
             [=] { emit onDiscoverScanFinishedSig( raddr ); } );
  }

  BtDiscoverRemoteService::~BtDiscoverRemoteService()
  {
    lg->debug( "BtServiceDiscover::~BtServiceDiscover..." );
    // sämtliche Verbindungen kappen...
    disconnect( discoveryAgent.get(), nullptr, nullptr, nullptr );
  }

  bool BtDiscoverRemoteService::setServiceFilter( const QString &expr )
  {
    expression.setPattern( expr );
    if ( expression.isValid() )
      return ( true );
    expression.setPattern( ".*" );
    return ( false );
  }

  void BtDiscoverRemoteService::resetServiceFilter()
  {
    expression.setPattern( ".*" );
  }

  void BtDiscoverRemoteService::start()
  {
    //
    // starte das suchen nach Services
    //
    discoveryAgent->start();
  }

  void BtDiscoverRemoteService::cancelDiscover()
  {
    discoveryAgent->stop();
  }

  int BtDiscoverRemoteService::servicesDiscovered()
  {
    return ( servicesCount );
  }

  void BtDiscoverRemoteService::onDiscoveredServiceSlot( const QBluetoothServiceInfo &info )
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
