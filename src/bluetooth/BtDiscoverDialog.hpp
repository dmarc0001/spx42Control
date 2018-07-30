#ifndef MAINDAILOG_HPP
#define MAINDAILOG_HPP

#include <QBluetoothDeviceDiscoveryAgent>
#include <QBluetoothLocalDevice>
#include <QDialog>
#include <QMenu>
#include <QTimer>
#include <memory>
#include "bluetooth/SPX42BtDevices.hpp"
#include "database/SPX42Database.hpp"
#include "logging/Logger.hpp"
#include "ui_BtDiscoverDialog.h"

namespace Ui
{
  class BtDiscoverDialog;
}

namespace spx
{
  // TODO: implementieren, dass die DB übergeben wird
  static const QString testDbName = "spx42Database.db";

  class BtDiscoverDialog : public QDialog
  {
    Q_OBJECT

    private:
    std::shared_ptr< Logger > lg;                 //! Zeiger auf Logger
    std::unique_ptr< SPX42BtDevices > btDevices;  //! Zeiger auf BT Geräte
    std::unique_ptr< Ui::BtDiscoverDialog > ui;   //! GUI Objekt
    std::unique_ptr< QMovie > movie;              //! Animation
    QTimer msgTimer;                              //! Timeout für Meldungen
    qint16 timerCountdowwn;                       //! Countdown
    std::shared_ptr< SPX42Database > database;    //! Datenbank, geöffnet und bereit
    static const qint16 timerStartValue = 6;      //! Timer Zyklen bis zum löschen der Meldung
    static const int timerInterval = 1000;        //! Timer interval in ms

    public:
    explicit BtDiscoverDialog( QWidget *parent = nullptr );
    ~BtDiscoverDialog();
    void debugSetDatabase( const QString strdPath );

    protected:
    void changeEvent( QEvent *e );

    private:
    void setMessage( const QString &msg );
    void debugCloseDatabase( void );

    public slots:
    void slotDiscoveredDevice( const QBluetoothDeviceInfo & );
    void slotGuiPowerClicked( bool clicked );
    void slotGuiDisplayPairingMenu( const QPoint &pos );
    void slotDevicePairingDone( const QBluetoothAddress &, QBluetoothLocalDevice::Pairing );

    private slots:
    void slotGuiStartScan();
    void slotDiscoverScanFinished();
    void slotGuiItemActivated( QListWidgetItem *item );
    void slotDeviceHostModeStateChanged( QBluetoothLocalDevice::HostMode );
    void slotMessageTimer( void );
  };
}
#endif  // MAINDAILOG_HPP
