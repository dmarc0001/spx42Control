#include "gasFragment.hpp"
#include "ui_gasFragment.h"

namespace spx42
{
  GasFragmentGuiRef::GasFragmentGuiRef(QSpinBox *o2SpinBox, QSpinBox *heSpinBox, QLineEdit *n2LineEdit, QLabel *gasNameLabel) :
    o2Spin( o2SpinBox ),
    heSpin( heSpinBox ),
    n2Line( n2LineEdit ),
    gasName( gasNameLabel )
  {
    if( o2Spin != Q_NULLPTR )
    {
      if( o2Spin->value() == 0 )
      {
        o2Spin->setValue(21);
      }
    }
    if( n2LineEdit != Q_NULLPTR )
    {
      n2LineEdit->setText( QString("%1").arg( (100 - o2Spin->value() - heSpin->value() ), 2, 10, QChar('0')  ));
    }
  }

  GasFragment::GasFragment(QWidget *parent, Logger *logger, SPX42Config *spxCfg) :
    QWidget(parent),
    ui(new Ui::GasForm),
    lg(logger),
    spxConfig( spxCfg )
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
    connect( gRef[0]->heSpin, QSpinboxIntValueChanged, this, [=] (int i){ spinHeValueChanged( 0, i ); } );
    connect( gRef[1]->heSpin, QSpinboxIntValueChanged, this, [=] (int i){ spinHeValueChanged( 1, i ); } );
    connect( gRef[2]->heSpin, QSpinboxIntValueChanged, this, [=] (int i){ spinHeValueChanged( 2, i ); } );
    connect( gRef[3]->heSpin, QSpinboxIntValueChanged, this, [=] (int i){ spinHeValueChanged( 3, i ); } );
    connect( gRef[4]->heSpin, QSpinboxIntValueChanged, this, [=] (int i){ spinHeValueChanged( 4, i ); } );
    connect( gRef[5]->heSpin, QSpinboxIntValueChanged, this, [=] (int i){ spinHeValueChanged( 5, i ); } );
    connect( gRef[6]->heSpin, QSpinboxIntValueChanged, this, [=] (int i){ spinHeValueChanged( 6, i ); } );
    connect( gRef[7]->heSpin, QSpinboxIntValueChanged, this, [=] (int i){ spinHeValueChanged( 7, i ); } );
    //
    connect( gRef[0]->o2Spin, QSpinboxIntValueChanged, this, [=] (int i){ spinO2ValueChanged( 0, i ); } );
    connect( gRef[1]->o2Spin, QSpinboxIntValueChanged, this, [=] (int i){ spinO2ValueChanged( 1, i ); } );
    connect( gRef[2]->o2Spin, QSpinboxIntValueChanged, this, [=] (int i){ spinO2ValueChanged( 2, i ); } );
    connect( gRef[3]->o2Spin, QSpinboxIntValueChanged, this, [=] (int i){ spinO2ValueChanged( 3, i ); } );
    connect( gRef[4]->o2Spin, QSpinboxIntValueChanged, this, [=] (int i){ spinO2ValueChanged( 4, i ); } );
    connect( gRef[5]->o2Spin, QSpinboxIntValueChanged, this, [=] (int i){ spinO2ValueChanged( 5, i ); } );
    connect( gRef[6]->o2Spin, QSpinboxIntValueChanged, this, [=] (int i){ spinO2ValueChanged( 6, i ); } );
    connect( gRef[7]->o2Spin, QSpinboxIntValueChanged, this, [=] (int i){ spinO2ValueChanged( 7, i ); } );
  }

  void GasFragment::spinO2ValueChanged( int index, int o2Val )
  {
    volatile static int whereIgnored = -1;
    if( whereIgnored == index )
    {
      //
      // hier soll ich das ignorieren!
      //
      return;
    }
    lg->debug( QString("GasFragment::spinO2ValueChanged -> gas nr <%1> was changed...").arg(index, 2, 10, QChar('0')));
    //
    // Gas setzen, Plausibilität prüfen, ggf korrigieren
    //
    GasFragmentGuiRef *currRef = gRef[index];
    SPX42Gas& currGas = spxConfig->getGasAt(index);
    currGas.setO2(o2Val, spxConfig->getLicType());
    whereIgnored = index; // igfnorieren weitere Aufrufe für diesen index, GUI verändern
    currRef->o2Spin->setValue(currGas.getO2());
    currRef->heSpin->setValue(currGas.getHe());
    whereIgnored = -1;
    currRef->n2Line->setText( QString("%1").arg( currGas.getN2(), 2, 10, QChar('0')  ));
    currRef->gasName->setText(currGas.getGasName());
    // TODO: Gas noch färben, je nach O2-Level
  }

  void GasFragment::spinHeValueChanged( int index, int heVal )
  {
    volatile static int whereIgnored = -1;
    if( whereIgnored == index )
    {
      //
      // hier soll ich das ignorieren!
      //
      return;
    }
    lg->debug( QString("GasFragment::spinHeValueChanged -> gas nr <%1> was changed...").arg(index, 2, 10, QChar('0')));
    //
    // Gas setzen, Plausibilität prüfen, ggf korrigieren
    //
    GasFragmentGuiRef *currRef = gRef[index];
    SPX42Gas& currGas = spxConfig->getGasAt(index);
    currGas.setHe(heVal, spxConfig->getLicType());
    whereIgnored = index; // igfnorieren weitere Aufrufe für diesen index, GUI verändern
    currRef->o2Spin->setValue(currGas.getO2());
    currRef->heSpin->setValue(currGas.getHe());
    whereIgnored = -1;
    currRef->n2Line->setText( QString("%1").arg( currGas.getN2(), 2, 10, QChar('0')  ));
    currRef->gasName->setText(currGas.getGasName());
    // TODO: Gas noch färben, je nach O2-Level
  }

}
