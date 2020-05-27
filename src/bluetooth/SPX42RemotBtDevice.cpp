﻿#include "SPX42RemotBtDevice.hpp"

namespace spx
{
  //
  // suchmustrer für Lineend
  //
  // const QByteArray SPX42RemotBtDevice::lineEnd( QByteArray( SPX42CommandDef::CR, SPX42CommandDef::LF ) );

  SPX42RemotBtDevice::SPX42RemotBtDevice( std::shared_ptr< Logger > logger, QObject *parent )
      : QObject( parent )
      , lg( logger )
      , sendTimer( parent )
      , socket( nullptr )
      , btUuiid( ProjectConst::RFCommUUID )
      , remoteAddr()
      , wasSocketError( false )
      , ignoreSendTimer( false )
      , isNormalCommandMode( true )
      , currentDiveNumberForLogDetail( -1 )
      , currentDetailSequenceNumber( -1 )
  {
    // das interval des Teimer auf 80 ms setzten
    sendTimer.setInterval( SEND_TIMERVAL );
    //
    // verbinde das Timerevent mit der Senderoutine
    //
    connect( &sendTimer, &QTimer::timeout, this, &SPX42RemotBtDevice::onSendSocketTimerSlot );
    // das Suchmuster für CRLF bauen
    lineEnd.clear();
    lineEnd.append( SPX42CommandDef::CR );
    lineEnd.append( SPX42CommandDef::LF );
  }

  /**
   * @brief SPX42RemotBtDevice::~SPX42RemotBtDevice
   */
  SPX42RemotBtDevice::~SPX42RemotBtDevice()
  {
    *lg << LDEBUG << "SPX42RemotBtDevice::~SPX42RemotBtDevice() -> check if close bluethooth connection nessesary..." << Qt::endl;
    sendTimer.stop();
    disconnect( &sendTimer, nullptr, nullptr, nullptr );
    if ( socket != nullptr )
    {
      disconnect( socket, nullptr, nullptr, nullptr );
      if ( socket->state() != QBluetoothSocket::UnconnectedState )
      {
        *lg << LINFO << "abort bluethooth connection" << Qt::endl;
        socket->abort();
      }
      *lg << LDEBUG << "SPX42RemotBtDevice::~SPX42RemotBtDevice() -> disconnect all bt socket signals..." << Qt::endl;
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
      *lg << LDEBUG << "SPX42RemotBtDevice::startConnection -> disconnect/remove old connection..." << Qt::endl;
      endConnection();
      socket->deleteLater();
      socket = nullptr;
      *lg << LDEBUG << "SPX42RemotBtDevice::startConnection -> disconnect/remove old connection...OK" << Qt::endl;
    }
    // merken der Daten
    remoteAddr = QBluetoothAddress( mac );
    //
    // zunächst prüfen, ob ein gültiger Service vorhanden ist
    //
    //
    if ( remoteAddr.isNull() )
    {
      *lg << LWARN << "SPX42RemotBtDevice::startConnection -> remote addr is not set!" << Qt::endl;
      return;
    }
    //
    // Signale des BT Sockets mit slots verbinden
    //
    *lg << LDEBUG << "SPX42RemotBtDevice::startConnection -> connecting bt socket sigs..." << Qt::endl;
    socket = new QBluetoothSocket( QBluetoothServiceInfo::RfcommProtocol );
    connect( socket, QOverload< QBluetoothSocket::SocketError >::of( &QBluetoothSocket::error ), this,
             &SPX42RemotBtDevice::onSocketErrorSlot );
    connect( socket, &QBluetoothSocket::stateChanged, this, &SPX42RemotBtDevice::onStateChangedSlot );
    connect( socket, &QBluetoothSocket::readyRead, this, &SPX42RemotBtDevice::onReadSocketSlot );
    *lg << LDEBUG << "SPX42RemotBtDevice::startConnection -> connecting bt socket sigs...OK" << Qt::endl;
    //
    // versuche zu verbinden
    //
    *lg << LDEBUG << "SPX42RemotBtDevice::startConnection -> connect remote SPX42..." << Qt::endl;
    *lg << LINFO << "SPX42RemotBtDevice::startConnection -> connecting to remote SPX42..." << Qt::endl;
    // Verbinden!
    socket->connectToService( remoteAddr, btUuiid );
    *lg << LDEBUG << "SPX42RemotBtDevice::startConnection -> connect remote SPX42...OK" << Qt::endl;
  }

  /**
   * @brief SPX42RemotBtDevice::endConnection
   */
  void SPX42RemotBtDevice::endConnection()
  {
    *lg << LDEBUG << "SPX42RemotBtDevice::endConnection -> try to disconnect bluethoot connection..." << Qt::endl;
    if ( socket != nullptr )
    {
      if ( socket->state() != QBluetoothSocket::UnconnectedState )
      {
        *lg << LINFO << "close bluethooth connection" << Qt::endl;
        socket->close();
      }
      QThread::msleep( 120 );
      if ( socket->state() != QBluetoothSocket::UnconnectedState )
      {
        socket->abort();
      }
    }
  }

  /**
   * @brief SPX42RemotBtDevice::getRemoteConnected
   * @return
   */
  QString SPX42RemotBtDevice::getRemoteConnected()
  {
    if ( socket != nullptr && socket->state() == QBluetoothSocket::ConnectedState )
    {
      return ( remoteAddr.toString() );
    }
    return ( QString() );
  }

  /**
   * @brief SPX42RemotBtDevice::sendCommand
   * @param telegram
   */
  void SPX42RemotBtDevice::sendCommand( const SendListEntry &entry )
  {
    //
    // in die sendequeue packen
    //
    sendList.append( entry );
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
    *lg << LWARN << "SPX42RemotBtDevice::onSocketErrorSlot -> error while processing bt socket..." << Qt::endl;
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
        *lg << LCRIT << "SPX42RemotBtDevice::onSocketErrorSlot -> no error handdling implemented yet..." << Qt::endl;
        break;
    }
  }

  /**
   * @brief SPX42RemotBtDevice::onStateChangedSlot
   * @param state
   */
  void SPX42RemotBtDevice::onStateChangedSlot( QBluetoothSocket::SocketState state )
  {
    switch ( state )
    {
      case QBluetoothSocket::UnconnectedState:
        emit onStateChangedSig( state );
        *lg << LDEBUG << "SPX42RemotBtDevice::onStateChangedSlot -> bluethooth state is now <UnconnectedState>" << Qt::endl;
        break;
      case QBluetoothSocket::ServiceLookupState:
        *lg << LDEBUG << "SPX42RemotBtDevice::onStateChangedSlot -> bluethooth state is now <ServiceLookupState>" << Qt::endl;
        break;
      case QBluetoothSocket::ConnectingState:
        emit onStateChangedSig( state );
        *lg << LDEBUG << "SPX42RemotBtDevice::onStateChangedSlot -> bluethooth state is now <ConnectingState>" << Qt::endl;
        break;
      case QBluetoothSocket::ConnectedState:
        //
        // den Timer für die Sendequeue starten
        // Voreinstellungen machen
        //
        wasSocketError = false;
        sendList.clear();
        recQueue.clear();
        rCmdQueue.clear();
        if ( !sendTimer.isActive() )
          sendTimer.start();
        emit onStateChangedSig( state );
        *lg << LDEBUG << "SPX42RemotBtDevice::onStateChangedSlot -> bluethooth state is now <ConnectedState>" << Qt::endl;
        break;
      case QBluetoothSocket::BoundState:
        *lg << LDEBUG << "SPX42RemotBtDevice::onStateChangedSlot -> bluethooth state is now <BoundState>" << Qt::endl;
        break;
      case QBluetoothSocket::ClosingState:
        emit onStateChangedSig( state );
        *lg << LDEBUG << "SPX42RemotBtDevice::onStateChangedSlot -> bluethooth state is now <ClosingState>" << Qt::endl;
        break;
      case QBluetoothSocket::ListeningState:
        *lg << LDEBUG << "SPX42RemotBtDevice::onStateChangedSlot -> bluethooth state is now <ListeningState>" << Qt::endl;
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
      // recBuffer.append( socket->readAll() );
      recBuffer.append( socket->read( ProjectConst::BUFFER_LEN ) );
      //
      // suche nach dem Ende eines Datagrammee / eines Logeintrages
      //
      int idxOfETX = recBuffer.indexOf( SPX42CommandDef::ETX );
      int idxOfSTX = -1;
      int idxDetailEnd = -1;
      //#######################################################################
      // guck mal in welchem Modus wir sind, und ob da noch ein
      // normales kommando rumlungert
      //#######################################################################
      if ( !isNormalCommandMode )
      {
        // gibt es ein Ende eines Log Strings
        idxDetailEnd = recBuffer.indexOf( SPX42RemotBtDevice::lineEnd );
      }
      if ( !isNormalCommandMode && idxDetailEnd > -1 )
      {
        //
        // Da ist ein Tauchgang Detail, und ist es ist VOR dem Kommando
        //
        if ( idxOfETX > -1 && idxOfETX < idxDetailEnd )
        {
          // ein Kommando ist noch davor...
          *lg << LDEBUG
              << "SPX42RemotBtDevice::onReadSocketSlot -> not normal mode: there is a command response brefore logdetail dataset...."
              << Qt::endl;
        }
        else
        {
          //
          // hier kommt ein log detail eintrag geflogen
          // alles vor dem ist ein Detail-Datensatz
          //
          QByteArray _datagram = recBuffer.left( idxDetailEnd - 1 );
          // QString line = QString( _datagram );
          // lg->info( QString( "DETAIL:<%1>" ).arg( line ) );
          recBuffer.remove( 0, idxDetailEnd + 2 );
          if ( _datagram.size() > 0 )
          {
            if ( decodeLogDetailLine( _datagram ) == ProjectConst::LOG_FIELD_COUNT )
            {
              // es müssen 36 Elemente sein
              rCmdQueue.enqueue( spSingleCommand( new SPX42SingleCommand(
                  SPX42CommandDef::SPX_GET_LOG_DETAIL, params, currentDiveNumberForLogDetail, ++currentDetailSequenceNumber ) ) );
              // ein timerereignis async zum versenden
              QTimer::singleShot( 5, [this] { emit onCommandRecivedSig(); } );
            }
            else
            {
              currentDetailSequenceNumber++;
              //
              // da ist ein fetter Fehler, entweder in der Übertragung oder in der Datei auf dem SPX
              // TODO: Dummy Eintrag einfügen, damit Zeitachse stimmt
              //
              *lg << LWARN
                  << "SPX42RemotBtDevice::onReadSocketSlot -> log datagram has not exact %1 elements. this ist incorrect. ignored."
                  << Qt::endl;
            }
          }
        }
      }
      //#######################################################################
      // ende logdetail
      //#######################################################################

      //
      // normale datagramme bearbeiten
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
          // dei Länge ergibt sich aus dem Ende minus das 0x03 und minus Anfang
          QByteArray _datagramm = recBuffer.mid( idxOfSTX + 1, ( idxOfETX - idxOfSTX ) - 1 );
#ifdef DEBUG
          *lg << LDEBUG << "SPX42RemotBtDevice::onReadSocketSlot -> datagram: <" << _datagramm << ">" << Qt::endl;
#endif
          // und noch das Datagramm aus dem Puffer tilgen
          recBuffer.remove( 0, idxOfETX + 1 );
          // welcehs Kommando?
          char cmd = decodeCommand( _datagramm );
          // in die Empfangsqueue setzen
          rCmdQueue.enqueue( spSingleCommand( new SPX42SingleCommand( cmd, params ) ) );
          //
          // ändert sich der Status des Kommandomode? Normal/Logdetail
          //
          if ( cmd == SPX42CommandDef::SPX_GET_LOG_NUMBER_SE )
          {
            //
            // Kommando beginn oder Ende LOG Details
            // params[1] on/off 0/1
            // params[2] nummer
            //
            if ( params.at( SPXCmdParam::LOGDETAIL_START_END ).toInt() == 1 )
            {
              *lg << LINFO << "SPX42RemotBtDevice::onReadSocketSlot ->  COMMAND GET_LOG_NUMBER_SE ON" << Qt::endl;
              isNormalCommandMode = false;
              currentDiveNumberForLogDetail = params.at( SPXCmdParam::LOGDETAIL_NUMBER ).toInt( nullptr, 16 );
              currentDetailSequenceNumber = -1;
              //
              // weitere zu sendende Sachen blocken
              //
              ignoreSendTimer = true;
            }
            else
            {
              *lg << LINFO << "SPX42RemotBtDevice::onReadSocketSlot -> COMMAND GET_LOG_NUMBER_SE OFF" << Qt::endl;
              isNormalCommandMode = true;
              currentDiveNumberForLogDetail = -1;
              ignoreSendTimer = false;
              currentDetailSequenceNumber = -1;
            }
          }
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
        // das Datagrammende Ende neu finden, falls vorhanden
        idxOfETX = recBuffer.indexOf( SPX42CommandDef::ETX );
      }
      //
      // Puffer auf überlauf prüfen und ggt reagieren
      //
      if ( recBuffer.size() > ProjectConst::BUFFER_LEN )
      {
        *lg << LCRIT << "buffer (recive buffer) overflow... data will lost..." << Qt::endl;
        recBuffer.clear();
      }
    }
    else
    {
      QThread::msleep( 100 );
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
    if ( ignoreSendTimer )
      return;
    if ( ( socket != nullptr ) && ( socket->state() == QBluetoothSocket::ConnectedState ) && !sendList.isEmpty() )
    {
      ignoreSendTimer = true;
      SendListEntry entry( sendList.takeFirst() );
#ifdef DEBUG
      *lg << LDEBUG << "SPX42RemotBtDevice::onSendSocketTimerSlot -> send telegram <" << entry.second << ">..." << Qt::endl;
#endif
      socket->write( entry.second );
      // Wartezeit startet neu
      sendTimer.start();
      ignoreSendTimer = false;
    }
  }

  bool SPX42RemotBtDevice::getIsNormalCommandMode() const
  {
    return isNormalCommandMode;
  }
}  // namespace spx
