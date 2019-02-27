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
    //! Zeiger auf Loggerobjekt
    std::shared_ptr< Logger > lg;
    //! Timer zum versenden von Kommandos
    QTimer sendTimer;
    //! Liste mit zu sendenden Telegrammen
    QList< SendListEntry > sendList;
    //! Liste mit empfangenen Telegrammen
    QQueue< QByteArray > recQueue;
    //! Decodierte Liste mit empfangenen Kommandos
    QQueue< spSingleCommand > rCmdQueue;
    //! Zeiger auf einen Socket
    QBluetoothSocket *socket;
    //! die Bluethooth UUID für RFCOMM
    const QBluetoothUuid btUuiid;
    //! die MAC des gegenübers
    QBluetoothAddress remoteAddr;
    //! empfangspuffer für Telegramme
    QByteArray recBuffer;
    //! gab es einen Socketfehler?
    bool wasSocketError;
    //! während des Sendens ignorieren
    bool ignoreSendTimer;
    //! sind wir im normalen Betriebsmode
    bool isNormalCommandMode;
    //! aktuell übertragen
    int currentDiveNumberForLogDetail;
    //! Sequenznummer der Detailübertragung
    int currentDetailSequenceNumber;
    //! Suchmuster für Line-End
    QByteArray lineEnd;

    public:
    explicit SPX42RemotBtDevice( std::shared_ptr< Logger > logger, QObject *parent = nullptr );
    ~SPX42RemotBtDevice();
    //! starte eine BT Verbindung
    void startConnection( const QString &mac );
    //! trenne die BT Verbindung
    void endConnection( void );
    //! sende ein Datagramm zum SPX42
    void sendCommand( const SendListEntry &entry );
    //! verbindungsstatus erfragen
    SPX42ConnectStatus getConnectionStatus( void );
    //! nächtes Kommand holen, shared ptr zurück
    spSingleCommand getNextRecCommand( void );
    //! mit wem bin ich verbunden
    QString getRemoteConnected( void );
    //! ist normale Mode (NICHT log)
    bool getIsNormalCommandMode() const;

    signals:
    //! Signal, wenn Onlinestatus sich ändert
    void onStateChangedSig( QBluetoothSocket::SocketState state );
    //! Signal bei Fhlern im BT Socket
    void onSocketErrorSig( QBluetoothSocket::SocketError error );
    //! Signal wenn ein Kommando empfangen wurde
    void onCommandRecivedSig( void );

    public slots:

    private slots:
    void onSocketErrorSlot( QBluetoothSocket::SocketError error );
    void onStateChangedSlot( QBluetoothSocket::SocketState state );
    void onReadSocketSlot( void );
    void onSendSocketTimerSlot( void );
  };
}  // namespace spx
#endif  // SPX42REMOTBTDEVICE_HPP
