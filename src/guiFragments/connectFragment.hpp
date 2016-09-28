#ifndef CONNECTFORM_HPP
#define CONNECTFORM_HPP

#include <QWidget>

#include "../logging/Logger.hpp"
#include "../utils/SPX42Config.hpp"

namespace Ui
{
  class ConnectForm;
}

namespace spx42
{
  class ConnectFragment : public QWidget
  {
    private:
      Q_OBJECT
      Ui::ConnectForm *ui;                                      //! Zeiger auf die GUI Objekte
      Logger *lg;                                               //! Zeiger auf das Log-Objekt
      SPX42Config *spxConfig;                                   //! Zeiger auf das SPX42 Config Objekt

    public:
      explicit ConnectFragment(QWidget *parent, Logger *logger , SPX42Config *spxCfg); //! Konstruktor
      ~ConnectFragment();                                       //! Destruktor, muss GUI säubern

    private slots:
      void connectButtonSlot(void);                             //! Wenn der Verbinde-Knopf gedrückt wurde
      void propertyButtonSlot( void );                          //! Verbindungs/Geräte eigenschaften
      void discoverButtonSlot( void );                          //! Suche nach BT Geräten
      void currentIndexChangedSlot(int index);                  //! Dropdown box: Auswahl geändert

  };
}
#endif // CONNECTFORM_HPP
