#ifndef SPX42REMOTBTDEVICE_HPP
#define SPX42REMOTBTDEVICE_HPP

#include <QBluetoothSocket>
#include <QByteArray>
#include <QQueue>
#include <QRegExp>
#include <QTimer>
#include <memory>
#include "bluetooth/SPX42BtDevices.hpp"
#include "config/ProjectConst.hpp"
#include "logging/Logger.hpp"
#include "spx42/SPX42CommandDef.hpp"

namespace spx
{
  class SPX42RemotBtDevice : public QObject
  {
    Q_OBJECT
    public:
    enum SPX42ConnectStatus : qint8
    {
      SPX42_DISCONNECTED,
      SPX42_CONNECTING,
      SPX42_CONNECTED,
      SPX42_DISCONNECTING,
      SPX42_ERROR
    };

    private:
    std::shared_ptr< Logger > lg;    //! Zeiger auf Loggerobjekt
    QTimer sendTimer;                //! Timer zum versenden von Kommandos
    QQueue< QByteArray > sendQueue;  //! Liste mit zu sendenden Telegrammen
    QQueue< QByteArray > recQueue;   //! Liste mit empfangenen Telegrammen
    QBluetoothSocket *socket;        //! Zeiger auf einen Socket
    const QBluetoothUuid btUuiid;    //! die Bluethooth UUID für RFCOMM
    QBluetoothAddress remoteAddr;    //! die MAC des gegenübers
    QByteArray recBuffer;            //! empfangspuffer für Telegramme
    bool wasSocketError;             //! gab es einen Socketfehler?
    bool ignoreTimer;                //! während des Sendens ignorieren

    public:
    explicit SPX42RemotBtDevice( std::shared_ptr< Logger > logger, QObject *parent = nullptr );
    ~SPX42RemotBtDevice();
    void startConnection( const QString &mac );      //! starte eine BT Verbindung
    void endConnection( void );                      //! trenne die BT Verbindung
    void sendCommand( const QByteArray &telegram );  //! sende ein Datagramm zum SPX42
    SPX42ConnectStatus getConnectionStatus( void );  //! verbindungsstatus erfragen
    bool getNextDatagram( QByteArray &array );       //! hole ein empfangenes Datagramm ab (oder leer)

    signals:
    void onStateChangedSig( QBluetoothSocket::SocketState state );  //! Signal, wenn Onlinestatus sich ändert
    void onSocketErrorSig( QBluetoothSocket::SocketError error );   //! Signal bei Fhlern im BT Socket
    void onDatagramRecivedSig( void );                              // Signal, wenn ein Datentelegramm empfangen wurde

    public slots:

    private slots:
    void onSocketErrorSlot( QBluetoothSocket::SocketError error );
    void onStateChangedSlot( QBluetoothSocket::SocketState state );
    void onReadSocketSlot( void );
    void onSendSocketTimerSlot( void );
  };
}
#endif  // SPX42REMOTBTDEVICE_HPP
