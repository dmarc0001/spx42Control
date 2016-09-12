#ifndef CONNECTFORM_HPP
#define CONNECTFORM_HPP

#include <QWidget>

#include "../logging/Logger.hpp"

namespace Ui {
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

    public:
      explicit ConnectFragment(QWidget *parent, Logger *logger ); //! Konstruktor
      ~ConnectFragment();                                       //! Destruktor, muss GUI säubern

    private slots:
      void connectButtonSlot(void);                             //! Wenn der Verbinde-Knopf gedrückt wurde
      void propertyButtonSlot( void );                          //! Verbindungs/Geräte eigenschaften
      void discoverButtonSlot( void );                          //! Suche nach BT Geräten
      void currentIndexChangedSlot(int index);                  //! Dropdown box: Auswahl geändert

  };
}
#endif // CONNECTFORM_HPP
