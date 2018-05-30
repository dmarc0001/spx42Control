#include "BTDiscoverThread.hpp"
#include <string>

namespace spx
{
  const ulong ulResultSetAllocSize = 8192;
  const ulong ulPQSSize = sizeof(WSAQUERYSET);
  const ulong BTDiscoverThread::ulDeviceSearchFlags = LUP_RETURN_NAME | LUP_CONTAINERS | LUP_RETURN_ADDR | LUP_FLUSHCACHE | LUP_RETURN_TYPE;
  const ulong BTDiscoverThread::ulServiceBeginFlags = LUP_FLUSHCACHE | LUP_RETURN_NAME | LUP_RETURN_TYPE | LUP_RETURN_ADDR | LUP_RETURN_BLOB | LUP_RETURN_COMMENT;
  const ulong BTDiscoverThread::ulServiceNextFlags = LUP_FLUSHCACHE | LUP_RETURN_NAME | LUP_RETURN_ADDR | LUP_RETURN_BLOB | LUP_RETURN_COMMENT;

  /*
   * Konstruktor, wird mit Parent und Logger aufgerufen
   */
  BTDiscoverThread::BTDiscoverThread( QObject *parent, Logger *logger ) :
    QThread( parent ),
    lg( logger ),
    currDevice( nullptr ),
    pWSAquerySet( nullptr ),
    pWSAquerySetDevice( nullptr ),
    pWSAquerySetResult( nullptr ),
    pWSAquerySetResult2( nullptr ),
    hDefaultProcessHeap( GetProcessHeap() )
  {
    setObjectName( "WinDiscoverThread" );
  }

  /*
   * Destruktor
   */
  BTDiscoverThread::~BTDiscoverThread()
  {
  }

  /*
   * Allociert den benötigten Speicher vom Heap für die diversen Anfragen
   */
  bool BTDiscoverThread::allocHeap( void )
  {
    //
    // allocieren der WSAQUERRYSETS
    //
    pWSAquerySet = (PWSAQUERYSETW)HeapAlloc( hDefaultProcessHeap, HEAP_ZERO_MEMORY, ulPQSSize );
    pWSAquerySetDevice = (PWSAQUERYSETW)HeapAlloc( hDefaultProcessHeap, HEAP_ZERO_MEMORY, ulPQSSize );
    pWSAquerySetResult = (PWSAQUERYSETW)HeapAlloc( hDefaultProcessHeap, HEAP_ZERO_MEMORY, ulResultSetAllocSize );
    pWSAquerySetResult2 = (PWSAQUERYSETW)HeapAlloc( hDefaultProcessHeap, HEAP_ZERO_MEMORY, ulResultSetAllocSize );
    if( pWSAquerySet == nullptr || pWSAquerySetDevice == nullptr || pWSAquerySetResult == nullptr || pWSAquerySetResult2 == nullptr )
    {
      freeHeap();
      return (false);
    }
    return (true);
  }

  /*
   * gibt den reservierten Speicher für die Anfagen wieder frei
   */
  void BTDiscoverThread::freeHeap( void )
  {
    if( pWSAquerySet != nullptr )
    {
      HeapFree( hDefaultProcessHeap, 0, pWSAquerySet );
      pWSAquerySet = nullptr;
    }
    if( pWSAquerySetDevice != nullptr )
    {
      HeapFree( hDefaultProcessHeap, 0, pWSAquerySetDevice );
      pWSAquerySetDevice = nullptr;
    }
    if( pWSAquerySetResult != nullptr )
    {
      HeapFree( hDefaultProcessHeap, 0, pWSAquerySetResult );
      pWSAquerySetResult = nullptr;
    }
    if( pWSAquerySetResult != nullptr )
    {
      HeapFree( hDefaultProcessHeap, 0, pWSAquerySetResult2 );
      pWSAquerySetResult2 = nullptr;
    }
  }

  /*
   * Sucht nach Servivces auf dem angegebenen Gerät
   *
   * @param das Gerät auf dem Gesucht werden soll
   */
  bool BTDiscoverThread::searchServices( BluetoothDeviceDescriber *currDevice )
  {
    HANDLE hServices;
    ulong ulResultSetSize = 0;
    int result;
    int lastError;
    //
    // Frage nach Services auf dem Gerät, pWSAquerySetDevice ist gesetzt in run()
    //
    result = WSALookupServiceBeginW( pWSAquerySetDevice, ulServiceBeginFlags, &hServices );
    if( result != 0 )
    {
      lastError = WSAGetLastError();
      if( lastError == WSASERVICE_NOT_FOUND )
      {
        lg->info("WinDiscoverThread::searchServices -> not services found");
        delete( currDevice );
        return( true );
      }
      lg->crit("WinDiscoverThread::searchServices -> An error occured while initialising query for devices, closing....");
      delete (currDevice);
      lg->crit( QString("WinDiscoverThread::searchServices -> Error: <%1>").arg(lastError, 0, 8, QChar('0')));
      return (false);
    }
    //
    //Start quering for device services
    //
    while( result == 0 )
    {
      //
      // Wenn Services gestartet wurde
      //
      ZeroMemory( pWSAquerySetResult2, ulResultSetAllocSize );
      ulResultSetSize = ulResultSetAllocSize;
      //
      // frage nach Services auf dem Gerät
      //
      msleep( 600 );
      result = WSALookupServiceNextW( hServices, ulServiceNextFlags, &ulResultSetSize, pWSAquerySetResult2 );
      if( result == 0 )
      {
        //
        // Wurde ein Service gefunden:
        //
        QString serviceName = QString::fromWCharArray( (wchar_t*)pWSAquerySetResult2->lpszServiceInstanceName );
        lg->info(QString("WinDiscoverThread::searchServices -> Service found: <").append(serviceName).append(">"));
        //
        // Ist es ein gesuchter Service?
        //
        if( serviceName.contains( ProjectConst::searchedServiceRegex ) )
        {
          lg->info( "WinDiscoverThread::searchServices -> found device for connection " );
          currDevice->addServiceName( serviceName );
        }
        else
        {
          lg->debug( "WinDiscoverThread::searchServices -> not serached service found on device. Ignore! " );
        }
      }
      else
      {
        lg->debug(QString("WinDiscoverThread::searchServices -> not more services found. WSAErrorcode <%1>").arg(WSAGetLastError(), 0, 8, QChar('0')));
      }
    }
    if( currDevice->istDefined() )
    {
      // Signal für neues Gerät senden
      lg->info(QString("WinDiscoverThread::searchServices -> add device: <").append(currDevice->getDevName()).append(">, send SIGNAL new device found..."));
      emit newDeviceFoundSig( currDevice );
    }
    else
    {
      lg->debug("WinDiscoverThread::searchServices -> not searched services found.ignore device");
      delete (currDevice);
    }
    currDevice = NULL;
    return (true);
  }

  /*
   * hier startet der Thread, ende heisst Threadende, eigene WSAStartup-Abteilung
   */
  void BTDiscoverThread::run()
  {
    WSADATA data;
    HANDLE hDevices;
    ulong ulResultSetSize = 0;
    BluetoothDeviceDescriber *currDevice = nullptr;
    SOCKET sock;
    WSAPROTOCOL_INFOW protocolInfo;
    int protocolInfoSize;
    int result;
    //
    lg->debug("WinDiscoverThread::run -> thread start...");
    protocolInfoSize = sizeof(protocolInfo);
    //
    //initializing winsock
    //
    result = WSAStartup( MAKEWORD( 2, 2 ), &data );
    if( result != 0 )
    {
      lg->crit("WinDiscoverThread::run -> An error occured while initialising winsock, closing....");
      WSACleanup();
      btConnectErrorSig( result );
      return;
    }
    //
    // Create a bluetooth socket
    //
    sock = ::socket( AF_BTH, SOCK_STREAM, BTHPROTO_RFCOMM );
    if( sock == INVALID_SOCKET )
    {
      result = WSAGetLastError();
      lg->crit( QString("WinDiscoverThread::run -> Failed to get bluetooth socket with error code <%1>").arg( result, 0, 10, QChar('0') ));
      WSACleanup();
      btConnectErrorSig( result );
      return;
    }
    //
    // Get the bluetooth device info using getsockopt()
    //
    if( ::getsockopt( sock, SOL_SOCKET, SO_PROTOCOL_INFO, (char*)&protocolInfo, &protocolInfoSize ) != 0 )
    {
      result = WSAGetLastError();
      lg->crit( QString("WinDiscoverThread::run -> Failed to get protocol info, error <%1>").arg( result, 0, 10, QChar('0')) );
      ::closesocket( sock );
      WSACleanup();
      btConnectErrorSig( result );
      return;
    }
    if( !allocHeap() )
    {
      lg->crit("WinDiscoverThread::run -> An error occured while allocate memory for WSAQUERYSET, closing....");
      ::closesocket( sock );
      WSACleanup();
      btConnectErrorSig( result );
      return;
    }
    //
    // Speicher sind allociert, es kann weiter gehen
    // signalisiere Beginn Discovering
    //
    if( protocolInfo.iAddressFamily == AF_BTH )
    {
      lg->debug( "WinDiscoverThread::run -> protocolInfo.iAddressFamily = AF_BTH: TRUE");
    }
    else
    {
      lg->crit( "WinDiscoverThread::run -> protocolInfo.iAddressFamily = AF_BTH: FALSE");
    }
    ULONG au = 1L;
    ::setsockopt( sock, SOL_SOCKET, SO_BTH_AUTHENTICATE , (char*)&au, sizeof(ULONG) );
    emit btDiscoveringSig();
    ZeroMemory( pWSAquerySet, ulPQSSize );
    pWSAquerySet->dwSize = ulPQSSize;
    pWSAquerySet->dwNameSpace = NS_BTH;
    //
    // Service starten
    //
    result = WSALookupServiceBeginW( pWSAquerySet, ulDeviceSearchFlags, &hDevices );
    if( result != 0 )
    {
      lg->crit( "WinDiscoverThread::run -> An error occured while initialising look for devices, closing...." );
      ::closesocket( sock );
      freeHeap();
      WSACleanup();
      btConnectErrorSig( result );
      return;
    }
    //
    // starte Suche nach BT Geräten, solange Geräte gefunden werden
    // vorher etwas warten, um den Geräten Zeit zum Scannen zu geben
    //
    while( result == 0 )
    {
      QString devName;
      //
      // nächstes Gerät finden
      //
      ZeroMemory( pWSAquerySetResult, ulResultSetAllocSize );
      ulResultSetSize = ulResultSetAllocSize;
      result = WSALookupServiceNextW( hDevices,
                                      ulDeviceSearchFlags,
                                      &ulResultSetSize,
                                      pWSAquerySetResult
                                      );
      if( pWSAquerySetResult->lpszServiceInstanceName != nullptr )
      {
        devName = QString::fromWCharArray( pWSAquerySetResult->lpszServiceInstanceName );
      }
      else
      {
        devName.clear();
      }
      if( result == 0 && !devName.isEmpty() )
      {
        //
        // Wenn ein weiteres Gerät gefunden wurde
        //
        lg->info(QString("WinDiscoverThread::run -> found device, name <")
                 .append(devName)
                 .append("> found. quering for services"));
        //
        // Initialisiere die Suche nach Services auf dem Gerät
        //
        ZeroMemory( pWSAquerySetDevice, ulPQSSize );
        pWSAquerySetDevice->dwSize = sizeof(WSAQUERYSET);
        pWSAquerySetDevice->dwNameSpace = NS_BTH;
        pWSAquerySetDevice->dwNumberOfCsAddrs = 0;
        // BT Adresse bestimmen
        CSADDR_INFO * addr = (CSADDR_INFO *)pWSAquerySetResult->lpcsaBuffer;
        wchar_t addressAsString[1000];
        wchar_t *aas = addressAsString;
        DWORD addressSize = sizeof(addressAsString);
        WSAAddressToStringW( addr->RemoteAddr.lpSockaddr, addr->RemoteAddr.iSockaddrLength, NULL, aas, &addressSize );
        // BT Adresse als String eintragen
        pWSAquerySetDevice->lpszContext = aas;
        // Protokoll in die Anfrage eintragen
        GUID protocol = L2CAP_PROTOCOL_UUID;
        pWSAquerySetDevice->lpServiceClassId = &protocol;
        //
        // ein Gerät gefunden, zunächst mal als Objekt erzeugen
        //
        currDevice = new BluetoothDeviceDescriber( QString::fromWCharArray( aas ) );
        currDevice->setDevName( devName );
        //
        // Nun zugehörige Services Finden, in Liste eintragen, wenn Service gefunden
        //
        msleep( 100 );
        if( !searchServices( currDevice ) )
        {
          ::closesocket( sock );
          freeHeap();
          WSACleanup();
          return;
        }
        currDevice = nullptr;
      }
      // laut MSDN Wartezeit einbauen
      lg->debug( "WinDiscoverThread::run -> sleep a while" );
      yieldCurrentThread();
      msleep( 2500 );
      yieldCurrentThread();
      msleep( 20 );
    }
    lg->debug( "WinDiscoverThread::run -> all devices found, send SIGNAL" );
    ::closesocket( sock );
    freeHeap();
    WSACleanup();
    yieldCurrentThread();
    msleep(20);
    yieldCurrentThread();
    //
    // Sende ein Signal (Callback) an einen verbundenen Slot (wenn verbunden...)
    //
    lg->debug( "WinDiscoverThread::run -> send signal finish" );
    emit btDiscoverEndSig();
    lg->debug( "WinDiscoverThread::run -> finish" );
  }
}
