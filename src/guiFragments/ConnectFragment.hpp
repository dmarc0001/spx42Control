#ifndef CONNECTFORM_HPP
#define CONNECTFORM_HPP

#include <QPair>
#include <QWidget>
#include <memory>
#include "IFragmentInterface.hpp"
#include "bluetooth/BtDiscoverDialog.hpp"
#include "bluetooth/SPX42RemotBtDevice.hpp"
#include "database/SPX42Database.hpp"
#include "logging/Logger.hpp"
#include "utils/SPX42Config.hpp"

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
    std::unique_ptr< Ui::connectForm > ui;  //! Zeiger auf die GUI Objekte
    DeviceAliasHash devices;

    public:
    explicit ConnectFragment( QWidget *parent,
                              std::shared_ptr< Logger > logger,
                              std::shared_ptr< SPX42Database > spx42Database,
                              std::shared_ptr< SPX42Config > spxCfg,
                              std::shared_ptr< SPX42RemotBtDevice > remSPX42 );  //! Konstruktor
    ~ConnectFragment() override;                                                 //! Destruktor, muss GUI säubern
    virtual void deactivateTab( void ) override;                                 //! deaktiviere eventuelle signale

    private:
    void fillDevicesList( void );  //! fülle die Liste mit den Geräten neu

    signals:
    void onWarningMessageSig( const QString &msg, bool asPopup = false ) override;  //! eine Warnmeldung soll das Main darstellen
    void onErrorgMessageSig( const QString &msg, bool asPopup = false ) override;   //! eine Warnmeldung soll das Main darstellen

    public slots:
    virtual void onOnlineStatusChangedSlot( bool isOnline ) override;                //! Wenn sich der Onlinestatus des SPX42 ändert
    virtual void onSocketErrorSlot( QBluetoothSocket::SocketError error ) override;  //! wenn bei einer Verbindung ein Fehler auftritt
    virtual void onConfLicChangedSlot( void ) override;                              //! Wenn sich die Lizenz ändert
    virtual void onCloseDatabaseSlot( void ) override;                               //! wenn die Datenbank geschlosen wird

    private slots:
    void onConnectButtonSlot( void );             //! Wenn der Verbinde-Knopf gedrückt wurde
    void onPropertyButtonSlot( void );            //! Verbindungs/Geräte eigenschaften
    void onDiscoverButtonSlot( void );            //! Suche nach BT Geräten
    void onCurrentIndexChangedSlot( int index );  //! Dropdown box: Auswahl geändert
  };
}
#endif  // CONNECTFORM_HPP
