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
    *lg << LDEBUG << "BtDiscoverRemoteService::BtDiscoverRemoteService -> local adapter addr: " << adapterAddress.toString()
        << Qt::endl;
    *lg << LDEBUG << "BtDiscoverRemoteService::BtDiscoverRemoteService -> remote adapter addr: " << raddr.toString() << Qt::endl;
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
    *lg << LDEBUG << "BtDiscoverRemoteService::~BtDiscoverRemoteService..." << Qt::endl;
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
    *lg << LINFO << "BtDiscoverRemoteService::onDiscoveredServiceSlot -> " << line << " on " << raddr.toString() << Qt::endl;
    //
    // ist das ein gesuchter service
    //
    if ( expression.isValid() )
    {
      if ( expression.indexIn( line ) < 0 )
      {
        *lg << LDEBUG << "BtDiscoverRemoteService::onDiscoveredServiceSlot -> service " << line << " is not matching. ignore."
            << Qt::endl;
        return;
      }
    }
    //
    if ( !info.serviceDescription().isEmpty() )
      line.append( " " + info.serviceDescription() );
    if ( !info.serviceProvider().isEmpty() )
      line.append( " " + info.serviceProvider() );
    *lg << LINFO << "BtDiscoverRemoteService::onDiscoveredServiceSlot -> " << line << " on " << raddr.toString()
        << " signal to list..." << Qt::endl;
    //
    // signalisiere dem interessierten dass ein Service gefunden wurde
    //
    emit onDiscoveredServiceSig( raddr, info );
    servicesCount++;
  }
}  // namespace spx
