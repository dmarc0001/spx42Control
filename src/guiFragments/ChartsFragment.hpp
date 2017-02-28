#ifndef CHARTSFRAGMENT_HPP
#define CHARTSFRAGMENT_HPP

#include <memory>
#include <QWidget>

#include "../logging/Logger.hpp"
#include "../utils/SPX42Config.hpp"
#include "IFragmentInterface.hpp"

namespace Ui
{
  class  ChartsFragment;
}

namespace spx42
{
  class ChartsFragment : public QWidget, IFragmentInterface
  {
    private:
      Q_OBJECT
      std::unique_ptr<Ui::ChartsFragment> ui;                      //! Zeiger auf die GUI Objekte

    public:
      explicit ChartsFragment(QWidget *parent, std::shared_ptr<Logger> logger , std::shared_ptr<SPX42Config> spxCfg); //! Konstruktor
      ~ChartsFragment();                                       //! Destruktor, muss GUI säubern

    private slots:
      virtual void onlineStatusChangedSlot( bool isOnline ) Q_DECL_OVERRIDE; //! Wenn sich der Onlinestatus des SPX42 ändert
      virtual void confLicChangedSlot( void ) Q_DECL_OVERRIDE;  //! Wenn sich die Lizenz ändert
  };
}
#endif // CHARTSFRAGMENT_HPP
