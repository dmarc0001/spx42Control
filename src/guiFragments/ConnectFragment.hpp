#ifndef CONNECTFORM_HPP
#define CONNECTFORM_HPP

#include <QDateTime>
#include <QMovie>
#include <QPair>
#include <QPixmap>
#include <QTableWidgetItem>
#include <QTimer>
#include <QWidget>
#include <QWindow>
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
    //! Liste mit gefundenen (discover) SPX42, statisch um nicht dauernd nachzufragen
    static SPXDeviceList spx42Devices;
    //! Zeiger auf die GUI Objekte
    std::unique_ptr< Ui::connectForm > ui;
    //! gehashte Liste der verfügbaren Geräte aus DB
    DeviceAliasHash spxDevicesAliasHash;
    //! Fehlermeldungen BT Connection
    QString errMsg;
    //! Objekt zum Discovern der Geräte
    std::unique_ptr< BtDiscoverRemoteDevice > discoverObj;
    //! ansicht SPX42 ohne Status (beim Start...)
    const QPixmap normalSpx;
    //! spx42 beschäftigt
    QMovie *busySpx;
    //! Bild verbundener SPX
    const QPixmap connectedSpx;
    //! Bild nicht verbundener SPX
    const QPixmap disConnectedSpx;
    //! Fehlerbild
    const QPixmap errorSpx;
    //! das Muster (lokalisierungsfähig) für Fragmentüberschrift
    QString fragmentTitlePattern;

    public:
    //! Konstruktor
    explicit ConnectFragment( QWidget *parent,
                              std::shared_ptr< Logger > logger,
                              std::shared_ptr< SPX42Database > spx42Database,
                              std::shared_ptr< SPX42Config > spxCfg,
                              std::shared_ptr< SPX42RemotBtDevice > remSPX42 );
    //! Destruktor, muss GUI säubern
    ~ConnectFragment() override;

    private:
    //! Stati in der GUI setzten
    void setGuiConnected( bool isConnected );
    //! trage einen eintrag in die Liste ein
    void addDeviceComboEntry( const SPXDeviceDescr &deviceInfo );
    //! fülle die Dropdownliste
    void fillDeviceCombo( void );
    //! versuche einen eintrag zu selektieren
    void trySetIndex( void );

    signals:
    //! eine Warnmeldung soll das Main darstellen
    void onWarningMessageSig( const QString &msg, bool asPopup = false ) override;
    //! eine Warnmeldung soll das Main darstellen
    void onErrorgMessageSig( const QString &msg, bool asPopup = false ) override;
    //! signalisiert, dass der Akku eine Spanniung hat
    void onAkkuValueChangedSig( double aValue ) override;

    public slots:
    //! Wenn sich der Onlinestatus des SPX42 ändert
    virtual void onOnlineStatusChangedSlot( bool isOnline ) override;
    //! wenn bei einer Verbindung ein Fehler auftritt
    virtual void onSocketErrorSlot( QBluetoothSocket::SocketError error ) override;
    //! Wenn sich die Lizenz ändert
    virtual void onConfLicChangedSlot( void ) override;
    //! wenn die Datenbank geschlosen wird
    virtual void onCloseDatabaseSlot( void ) override;

    private slots:
    //! wenn ein Datentelegramm empfangen wurde
    virtual void onCommandRecivedSlot( void ) override;
    //! Wenn der Verbinde-Knopf gedrückt wurde
    void onConnectButtonSlot( void );
    //! Verbindungs/Geräte eigenschaften
    void onPropertyButtonSlot( void );
    //! Suche nach BT Geräten
    void onDiscoverButtonSlot( void );
    //! Dropdown box: Auswahl geändert
    void onCurrentIndexChangedSlot( int index );
    //! wurde ein neues gerät gefunden...
    void onDiscoveredDeviceSlot( const SPXDeviceDescr &deviceInfo );
    //! wenn das discovering abgeschlossen ist
    void onDiscoverScanFinishedSlot( void );
    //! wenn im Editor er Alias geändert wurde
    void onAliasEditItemChanged( QTableWidgetItem *edItem );
  };
}  // namespace spx
#endif  // CONNECTFORM_HPP
