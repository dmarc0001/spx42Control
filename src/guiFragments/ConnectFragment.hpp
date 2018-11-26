#ifndef CONNECTFORM_HPP
#define CONNECTFORM_HPP

#include <QDateTime>
#include <QPair>
#include <QTimer>
#include <QWidget>
#include <memory>
#include "IFragmentInterface.hpp"
#include "bluetooth/BtDiscoverRemoteDevice.hpp"
#include "bluetooth/BtTypes.hpp"
#include "bluetooth/SPX42RemotBtDevice.hpp"
#include "database/SPX42Database.hpp"
#include "logging/Logger.hpp"
#include "spx42/SPX42CommandDef.hpp"
#include "spx42/SPX42Config.hpp"

namespace Ui
{
  class connectForm;
}

namespace spx
{
  class ConnectFragment : public QWidget, IFragmentInterface
  {
    private:
    Q_OBJECT
    Q_INTERFACES( spx::IFragmentInterface )
    static SPXDeviceList spx42Devices;                      //! Liste mit gefundenen SPX42
    std::unique_ptr< Ui::connectForm > ui;                  //! Zeiger auf die GUI Objekte
    DeviceAliasHash devices;                                //! gehashte Liste der verfügbaren Geräte
    QString errMsg;                                         //! Fehlermeldungen BT Connection
    std::unique_ptr< BtDiscoverRemoteDevice > discoverObj;  //! Objekt zum Discovern der Geräte
    QString fragmentTitlePattern;                           //! das Muster (lokalisierungsfähig) für Fragmentüberschrift

    public:
    explicit ConnectFragment( QWidget *parent,
                              std::shared_ptr< Logger > logger,
                              std::shared_ptr< SPX42Database > spx42Database,
                              std::shared_ptr< SPX42Config > spxCfg,
                              std::shared_ptr< SPX42RemotBtDevice > remSPX42 );  //! Konstruktor
    ~ConnectFragment() override;                                                 //! Destruktor, muss GUI säubern
    virtual void deactivateTab( void ) override;                                 //! deaktiviere eventuelle signale

    private:
    void setGuiConnected( bool isConnected );                      //! Stati in der GUI setzten
    void addDeviceComboEntry( const SPXDeviceDescr &deviceInfo );  //! trage einen eintrag in die Liste ein

    signals:
    void onWarningMessageSig( const QString &msg, bool asPopup = false ) override;  //! eine Warnmeldung soll das Main darstellen
    void onErrorgMessageSig( const QString &msg, bool asPopup = false ) override;   //! eine Warnmeldung soll das Main darstellen
    void onAkkuValueChangedSig( double aValue ) override;                          //! signalisiert, dass der Akku eine Spanniung hat

    public slots:
    virtual void onOnlineStatusChangedSlot( bool isOnline ) override;                //! Wenn sich der Onlinestatus des SPX42 ändert
    virtual void onSocketErrorSlot( QBluetoothSocket::SocketError error ) override;  //! wenn bei einer Verbindung ein Fehler auftritt
    virtual void onConfLicChangedSlot( void ) override;                              //! Wenn sich die Lizenz ändert
    virtual void onCloseDatabaseSlot( void ) override;                               //! wenn die Datenbank geschlosen wird

    private slots:
    virtual void onCommandRecivedSlot( void ) override;               //! wenn ein Datentelegramm empfangen wurde
    void onConnectButtonSlot( void );                                 //! Wenn der Verbinde-Knopf gedrückt wurde
    void onPropertyButtonSlot( void );                                //! Verbindungs/Geräte eigenschaften
    void onDiscoverButtonSlot( void );                                //! Suche nach BT Geräten
    void onCurrentIndexChangedSlot( int index );                      //! Dropdown box: Auswahl geändert
    void onDiscoveredDeviceSlot( const SPXDeviceDescr &deviceInfo );  //! wurde ein neues gerät gefunden...
    void onDiscoverScanFinishedSlot( void );                          //! wenn das discovering abgeschlossen ist
  };
}
#endif  // CONNECTFORM_HPP
