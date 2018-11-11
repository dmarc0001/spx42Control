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
      , btUuiid( QBluetoothUuid::Rfcomm )
      , remoteAddr()
  {
    //
    // Signale des BT Sockets mit slots verbinden
    //
    lg->debug( "SPX42RemotBtDevice::SPX42RemotBtDevice -> connecting bt socket sigs..." );
    connect( socket.get(), QOverload< QBluetoothSocket::SocketError >::of( &QBluetoothSocket::error ), this,
             &SPX42RemotBtDevice::onSocketErrorSlot );
    connect( socket.get(), &QBluetoothSocket::stateChanged, this, &SPX42RemotBtDevice::onStateChangedSlot );
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
  void SPX42RemotBtDevice::startConnection( const QString &mac )
  {
    // TODO: wenn verbunden, trennen oder was?

    // merken der Daten
    remoteAddr = QBluetoothAddress( mac );
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
    lg->debug( "SPX42RemotBtDevice::startConnection -> check for remote addr..." );
    //
    if ( !remoteAddr.isNull() )
    {
      lg->warn( "SPX42RemotBtDevice::startConnection -> remote addr is not set!" );
      return;
    }
    lg->debug( "SPX42RemotBtDevice::startConnection -> check for remote addr...OK" );
    //
    lg->debug( "SPX42RemotBtDevice::startConnection -> connect remote SPX42..." );
    socket->connectToService( remoteAddr, btUuiid );
    lg->info( "SPX42RemotBtDevice::startConnection -> connecting to remote SPX42..." );
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

  void SPX42RemotBtDevice::onSocketErrorSlot( QBluetoothSocket::SocketError error )
  {
    //
    // ein Fehler beim SOCKET trat auf
    // TODO: Fehler behandeln
    //
    lg->warn( "SPX42RemotBtDevice::onSocketErrorSlot -> error while processing bt socket..." );
    switch ( error )
    {
      case QBluetoothSocket::UnknownSocketError:
      case QBluetoothSocket::NoSocketError:
      case QBluetoothSocket::HostNotFoundError:
      case QBluetoothSocket::ServiceNotFoundError:
      case QBluetoothSocket::NetworkError:
      case QBluetoothSocket::UnsupportedProtocolError:
      case QBluetoothSocket::OperationError:
      case QBluetoothSocket::RemoteHostClosedError:
        lg->crit( "SPX42RemotBtDevice::onSocketErrorSlot -> no error handdling implemented yet..." );
        break;
    }
  }

  void SPX42RemotBtDevice::onStateChangedSlot( QBluetoothSocket::SocketState state )
  {
    // TODO: drum kümmern
    lg->debug( "SPX42RemotBtDevice::onStateChangedSlot -> bluethooth onlinestatus has changed..." );
    switch ( state )
    {
      case QBluetoothSocket::UnconnectedState:
        lg->debug( "SPX42RemotBtDevice::onStateChangedSlot -> bluethooth state is now <UnconnectedState>" );
        break;
      case QBluetoothSocket::ClosingState:
        lg->debug( "SPX42RemotBtDevice::onStateChangedSlot -> bluethooth state is now <ClosingState>" );
        break;
      case QBluetoothSocket::ConnectingState:
        lg->debug( "SPX42RemotBtDevice::onStateChangedSlot -> bluethooth state is now <ConnectingState>" );
        break;
      case QBluetoothSocket::ConnectedState:
        lg->debug( "SPX42RemotBtDevice::onStateChangedSlot -> bluethooth state is now <ConnectedState>" );
        break;
      default:
        lg->debug( "SPX42RemotBtDevice::onStateChangedSlot -> bluethooth state is now <ErrorState/undefined>" );
    }
    //
    // Melde das weiter!
    //
    emit onStateChangedSig( state );
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
}
