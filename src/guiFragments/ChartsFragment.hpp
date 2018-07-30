#ifndef CHARTSFRAGMENT_HPP
#define CHARTSFRAGMENT_HPP

#include <QWidget>
#include <memory>
#include "IFragmentInterface.hpp"
#include "database/SPX42Database.hpp"
#include "logging/Logger.hpp"
#include "utils/SPX42Config.hpp"

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
    std::unique_ptr< Ui::ChartsFragment > ui;  //! Zeiger auf die GUI Objekte

    public:
    explicit ChartsFragment( QWidget *parent,
                             std::shared_ptr< Logger > logger,
                             std::shared_ptr< SPX42Database > spx42Database,
                             std::shared_ptr< SPX42Config > spxCfg );  //! Konstruktor
    ~ChartsFragment() override;                                        //! Destruktor, muss GUI säubern

    private slots:
    virtual void onOnlineStatusChangedSlot( bool isOnline ) override;  //! Wenn sich der Onlinestatus des SPX42 ändert
    virtual void onConfLicChangedSlot( void ) override;                //! Wenn sich die Lizenz ändert
    virtual void onCloseDatabaseSlot( void ) override;                 //! wenn die Datenbank geschlosen wird
  };
}
#endif  // CHARTSFRAGMENT_HPP
