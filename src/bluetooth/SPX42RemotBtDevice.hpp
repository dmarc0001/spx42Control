#ifndef SPX42REMOTBTDEVICE_HPP
#define SPX42REMOTBTDEVICE_HPP

#include <QBluetoothSocket>
#include <QByteArray>
#include <QList>
#include <QQueue>
#include <QRegExp>
#include <QStringList>
#include <QThread>
#include <QTime>
#include <QTimer>
#include <memory>
#include "bluetooth/SPX42BtDevicesManager.hpp"
#include "config/ProjectConst.hpp"
#include "logging/Logger.hpp"
#include "spx42/SPX42CommandDef.hpp"
#include "spx42/SPX42Commands.hpp"
#include "spx42/SPX42SingleCommand.hpp"

namespace spx
{
  constexpr int SEND_TIMERVAL = 200;

  class SPX42RemotBtDevice : public QObject, public SPX42Commands
  {
    Q_OBJECT
    public:
    //! Auflistung der SPX Verbindungsstati
    enum SPX42ConnectStatus : qint8
    {
      SPX42_DISCONNECTED,
      SPX42_CONNECTING,
      SPX42_CONNECTED,
      SPX42_DISCONNECTING,
      SPX42_ERROR
    };

    private:
    const QByteArray tst_space{" "};      //! Array für Leerzeichen, zum ersetzten derselben
    const QByteArray repl_none;           //! zum ersetzern mit NIX
    const QByteArray tst_tab{"\t"};       //! zum finden von TABS
    const QByteArray repl_tab{"|"};       //! zum ersetzten mit "|"
    const QByteArray tst_cr{"\r"};        //! zum finden von \r
    std::shared_ptr< Logger > lg;         //! Zeiger auf Loggerobjekt
    QTimer sendTimer;                     //! Timer zum versenden von Kommandos
    QList< SendListEntry > sendList;      //! Liste mit zu sendenden Telegrammen
    QQueue< QByteArray > recQueue;        //! Liste mit empfangenen Telegrammen
    QQueue< spSingleCommand > rCmdQueue;  //! Decodierte Liste mit empfangenen Kommandos
    QBluetoothSocket *socket;             //! Zeiger auf einen Socket
    const QBluetoothUuid btUuiid;         //! die Bluethooth UUID für RFCOMM
    QBluetoothAddress remoteAddr;         //! die MAC des gegenübers
    QByteArray recBuffer;                 //! empfangspuffer für Telegramme
    bool wasSocketError;                  //! gab es einen Socketfehler?
    bool ignoreSendTimer;                 //! während des Sendens ignorieren
    bool isNormalCommandMode;             //! sind wir im normalen Betriebsmode
    int currentDiveNumberForLogDetail;    //! aktuell übertragen
    int currentDetailSequenceNumber;      //! Sequenznummer der Detailübertragung
    QByteArray lineEnd;                   //! Suchmuster für Line-End

    public:
    explicit SPX42RemotBtDevice( std::shared_ptr< Logger > logger, QObject *parent = nullptr );
    ~SPX42RemotBtDevice();
    void startConnection( const QString &mac );      //! starte eine BT Verbindung
    void endConnection( void );                      //! trenne die BT Verbindung
    void sendCommand( const SendListEntry &entry );  //! sende ein Datagramm zum SPX42
    SPX42ConnectStatus getConnectionStatus( void );  //! verbindungsstatus erfragen
    spSingleCommand getNextRecCommand( void );       //! nächtes Kommand holen, shared ptr zurück
    QString getRemoteConnected( void );              //! mit wem bin ich verbunden
    bool getIsNormalCommandMode() const;             //! ist normale Mode (NICHT log)

    private:
    void computeLogDetailDataset( int idxDetailEnd );
    void computeNormalDataSet( int idxOfETX );

    signals:
    void onStateChangedSig( QBluetoothSocket::SocketState state );  //! Signal, wenn Onlinestatus sich ändert
    void onSocketErrorSig( QBluetoothSocket::SocketError error );   //! Signal bei Fhlern im BT Socket
    void onCommandRecivedSig( void );                               //! Signal wenn ein Kommando empfangen wurde

    public slots:

    private slots:
    void onSocketErrorSlot( QBluetoothSocket::SocketError error );   //! wenn ein socket fehler erkannt wurde
    void onStateChangedSlot( QBluetoothSocket::SocketState state );  //! wenn der socket den status ändert
    void onReadSocketSlot( void );                                   //! wenn daten angekommen sind
    void onSendSocketTimerSlot( void );                              //! Timer zum versenden von Nachrichten
  };
}  // namespace spx
#endif  // SPX42REMOTBTDEVICE_HPP
