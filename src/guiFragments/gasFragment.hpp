#ifndef GASFORM_HPP
#define GASFORM_HPP

#include <QWidget>
#include <QList>
#include <QSpinBox>
#include <QRegExp>
#include <QLineEdit>
#include <QLabel>

#include "../logging/Logger.hpp"

namespace Ui {
  class GasForm;
}

namespace spx42
{
  enum class LicenseType : qint8 { LIC_NITROX, LIC_NORMOXIX, LIC_FULLTMX, LIC_MIL };

  class GasFragmentGuiRef
  {
    public:
      QSpinBox *o2Spin;
      QSpinBox *heSpin;
      QLineEdit *n2Line;
      QLabel *gasName;
      int currO2;
      int currHe;
      explicit GasFragmentGuiRef( QSpinBox *o2SpinBox, QSpinBox *heSpinBox, QLineEdit *n2LineEdit, QLabel *gasNameLabel );
      ~GasFragmentGuiRef();
  };

  class GasFragment : public QWidget
  {
    private:
      Q_OBJECT
      Ui::GasForm *ui;                                          //! Zeiger auf GUI-Objekte
      Logger *lg;                                               //! Zeiger auf Loggerobjekt
      LicenseType licType;                                      //! Lizenztyp, beeinflusst Gase
      GasFragmentGuiRef* gRef[8];                               //! Referenzen für acht GUI-Objekte

    public:
      explicit GasFragment(QWidget *parent, Logger *logger);    //! Konstruktor
      ~GasFragment();                                           //! der Zerstörer

    private:
      void fillReferences( void );                              //! fülle die indizies mit Referenzen
      void connectSlots( void );                                //! verbinde Slots mit Signalen

    private slots:
      void spinGasValueChanged( int index );                    //! Wert eines Gases hat sich verändert
  };

}
#endif // GASFORM_HPP
