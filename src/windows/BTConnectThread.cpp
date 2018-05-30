#include "BtConnectThread.hpp"

namespace spx
{
  BTConnectThread::BTConnectThread(QObject *parent, Logger *log, const QByteArray &dName, const QByteArray &pi, SOCKET bts ) :
    QThread( parent ),
    lg(log),
    deviceName( QByteArray(dName ) ),
    pin( QByteArray(pi) ),
    authError( false ),
    btSocket( bts )
  {
  }

  BTConnectThread::~BTConnectThread()
  {
    lg->debug("WinBTConnectThread::~WinBTConnectThread....");
  }

  void BTConnectThread::isFinishedSlot()
  {
    lg->debug( "WinBTConnectThread::isFinishedSlot()...." );
    deleteLater();
  }

  void BTConnectThread::run()
  {
    int lastError = ProjectConst::CXN_SUCCESS;
    DWORD dwRet;

    setObjectName( "WinBTConnectThread" );
    lg->debug("WinBTConnectThread::run -> connecting thread start...");
    emit btConnectingSig();
    //
    // Adress family auf Bluetooth setzen, RFCOMM setzen, Port sollte 0 sein
    //
    ZeroMemory( &SockAddrBthServer, sizeof( SOCKADDR_BTH ) );
    SockAddrBthServer.addressFamily = AF_BTH;
    SockAddrBthServer.serviceClassId = RFCOMM_PROTOCOL_UUID;
    SockAddrBthServer.port = BT_PORT_ANY;
    lastError = BTConnectThread::str2ba( deviceName.constData(), &SockAddrBthServer.btAddr );
    //
    // initialisiere die Struktur für ein BT Gerät
    // benötigt für Pairing
    //
    btdi.dwSize = sizeof(BLUETOOTH_DEVICE_INFO);
    btdi.Address.ullLong = SockAddrBthServer.btAddr;
    btdi.ulClassofDevice = 0;
    btdi.fConnected = false;
    btdi.fRemembered = false;
    btdi.fAuthenticated = false;
    btdi.stLastSeen = SYSTEMTIME();
    btdi.stLastUsed = SYSTEMTIME();
    //
    // Socket konfigurieren, Auth AN, CRYPT AUS
    //
    ULONG au1 = 1L;
    ::setsockopt( btSocket, SOL_SOCKET, SO_BTH_AUTHENTICATE , (char*)&au1, sizeof(ULONG) );
    ULONG au2 = 0L;
    ::setsockopt( btSocket, SOL_SOCKET, SO_BTH_ENCRYPT  , (char*)&au2, sizeof(ULONG) );
    // dem Socket sofortiges unterbrechen beibringen
    LINGER li = {1, 0};
    ::setsockopt( btSocket, SOL_SOCKET, SO_LINGER, (char*)&li, sizeof(li) );
    //
    // auth callback setzen
    //
    lg->debug("WinBTConnectThread::run -> set auth callback....");
    hRegHandle = 0;
    dwRet = BluetoothRegisterForAuthenticationEx(
                &btdi,
                &hRegHandle,
                reinterpret_cast<PFN_AUTHENTICATION_CALLBACK_EX>(&BTConnectThread::BluetoothAuthCallback),
                (PVOID)this/* this->pin...*/);
    //
    // Wurde der Callback erfolgreich registiriert?
    //
    if(dwRet != ERROR_SUCCESS)
    {
      lg->debug( QString("WinBTConnectThread::run -> BluetoothRegisterForAuthenticationEx ret <%1>").arg(dwRet, 10, 10, QChar('0')) );
      lastError = ProjectConst::CXN_ERROR;
    }
    //dwRet = BluetoothAuthenticateDeviceEx( NULL, NULL , &btdi, &btdi, MITMProtectionNotRequired );
    //
    // Verbinde den Socket mit einem entfernten Gerät via dessen Addr
    //
    if( lastError == ProjectConst::CXN_SUCCESS )
    {
      lg->debug("WinBTDevice::connectBt -> connect socket....");
      //
      // hier wird dann auch bei Bedarf der Auth-Callback gerufen
      //
      lastError = ::connect( btSocket, (struct sockaddr *)&SockAddrBthServer, sizeof(SOCKADDR_BTH) );
      msleep(40);
      // den Callback entfernen
      BluetoothUnregisterAuthentication( hRegHandle );
      if( SOCKET_ERROR == lastError )
      {
        lastError = WSAGetLastError();
        lg->crit(QString("WinBTDevice::connectBt -> connect failed, errcode: <%1>").arg(lastError, 0, 10,QChar('0')));
        if( authError )
        {
          lg->crit("WinBTDevice::connectBt -> authentification error");
        }
        ::closesocket( btSocket );
      }
    }
    //
    // war das erfolgreich, sende "Verbunden" und beende den Thread
    //
    if( lastError == ProjectConst::CXN_SUCCESS )
    {
      lg->debug("WinBTConnectThread::run -> connect socket...OK");
      emit btConnectedSig(deviceName);
    }
    else
    {
      lg->crit("WinBTConnectThread::run -> connect socket...ERROR");
      if( authError )
      {
        emit btConnectErrorSig(WSAEACCES);
      }
      else
      {
        emit btConnectErrorSig(lastError);
      }
    }
    lg->debug("WinBTConnectThread::run -> Thread end");
  }

  void BTConnectThread::cancel()
  {
    lg->debug("WinBTConnectThread::cancel -> thread should cancel...");
    deleteLater();
  }

  bool WINAPI BTConnectThread::BluetoothAuthCallback(LPVOID pvParam, PBLUETOOTH_AUTHENTICATION_CALLBACK_PARAMS pAuthCallbackParams)
  {
    DWORD dwRet;
    QString pinStr;
    BTConnectThread *wt = nullptr;
    //
    // Wenn der user-Parameter ein Zeiger ist, dann ist er Zeiger auf den Thread
    //
    if( pvParam != nullptr )
    {
      //
      // Hole aus dem Threadobjekt die PIN
      //
      wt = static_cast<BTConnectThread*>(pvParam);
      pinStr = QString(wt->pin);
    }
    else
    {
      // und gleich FEHLER senden
      // da ist keine PIN zu holen
      return( false );
    }
    qDebug().noquote() << QString("WinBTConnectThread::BluetoothAuthCallback -> applicate pin: ").append(pinStr);
    //
    // die PIN als wchar_t* bereitstellen
    //
    wchar_t* pass;
    char buffer[1024];
    pass = (wchar_t*)&buffer[0];
    int length = pinStr.toWCharArray(pass);
    pass[length]=0; // Nullterminierend
    //
    dwRet = BluetoothSendAuthenticationResponse(NULL, &(pAuthCallbackParams->deviceInfo), pass );
    //msleep(20);
    if(dwRet != ERROR_SUCCESS)
    {
      if( wt != nullptr )
      {
        wt->authError = true;
      }
      return( false );
    }
    return( true );
  }

  /**
 * Statische Funktion zum Umrechnen von BT-Addr in String zu BT Addr ulong
 */
  int BTConnectThread::str2ba( const char *straddr, BTH_ADDR *btaddr )
  {
    int i;
    unsigned int aaddr[6];
    BTH_ADDR tmpaddr = 0;
    if( sscanf( straddr, "%02x:%02x:%02x:%02x:%02x:%02x", &aaddr[0], &aaddr[1], &aaddr[2], &aaddr[3], &aaddr[4], &aaddr[5] )
        != 6 )
    {
      return (ProjectConst::CXN_ERROR);
    }
    *btaddr = 0;
    for( i = 0; i < 6; i++ )
    {
      tmpaddr = (BTH_ADDR)(aaddr[i] & 0xff);
      *btaddr = ((*btaddr) << 8) + tmpaddr;
    }
    return (ProjectConst::CXN_SUCCESS);
  }

}
