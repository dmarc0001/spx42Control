#include "BTCommWorker.hpp"

namespace spx
{
  BTCommWorker::BTCommWorker(QObject *parent, Logger *log, SOCKET bts) :
    QThread(parent),
    lg( log ),
    btSocket( bts )
  {

  }

  void BTCommWorker::run(void)
  {
    int selectRet;
    char *bPtr;
    //
    isWorkerRunning = true;
    //
    // los geht es
    //
    lg->debug("WinCommWorker::run -> thread start...");
    while( isWorkerRunning )
    {
      bPtr = recBuff;
      ZeroMemory( bPtr, sizeof( recBuff ) );
      selectRet = ::recv( btSocket, bPtr, sizeof( recBuff ), 0 );
      //
      // Ergebnis auswerten
      //
      if ( selectRet < 1  )
      {
        lg->crit( "WinCommWorker::run -> connection closed");
        emit btDisconnectSig();
        isWorkerRunning = false;
        continue;
      }
      else
      {
        emit btDataRecivedSig( bPtr, selectRet );
        continue;
      }
    }
    lg->debug("WinCommWorker::run -> thread end...");
  }

  int BTCommWorker::write( const char *str, int len )
  {
    mutex.lock();
    int retVal;
    //
    retVal = ::send(btSocket, str, len, 0 );
    if( retVal == SOCKET_ERROR )
    {
      retVal = WSAGetLastError();
      lg->crit( QString("WinCommWorker::write -> socket error <%1>").arg(retVal, 8, 10, QChar('0')));
      emit btConnectErrorSig(retVal);
      isWorkerRunning = false;
    }
    else if( retVal != len )
    {
      lg->crit( QString("WinCommWorker::write -> write not equal written bytes. to write <%1>, written <%2>").arg(len, 4, 10, QChar('0')).arg(retVal, 4, 10, QChar('0')));
      emit btConnectErrorSig(retVal);
    }
    mutex.unlock();
    return(retVal);
  }

  int BTCommWorker::write( const QByteArray& cmd )
  {
    const char *sPtr = cmd.constData();
    int len = cmd.length();
    // jetzt zur eigentlichen Schreiberei
    return( write( sPtr, len ) );
  }

  void BTCommWorker::cancel(void)
  {
    isWorkerRunning = false;
  }
}
