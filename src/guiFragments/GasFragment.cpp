#include "GasFragment.hpp"
#include "ui_GasFragment.h"

namespace spx
{
  GasFragmentGuiRef::GasFragmentGuiRef( QSpinBox *o2SpinBox,
                                        QSpinBox *heSpinBox,
                                        QLineEdit *n2LineEdit,
                                        QLabel *gasNameLabel,
                                        QCheckBox *dil1,
                                        QCheckBox *dil2,
                                        QCheckBox *ba )
      : o2Spin( o2SpinBox )
      , heSpin( heSpinBox )
      , n2Line( n2LineEdit )
      , gasName( gasNameLabel )
      , dil1CheckBox( dil1 )
      , dil2CheckBox( dil2 )
      , baCheckBox( ba )
  {
    if ( o2Spin != Q_NULLPTR )
    {
      if ( o2Spin->value() == 0 )
      {
        o2Spin->setValue( 21 );
      }
      if ( n2LineEdit != Q_NULLPTR && heSpin != Q_NULLPTR )
      {
        n2LineEdit->setText( QString( "%1" ).arg( ( 100 - o2Spin->value() - heSpin->value() ), 2, 10, QChar( '0' ) ) );
      }
    }
  }

  GasFragment::GasFragment( QWidget *parent,
                            std::shared_ptr< Logger > logger,
                            std::shared_ptr< SPX42Database > spx42Database,
                            std::shared_ptr< SPX42Config > spxCfg )
      : QWidget( parent ), IFragmentInterface( logger, spx42Database, spxCfg ), ui( new Ui::GasForm ), areSlotsConnected( false )
  {
    lg->debug( "GasFragment::GasFragment..." );
    ui->setupUi( this );
    ui->transferProgressBar->setVisible( false );
    fillReferences();
    initGuiWithConfig();
    onConfLicChangedSlot();
    connectSlots();
  }

  GasFragment::~GasFragment()
  {
    lg->debug( "GasFragment::~GasFragment..." );
    // delete ui;
  }

  void GasFragment::fillReferences()
  {
    //
    // Referenzen auf die GUI-Objekte in Arrays ablegen,
    // damit ich indiziert zugreifen kann
    //
    gRef.at( 0 ) = std::unique_ptr< GasFragmentGuiRef >(
        new GasFragmentGuiRef( ui->gas01O2spinBox, ui->gas01HESpinBox, ui->gas01N2LineEdit, ui->gas01NameLabel, ui->gas01dil01CheckBox,
                               ui->gas01dil02CheckBox, ui->gas01bailoutCheckBox ) );
    gRef.at( 1 ) = std::unique_ptr< GasFragmentGuiRef >(
        new GasFragmentGuiRef( ui->gas02O2spinBox, ui->gas02HESpinBox, ui->gas02N2LineEdit, ui->gas02NameLabel, ui->gas02dil01CheckBox,
                               ui->gas02dil02CheckBox, ui->gas02bailoutCheckBox ) );
    gRef.at( 2 ) = std::unique_ptr< GasFragmentGuiRef >(
        new GasFragmentGuiRef( ui->gas03O2spinBox, ui->gas03HESpinBox, ui->gas03N2LineEdit, ui->gas03NameLabel, ui->gas03dil01CheckBox,
                               ui->gas03dil02CheckBox, ui->gas03bailoutCheckBox ) );
    gRef.at( 3 ) = std::unique_ptr< GasFragmentGuiRef >(
        new GasFragmentGuiRef( ui->gas04O2spinBox, ui->gas04HESpinBox, ui->gas04N2LineEdit, ui->gas04NameLabel, ui->gas04dil01CheckBox,
                               ui->gas04dil02CheckBox, ui->gas04bailoutCheckBox ) );
    gRef.at( 4 ) = std::unique_ptr< GasFragmentGuiRef >(
        new GasFragmentGuiRef( ui->gas05O2spinBox, ui->gas05HESpinBox, ui->gas05N2LineEdit, ui->gas05NameLabel, ui->gas05dil01CheckBox,
                               ui->gas05dil02CheckBox, ui->gas05bailoutCheckBox ) );
    gRef.at( 5 ) = std::unique_ptr< GasFragmentGuiRef >(
        new GasFragmentGuiRef( ui->gas06O2spinBox, ui->gas06HESpinBox, ui->gas06N2LineEdit, ui->gas06NameLabel, ui->gas06dil01CheckBox,
                               ui->gas06dil02CheckBox, ui->gas06bailoutCheckBox ) );
    gRef.at( 6 ) = std::unique_ptr< GasFragmentGuiRef >(
        new GasFragmentGuiRef( ui->gas07O2spinBox, ui->gas07HESpinBox, ui->gas07N2LineEdit, ui->gas07NameLabel, ui->gas07dil01CheckBox,
                               ui->gas07dil02CheckBox, ui->gas07bailoutCheckBox ) );
    gRef.at( 7 ) = std::unique_ptr< GasFragmentGuiRef >(
        new GasFragmentGuiRef( ui->gas08O2spinBox, ui->gas08HESpinBox, ui->gas08N2LineEdit, ui->gas08NameLabel, ui->gas08dil01CheckBox,
                               ui->gas08dil02CheckBox, ui->gas08bailoutCheckBox ) );
  }

  void GasFragment::initGuiWithConfig()
  {
    bool wasSlotsConnected = areSlotsConnected;
    //
    // Gase sind im Config Objekt enthalten, also erst mal die GUI nach den Gasen aufbauen
    // Das sollte VOR dem Verbinden der Slots passieren
    // ODER disconnectSlots() => initGuiWithConfig() => connectSlots()
    //
    if ( wasSlotsConnected )
    {
      disconnectSlots();
    }
    ui->tabHeaderLabel->setText(
        QString( tr( "GASLIST SPX42 SERIAL [%1] LIC: %2" ).arg( spxConfig->getSerialNumber() ).arg( spxConfig->getLicName() ) ) );
    for ( quint64 i = 0; i < 8; i++ )
    {
      SPX42Gas &currGas = spxConfig->getGasAt( static_cast< int >( i ) );
      gRef.at( i )->o2Spin->setValue( currGas.getO2() );
      gRef.at( i )->heSpin->setValue( currGas.getHe() );
      gRef.at( i )->n2Line->setText( QString( "%1" ).arg( currGas.getN2(), 2, 10, QChar( '0' ) ) );
      gRef.at( i )->gasName->setText( currGas.getGasName() );
    }
    if ( wasSlotsConnected )
    {
      connectSlots();
    }
  }

  void GasFragment::connectSlots()
  {
    //
    // Alle Spinboxen mit Lambdafunktionen zum eintragen der Werte
    // und dann ausführen der change-Funktion
    // dann de DIL-Checkboxen mit Labdas versehen (DIL1 xor DIL2)
    //
    for ( quint64 i = 0; i < 8; i++ )
    {
      connect( gRef.at( i )->heSpin, QSpinboxIntValueChanged, this,
               [=]( int val ) { onSpinHeValueChangedSlot( static_cast< int >( i ), val ); } );
      connect( gRef.at( i )->o2Spin, QSpinboxIntValueChanged, this,
               [=]( int val ) { onSpinO2ValueChangedSlot( static_cast< int >( i ), val ); } );
      connect( gRef.at( i )->dil1CheckBox, &QCheckBox::stateChanged, this,
               [=]( int state ) { onGasUseTypChangeSlot( static_cast< int >( i ), DiluentType::DIL_01, state ); } );
      connect( gRef.at( i )->dil2CheckBox, &QCheckBox::stateChanged, this,
               [=]( int state ) { onGasUseTypChangeSlot( static_cast< int >( i ), DiluentType::DIL_02, state ); } );
      connect( gRef.at( i )->baCheckBox, &QCheckBox::stateChanged, this,
               [=]( int state ) { onBaCheckChangeSlot( static_cast< int >( i ), state ); } );
    }
    connect( spxConfig.get(), &SPX42Config::licenseChangedSig, this, &GasFragment::onConfLicChangedSlot );
  }

  void GasFragment::disconnectSlots()
  {
    //
    // Alle Spinboxen trennen
    //
    for ( auto &i : gRef )
    {
      disconnect( i->heSpin );
      disconnect( i->o2Spin );
    }
    disconnect( spxConfig.get() );
  }

  void GasFragment::onSpinO2ValueChangedSlot( int index, int o2Val )
  {
    volatile static int whereIgnored = -1;
    quint64 g_idx = static_cast< quint64 >( index );

    if ( whereIgnored == index )
    {
      //
      // hier soll ich das ignorieren!
      //
      return;
    }
    lg->debug(
        QString( "GasFragment::onSpinO2ValueChangedSlot -> gas nr <%1> was changed..." ).arg( index + 1, 2, 10, QChar( '0' ) ) );
    //
    // Gas setzen, Plausibilität prüfen, ggf korrigieren
    //
    SPX42Gas &currGas = spxConfig->getGasAt( index );
    currGas.setO2( static_cast< quint8 >( o2Val ), spxConfig->getLicense().getLicType() );
    whereIgnored = index;  // igfnorieren weitere Aufrufe für diesen index, GUI verändern
    gRef.at( g_idx )->o2Spin->setValue( currGas.getO2() );
    gRef.at( g_idx )->heSpin->setValue( currGas.getHe() );
    whereIgnored = -1;
    gRef.at( g_idx )->n2Line->setText( QString( "%1" ).arg( currGas.getN2(), 2, 10, QChar( '0' ) ) );
    gRef.at( g_idx )->gasName->setText( currGas.getGasName() );
    // TODO: Gas noch färben, je nach O2-Level
  }

  void GasFragment::onSpinHeValueChangedSlot( int index, int heVal )
  {
    volatile static int whereIgnored = -1;
    quint64 g_idx = static_cast< quint64 >( index );

    if ( whereIgnored == index )
    {
      //
      // hier soll ich das ignorieren!
      //
      return;
    }
    lg->debug(
        QString( "GasFragment::onSpinHeValueChangedSlot -> gas nr <%1> was changed..." ).arg( index + 1, 2, 10, QChar( '0' ) ) );
    //
    // Gas setzen, Plausibilität prüfen, ggf korrigieren
    //
    SPX42Gas &currGas = spxConfig->getGasAt( index );
    currGas.setHe( static_cast< quint8 >( heVal ), spxConfig->getLicense().getLicType() );
    whereIgnored = index;  // igfnorieren weitere Aufrufe für diesen index, GUI verändern
    gRef.at( g_idx )->o2Spin->setValue( currGas.getO2() );
    gRef.at( g_idx )->heSpin->setValue( currGas.getHe() );
    whereIgnored = -1;
    gRef.at( g_idx )->n2Line->setText( QString( "%1" ).arg( currGas.getN2(), 2, 10, QChar( '0' ) ) );
    gRef.at( g_idx )->gasName->setText( currGas.getGasName() );
    // TODO: Gas noch färben, je nach O2-Level
  }

  void GasFragment::checkGases()
  {
    for ( quint64 i = 0; i < 8; i++ )
    {
      SPX42Gas &currGas = spxConfig->getGasAt( static_cast< int >( i ) );
      int currHe = currGas.getHe();
      int currO2 = currGas.getO2();
      //
      int current = currGas.setHe( currGas.getHe(), spxConfig->getLicense().getLicType() );
      if ( currHe != current )
      {
        // GUI setzten
        gRef[ i ]->heSpin->setValue( current );
      }

      current = currGas.setO2( currGas.getO2(), spxConfig->getLicense().getLicType() );
      if ( currO2 != current )
      {
        // GUI setzten
        gRef[ i ]->o2Spin->setValue( current );
      }
    }
  }

  void GasFragment::onGasUseTypChangeSlot( int index, DiluentType which, int state )
  {
    const quint64 g_idx = static_cast< quint64 >( index );
    //
    // FIXME: Alle anderen Gase dürfen dann NICHT which sein!
    // Nur ein DIL1, beliegig DIL2 aber nicht DIL1 == DIL2
    //
    volatile static int whereIgnored = -1;
    if ( whereIgnored == index )
    {
      //
      // hier soll ich das ignorieren!
      //
      return;
    }
    lg->debug( QString( "GasFragment::onGasUseTypChangeSlot -> gas nr <%1> , DIL <%2> was changed to <%3>..." )
                   .arg( index + 1, 2, 10, QChar( '0' ) )
                   .arg( static_cast< int >( which ), 2, 10, QChar( '0' ) )
                   .arg( state, 2, 10, QChar( '0' ) ) );
    SPX42Gas &currGas = spxConfig->getGasAt( index );
    //
    // ich nehme den Fall, dass ein DIL gesetzt wird
    // daher setzten und prüfen dass das andere DIL gelöscht wird
    //
    if ( state == Qt::Checked )
    {
      currGas.setDiluentType( which );
      if ( which == DiluentType::DIL_01 )
      {
        for ( quint64 dil1Nr = 0; dil1Nr < 8; dil1Nr++ )
        {
          if ( g_idx != dil1Nr )
          {
            // alle anderen DIL1 löschen!
            whereIgnored = static_cast< int >( dil1Nr );
            lg->debug( QString( "GasFragment::onGasUseTypChangeSlot: uncheck DIL1 Nr <%1>" ).arg( dil1Nr ) );
            gRef[ dil1Nr ]->dil1CheckBox->setCheckState( Qt::Unchecked );
          }
        }
        whereIgnored = index;
        gRef[ g_idx ]->dil2CheckBox->setCheckState( Qt::Unchecked );
        whereIgnored = -1;
      }
      if ( which == DiluentType::DIL_02 )
      {
        whereIgnored = index;
        gRef[ g_idx ]->dil1CheckBox->setCheckState( Qt::Unchecked );
        whereIgnored = -1;
      }
      return;
    }
    //
    // state == 0
    // dann kann (siehe oben) die andere Box nicht markiert sein
    //
    currGas.setDiluentType( DiluentType::DIL_NONE );
    whereIgnored = index;
    gRef[ g_idx ]->dil2CheckBox->setCheckState( Qt::Unchecked );
    gRef[ g_idx ]->dil2CheckBox->setCheckState( Qt::Unchecked );
    whereIgnored = -1;
  }

  void GasFragment::onBaCheckChangeSlot( int index, int state )
  {
    lg->debug( QString( "GasFragment::onBaCheckChangeSlot -> gas nr <%1> was changed..." ).arg( index + 1, 2, 10, QChar( '0' ) ) );
    if ( state == Qt::Checked )
    {
      spxConfig->getGasAt( index ).setBailout( true );
    }
    else
    {
      spxConfig->getGasAt( index ).setBailout( false );
    }
  }

  void GasFragment::onConfLicChangedSlot()
  {
    lg->debug(
        QString( "GasFragment::onConfLicChangedSlot -> set: %1" ).arg( static_cast< int >( spxConfig->getLicense().getLicType() ) ) );
    ui->tabHeaderLabel->setText(
        QString( tr( "GASLIST SPX42 Serial [%1] Lic: %2" ).arg( spxConfig->getSerialNumber() ).arg( spxConfig->getLicName() ) ) );
    checkGases();
    // TODO: GUI überarbeiten!
  }

  void GasFragment::onOnlineStatusChangedSlot( bool )
  {
    // TODO: was machen
  }

  void GasFragment::onCloseDatabaseSlot()
  {
    // TODO: implementieren
  }
}  // namespace spx
