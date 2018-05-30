#include "BTDisconnectThread.hpp"

namespace spx
{

  BTDisconnectThread::BTDisconnectThread( QObject *parent, Logger *log, SOCKET bts) :
    QThread( parent ),
    lg( log ),
    btSocket( bts )
  {

  }

  BTDisconnectThread::~BTDisconnectThread(void)
  {

  }

  void BTDisconnectThread::run()
  {
    int result;

    lg->debug( "WinDisconnectThread::run..." );
    //
    // Close the socket
    //
    lg->debug( "WinDisconnectThread::run -> socket shutdown..." );
    result = ::shutdown( btSocket, SD_SEND );
    if( result == SOCKET_ERROR )
    {
      lg->crit("WinDisconnectThread::run -> An error occured while initialising winsock, closing....");
      lg->crit(QString("WinDisconnectThread::run -> errorcode <%1>").arg( WSAGetLastError(), 0, 8, QChar('0') ));
      ::closesocket( btSocket );
      emit btConnectErrorSig(result);
    }
    else
    {
      //
      lg->debug("WinDisconnectThread::run -> socket is shutdown, close socket...");
      //
      result = ::closesocket( btSocket ) ;
      if( result == SOCKET_ERROR )
      {
        emit btConnectErrorSig(result);
      }
      btSocket = INVALID_SOCKET;
      emit btDisconnectSig();
    }
    lg->debug("WinDisconnectThread::run -> end thread...");
    deleteLater();
  }
}
