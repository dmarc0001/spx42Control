﻿#include "BtServiceDiscover.hpp"

namespace spx
{
  BtServiceDiscover::BtServiceDiscover( std::shared_ptr< Logger > logger,
                                        QBluetoothAddress &l_addr,
                                        QBluetoothAddress &r_addr,
                                        QObject *parent )
      : QObject( parent ), lg( logger ), laddr( l_addr ), raddr( r_addr ), expression( ".*" )
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
    connect( discoveryAgent.get(), &QBluetoothServiceDiscoveryAgent::finished, this, [=] { emit sigDiscoverScanFinished( raddr ); } );
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

  void BtServiceDiscover::resetServiceFilter( void )
  {
    expression.setPattern( ".*" );
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
    lg->info( QString( "BtServiceDiscover::slotDiscoveredService: %1 on %2" ).arg( line ).arg( raddr.toString() ) );
    //
    // ist das ein gesuchter service
    //
    if ( expression.isValid() )
    {
      if ( expression.indexIn( line ) < 0 )
      {
        lg->debug( QString( "BtServiceDiscover::slotDiscoveredService: service %1 is not matching. ignore" ).arg( line ) );
        return;
      }
    }
    //
    if ( !info.serviceDescription().isEmpty() )
      line.append( " " + info.serviceDescription() );
    if ( !info.serviceProvider().isEmpty() )
      line.append( " " + info.serviceProvider() );
    lg->info( QString( "BtServiceDiscover::slotDiscoveredService: %1 on %2 signal to list..." ).arg( line ).arg( raddr.toString() ) );
    //
    // signalisiere dem interessierten dass ein Service gefunden wurde
    //
    emit sigDiscoveredService( raddr, info );
    servicesCount++;
  }
}