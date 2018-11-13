#include "SPX42RemotBtDevice.hpp"

namespace spx
{
  /**
   * @brief SPX42RemotBtDevice::SPX42RemotBtDevice
   * @param logger
   * @param parent
   */
  SPX42RemotBtDevice::SPX42RemotBtDevice( std::shared_ptr< Logger > logger, QObject *parent )
      : QObject( parent ), lg( logger ), socket( nullptr ), btUuiid( ProjectConst::RFCommUUID ), remoteAddr()
  {
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
    // für alle Fälle
    endConnection();
    if ( socket != nullptr )
    {
      disconnect( socket, nullptr, nullptr, nullptr );
      delete socket;
    }
  }

  /**
   * @brief SPX42RemotBtDevice::startConnection
   * @param remService
   */
  void SPX42RemotBtDevice::startConnection( const QString &mac )
  {
    // TODO: wenn verbunden, trennen oder was?
    if ( socket != nullptr )
    {
      endConnection();
      delete socket;
      socket = nullptr;
    }
    // merken der Daten
    remoteAddr = QBluetoothAddress( mac );
    //
    // zunächst prüfen, ob ein gültiger Service vorhanden ist
    //
    //
    if ( remoteAddr.isNull() )
    {
      lg->warn( "SPX42RemotBtDevice::startConnection -> remote addr is not set!" );
      return;
    }
    //
    // Signale des BT Sockets mit slots verbinden
    //
    lg->debug( "SPX42RemotBtDevice::startConnection -> connecting bt socket sigs..." );
    socket = new QBluetoothSocket( QBluetoothServiceInfo::RfcommProtocol );
    connect( socket, QOverload< QBluetoothSocket::SocketError >::of( &QBluetoothSocket::error ), this,
             &SPX42RemotBtDevice::onSocketErrorSlot );
    connect( socket, &QBluetoothSocket::stateChanged, this, &SPX42RemotBtDevice::onStateChangedSlot );
    connect( socket, &QBluetoothSocket::readyRead, this, &SPX42RemotBtDevice::onReadSocketSlot );
    lg->debug( "SPX42RemotBtDevice::startConnection -> connecting bt socket sigs...OK" );
    //
    // versuche zu verbinden
    //
    lg->debug( "SPX42RemotBtDevice::startConnection -> connect remote SPX42..." );
    lg->info( "SPX42RemotBtDevice::startConnection -> connecting to remote SPX42..." );
    // Verbinden!
    socket->connectToService( remoteAddr, btUuiid );
    lg->debug( "SPX42RemotBtDevice::startConnection -> connect remote SPX42...OK" );
  }

  /**
   * @brief SPX42RemotBtDevice::endConnection
   */
  void SPX42RemotBtDevice::endConnection( void )
  {
    lg->debug( "SPX42RemotBtDevice::endConnection -> try to disconnect bluethoot connection..." );
    if ( socket != nullptr )
    {
      if ( socket->state() != QBluetoothSocket::UnconnectedState )
      {
        lg->info( "close bluethooth connection" );
        socket->close();
      }
      if ( socket->state() != QBluetoothSocket::UnconnectedState )
      {
        socket->abort();
      }
    }
  }

  /**
   * @brief SPX42RemotBtDevice::getConnectionStatus
   * @return
   */
  SPX42RemotBtDevice::SPX42ConnectStatus SPX42RemotBtDevice::getConnectionStatus()
  {
    if ( socket == nullptr )
      return ( SPX42RemotBtDevice::SPX42_DISCONNECTED );

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
    //
    lg->warn( "SPX42RemotBtDevice::onSocketErrorSlot -> error while processing bt socket..." );
    emit onSocketErrorSig( error );
    //
    // debug...
    //
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
        // TODO: Fehler behandeln, evtl intelligent neu verbinden?
        lg->crit( "SPX42RemotBtDevice::onSocketErrorSlot -> no error handdling implemented yet..." );
        break;
    }
  }

  void SPX42RemotBtDevice::onStateChangedSlot( QBluetoothSocket::SocketState state )
  {
    // TODO: drum kümmern
    // lg->debug( "SPX42RemotBtDevice::onStateChangedSlot -> bluethooth onlinestatus has changed..." );
    switch ( state )
    {
      case QBluetoothSocket::UnconnectedState:
        emit onStateChangedSig( state );
        lg->debug( "SPX42RemotBtDevice::onStateChangedSlot -> bluethooth state is now <UnconnectedState>" );
        break;
      case QBluetoothSocket::ServiceLookupState:
        lg->debug( "SPX42RemotBtDevice::onStateChangedSlot -> bluethooth state is now <ServiceLookupState>" );
        break;
      case QBluetoothSocket::ConnectingState:
        emit onStateChangedSig( state );
        lg->debug( "SPX42RemotBtDevice::onStateChangedSlot -> bluethooth state is now <ConnectingState>" );
        break;
      case QBluetoothSocket::ConnectedState:
        emit onStateChangedSig( state );
        lg->debug( "SPX42RemotBtDevice::onStateChangedSlot -> bluethooth state is now <ConnectedState>" );
        break;
      case QBluetoothSocket::BoundState:
        lg->debug( "SPX42RemotBtDevice::onStateChangedSlot -> bluethooth state is now <BoundState>" );
        break;
      case QBluetoothSocket::ClosingState:
        emit onStateChangedSig( state );
        lg->debug( "SPX42RemotBtDevice::onStateChangedSlot -> bluethooth state is now <ClosingState>" );
        break;
      case QBluetoothSocket::ListeningState:
        lg->debug( "SPX42RemotBtDevice::onStateChangedSlot -> bluethooth state is now <ListeningState>" );
        break;
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
}
