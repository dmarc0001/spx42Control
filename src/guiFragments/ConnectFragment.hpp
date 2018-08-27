#ifndef CONNECTFORM_HPP
#define CONNECTFORM_HPP

#include <QPair>
#include <QWidget>
#include <memory>
#include "IFragmentInterface.hpp"
#include "bluetooth/BtDiscoverDialog.hpp"
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
    std::unique_ptr< Ui::connectForm > ui;  //! Zeiger auf die GUI Objekte
    DeviceAliasHash devices;

    public:
    explicit ConnectFragment( QWidget *parent,
                              std::shared_ptr< Logger > logger,
                              std::shared_ptr< SPX42Database > spx42Database,
                              std::shared_ptr< SPX42Config > spxCfg );  //! Konstruktor
    ~ConnectFragment() override;                                        //! Destruktor, muss GUI säubern

    private:
    void fillDevicesList( void );  //! fülle die Liste mit den Geräten neu

    private slots:
    virtual void onOnlineStatusChangedSlot( bool isOnline ) override;  //! Wenn sich der Onlinestatus des SPX42 ändert
    virtual void onConfLicChangedSlot( void ) override;                //! Wenn sich die Lizenz ändert
    virtual void onCloseDatabaseSlot( void ) override;                 //! wenn die Datenbank geschlosen wird
    void connectButtonSlot( void );                                    //! Wenn der Verbinde-Knopf gedrückt wurde
    void propertyButtonSlot( void );                                   //! Verbindungs/Geräte eigenschaften
    void discoverButtonSlot( void );                                   //! Suche nach BT Geräten
    void currentIndexChangedSlot( int index );                         //! Dropdown box: Auswahl geändert
  };
}
#endif  // CONNECTFORM_HPP
