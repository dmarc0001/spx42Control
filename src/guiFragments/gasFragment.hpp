#ifndef GASFORM_HPP
#define GASFORM_HPP

#include <QWidget>
#include <QList>
#include <QSpinBox>
#include <QRegExp>
#include <QLineEdit>
#include <QLabel>
#include <QSpinBox>

#include "../logging/Logger.hpp"
#include "../utils/SPX42Config.hpp"

namespace Ui {
  class GasForm;
}

namespace spx42
{
  #define QSpinboxIntValueChanged  static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged)

  class GasFragmentGuiRef
  {
    public:
      QSpinBox *o2Spin;
      QSpinBox *heSpin;
      QLineEdit *n2Line;
      QLabel *gasName;
      explicit GasFragmentGuiRef( QSpinBox *o2SpinBox, QSpinBox *heSpinBox, QLineEdit *n2LineEdit, QLabel *gasNameLabel );
      ~GasFragmentGuiRef();
  };

  class GasFragment : public QWidget
  {
    private:
      Q_OBJECT
      Ui::GasForm *ui;                                          //! Zeiger auf GUI-Objekte
      Logger *lg;                                               //! Zeiger auf Loggerobjekt
      SPX42Config *spxConfig;                                   //! Zeiger auf das SPX42 Config Objekt
      GasFragmentGuiRef* gRef[8];                               //! Referenzen für acht GUI-Objekte

    public:
      explicit GasFragment(QWidget *parent, Logger *logger, SPX42Config *spxCfg);    //! Konstruktor
      ~GasFragment();                                           //! der Zerstörer

    private:
      void fillReferences( void );                              //! fülle die indizies mit Referenzen
      void connectSlots( void );                                //! verbinde Slots mit Signalen

    private slots:
      void spinO2ValueChanged(int index, int o2Val );           //! O2 Wert eines Gases hat sich verändert
      void spinHeValueChanged(int index, int heVal );           //! HE Wert eines Gases hat sich verändert
  };

}
#endif // GASFORM_HPP
