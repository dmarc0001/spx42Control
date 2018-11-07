#include "SPX42RemotBtDevice.hpp"

namespace spx
{
  SPX42RemotBtDevice::SPX42RemotBtDevice( std::shared_ptr< Logger > logger, QObject *parent )
      : QObject( parent )
      , lg( logger )
      , socket( std::unique_ptr< QBluetoothSocket >( new QBluetoothSocket( QBluetoothServiceInfo::RfcommProtocol ) ) )
      , remoteService()
  {
  }

  SPX42RemotBtDevice::~SPX42RemotBtDevice()
  {
  }

  void SPX42RemotBtDevice::startConnection( const QBluetoothServiceInfo &remService )
  {
    // TODO: wenn verbunden, trennen oder was?

    // merken der Daten
    remoteService = remService;
    // Connect to service
    socket->connectToService( remService );
    lg->info( "SPX42RemotBtDevice::startConnection -> connect to service..." );

    connect( socket.get(), SIGNAL( readyRead() ), this, SLOT( readSocket() ) );
    connect( socket.get(), SIGNAL( connected() ), this, SLOT( connected() ) );
    connect( socket.get(), SIGNAL( disconnected() ), this, SIGNAL( disconnected() ) );
  }
}
