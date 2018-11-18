#ifndef IFRAGMENTINTERFACE_HPP
#define IFRAGMENTINTERFACE_HPP

#include <memory>

#include "bluetooth/SPX42RemotBtDevice.hpp"
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
    std::shared_ptr< Logger > lg;                       //! Zeiger auf das Log-Objekt
    std::shared_ptr< SPX42Database > database;          //! Zeiger auf die Datenbank
    std::shared_ptr< SPX42Config > spxConfig;           //! Zeiger auf das SPX42 Config Objekt
    std::shared_ptr< SPX42RemotBtDevice > remoteSPX42;  //! Zeiger auf das VErbindungsobjekt zum SPX42
    std::shared_ptr< SPX42Commands > spxCommands;       //! Objekt zum erzeugen/decodieren der SPX Kommandos
    double ackuVal;                                     //! aktueller Akkustand

    public:
    explicit IFragmentInterface( std::shared_ptr< Logger > logger,
                                 std::shared_ptr< SPX42Database > spx42Database,
                                 std::shared_ptr< SPX42Config > spxCfg,
                                 std::shared_ptr< SPX42RemotBtDevice > remSPX42,
                                 std::shared_ptr< SPX42Commands > spxCmds );  //! Standartkonstruktor
    virtual ~IFragmentInterface( void ) = default;                            //! standartverhalten
    virtual void deactivateTab( void ) = 0;                                   //! deaktiviere alle eventuellen signale etc.

    signals:
    virtual void onWarningMessageSig( const QString &msg, bool asPopup = false ) = 0;  //! eine Warnmeldung soll das Main darstellen
    virtual void onErrorgMessageSig( const QString &msg, bool asPopup = false ) = 0;   //! eine Warnmeldung soll das Main darstellen
    virtual void onAkkuValueChangedSlot( double aValue ) = 0;  //! signalisiert, dass der Akku eine Spanniung hat

    private slots:
    virtual void onDatagramRecivedSlot( void ) = 0;  //! wenn ein Datentelegramm empfangen wurde

    public slots:
    virtual void onOnlineStatusChangedSlot( bool isOnline ) = 0;                //! Wenn sich der Onlinestatus des SPX42 ändert
    virtual void onSocketErrorSlot( QBluetoothSocket::SocketError error ) = 0;  //! wenn bei einer Verbindung ein Fehler auftritt
    virtual void onConfLicChangedSlot( void ) = 0;                              //! Wenn sich die Lizenz ändert
    virtual void onCloseDatabaseSlot( void ) = 0;                               //! Wenn die Datenbank geschlossen wird
  };
}

// INTERFACE deklarieren
Q_DECLARE_INTERFACE( spx::IFragmentInterface, "IFragmentInterface" )

#endif  // IFRAGMENTINTERFACE_HPP
