#include "SPX42RemotBtDevice.hpp"

namespace spx
{
  /**
   * @brief SPX42RemotBtDevice::SPX42RemotBtDevice
   * @param logger
   * @param parent
   */
  SPX42RemotBtDevice::SPX42RemotBtDevice( std::shared_ptr< Logger > logger, QObject *parent )
      : QObject( parent )
      , lg( logger )
      , socket( std::unique_ptr< QBluetoothSocket >( new QBluetoothSocket( QBluetoothServiceInfo::RfcommProtocol ) ) )
      , remoteService()
  {
    //
    // Signale des BT Sockets mit slots verbinden
    //
    lg->debug( "SPX42RemotBtDevice::SPX42RemotBtDevice -> connecting bt socket sigs..." );
    connect( socket.get(), &QBluetoothSocket::connected, this, &SPX42RemotBtDevice::onConnectSlot );
    connect( socket.get(), &QBluetoothSocket::disconnected, this, &SPX42RemotBtDevice::onDisconnectSlot );
    connect( socket.get(), &QBluetoothSocket::readyRead, this, &SPX42RemotBtDevice::onReadSocketSlot );
    lg->debug( "SPX42RemotBtDevice::SPX42RemotBtDevice -> connecting bt socket sigs...OK" );
  }

  /**
   * @brief SPX42RemotBtDevice::~SPX42RemotBtDevice
   */
  SPX42RemotBtDevice::~SPX42RemotBtDevice()
  {
    lg->debug( "SPX42RemotBtDevice::~SPX42RemotBtDevice() -> check if close bluethooth connection nessesary..." );
    if ( socket->state() != QBluetoothSocket::UnconnectedState )
    {
      lg->info( "abort bluethooth connection" );
      socket->abort();
    }
    lg->debug( "SPX42RemotBtDevice::~SPX42RemotBtDevice() -> disconnect all bt socket signals..." );
    disconnect( socket.get(), nullptr, nullptr, nullptr );
  }

  /**
   * @brief SPX42RemotBtDevice::startConnection
   * @param remService
   */
  void SPX42RemotBtDevice::startConnection( const QBluetoothServiceInfo &remService )
  {
    // TODO: wenn verbunden, trennen oder was?

    // merken der Daten
    remoteService = remService;
    // Connect to service
    startConnection();
  }

  /**
   * @brief startConnection
   */
  void SPX42RemotBtDevice::startConnection()
  {
    //
    // zunächst prüfen, ob ein gültiger Service vorhanden ist
    //
    lg->debug( "SPX42RemotBtDevice::startConnection -> check for remote service..." );
    if ( !remoteService.isValid() )
    {
      lg->warn( "SPX42RemotBtDevice::startConnection -> remote service description is not valid!" );
      return;
    }
    if ( !remoteService.isRegistered() )
    {
      lg->warn( "SPX42RemotBtDevice::startConnection -> remote service description is not register in SDP implementation!" );
      return;
    }
    if ( !remoteService.isComplete() )
    {
      lg->warn( "SPX42RemotBtDevice::startConnection -> remote service description is not complete!" );
      return;
    }
    lg->debug( "SPX42RemotBtDevice::startConnection -> check for remote service...OK" );
    lg->debug( "SPX42RemotBtDevice::startConnection -> connect remote SPX42..." );

    socket->connectToService( remoteService );
    lg->info( "SPX42RemotBtDevice::startConnection -> connect to service..." );
  }

  /**
   * @brief SPX42RemotBtDevice::endConnection
   */
  void SPX42RemotBtDevice::endConnection( void )
  {
    lg->debug( "SPX42RemotBtDevice::endConnection -> try to disconnect bluethoot connection..." );
    if ( socket->state() != QBluetoothSocket::UnconnectedState )
    {
      lg->info( "close bluethooth connection" );
      socket->close();
    }
  }

  /**
   * @brief SPX42RemotBtDevice::getConnectionStatus
   * @return
   */
  SPX42RemotBtDevice::SPX42ConnectStatus SPX42RemotBtDevice::getConnectionStatus()
  {
    switch ( socket->state() )
    {
      case QBluetoothSocket::UnconnectedState:
      case QBluetoothSocket::ClosingState:
        return ( SPX42RemotBtDevice::SPX42_DISCONNECTED );

      case QBluetoothSocket::ConnectingState:
        return ( SPX42RemotBtDevice::SPX42_CONNECTING );

      case QBluetoothSocket::ConnectedState:
        return ( SPX42RemotBtDevice::SPX42_CONNECTED );

      default:
        return ( SPX42RemotBtDevice::SPX42_ERROR );
    }
  }

  /**
   * @brief SPX42RemotBtDevice::onReadSocketSlot daten können vom BT gelesen werden
   */
  void SPX42RemotBtDevice::onReadSocketSlot()
  {
    //
    // lese Daten vom Socket...
    //
  }

  void SPX42RemotBtDevice::onConnectSlot( void )
  {
    //
    // Wenn der BT Socket verbunden wurde, diesen Slot aufrufen
    //
  }

  void SPX42RemotBtDevice::onDisconnectSlot( void )
  {
    //
    // Wenn der BT Socket geschlossen wurde, diesen Slot aufrufen
    //
  }
}
