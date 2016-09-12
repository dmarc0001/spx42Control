#include "gasFragment.hpp"
#include "ui_gasFragment.h"

namespace spx42
{
  GasFragmentGuiRef::GasFragmentGuiRef(QSpinBox *o2SpinBox, QSpinBox *heSpinBox, QLineEdit *n2LineEdit, QLabel *gasNameLabel) :
    o2Spin( o2SpinBox ),
    heSpin( heSpinBox ),
    n2Line( n2LineEdit ),
    gasName( gasNameLabel ),
    currO2(21),
    currHe(0)
  {
    if( o2Spin != Q_NULLPTR )
    {
      if( o2Spin->value() == 0 )
      {
        o2Spin->setValue(21);
      }
      currO2 = o2Spin->value();
    }
    if( heSpin != Q_NULLPTR )
    {
      currHe = heSpin->value();
    }
    if( n2LineEdit != Q_NULLPTR )
    {
      n2LineEdit->setText( QString("%1").arg( (100 - currO2 - currHe), 2, 10, QChar('0')  ));
    }
  }

  GasFragment::GasFragment(QWidget *parent, Logger *logger) :
    QWidget(parent),
    ui(new Ui::GasForm),
    lg(logger),
    licType(LicenseType::LIC_FULLTMX) //TODO: auf NITROX setzen
  {
    lg->debug( "GasForm::GasForm...");
    ui->setupUi(this);
    fillReferences();
    connectSlots();
  }

  GasFragment::~GasFragment()
  {
    lg->debug( "GasForm::~GasForm...");
    delete ui;
  }


  void GasFragment::fillReferences( void )
  {
    //
    // Referenzen auf die GUI-Objekte in Arrays ablegen,
    // damit ich indiziert zugreifen kann
    //
    gRef[0] = new GasFragmentGuiRef( ui->gas01O2spinBox, ui->gas01HESpinBox, ui->gas01N2LineEdit, ui->gas01NameLabel );
    gRef[1] = new GasFragmentGuiRef( ui->gas02O2spinBox, ui->gas02HESpinBox, ui->gas02N2LineEdit, ui->gas02NameLabel );
    gRef[2] = new GasFragmentGuiRef( ui->gas03O2spinBox, ui->gas03HESpinBox, ui->gas03N2LineEdit, ui->gas03NameLabel );
    gRef[3] = new GasFragmentGuiRef( ui->gas04O2spinBox, ui->gas04HESpinBox, ui->gas04N2LineEdit, ui->gas04NameLabel );
    gRef[4] = new GasFragmentGuiRef( ui->gas05O2spinBox, ui->gas05HESpinBox, ui->gas05N2LineEdit, ui->gas05NameLabel );
    gRef[5] = new GasFragmentGuiRef( ui->gas06O2spinBox, ui->gas06HESpinBox, ui->gas06N2LineEdit, ui->gas06NameLabel );
    gRef[6] = new GasFragmentGuiRef( ui->gas07O2spinBox, ui->gas07HESpinBox, ui->gas07N2LineEdit, ui->gas07NameLabel );
    gRef[7] = new GasFragmentGuiRef( ui->gas08O2spinBox, ui->gas08HESpinBox, ui->gas08N2LineEdit, ui->gas08NameLabel );
  }

  void GasFragment::connectSlots( void )
  {
    //
    // Alle Spinboxen mit Lambdafunktionen zum eintragen der Werte
    // und dann ausführen der change-Funktion
    //
    connect( gRef[0]->heSpin, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), this, [=] (int i){ gRef[0]->currHe = i; spinGasValueChanged( 0 ); } );
    connect( gRef[1]->heSpin, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), this, [=] (int i){ gRef[1]->currHe = i; spinGasValueChanged( 1 ); } );
    connect( gRef[2]->heSpin, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), this, [=] (int i){ gRef[2]->currHe = i; spinGasValueChanged( 2 ); } );
    connect( gRef[3]->heSpin, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), this, [=] (int i){ gRef[3]->currHe = i; spinGasValueChanged( 3 ); } );
    connect( gRef[4]->heSpin, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), this, [=] (int i){ gRef[4]->currHe = i; spinGasValueChanged( 4 ); } );
    connect( gRef[5]->heSpin, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), this, [=] (int i){ gRef[5]->currHe = i; spinGasValueChanged( 5 ); } );
    connect( gRef[6]->heSpin, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), this, [=] (int i){ gRef[6]->currHe = i; spinGasValueChanged( 6 ); } );
    connect( gRef[7]->heSpin, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), this, [=] (int i){ gRef[7]->currHe = i; spinGasValueChanged( 7 ); } );
    //
    connect( gRef[0]->o2Spin, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), this, [=] (int i){ gRef[0]->currO2 = i; spinGasValueChanged( 0 ); } );
    connect( gRef[1]->o2Spin, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), this, [=] (int i){ gRef[1]->currO2 = i; spinGasValueChanged( 1 ); } );
    connect( gRef[2]->o2Spin, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), this, [=] (int i){ gRef[2]->currO2 = i; spinGasValueChanged( 2 ); } );
    connect( gRef[3]->o2Spin, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), this, [=] (int i){ gRef[3]->currO2 = i; spinGasValueChanged( 3 ); } );
    connect( gRef[4]->o2Spin, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), this, [=] (int i){ gRef[4]->currO2 = i; spinGasValueChanged( 4 ); } );
    connect( gRef[5]->o2Spin, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), this, [=] (int i){ gRef[5]->currO2 = i; spinGasValueChanged( 5 ); } );
    connect( gRef[6]->o2Spin, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), this, [=] (int i){ gRef[6]->currO2 = i; spinGasValueChanged( 6 ); } );
    connect( gRef[7]->o2Spin, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), this, [=] (int i){ gRef[7]->currO2 = i; spinGasValueChanged( 7 ); } );
  }

  void GasFragment::spinGasValueChanged( int index )
  {
    volatile static int whereIgnored = -1;
    if( whereIgnored == index )
    {
      //
      // hier soll ich das ignorieren!
      //
      return;
    }
    lg->debug( QString("GasFragment::spinGasValueChanged -> gas nr <%1> was changed...").arg(index, 2, 10, QChar('0')));

    GasFragmentGuiRef *currRef = gRef[index];
    int n2;

    switch( static_cast<qint8>(licType) )
    {
      case static_cast<qint8>(LicenseType::LIC_NITROX ):
        if( currRef->currHe != 0 )
        {
          whereIgnored = index; // igfnorieren weitere Aufrufe für diesen index, GUI verändern
          currRef->currHe = 0;
          currRef->heSpin->setValue(0);
          whereIgnored = -1;
        }
        // weiter bei normoxic
      case static_cast<qint8>(LicenseType::LIC_NORMOXIX ):
        if( currRef->currO2 > 21 )
        {
          whereIgnored = index; // igfnorieren weitere Aufrufe für diesen index, GUI verändern
          currRef->currO2 = 21;
          currRef->o2Spin->setValue(currRef->currO2);
          whereIgnored = -1;
        }
        // weiter bei höheren Lizenzen
      case static_cast<qint8>(LicenseType::LIC_FULLTMX ):
      case static_cast<qint8>(LicenseType::LIC_MIL ):
        // Priorität hat O2, helium runter, wenn es nicht passt
        n2 = (100 - currRef->currO2 - currRef->currHe);
        if( n2 < 0 )
        {
          whereIgnored = index; // igfnorieren weitere Aufrufe für diesen index, GUI verändern
          currRef->currHe = 100 - currRef->currO2;
          currRef->heSpin->setValue( currRef->currHe );
          whereIgnored = -1;
          n2 = (100 - currRef->currO2 - currRef->currHe);
        }
    }
    currRef->n2Line->setText( QString("%1").arg( (100 - currRef->currO2 - currRef->currHe), 2, 10, QChar('0')  ));
    // TODO: Namen für das Gas anzeigen
    // TODO: Gas noch färben, je nach O2-Level
  }

}
