#ifndef CHARTSFRAGMENT_HPP
#define CHARTSFRAGMENT_HPP

#include <QWidget>
#include <memory>
#include "IFragmentInterface.hpp"
#include "bluetooth/SPX42RemotBtDevice.hpp"
#include "database/SPX42Database.hpp"
#include "logging/Logger.hpp"
#include "spx42/SPX42Config.hpp"

namespace Ui
{
  class ChartsFragment;
}

namespace spx
{
  class ChartsFragment : public QWidget, IFragmentInterface
  {
    private:
    Q_OBJECT
    Q_INTERFACES( spx::IFragmentInterface )
    std::unique_ptr< Ui::ChartsFragment > ui;  //! Zeiger auf die GUI Objekte

    public:
    explicit ChartsFragment( QWidget *parent,
                             std::shared_ptr< Logger > logger,
                             std::shared_ptr< SPX42Database > spx42Database,
                             std::shared_ptr< SPX42Config > spxCfg,
                             std::shared_ptr< SPX42RemotBtDevice > remSPX42,
                             std::shared_ptr< SPX42Commands > spxCmds );  //! Konstruktor
    ~ChartsFragment() override;                                           //! Destruktor, muss GUI säubern
    virtual void deactivateTab( void ) override;                          //! deaktiviere eventuelle signale

    signals:
    void onWarningMessageSig( const QString &msg, bool asPopup = false ) override;  //! eine Warnmeldung soll das Main darstellen
    void onErrorgMessageSig( const QString &msg, bool asPopup = false ) override;   //! eine Warnmeldung soll das Main darstellen
    void onAkkuValueChangedSlot( double aValue ) override;                          //! signalisiert, dass der Akku eine Spanniung hat

    private slots:
    virtual void onDatagramRecivedSlot( void ) override;                             //! wenn ein Datentelegramm empfangen wurde
    virtual void onOnlineStatusChangedSlot( bool isOnline ) override;                //! Wenn sich der Onlinestatus des SPX42 ändert
    virtual void onSocketErrorSlot( QBluetoothSocket::SocketError error ) override;  //! wenn bei einer Verbindung ein Fehler auftritt
    virtual void onConfLicChangedSlot( void ) override;                              //! Wenn sich die Lizenz ändert
    virtual void onCloseDatabaseSlot( void ) override;                               //! wenn die Datenbank geschlosen wird
  };
}
#endif  // CHARTSFRAGMENT_HPP
