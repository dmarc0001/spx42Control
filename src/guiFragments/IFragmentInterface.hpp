#ifndef IFRAGMENTINTERFACE_HPP
#define IFRAGMENTINTERFACE_HPP

#include <memory>

#include "config/SPX42Defs.hpp"
#include "database/SPX42Database.hpp"
#include "logging/Logger.hpp"
#include "utils/SPX42Config.hpp"

namespace spx
{
  class IFragmentInterface
  {
    protected:
    std::shared_ptr< Logger > lg;               //! Zeiger auf das Log-Objekt
    std::shared_ptr< SPX42Database > database;  //! Zeiger auf die Datenbank
    std::shared_ptr< SPX42Config > spxConfig;   //! Zeiger auf das SPX42 Config Objekt

    public:
    explicit IFragmentInterface( std::shared_ptr< Logger > logger,
                                 std::shared_ptr< SPX42Database > spx42Database,
                                 std::shared_ptr< SPX42Config > spxCfg );  //! Standartkonstruktor
    virtual ~IFragmentInterface( void );                                   //! Destruktor

    public slots:
    virtual void onOnlineStatusChangedSlot( bool isOnline ) = 0;  //! Wenn sich der Onlinestatus des SPX42 ändert
    virtual void onConfLicChangedSlot( void ) = 0;                //! Wenn sich die Lizenz ändert
    virtual void onCloseDatabaseSlot( void ) = 0;                 //! Wenn die Datenbank geschlossen wird
  };
}

#endif  // IFRAGMENTINTERFACE_HPP
