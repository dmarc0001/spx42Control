#ifndef CONNECTFORM_HPP
#define CONNECTFORM_HPP

#include <memory>
#include <QWidget>

#include "../logging/Logger.hpp"
#include "../utils/SPX42Config.hpp"
#include "IFragmentInterface.hpp"

namespace Ui
{
  class connectForm;
}

namespace spx42
{
  class ConnectFragment : public QWidget, IFragmentInterface
  {
    private:
      Q_OBJECT
      std::unique_ptr<Ui::connectForm> ui;                      //! Zeiger auf die GUI Objekte

    public:
      explicit ConnectFragment(QWidget *parent, std::shared_ptr<Logger> logger , std::shared_ptr<SPX42Config> spxCfg); //! Konstruktor
      ~ConnectFragment();                                       //! Destruktor, muss GUI säubern

    private slots:
      void connectButtonSlot(void);                             //! Wenn der Verbinde-Knopf gedrückt wurde
      void propertyButtonSlot( void );                          //! Verbindungs/Geräte eigenschaften
      void discoverButtonSlot( void );                          //! Suche nach BT Geräten
      void currentIndexChangedSlot(int index);                  //! Dropdown box: Auswahl geändert

  };
}
#endif // CONNECTFORM_HPP
