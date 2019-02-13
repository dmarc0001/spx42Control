#ifndef IFRAGMENTINTERFACE_HPP
#define IFRAGMENTINTERFACE_HPP

#include <memory>

#include "bluetooth/SPX42RemotBtDevice.hpp"
#include "config/AppConfigClass.hpp"
#include "database/SPX42Database.hpp"
#include "logging/Logger.hpp"
#include "spx42/SPX42Commands.hpp"
#include "spx42/SPX42Config.hpp"
#include "spx42/SPX42Defs.hpp"

namespace spx
{
  class IFragmentInterface
  {
    protected:
    //! Zeiger auf das Log-Objekt
    std::shared_ptr< Logger > lg;
    //! Zeiger auf die Datenbank
    std::shared_ptr< SPX42Database > database;
    //! Zeiger auf das SPX42 Config Objekt
    std::shared_ptr< SPX42Config > spxConfig;
    //! Zeiger auf das Verbindungsobjekt zum SPX42
    std::shared_ptr< SPX42RemotBtDevice > remoteSPX42;
    //! Applications config
    AppConfigClass *appConfig;
    double ackuVal;  //! aktueller Akkustand

    public:
    //! Standartkonstruktor
    explicit IFragmentInterface( std::shared_ptr< Logger > logger,
                                 std::shared_ptr< SPX42Database > spx42Database,
                                 std::shared_ptr< SPX42Config > spxCfg,
                                 std::shared_ptr< SPX42RemotBtDevice > remSPX42,
                                 AppConfigClass *appconf );
    //! standartverhalten
    virtual ~IFragmentInterface( void ) = default;

    signals:
    //! eine Warnmeldung soll das Main darstellen
    virtual void onWarningMessageSig( const QString &msg, bool asPopup = false ) = 0;
    //! eine Warnmeldung soll das Main darstellen
    virtual void onErrorgMessageSig( const QString &msg, bool asPopup = false ) = 0;
    //! signalisiert, dass der Akku eine Spannung hat
    virtual void onAkkuValueChangedSig( double aValue ) = 0;

    private slots:
    //! wenn ein Kommando empfangen wurde
    virtual void onCommandRecivedSlot( void ) = 0;

    public slots:
    //! Wenn sich der Onlinestatus des SPX42 ändert
    virtual void onOnlineStatusChangedSlot( bool isOnline ) = 0;
    //! wenn bei einer Verbindung ein Fehler auftritt
    virtual void onSocketErrorSlot( QBluetoothSocket::SocketError error ) = 0;
    //! Wenn sich die Lizenz ändert
    virtual void onConfLicChangedSlot( void ) = 0;
    //! Wenn die Datenbank geschlossen wird
    virtual void onCloseDatabaseSlot( void ) = 0;
  };
}  // namespace spx

// INTERFACE deklarieren
Q_DECLARE_INTERFACE( spx::IFragmentInterface, "IFragmentInterface" )

#endif  // IFRAGMENTINTERFACE_HPP
