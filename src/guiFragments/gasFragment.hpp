#ifndef GASFORM_HPP
#define GASFORM_HPP

#include <QWidget>

#include "../logging/Logger.hpp"

namespace Ui {
  class GasForm;
}

namespace spx42
{
  class GasFragment : public QWidget
  {
    private:
      Q_OBJECT
      Ui::GasForm *ui;                                          //! Zeiger auf GUI-Objekte
      Logger *lg;                                               //! Zeiger auf Loggerobjekt

    public:
      explicit GasFragment(QWidget *parent, Logger *logger);    //! Konstruktor
      ~GasFragment();                                           //! der Zerstörer

  };
}
#endif // GASFORM_HPP
