#ifndef MAINDAILOG_HPP
#define MAINDAILOG_HPP

#include <QBluetoothDeviceDiscoveryAgent>
#include <QBluetoothLocalDevice>
#include <QDialog>
#include <QMenu>
#include <QTimer>
#include <memory>
#include "bluetooth/SPX42BtDevices.hpp"
#include "logging/Logger.hpp"
#include "ui_BtDiscoverDialog.h"

namespace Ui
{
  class BtDiscoverDialog;
}

namespace spx
{
  class BtDiscoveringDialog : public QDialog
  {
    Q_OBJECT

    private:
    std::shared_ptr< Logger > lg;
    std::unique_ptr< SPX42BtDevices > btDevices;
    std::unique_ptr< Ui::BtDiscoverDialog > ui;
    std::unique_ptr< QMovie > movie;
    QTimer msgTimer;
    qint16 timerCountdowwn;
    static const qint16 timerStartValue = 6;  //! Timer Zyklen bis zum löschen der Meldung
    static const int timerInterval = 1000;    //! Timer interval in ms

    public:
    explicit BtDiscoveringDialog( QWidget *parent = nullptr );
    ~BtDiscoveringDialog();

    protected:
    void changeEvent( QEvent *e );

    private:
    void setMessage( const QString &msg );

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
