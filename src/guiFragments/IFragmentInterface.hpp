#ifndef IFRAGMENTINTERFACE_HPP
#define IFRAGMENTINTERFACE_HPP

#include <memory>

#include "../logging/Logger.hpp"
#include "../utils/SPX42Config.hpp"
#include "../config/SPX42Defs.hpp"

namespace spx
{
  class IFragmentInterface
  {
    protected:
      std::shared_ptr<Logger> lg;                               //! Zeiger auf das Log-Objekt
      std::shared_ptr<SPX42Config> spxConfig;                   //! Zeiger auf das SPX42 Config Objekt

    public:
      explicit IFragmentInterface(std::shared_ptr<Logger> logger, std::shared_ptr<SPX42Config> spxCfg); //! Standartkonstruktor
      virtual ~IFragmentInterface(void);                        //! Destruktor

    public slots:
      virtual void onlineStatusChangedSlot( bool isOnline ) = 0L;//! Wenn sich der Onlinestatus des SPX42 ändert
      virtual void confLicChangedSlot( void ) = 0L;             //! Wenn sich die Lizenz ändert

  };
}

#endif // IFRAGMENTINTERFACE_HPP
