#include "DeviceConfigFragment.hpp"
#include "ui_DeviceConfig.h"

namespace spx42
{
  /**
   * @brief Konstruktor für Konfigurationsfragment
   * @param parent Parentobjekt
   * @param logger Logger
   * @param spxCfg SPX42 Konfigurationsobjekt
   */
  DeviceConfigFragment::DeviceConfigFragment(QWidget *parent, std::shared_ptr<Logger> logger , std::shared_ptr<SPX42Config> spxCfg) :
    QWidget(parent),
    IFragmentInterface(logger, spxCfg),
    ui(new Ui::DeviceConfig),
    areSlotsConnected( false ),
    gradentSlotsIgnore( false )
  {
    lg->debug( "DeviceConfigFragment::DeviceConfigFragment...");
    ui->setupUi(this);

    //fillReferences();
    initGuiWithConfig();
    connectSlots();

  }

  /**
   * @brief Destruktor, aufräumen
   */
  DeviceConfigFragment::~DeviceConfigFragment()
  {
    lg->debug( "DeviceConfigFragment::~DeviceConfigFragment...");
    //delete ui;
  }

  /**
   * @brief Initialisiere das Fragment (GUI Aufbauen)
   */
  void DeviceConfigFragment::initGuiWithConfig( void )
  {
    bool wasSlotsConnected = areSlotsConnected;
    //
    // Initialisiere die GUI
    //
    if( wasSlotsConnected )
    {
      disconnectSlots();
    }
    ui->deviceConfigHeaderLabel->setText( QString(tr("SETTINGS SPX42 SERIAL [%1] LIC: %2")
                                             .arg(spxConfig->getSerialNumber())
                                             .arg(spxConfig->getLicName()))
                                     );
    //
    // Deko gradienten auf sinnvolle Anfangswerte
    //
    DecoGradient newGrad = spxConfig->getPresetValues( DecompressionPreset::DECO_KEY_MODERATE );
    ui->gradientLowSpinBox->setValue( newGrad.first );
    ui->gradientHighSpinBox->setValue( newGrad.second );
    //
    if( wasSlotsConnected )
    {
      connectSlots();
    }

  }

  /**
   * @brief Verbinde Slots mit Signalen
   */
  void DeviceConfigFragment::connectSlots( void )
  {
    //
    // Slots verbinden
    //
    connect( spxConfig.get(), &SPX42Config::licenseChangedSig, this, &DeviceConfigFragment::licChangedSlot );
    connect( ui->conservatismComboBox, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &DeviceConfigFragment::decoComboChangedSlot );
    connect( ui->gradientLowSpinBox, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), this, &DeviceConfigFragment::decoGradientLowChangedSlot );
    connect( ui->gradientHighSpinBox, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), this, &DeviceConfigFragment::decoGradientHighChangedSlot );
  }

  /**
   * @brief trenne Slots von Signalen
   */
  void DeviceConfigFragment::disconnectSlots( void )
  {
    //
    // Alle Slots trennen
    //
    disconnect( ui->gradientHighSpinBox, 0, 0, 0 );
    disconnect( ui->gradientLowSpinBox, 0, 0, 0 );
    disconnect( ui->conservatismComboBox, 0, 0, 0 );
    disconnect( spxConfig.get(), 0, 0, 0 );
  }

  /**
   * @brief changeEvent Signal vom System bearbeiten
   * @param Event
   */
  void DeviceConfigFragment::changeEvent(QEvent *e)
  {
    QWidget::changeEvent(e);
    switch (e->type()) {
      case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
      default:
        break;
    }
  }

  /**
   * @brief Slot beim ändern der Lizenz
   * @param Lizenz ders SPX
   */
  void DeviceConfigFragment::licChangedSlot(SPX42License &lic )
  {
    lg->debug( QString("DeviceConfigFragment::licChangedSlot -> set: %1").arg(static_cast<int>(lic.getLicType())) );
    ui->deviceConfigHeaderLabel->setText( QString(tr("SETTINGS SPX42 SERIAL [%1] LIC: %2")
                                             .arg(spxConfig->getSerialNumber())
                                             .arg(spxConfig->getLicName()))
                                     );
    // TODO: GUI überarbeiten!
  }

  /**
   * @brief Dekompressionseinstellung geändert
   * @param der neue Index
   */
  void DeviceConfigFragment::decoComboChangedSlot( int index )
  {
    if( gradentSlotsIgnore ) return;
    //
    // index korrespondiert mit dem Key des Deko-Presets
    // QHash aus SPX42Config
    //
    DecoGradient newGrad = spxConfig->getPresetValues( static_cast<DecompressionPreset>(index) );
    lg->debug( QString("DeviceConfigFragment::decoComboChangedSlot: combobox new index: %1, new gradients low: %2, high: %3 ")
               .arg(index)
               .arg(newGrad.first)
               .arg(newGrad.second));
    //
    // Slots für changeInput kurz deaktivieren
    // und dann Werte neu eintragen
    //
    gradentSlotsIgnore = true;
    ui->gradientLowSpinBox->setValue( newGrad.first );
    ui->gradientHighSpinBox->setValue( newGrad.second );
    gradentSlotsIgnore = false;
  }

  /**
   * @brief der LOW Gradient wurde manuell geändert
   * @param der neue LOW Wert
   */
  void DeviceConfigFragment::decoGradientLowChangedSlot( int low )
  {
    DecompressionPreset preset;
    qint8 high;
    //
    if( gradentSlotsIgnore ) return;
    high = static_cast<qint8>(ui->gradientHighSpinBox->value());
    lg->debug( QString("DeviceConfigFragment::decoGradientLowChangedSlot: gradients changed to low: %1 high: %2" )
               .arg(static_cast<int>(low))
               .arg(static_cast<int>(high)));
    //
    // Passen die Werte zu einem Preset?
    //
    preset = spxConfig->getPresetForGradient( static_cast<qint8>(low), static_cast<qint8>(high) );
    spxConfig->setCurrentPreset( preset, low, high );
    //
    // hat sich dabei das Preset verändert?
    //
    if( ui->conservatismComboBox->currentIndex() != static_cast<qint8>(preset) )
    {
      setGradientPresetWithoutCallback( preset );
    }
  }

  /**
   * @brief der HIGH Gradient wurde manuell geändert
   * @param der neue HIGH Wert
   */
  void DeviceConfigFragment::decoGradientHighChangedSlot( int high )
  {
    DecompressionPreset preset;
    qint8 low;
    //
    if( gradentSlotsIgnore ) return;
    low = static_cast<qint8>(ui->gradientLowSpinBox->value());
    lg->debug( QString("DeviceConfigFragment::decoGradientHighChangedSlot: gradients changed to low: %1 high: %2" )
               .arg(static_cast<int>(low))
               .arg(static_cast<int>(high)));
    //
    // Passen die Werte zu einem Preset?
    //
    preset = spxConfig->getPresetForGradient( static_cast<qint8>(low), static_cast<qint8>(high) );
    spxConfig->setCurrentPreset( preset, low, high );
    if( ui->conservatismComboBox->currentIndex() != static_cast<qint8>(preset) )
    {
      setGradientPresetWithoutCallback( preset );
    }
  }

  /**
   * @brief Hilfsfunktion zum ändern des Presets in der Dropdown ohne Callback auszulösen
   * @param der NEUE Preset
   */
  void DeviceConfigFragment::setGradientPresetWithoutCallback( DecompressionPreset preset )
  {
    //
    // zuerst Callbacks ignorieren, sondst gibt das eien Endlosschleife
    //
    gradentSlotsIgnore = true;
    ui->conservatismComboBox->setCurrentIndex( static_cast<int>(preset) );
    lg->debug( QString("DeviceConfigFragment::setGradientPresetWithoutCallback: combobox new index: %1")
               .arg(static_cast<int>(preset)));
    //
    // Callbacks wieder erlauben
    //
    gradentSlotsIgnore = false;
  }
}
