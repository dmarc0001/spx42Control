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
    //Lizenz
    connect( spxConfig.get(), &SPX42Config::licenseChangedSig, this, &DeviceConfigFragment::licChangedSlot );
    // dekompression
    connect( ui->conservatismComboBox, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &DeviceConfigFragment::decoComboChangedSlot );
    connect( ui->gradientLowSpinBox, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), this, &DeviceConfigFragment::decoGradientLowChangedSlot );
    connect( ui->gradientHighSpinBox, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), this, &DeviceConfigFragment::decoGradientHighChangedSlot );
    connect( ui->dynamicGradientsOnCheckBox, QCheckBox::stateChanged, this, &DeviceConfigFragment::decoDynamicGradientStateChangedSlot );
    connect( ui->deepStopOnCheckBox, QCheckBox::stateChanged, this, &DeviceConfigFragment::decoDeepStopsEnableChangedSlot );
    // Displayeinstellungen
    connect( ui->displayBrightnessComboBox, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &DeviceConfigFragment::displayBrightnessChangedSlot);
    connect( ui->displayOrientationComboBox, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &DeviceConfigFragment::displayOrientationChangedSlot);
    // Einheiteneinstellungen
    connect( ui->unitsTemperaturComboBox, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &DeviceConfigFragment::unitsTemperatureChangedSlot);
    connect( ui->unitsDeepComboBox, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &DeviceConfigFragment::unitsLengthChangedSlot);
    connect( ui->unitsWaterTypeComboBox, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &DeviceConfigFragment::unitsWatertypeChangedSlot);
    // Setpointeinstellungen
    connect( ui->setpointAutoOnComboBox, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &DeviceConfigFragment::setpointAutoChangedSlot);
    connect( ui->setpointSetpointComboBox, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &DeviceConfigFragment::setpointValueChangedSlot);
    // Individual Einstellungen
    connect( ui->individualSeonsorsOnCheckBox, QCheckBox::stateChanged, this, &DeviceConfigFragment::individualSensorsOnChangedSlot );
    connect( ui->individualPSCRModeOnCheckBox, QCheckBox::stateChanged, this, &DeviceConfigFragment::individualPscrModeChangedSlot );
    connect( ui->individualSensorsCountComboBox, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &DeviceConfigFragment::individualSensorsCountChangedSlot);
    connect( ui->individualAcousticWarningsOnCheckBox, QCheckBox::stateChanged, this, &DeviceConfigFragment::individualAcousticChangedSlot );
    connect( ui->individualLogIntervalComboBox, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &DeviceConfigFragment::individualLogIntervalChangedSlot);
  }

  /**
   * @brief trenne Slots von Signalen
   */
  void DeviceConfigFragment::disconnectSlots( void )
  {
    //
    // Alle Slots trennen
    //
    // INDIVIDUAL
    disconnect( ui->individualLogIntervalComboBox, 0, 0, 0);
    disconnect( ui->individualAcousticWarningsOnCheckBox, 0, 0, 0 );
    disconnect( ui->individualSensorsCountComboBox, 0, 0, 0 );
    disconnect( ui->individualPSCRModeOnCheckBox, 0, 0, 0 );
    disconnect( ui->individualSeonsorsOnCheckBox, 0, 0, 0 );
    // SETPOINT
    disconnect( ui->setpointSetpointComboBox, 0, 0, 0 );
    disconnect( ui->setpointAutoOnComboBox, 0, 0, 0 );
    // EINHEITEN
    disconnect( ui->unitsWaterTypeComboBox, 0, 0, 0 );
    disconnect( ui->unitsDeepComboBox, 0, 0, 0 );
    disconnect( ui->unitsTemperaturComboBox, 0, 0, 0 );
    // DISPLAY
    disconnect( ui->displayOrientationComboBox, 0, 0, 0);
    disconnect( ui->displayBrightnessComboBox, 0, 0, 0);
    // Dekompression
    disconnect( ui->deepStopOnCheckBox, 0, 0, 0);
    disconnect( ui->dynamicGradientsOnCheckBox, 0, 0, 0 );
    disconnect( ui->gradientHighSpinBox, 0, 0, 0 );
    disconnect( ui->gradientLowSpinBox, 0, 0, 0 );
    disconnect( ui->conservatismComboBox, 0, 0, 0 );
    //Lizenz
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
    DecoGradient newGradient;
    //
    if( gradentSlotsIgnore ) return;
    //
    // index korrespondiert mit dem Key des Deko-Presets
    // QHash aus SPX42Config
    //
    newGradient = spxConfig->getPresetValues( static_cast<DecompressionPreset>(index) );
    lg->debug( QString("DeviceConfigFragment::decoComboChangedSlot: combobox new index: %1, new gradients low: %2, high: %3 ")
               .arg(index)
               .arg(newGradient.first)
               .arg(newGradient.second));
    //
    // Slots für changeInput kurz deaktivieren
    // und dann Werte neu eintragen
    //
    gradentSlotsIgnore = true;
    ui->gradientLowSpinBox->setValue( newGradient.first );
    ui->gradientHighSpinBox->setValue( newGradient.second );
    spxConfig->setCurrentPreset( static_cast<DecompressionPreset>(index), newGradient.first, newGradient.second );
    gradentSlotsIgnore = false;
  }

  /**
   * @brief der LOW Gradient wurde manuell geändert
   * @param der neue LOW Wert
   */
  void DeviceConfigFragment::decoGradientLowChangedSlot( int low )
  {
    DecompressionPreset preset;
    DecoGradient newGradient;
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
    //
    // hat sich dabei das Preset verändert?
    //
    if( ui->conservatismComboBox->currentIndex() != static_cast<qint8>(preset) )
    {
      setGradientPresetWithoutCallback( preset );
    }
    spxConfig->setCurrentPreset( preset, low, high );
  }

  /**
   * @brief der HIGH Gradient wurde manuell geändert
   * @param der neue HIGH Wert
   */
  void DeviceConfigFragment::decoGradientHighChangedSlot( int high )
  {
    DecompressionPreset preset;
    DecoGradient newGradient;
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
    spxConfig->setCurrentPreset( preset, low, high );
  }

  /**
   * @brief Hilfsfunktion zum ändern des Presets in der Dropdown ohne Callback auszulösen
   * @param der NEUE Preset
   */
  void DeviceConfigFragment::setGradientPresetWithoutCallback( DecompressionPreset preset )
  {
    //
    // zuerst Callbacks ignorieren, sondst gibt das eine Endlosschleife
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

  void DeviceConfigFragment::decoDynamicGradientStateChangedSlot( int state )
  {
    DecompressionDynamicGradient newState = DecompressionDynamicGradient::DYNAMIC_GRADIENT_OFF;
    //
    lg->debug( QString("DeviceConfigFragment::decoDynamicGradientStateChangedSlot: changed to %1").arg(state) );
    if( state == static_cast<int>(Qt::CheckState::Checked) )
      newState = DecompressionDynamicGradient::DYNAMIC_GRADIENT_ON;
    spxConfig->setIsDecoDynamicGradients( newState );
  }

  void DeviceConfigFragment::decoDeepStopsEnableChangedSlot( int state )
  {
    DecompressionDeepstops newState = DecompressionDeepstops::DEEPSTOPS_DISABLED;
    //
    lg->debug( QString("DeviceConfigFragment::decoDeepStopsEnableChangedSlot: changed to %1").arg(state) );
    if( state == static_cast<int>(Qt::CheckState::Checked) )
      newState = DecompressionDeepstops::DEEPSTOPS_ENABLED;
    spxConfig->setIsDeepstopsEnabled( newState );
  }

  void DeviceConfigFragment::displayBrightnessChangedSlot( int index )
  {
    //
    if( lg->getThreshold() == LgThreshold::LG_DEBUG )
    {
      //
      // das nur im debugging Modus machen
      //
      int brVal=0;
      switch( index )
      {
        case static_cast<int>(DisplayBrightness::BRIGHT_20):
          brVal = 20;
          break;
        case static_cast<int>(DisplayBrightness::BRIGHT_40):
          brVal = 40;
          break;
        case static_cast<int>(DisplayBrightness::BRIGHT_60):
          brVal = 60;
          break;
        case static_cast<int>(DisplayBrightness::BRIGHT_80):
          brVal = 80;
          break;
        case static_cast<int>(DisplayBrightness::BRIGHT_100):
        default:
          brVal = 100;
      }
      lg->debug( QString( "DeviceConfigFragment::displayBrightnessChangedSlot: set brightness to %1").arg( brVal ));
    }
    //
    // setze im config-Objekt die Helligkeit
    //
    spxConfig->setDisplayBrightness( static_cast<DisplayBrightness>(index) );
  }

  void DeviceConfigFragment::displayOrientationChangedSlot( int index )
  {
    DisplayOrientation orientation = static_cast<DisplayOrientation>(index);
    if( lg->getThreshold() == LgThreshold::LG_DEBUG )
    {
      QString orString;
      //
      if( orientation == DisplayOrientation::LANDSCAPE )
      {
        orString = "landscape";
      }
      else
      {
        orString = "landscape 180°";
      }
      lg->debug( QString("DeviceConfigFragment::displayOrientationChangedSlot: orientation to %1").arg(orString));
    }
    spxConfig->setDisplayOrientation( static_cast<DisplayOrientation>(index));
  }

  void DeviceConfigFragment::unitsTemperatureChangedSlot( int index )
  {
    DeviceTemperaturUnit tUnit = static_cast<DeviceTemperaturUnit>(index);
    if( lg->getThreshold() == LgThreshold::LG_DEBUG )
    {
      QString tmpString;
      //
      if( tUnit == DeviceTemperaturUnit::CELSIUS )
      {
        tmpString = "celsius";
      }
      else
      {
        tmpString = "fahrenheid";
      }
      lg->debug( QString("DeviceConfigFragment::unitsTemperatureChangedSlot: temperatur unit to %1").arg(tmpString));
    }
    spxConfig->setUnitsTemperatur( tUnit );
  }

  void DeviceConfigFragment::unitsLengthChangedSlot( int index )
  {
    DeviceLenghtUnit lUnit = static_cast<DeviceLenghtUnit>(index);
    if( lg->getThreshold() == LgThreshold::LG_DEBUG )
    {
      QString lenString;
      //
      if( lUnit == DeviceLenghtUnit::METRIC )
      {
        lenString = "metric";
      }
      else
      {
        lenString = "imperial";
      }
      lg->debug( QString("DeviceConfigFragment::unitsLengthChangedSlot: lengt unit to %1").arg(lenString));
    }
    spxConfig->setUnitsLength( lUnit );
  }

  void DeviceConfigFragment::unitsWatertypeChangedSlot( int index )
  {
    DeviceWaterType wUnit = static_cast<DeviceWaterType>(index);
    if( lg->getThreshold() == LgThreshold::LG_DEBUG )
    {
      QString typeString;
      //
      if( wUnit == DeviceWaterType::FRESHWATER )
      {
        typeString = "freshwater";
      }
      else
      {
        typeString = "saltwater";
      }
      lg->debug( QString("DeviceConfigFragment::unitsWatertypeChangedSlot: lengt unit to %1").arg(typeString));
    }
    spxConfig->setUnitsWaterType( wUnit );
  }

  void DeviceConfigFragment::setpointAutoChangedSlot( int index )
  {
    // TODO: imperiale Maße auch berücksichtigen
    DeviceSetpointAuto autoSP = static_cast<DeviceSetpointAuto>(index);
    if( lg->getThreshold() == LgThreshold::LG_DEBUG )
    {
      int autoDepth;
      switch( index )
      {
        case static_cast<int>(DeviceSetpointAuto::AUTO_06):
          autoDepth = 6;
          break;
        case static_cast<int>(DeviceSetpointAuto::AUTO_10):
          autoDepth = 10;
          break;
        case static_cast<int>(DeviceSetpointAuto::AUTO_15):
          autoDepth = 15;
          break;
        case static_cast<int>(DeviceSetpointAuto::AUTO_20):
          autoDepth = 20;
          break;
        case static_cast<int>(DeviceSetpointAuto::AUTO_OFF):
        default:
          autoDepth = 0;
      }
      lg->debug( QString("DeviceConfigFragment::setpointAutoChangedSlot: auto setpoint to %1").arg(autoDepth));
    }
    spxConfig->setSetpointAuto( autoSP );
  }

  void DeviceConfigFragment::setpointValueChangedSlot( int index )
  {
    // TODO: imperiale Maße auch berücksichtigen
    DeviceSetpointValue setpointValue = static_cast<DeviceSetpointValue>(index);
    if( lg->getThreshold() == LgThreshold::LG_DEBUG )
    {
      int setpoint;
      switch( index )
      {
        case static_cast<int>(DeviceSetpointValue::SETPOINT_10):
          setpoint = 10;
          break;
        case static_cast<int>(DeviceSetpointValue::SETPOINT_11):
          setpoint = 11;
          break;
        case static_cast<int>(DeviceSetpointValue::SETPOINT_12):
          setpoint = 12;
          break;
        case static_cast<int>(DeviceSetpointValue::SETPOINT_13):
          setpoint = 13;
          break;
        case static_cast<int>(DeviceSetpointValue::SETPOINT_14):
        default:
          setpoint = 14;
      }
      lg->debug( QString("DeviceConfigFragment::setpointValueChangedSlot: setpoint (ppo2) to %1").arg(setpoint));
    }
    spxConfig->setSetpointValue( setpointValue );
  }

  void DeviceConfigFragment::individualSensorsOnChangedSlot( int state )
  {
    DeviceIndividualSensors newState = DeviceIndividualSensors::SENSORS_OFF;
    //
    lg->debug( QString("DeviceConfigFragment::individualSensorsOnChangedSlot: changed to %1").arg(state) );
    if( state == static_cast<int>(Qt::CheckState::Checked) )
      newState = DeviceIndividualSensors::SENSORS_ON;
    spxConfig->setIndividualSensorsOn( newState );
  }

  void DeviceConfigFragment::individualPscrModeChangedSlot( int state )
  {
    DeviceIndividualPSCR newState = DeviceIndividualPSCR::PSCR_OFF;
    //
    lg->debug( QString("DeviceConfigFragment::individualSensorsOnChangedSlot: changed to %1").arg(state) );
    if( state == static_cast<int>(Qt::CheckState::Checked) )
      newState = DeviceIndividualPSCR::PSCR_ON;
    spxConfig->setIndividualPscrMode( newState );
  }

  void DeviceConfigFragment::individualSensorsCountChangedSlot( int index )
  {
    DeviceIndividualSensorCount setpointValue = static_cast<DeviceIndividualSensorCount>(index);
    if( lg->getThreshold() == LgThreshold::LG_DEBUG )
    {
      int sensorCount;
      switch( index )
      {
        case static_cast<int>(DeviceIndividualSensorCount::SENSOR_COUNT_01):
          sensorCount = 1;
          break;
        case static_cast<int>(DeviceIndividualSensorCount::SENSOR_COUNT_02):
          sensorCount = 2;
          break;
        case static_cast<int>(DeviceIndividualSensorCount::SENSOR_COUNT_03):
        default:
          sensorCount = 3;
      }
      lg->debug( QString("DeviceConfigFragment::individualSensorsCountChangedSlot: count sensors to %1").arg(sensorCount));
    }
    spxConfig->setIndividualSensorsCount( setpointValue );
  }

  void DeviceConfigFragment::individualAcousticChangedSlot( int state )
  {
    DeviceIndividualAcoustic newState = DeviceIndividualAcoustic::ACOUSTIC_OFF;
    //
    lg->debug( QString("DeviceConfigFragment::individualAcousticChangedSlot: changed to %1").arg(state) );
    if( state == static_cast<int>(Qt::CheckState::Checked) )
      newState = DeviceIndividualAcoustic::ACOUSTIC_ON;
    spxConfig->setIndividualAcoustic( newState );
  }

  void DeviceConfigFragment::individualLogIntervalChangedSlot( int index )
  {
    DeviceIndividualLogInterval logInterval = static_cast<DeviceIndividualLogInterval>(index);
    if( lg->getThreshold() == LgThreshold::LG_DEBUG )
    {
      int logInterval;
      switch( index )
      {
        case static_cast<int>(DeviceIndividualLogInterval::INTERVAL_20):
          logInterval = 20;
          break;
        case static_cast<int>(DeviceIndividualLogInterval::INTERVAL_30):
          logInterval = 30;
          break;
        case static_cast<int>(DeviceIndividualLogInterval::INTERVAL_60):
        default:
          logInterval = 60;
      }
      lg->debug( QString("DeviceConfigFragment::individualLogIntervalChangedSlot: interrval to %1").arg(logInterval));
    }
    spxConfig->setIndividualLogInterval( logInterval );
  }
}
