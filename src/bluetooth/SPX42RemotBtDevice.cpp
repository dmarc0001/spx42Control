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
      , sendTimer( parent )
      , socket( nullptr )
      , btUuiid( ProjectConst::RFCommUUID )
      , remoteAddr()
      , wasSocketError( false )
      , ignoreTimer( false )
  {
    // das interval des Teimer auf 80 ms setzten
    sendTimer.setInterval( SEND_TIMERVAL );
    //
    // verbinde das Timerevent mit der Senderoutine
    //
    connect( &sendTimer, &QTimer::timeout, this, &SPX42RemotBtDevice::onSendSocketTimerSlot );
  }

  /**
   * @brief SPX42RemotBtDevice::~SPX42RemotBtDevice
   */
  SPX42RemotBtDevice::~SPX42RemotBtDevice()
  {
    lg->debug( "SPX42RemotBtDevice::~SPX42RemotBtDevice() -> check if close bluethooth connection nessesary..." );
    sendTimer.stop();
    disconnect( &sendTimer, nullptr, nullptr, nullptr );
    if ( socket != nullptr )
    {
      disconnect( socket, nullptr, nullptr, nullptr );
      if ( socket->state() != QBluetoothSocket::UnconnectedState )
      {
        lg->info( "abort bluethooth connection" );
        socket->abort();
      }
      lg->debug( "SPX42RemotBtDevice::~SPX42RemotBtDevice() -> disconnect all bt socket signals..." );
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
  void SPX42RemotBtDevice::endConnection()
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
   * @brief SPX42RemotBtDevice::sendCommand
   * @param telegram
   */
  void SPX42RemotBtDevice::sendCommand( const QByteArray &telegram )
  {
    //
    // in die sendequeue packen
    //
    sendQueue.enqueue( telegram );
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
        if ( wasSocketError )
          return ( SPX42RemotBtDevice::SPX42_ERROR );
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
   * @brief SPX42RemotBtDevice::getNextRecCommand
   * @return
   */
  spSingleCommand SPX42RemotBtDevice::getNextRecCommand()
  {
    //
    // schaue, ob da was vorhanden war
    //
    if ( rCmdQueue.isEmpty() )
      return ( nullptr );
    //
    // es gibt ein Datum -> zurück
    //
    return ( rCmdQueue.dequeue() );
  }

  /**
   * @brief SPX42RemotBtDevice::onSocketErrorSlot
   * @param error
   */
  void SPX42RemotBtDevice::onSocketErrorSlot( QBluetoothSocket::SocketError error )
  {
    //
    // ein Fehler beim SOCKET trat auf
    //
    lg->warn( "SPX42RemotBtDevice::onSocketErrorSlot -> error while processing bt socket..." );
    wasSocketError = true;
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

  /**
   * @brief SPX42RemotBtDevice::onStateChangedSlot
   * @param state
   */
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
        //
        // den Timer für die Sendequeue starten
        // Voreinstellungen machen
        //
        wasSocketError = false;
        sendQueue.clear();
        recQueue.clear();
        rCmdQueue.clear();
        if ( !sendTimer.isActive() )
          sendTimer.start();
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
    if ( state != QBluetoothSocket::ConnectedState )
    {
      // den Timer für die sendeque stoppen
      sendTimer.stop();
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
    auto canRead = socket->bytesAvailable();
    if ( canRead > 0 )
    {
      //
      // alles lesen und an den Puffer anhängen
      //
      recBuffer.append( socket->readAll() );
      //
      // suche nach dem Ende eines Datagrammee / eines Logeintrages
      //
      int idxOfETX = recBuffer.indexOf( SPX42CommandDef::ETX );
      int idxOfSTX = -1;
      //
      // solange es ein Ende gibt muss ich Datagramme extraieren
      //
      while ( idxOfETX > -1 )
      {
        //
        // da ist ein Endezeichen, also sollte ein Datagramm zu finden sein
        //
        idxOfSTX = recBuffer.indexOf( SPX42CommandDef::STX );
        if ( idxOfSTX < idxOfETX )
        {
          // so wie es soll, das Ende ist nach dem Anfang
          // ich kopiere mir nun das Datagramm
          QByteArray _datagramm = recBuffer.mid( idxOfSTX + 1, idxOfETX - 2 );
#ifdef DEBUG
          lg->debug( QString( "SPX42RemotBtDevice::onReadSocketSlot -> datagram:: <%1>" ).arg( QString( _datagramm ) ) );
#endif
          // und noch das Datagramm aus dem Puffer tilgen
          recBuffer.remove( 0, idxOfETX + 1 );
          // welcehs Kommando?
          char cmd = decodeCommand( _datagramm );
          // in die Empfangsqueue setzen
          rCmdQueue.enqueue( spSingleCommand( new SPX42SingleCommand( cmd, params ) ) );
          // ein timerereignis async zum versenden
          QTimer::singleShot( 5, [this] { emit onCommandRecivedSig(); } );
        }
        else
        {
          // nanu, da ist ein neuer Anfang NACH dem Ende, aber kein Anfang davor
          // d.h. ich habe hier ein unvollständiges Datagramm
          // verwerfe es! also alles bis zum ersten Ende löschen
          recBuffer.remove( 0, idxOfETX + 1 );
          // nächste Runde
        }
        // das Datagrammende Ende neu finden
        idxOfETX = recBuffer.indexOf( SPX42CommandDef::ETX );
      }
      //
      // Puffer auf überlauf prüfen und ggt reagieren
      //
      if ( recBuffer.size() > 2048 )
      {
        lg->crit( "buffer (recive buffer) overflow... data will lost..." );
        recBuffer.clear();
      }
    }
    //
    // ENDE
    //
  }

  /**
   * @brief SPX42RemotBtDevice::onSendSocketTimerSlot on timer routine, wenn online senden an gerät
   */
  void SPX42RemotBtDevice::onSendSocketTimerSlot()
  {
    if ( ignoreTimer )
      return;
    if ( ( socket != nullptr ) && ( socket->state() == QBluetoothSocket::ConnectedState ) && !sendQueue.isEmpty() )
    {
      ignoreTimer = true;
      QByteArray telegram( sendQueue.dequeue() );
#ifdef DEBUG
      lg->debug( QString( "SPX42RemotBtDevice::onSendSocketTimerSlot -> send telegram <%1>..." ).arg( QString( telegram ) ) );
#endif
      socket->write( telegram );
      // Wartezeit startet neu
      sendTimer.start();
      ignoreTimer = false;
    }
  }
}
