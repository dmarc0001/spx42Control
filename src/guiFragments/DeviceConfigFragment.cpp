#include "DeviceConfigFragment.hpp"
#include "ui_DeviceConfigFragment.h"

namespace spx
{
  /**
   * @brief Konstruktor für Konfigurationsfragment
   * @param parent Parentobjekt
   * @param logger Logger
   * @param spxCfg SPX42 Konfigurationsobjekt
   */
  DeviceConfigFragment::DeviceConfigFragment( QWidget *parent,
                                              std::shared_ptr< Logger > logger,
                                              std::shared_ptr< SPX42Database > spx42Database,
                                              std::shared_ptr< SPX42Config > spxCfg,
                                              std::shared_ptr< SPX42RemotBtDevice > remSPX42,
                                              std::shared_ptr< SPX42Commands > spxCmds )
      : QWidget( parent )
      , IFragmentInterface( logger, spx42Database, spxCfg, remSPX42, spxCmds )
      , ui( new Ui::DeviceConfig )
      , gradentSlotsIgnore( false )
      , oldAutoSetpoint()
      , newAutoSetpoint()
  {
    lg->debug( QString( "DeviceConfigFragment::DeviceConfigFragment -> device connected: %1..." )
                   .arg( remoteSPX42->getConnectionStatus() == SPX42RemotBtDevice::SPX42_CONNECTED ) );
    ui->setupUi( this );
    ui->transferProgressBar->setVisible( false );
    oldAutoSetpoint << tr( "OFF" ) << tr( "5 METERS" ) << tr( "10 METERS" ) << tr( "15 METERS" ) << tr( "20 METERS" );
    newAutoSetpoint << tr( "OFF" ) << tr( "6 METERS" ) << tr( "10 METERS" ) << tr( "15 METERS" ) << tr( "20 METERS" );
    initGuiWithConfig();
    setGuiConnected( remoteSPX42->getConnectionStatus() == SPX42RemotBtDevice::SPX42_CONNECTED );
    updateGuiFirmwareSpecific();
    connectSlots();  // alle slots aktivieren
    if ( remoteSPX42->getConnectionStatus() == SPX42RemotBtDevice::SPX42_CONNECTED )
      QTimer::singleShot( 250, this, &DeviceConfigFragment::onConfigUpdateSlot );
  }

  /**
   * @brief Destruktor, aufräumen
   */
  DeviceConfigFragment::~DeviceConfigFragment()
  {
    lg->debug( "DeviceConfigFragment::~DeviceConfigFragment..." );
    deactivateTab();
  }

  void DeviceConfigFragment::deactivateTab( void )
  {
    // alle signale trennen...
    disconnectSlots();
  }

  /**
   * @brief Initialisiere das Fragment (GUI Aufbauen)
   */
  void DeviceConfigFragment::initGuiWithConfig()
  {
    //
    // Initialisiere die GUI
    //
    ui->tabHeaderLabel->setText(
        QString( tr( "SETTINGS SPX42 SERIAL [%1] LIC: %2" ).arg( spxConfig->getSerialNumber() ).arg( spxConfig->getLicName() ) ) );
    //
    // Dekompression
    //
    setGuiForDecompression();
    //
    // Display von config
    //
    setGuiForDisplay();
    //
    // Einheiten von config
    //
    setGuiForUnits();
    //
    setGuiForSetpoint();
    //
    setGuiForLicense();  // individual wird hier implizit mit aufgerufen!
  }

  /**
   * @brief GUI für Dekomprtession nach spxConfig einrichten
   */
  void DeviceConfigFragment::setGuiForDecompression()
  {
    //
    // Signale vorher trennen um Schleifen zu vermeiden
    //
    disconnectSlots( SIGNALS_DECOMPRESSION );
    //
    DecompressionPreset cPreset = spxConfig->getCurrentDecoGradientPresetType();
    DecoGradient newGrad = spxConfig->getPresetValues( spxConfig->getCurrentDecoGradientPresetType() );
    ui->conservatismComboBox->setCurrentIndex( static_cast< int >( cPreset ) );
    ui->gradientLowSpinBox->setValue( newGrad.first );
    ui->gradientHighSpinBox->setValue( newGrad.second );
    if ( DecompressionDeepstops::DEEPSTOPS_ENABLED == spxConfig->getIstDeepstopsEnabled() )
    {
      ui->deepStopOnCheckBox->setCheckState( Qt::CheckState::Checked );
    }
    else
    {
      ui->deepStopOnCheckBox->setCheckState( Qt::CheckState::Unchecked );
    }
    if ( DecompressionDynamicGradient::DYNAMIC_GRADIENT_ON == spxConfig->getIsDecoDynamicGradients() )
    {
      ui->dynamicGradientsOnCheckBox->setCheckState( Qt::CheckState::Checked );
    }
    else
    {
      ui->dynamicGradientsOnCheckBox->setCheckState( Qt::CheckState::Unchecked );
    }
    // Signale mit Slots verbinden
    connectSlots( SIGNALS_DECOMPRESSION );
  }

  /**
   * @brief GUI für Display nach spxConfig einrichten
   */
  void DeviceConfigFragment::setGuiForDisplay()
  {
    disconnectSlots( SIGNALS_DISPLAY );
    //
    ui->displayBrightnessComboBox->setCurrentIndex( static_cast< int >( spxConfig->getDisplayBrightness() ) );
    ui->displayOrientationComboBox->setCurrentIndex( static_cast< int >( spxConfig->getDisplayOrientation() ) );
    //
    connectSlots( SIGNALS_DISPLAY );
  }

  /**
   * @brief GUI für Einheiten nach spxConfig einrichten
   */
  void DeviceConfigFragment::setGuiForUnits()
  {
    disconnectSlots( SIGNALS_UNITS );
    //
    ui->unitsTemperaturComboBox->setCurrentIndex( static_cast< int >( spxConfig->getUnitsTemperatur() ) );
    ui->unitsDeepComboBox->setCurrentIndex( static_cast< int >( spxConfig->getUnitsLength() ) );
    ui->unitsWaterTypeComboBox->setCurrentIndex( static_cast< int >( spxConfig->getUnitsWaterType() ) );
    //
    connectSlots( SIGNALS_UNITS );
  }

  /**
   * @brief GUI für Setpoint nach spxConfig einrichten
   */
  void DeviceConfigFragment::setGuiForSetpoint()
  {
    disconnectSlots( SIGNALS_SETPOINT );
    //
    ui->setpointSetpointComboBox->setCurrentIndex( static_cast< int >( spxConfig->getSetpointValue() ) );
    ui->setpointAutoOnComboBox->setCurrentIndex( static_cast< int >( spxConfig->getSetpointAuto() ) );
    //
    connectSlots( SIGNALS_SETPOINT );
  }

  /**
   * @brief GUI für Lizenz nach spxConfig einrichten
   */
  void DeviceConfigFragment::setGuiForLicense()
  {
    SPX42License lic( spxConfig->getLicense() );

    lg->debug( QString( "DeviceConfigFragment::licChangedSlot -> set: %1" ).arg( static_cast< int >( lic.getLicType() ) ) );
    ui->tabHeaderLabel->setText(
        QString( tr( "SETTINGS SPX42 SERIAL [%1] LIC: %2" ).arg( spxConfig->getSerialNumber() ).arg( spxConfig->getLicName() ) ) );
    if ( lic.getLicInd() == IndividualLicense::LIC_INDIVIDUAL )
    {
      // der darf das!
      ui->individualVerticalWidget->setEnabled( true );
      ui->individualVerticalWidget->show();
    }
    else
    {
      // die Grundeinstellungen (zur Sicherheit)
      ui->individualVerticalWidget->setEnabled( false );
      ui->individualVerticalWidget->hide();
      // Einstellungen neutral
      disconnectSlots( SIGNALS_INDIVIDUAL );
      spxConfig->setIndividualSensorsOn( DeviceIndividualSensors::SENSORS_ON );
      spxConfig->setIndividualPscrMode( DeviceIndividualPSCR::PSCR_OFF );
      spxConfig->setIndividualSensorsCount( DeviceIndividualSensorCount::SENSOR_COUNT_03 );
      spxConfig->setIndividualAcoustic( DeviceIndividualAcoustic::ACOUSTIC_ON );
      spxConfig->setIndividualLogInterval( DeviceIndividualLogInterval::INTERVAL_60 );
      connectSlots( SIGNALS_INDIVIDUAL );
    }
    setGuiForIndividual();
  }

  /**
   * @brief GUI für Individual nach spxConfig einrichten
   */
  void DeviceConfigFragment::setGuiForIndividual()
  {
    if ( spxConfig->getLicense().getLicInd() == IndividualLicense::LIC_INDIVIDUAL )
    {
      disconnectSlots( SIGNALS_INDIVIDUAL );
      //
      if ( spxConfig->getIndividualSensorsOn() == DeviceIndividualSensors::SENSORS_ON )
      {
        ui->individualSeonsorsOnCheckBox->setCheckState( Qt::CheckState::Checked );
      }
      else
      {
        ui->individualSeonsorsOnCheckBox->setCheckState( Qt::CheckState::Unchecked );
      }
      if ( spxConfig->getIndividualPscrMode() == DeviceIndividualPSCR::PSCR_ON )
      {
        ui->individualPSCRModeOnCheckBox->setCheckState( Qt::CheckState::Checked );
      }
      else
      {
        ui->individualPSCRModeOnCheckBox->setCheckState( Qt::CheckState::Unchecked );
      }
      ui->individualSensorsCountComboBox->setCurrentIndex( static_cast< int >( spxConfig->getIndividualSensorsCount() ) );
      if ( spxConfig->getIndividualAcoustic() == DeviceIndividualAcoustic::ACOUSTIC_ON )
      {
        ui->individualAcousticWarningsOnCheckBox->setCheckState( Qt::CheckState::Checked );
      }
      else
      {
        ui->individualAcousticWarningsOnCheckBox->setCheckState( Qt::CheckState::Unchecked );
      }
      ui->individualLogIntervalComboBox->setCurrentIndex( static_cast< int >( spxConfig->getIndividualLogInterval() ) );
      //
      connectSlots( SIGNALS_INDIVIDUAL );
    }
  }

  void DeviceConfigFragment::setGuiConnected( bool connected )
  {
    // GUI online oder offline
    ui->decompressionGroupBox->setEnabled( connected );
    ui->displayGroupBox->setEnabled( connected );
    ui->unitGroupBox->setEnabled( connected );
    ui->setpointGroupBox->setEnabled( connected );
    //
    // individual abhängig von der Lizenz
    //
    if ( ( spxConfig->getLicense().getLicInd() == IndividualLicense::LIC_INDIVIDUAL ) && connected )
      ui->individualGroupBox->setEnabled( true );
    else
      ui->individualGroupBox->setEnabled( false );
  }

  /**
   * @brief trenne Slots von Signalen
   */
  void DeviceConfigFragment::disconnectSlots( quint8 which_signals )
  {
    //
    // Alle Slots trennen
    //
    if ( SIGNALS_INDIVIDUAL & which_signals )
    {
      // INDIVIDUAL
      disconnect( ui->individualLogIntervalComboBox );
      disconnect( ui->individualAcousticWarningsOnCheckBox );
      disconnect( ui->individualSensorsCountComboBox );
      disconnect( ui->individualPSCRModeOnCheckBox );
      disconnect( ui->individualSeonsorsOnCheckBox );
    }

    if ( SIGNALS_SETPOINT & which_signals )
    {
      // SETPOINT
      disconnect( ui->setpointSetpointComboBox );
      disconnect( ui->setpointAutoOnComboBox );
    }

    if ( SIGNALS_UNITS & which_signals )
    {
      // EINHEITEN
      disconnect( ui->unitsWaterTypeComboBox );
      disconnect( ui->unitsDeepComboBox );
      disconnect( ui->unitsTemperaturComboBox );
    }

    if ( SIGNALS_DISPLAY & which_signals )
    {
      // DISPLAY
      disconnect( ui->displayOrientationComboBox );
      disconnect( ui->displayBrightnessComboBox );
    }

    if ( SIGNALS_DECOMPRESSION & which_signals )
    {
      // Dekompression
      disconnect( ui->deepStopOnCheckBox );
      disconnect( ui->dynamicGradientsOnCheckBox );
      disconnect( ui->gradientHighSpinBox );
      disconnect( ui->gradientLowSpinBox );
      disconnect( ui->conservatismComboBox );
    }

    if ( SIGNALS_PROGRAM & which_signals )
    {
      // Lizenz
      disconnect( spxConfig.get(), nullptr, this, nullptr );
      // CONFIG
      disconnect( remoteSPX42.get(), nullptr, this, nullptr );
      disconnect( remoteSPX42.get(), nullptr, this, nullptr );
      disconnect( remoteSPX42.get(), nullptr, this, nullptr );
      disconnect( ui->lastDecoStopComboBox, nullptr, this, nullptr );
    }
  }

  /**
   * @brief DeviceConfigFragment::connectSlots
   */
  void DeviceConfigFragment::connectSlots( quint8 which_signals )
  {
    //
    // Alle Slots verbinden
    //
    if ( SIGNALS_INDIVIDUAL & which_signals )
    {
      // INDIVIDUAL
      connect( ui->individualSeonsorsOnCheckBox, &QCheckBox::stateChanged, this,
               &DeviceConfigFragment::onIndividualSensorsOnChangedSlot );
      connect( ui->individualPSCRModeOnCheckBox, &QCheckBox::stateChanged, this,
               &DeviceConfigFragment::onInIndividualPscrModeChangedSlot );
      connect( ui->individualSensorsCountComboBox, static_cast< void ( QComboBox::* )( int ) >( &QComboBox::currentIndexChanged ),
               this, &DeviceConfigFragment::onIndividualSensorsCountChangedSlot );
      connect( ui->individualAcousticWarningsOnCheckBox, &QCheckBox::stateChanged, this,
               &DeviceConfigFragment::onIndividualAcousticChangedSlot );
      connect( ui->individualLogIntervalComboBox, static_cast< void ( QComboBox::* )( int ) >( &QComboBox::currentIndexChanged ), this,
               &DeviceConfigFragment::onIndividualLogIntervalChangedSlot );
    }

    if ( SIGNALS_SETPOINT & which_signals )
    {
      // SETPOINT
      connect( ui->setpointAutoOnComboBox, static_cast< void ( QComboBox::* )( int ) >( &QComboBox::currentIndexChanged ), this,
               &DeviceConfigFragment::onSetpointAutoChangedSlot );
      connect( ui->setpointSetpointComboBox, static_cast< void ( QComboBox::* )( int ) >( &QComboBox::currentIndexChanged ), this,
               &DeviceConfigFragment::onSetpointValueChangedSlot );
    }

    if ( SIGNALS_UNITS & which_signals )
    {
      // EINHEITEN
      connect( ui->unitsTemperaturComboBox, static_cast< void ( QComboBox::* )( int ) >( &QComboBox::currentIndexChanged ), this,
               &DeviceConfigFragment::onUnitsTemperatureChangedSlot );
      connect( ui->unitsDeepComboBox, static_cast< void ( QComboBox::* )( int ) >( &QComboBox::currentIndexChanged ), this,
               &DeviceConfigFragment::onUnitsLengthChangedSlot );
      connect( ui->unitsWaterTypeComboBox, static_cast< void ( QComboBox::* )( int ) >( &QComboBox::currentIndexChanged ), this,
               &DeviceConfigFragment::onUnitsWatertypeChangedSlot );
    }

    if ( SIGNALS_INDIVIDUAL & which_signals )
    {
      // DISPLAY
      connect( ui->displayBrightnessComboBox, static_cast< void ( QComboBox::* )( int ) >( &QComboBox::currentIndexChanged ), this,
               &DeviceConfigFragment::onDisplayBrightnessChangedSlot );
      connect( ui->displayOrientationComboBox, static_cast< void ( QComboBox::* )( int ) >( &QComboBox::currentIndexChanged ), this,
               &DeviceConfigFragment::onDisplayOrientationChangedSlot );
    }

    if ( SIGNALS_DECOMPRESSION & which_signals )
    {
      // Dekompression
      connect( ui->conservatismComboBox, static_cast< void ( QComboBox::* )( int ) >( &QComboBox::currentIndexChanged ), this,
               &DeviceConfigFragment::onDecoComboChangedSlot );
      connect( ui->gradientLowSpinBox, static_cast< void ( QSpinBox::* )( int ) >( &QSpinBox::valueChanged ), this,
               &DeviceConfigFragment::onDecoGradientLowChangedSlot );
      connect( ui->gradientHighSpinBox, static_cast< void ( QSpinBox::* )( int ) >( &QSpinBox::valueChanged ), this,
               &DeviceConfigFragment::onDecoGradientHighChangedSlot );
      connect( ui->dynamicGradientsOnCheckBox, &QCheckBox::stateChanged, this,
               &DeviceConfigFragment::onDecoDynamicGradientStateChangedSlot );
      connect( ui->deepStopOnCheckBox, &QCheckBox::stateChanged, this, &DeviceConfigFragment::onDecoDeepStopsEnableChangedSlot );
      connect( ui->lastDecoStopComboBox, static_cast< void ( QComboBox::* )( int ) >( &QComboBox::currentIndexChanged ), this,
               &DeviceConfigFragment::onDecoLastStopChangedSlot );
    }

    if ( SIGNALS_PROGRAM & which_signals )
    {
      // Lizenz
      connect( spxConfig.get(), &SPX42Config::licenseChangedSig, this, &DeviceConfigFragment::onConfLicChangedSlot );
      // CONFIG
      connect( remoteSPX42.get(), &SPX42RemotBtDevice::onStateChangedSig, this, &DeviceConfigFragment::onOnlineStatusChangedSlot );
      connect( remoteSPX42.get(), &SPX42RemotBtDevice::onSocketErrorSig, this, &DeviceConfigFragment::onSocketErrorSlot );
      connect( remoteSPX42.get(), &SPX42RemotBtDevice::onDatagramRecivedSig, this, &DeviceConfigFragment::onDatagramRecivedSlot );
    }
  }

  /**
   * @brief changeEvent Signal vom System bearbeiten
   * @param Event
   */
  void DeviceConfigFragment::changeEvent( QEvent *e )
  {
    QWidget::changeEvent( e );
    switch ( e->type() )
    {
      case QEvent::LanguageChange:
        ui->retranslateUi( this );
        break;
      default:
        break;
    }
  }

  /**
   * @brief Slot beim ändern der Lizenz
   * @param Lizenz ders SPX
   */
  void DeviceConfigFragment::onConfLicChangedSlot()
  {
    setGuiForLicense();
  }

  void DeviceConfigFragment::onOnlineStatusChangedSlot( bool )
  {
    setGuiConnected( ( remoteSPX42->getConnectionStatus() == SPX42RemotBtDevice::SPX42_CONNECTED ) ||
                     remoteSPX42->getConnectionStatus() == SPX42RemotBtDevice::SPX42_CONNECTING );

    //
    // wenn der SPX gerade verbunden wurde, frag ihn nach den Angaben
    // aber ein wenig warten, damit andere Prozesse auch eine Chance haben
    //
    if ( remoteSPX42->getConnectionStatus() == SPX42RemotBtDevice::SPX42_CONNECTED )
      QTimer::singleShot( 450, this, &DeviceConfigFragment::onConfigUpdateSlot );
  }

  void DeviceConfigFragment::onSocketErrorSlot( QBluetoothSocket::SocketError )
  {
    // TODO: implementieren
  }

  void DeviceConfigFragment::onCloseDatabaseSlot()
  {
    // TODO: implementieren
  }

  void DeviceConfigFragment::updateGuiFirmwareSpecific()
  {
    //
    // Firmwarespezifische Sachen einstellen
    //
    if ( spxConfig->getIsSixMetersAutoSetpoint() )
    {
      // neuere Firmware, wert von 6, 10, 15 Meter
      // anpassen der GUI
      ui->setpointAutoOnComboBox->clear();
      ui->setpointAutoOnComboBox->addItems( newAutoSetpoint );
    }
    else
    {
      // alte Firmware deact, 6, 10, 15, 20 Meter
      // anpassen der GUI
      ui->setpointAutoOnComboBox->clear();
      ui->setpointAutoOnComboBox->addItems( oldAutoSetpoint );
    }
  }

  void DeviceConfigFragment::onDatagramRecivedSlot()
  {
    // TODO: implementieren
    QByteArray datagram;
    QDateTime nowDateTime;
    QByteArray value;
    char kdo;
    //
    lg->debug( "DeviceConfigFragment::onDatagramRecivedSlot..." );
    //
    // alle abholen...
    //
    while ( remoteSPX42->getNextDatagram( datagram ) )
    {
      // ja, es gab ein Datagram zum abholen
      kdo = spxCommands->decodeCommand( datagram );
      switch ( kdo )
      {
        case SPX42CommandDef::SPX_ALIVE:
          lg->debug( "DeviceConfigFragment::onDatagramRecivedSlot -> alive/acku..." );
          value = spxCommands->getParameter( 1 );
          if ( value.length() == 2 )
            value += "0";
          ackuVal = ( value.toInt( nullptr, 16 ) / 100.0 );
          // TODO: in der GUI anzeigen
          emit onAkkuValueChangedSlot( ackuVal );
          break;
        case SPX42CommandDef::SPX_APPLICATION_ID:
          lg->debug( "DeviceConfigFragment::onDatagramRecivedSlot -> firmwareversion..." );
          // Setzte die Version in die Config
          spxConfig->setSpxFirmwareVersion( spxCommands->getParameter( 1 ) );
          // Geht das Datum zu setzen?
          if ( spxConfig->getCanSetDate() )
          {
            // ja der kann das Datum online setzten
            nowDateTime = QDateTime::currentDateTime();
            // sende das Datum an den SPX
            remoteSPX42->sendCommand( spxCommands->setDateTime( nowDateTime ) );
          }
          updateGuiFirmwareSpecific();
          break;
        case SPX42CommandDef::SPX_SERIAL_NUMBER:
          lg->debug( "DeviceConfigFragment::onDatagramRecivedSlot -> serialnumber..." );
          spxConfig->setSerialNumber( spxCommands->getParameter( 1 ) );
          setGuiConnected( remoteSPX42->getConnectionStatus() == SPX42RemotBtDevice::SPX42_CONNECTED );
          break;
        case SPX42CommandDef::SPX_LICENSE_STATE:
          lg->debug( "DeviceConfigFragment::onDatagramRecivedSlot -> license state..." );
          spxConfig->setLicense( spxCommands->getParameter( 1 ), spxCommands->getParameter( 2 ) );
          setGuiConnected( remoteSPX42->getConnectionStatus() == SPX42RemotBtDevice::SPX42_CONNECTED );
          break;
        case SPX42CommandDef::SPX_GET_SETUP_DEKO:
          lg->debug( "DeviceConfigFragment::onDatagramRecivedSlot -> deco state..." );
          // Kommando DEC liefert zurück:
          // ~34:LL:HH:D:Y:C
          // LL=GF-Low, HH=GF-High,
          // D=Deepstops (0/1)
          // Y=Dynamische Gradienten (0/1)
          // C=Last Decostop (0=3 Meter/1=6 Meter)
          //
          // eintrag in GUI -> durch callback dann auch eintrag in config
          lg->debug( QString( "DeviceConfigFragment::onDatagramRecivedSlot -> LOW: %1 HIGH: %1" )
                         .arg( spxCommands->getParameter( 1 ).toInt( nullptr, 16 ) )
                         .arg( spxCommands->getParameter( 2 ).toInt( nullptr, 16 ) ) );
          ui->gradientLowSpinBox->setValue( spxCommands->getParameter( 1 ).toInt( nullptr, 16 ) );
          ui->gradientHighSpinBox->setValue( spxCommands->getParameter( 2 ).toInt( nullptr, 16 ) );
          ui->deepStopOnCheckBox->setCheckState(
              spxCommands->getParameter( 3 ).toInt( nullptr, 16 ) == 1 ? Qt::CheckState::Checked : Qt::CheckState::Unchecked );
          ui->dynamicGradientsOnCheckBox->setCheckState(
              spxCommands->getParameter( 4 ).toInt( nullptr, 16 ) == 1 ? Qt::CheckState::Checked : Qt::CheckState::Unchecked );
          ui->lastDecoStopComboBox->setCurrentIndex( spxCommands->getParameter( 5 ).toInt( nullptr, 16 ) == 1 ? 1 : 0 );
          break;
        case SPX42CommandDef::SPX_GET_SETUP_SETPOINT:
          lg->debug( "DeviceConfigFragment::onDatagramRecivedSlot -> setpoint..." );
          // Kommando GET_SETUP_SETPOINT liefert
          // ~35:A:P
          // A = Setpoint bei alte Firmware (0,1,2,3) = (deact,6,10,15,20)
          // A = Setpoint bei neue firmware (0,1,2) = (6,10,15)
          // P = Partialdruck (0..4) 1.0 .. 1.4
          lg->debug( QString( "DeviceConfigFragment::onDatagramRecivedSlot -> autosetpoint %1, setpoint 1.%1..." )
                         .arg( spxCommands->getParameter( 1 ).toInt( nullptr, 16 ) )
                         .arg( spxCommands->getParameter( 2 ).toInt( nullptr, 16 ) ) );
          switch ( spxCommands->getParameter( 1 ).toInt( nullptr, 16 ) )
          {
            // autosetpoint bei Tiefe....
            default:
            case 0:
              ui->setpointAutoOnComboBox->setCurrentIndex( 0 );
              break;
            case 1:
              ui->setpointAutoOnComboBox->setCurrentIndex( 1 );
              break;
            case 2:
              ui->setpointAutoOnComboBox->setCurrentIndex( 2 );
              break;
            case 3:
              ui->setpointAutoOnComboBox->setCurrentIndex( 3 );
              break;
            case 4:
              ui->setpointAutoOnComboBox->setCurrentIndex( 4 );
              break;
          }
          switch ( spxCommands->getParameter( 2 ).toInt( nullptr, 16 ) )
          {
            // setpoint wert 1.0 bis 1.4
            default:
            case 0:
              ui->setpointSetpointComboBox->setCurrentIndex( 0 );
              break;
            case 1:
              ui->setpointSetpointComboBox->setCurrentIndex( 1 );
              break;
            case 2:
              ui->setpointSetpointComboBox->setCurrentIndex( 2 );
              break;
            case 3:
              ui->setpointSetpointComboBox->setCurrentIndex( 3 );
              break;
            case 4:
              ui->setpointSetpointComboBox->setCurrentIndex( 4 );
              break;
          }
          break;
        case SPX42CommandDef::SPX_GET_SETUP_DISPLAYSETTINGS:
          lg->debug( "DeviceConfigFragment::onDatagramRecivedSlot -> display settings..." );
          break;
        case SPX42CommandDef::SPX_GET_SETUP_UNITS:
          lg->debug( "DeviceConfigFragment::onDatagramRecivedSlot -> unit settings..." );
          break;
        case SPX42CommandDef::SPX_GET_SETUP_INDIVIDUAL:
          lg->debug( "DeviceConfigFragment::onDatagramRecivedSlot -> individual..." );
          break;
      }
      //
      // falls es mehr gibt, lass dem Rest der App auch eine Chance
      //
      QCoreApplication::processEvents();
    }
  }

  /**
   * @brief Dekompressionseinstellung geändert
   * @param der neue Index
   */
  void DeviceConfigFragment::onDecoComboChangedSlot( int index )
  {
    DecoGradient newGradient;
    //
    if ( gradentSlotsIgnore )
      return;
    //
    // index korrespondiert mit dem Key des Deko-Presets
    // QHash aus SPX42Config
    //
    newGradient = spxConfig->getPresetValues( static_cast< DecompressionPreset >( index ) );
    lg->debug( QString( "DeviceConfigFragment::onDecoComboChangedSlot: combobox new index: %1, new gradients low: %2, high: %3 " )
                   .arg( index )
                   .arg( newGradient.first )
                   .arg( newGradient.second ) );
    //
    // Slots für changeInput kurz deaktivieren
    // und dann Werte neu eintragen
    //
    gradentSlotsIgnore = true;
    ui->gradientLowSpinBox->setValue( newGradient.first );
    ui->gradientHighSpinBox->setValue( newGradient.second );
    spxConfig->setCurrentPreset( static_cast< DecompressionPreset >( index ), newGradient.first, newGradient.second );
    gradentSlotsIgnore = false;
  }

  /**
   * @brief der LOW Gradient wurde manuell geändert
   * @param der neue LOW Wert
   */
  void DeviceConfigFragment::onDecoGradientLowChangedSlot( int low )
  {
    DecompressionPreset preset;
    // DecoGradient newGradient;
    qint8 high;
    //
    if ( gradentSlotsIgnore )
      return;
    high = static_cast< qint8 >( ui->gradientHighSpinBox->value() );
    lg->debug( QString( "DeviceConfigFragment::onDecoGradientLowChangedSlot: gradients changed to low: %1 high: %2" )
                   .arg( static_cast< int >( low ) )
                   .arg( static_cast< int >( high ) ) );
    //
    // Passen die Werte zu einem Preset?
    //
    preset = spxConfig->getPresetForGradient( static_cast< qint8 >( low ), static_cast< qint8 >( high ) );
    //
    // hat sich dabei das Preset verändert?
    //
    if ( ui->conservatismComboBox->currentIndex() != static_cast< qint8 >( preset ) )
    {
      setGradientPresetWithoutCallback( preset );
    }
    spxConfig->setCurrentPreset( preset, static_cast< qint8 >( low ), high );
  }

  /**
   * @brief der HIGH Gradient wurde manuell geändert
   * @param der neue HIGH Wert
   */
  void DeviceConfigFragment::onDecoGradientHighChangedSlot( int high )
  {
    DecompressionPreset preset;
    // DecoGradient newGradient;
    qint8 low;
    //
    if ( gradentSlotsIgnore )
      return;
    low = static_cast< qint8 >( ui->gradientLowSpinBox->value() );
    lg->debug( QString( "DeviceConfigFragment::onDecoGradientHighChangedSlot: gradients changed to low: %1 high: %2" )
                   .arg( static_cast< int >( low ) )
                   .arg( static_cast< int >( high ) ) );
    //
    // Passen die Werte zu einem Preset?
    //
    preset = spxConfig->getPresetForGradient( static_cast< qint8 >( low ), static_cast< qint8 >( high ) );
    spxConfig->setCurrentPreset( preset, low, static_cast< qint8 >( high ) );
    if ( ui->conservatismComboBox->currentIndex() != static_cast< qint8 >( preset ) )
    {
      setGradientPresetWithoutCallback( preset );
    }
    spxConfig->setCurrentPreset( preset, low, static_cast< qint8 >( high ) );
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
    ui->conservatismComboBox->setCurrentIndex( static_cast< int >( preset ) );
    lg->debug( QString( "DeviceConfigFragment::setGradientPresetWithoutCallback: combobox new index: %1" )
                   .arg( static_cast< int >( preset ) ) );
    //
    // Callbacks wieder erlauben
    //
    gradentSlotsIgnore = false;
  }

  /**
   * @brief Dynamiche gradienten ein/aus schalten
   * @param state neuer Status
   */
  void DeviceConfigFragment::onDecoDynamicGradientStateChangedSlot( int state )
  {
    DecompressionDynamicGradient newState = DecompressionDynamicGradient::DYNAMIC_GRADIENT_OFF;
    //
    lg->debug( QString( "DeviceConfigFragment::onDecoDynamicGradientStateChangedSlot: changed to %1" ).arg( state ) );
    if ( state == static_cast< int >( Qt::CheckState::Checked ) )
      newState = DecompressionDynamicGradient::DYNAMIC_GRADIENT_ON;
    spxConfig->setIsDecoDynamicGradients( newState );
  }

  /**
   * @brief Tiefe Deco-Stops ein/aus
   * @param state neuer Status
   */
  void DeviceConfigFragment::onDecoDeepStopsEnableChangedSlot( int state )
  {
    DecompressionDeepstops newState = DecompressionDeepstops::DEEPSTOPS_DISABLED;
    //
    lg->debug( QString( "DeviceConfigFragment::onDecoDeepStopsEnableChangedSlot: changed to %1" ).arg( state ) );
    if ( state == static_cast< int >( Qt::CheckState::Checked ) )
      newState = DecompressionDeepstops::DEEPSTOPS_ENABLED;
    spxConfig->setIsDeepstopsEnabled( newState );
  }

  /**
   * @brief DeviceConfigFragment::onDecoLastStopChangedSlot
   * @param index
   */
  void DeviceConfigFragment::onDecoLastStopChangedSlot( int index )
  {
    // index 0 ==> 3 Meter
    // index 1 ==> 6 Meter
    switch ( index )
    {
      default:
        spxConfig->setLastDecoStop( DecoLastStop::LAST_STOP_ON_3 );
        break;
      case 1:
        spxConfig->setLastDecoStop( DecoLastStop::LAST_STOP_ON_6 );
    }
  }

  /**
   * @brief Einstellung für Helligkeit des SPX42 Displays
   * @param index INdex der neuen Einstellung
   */
  void DeviceConfigFragment::onDisplayBrightnessChangedSlot( int index )
  {
    //
    if ( lg->getThreshold() == LgThreshold::LG_DEBUG )
    {
      //
      // das nur im debugging Modus machen
      //
      int brVal = 0;
      switch ( index )
      {
        case static_cast< int >( DisplayBrightness::BRIGHT_20 ):
          brVal = 20;
          break;
        case static_cast< int >( DisplayBrightness::BRIGHT_40 ):
          brVal = 40;
          break;
        case static_cast< int >( DisplayBrightness::BRIGHT_60 ):
          brVal = 60;
          break;
        case static_cast< int >( DisplayBrightness::BRIGHT_80 ):
          brVal = 80;
          break;
        case static_cast< int >( DisplayBrightness::BRIGHT_100 ):
        default:
          brVal = 100;
      }
      lg->debug( QString( "DeviceConfigFragment::onDisplayBrightnessChangedSlot: set brightness to %1" ).arg( brVal ) );
    }
    //
    // setze im config-Objekt die Helligkeit
    //
    spxConfig->setDisplayBrightness( static_cast< DisplayBrightness >( index ) );
  }

  /**
   * @brief Änderung der Ausrichtung des Displays
   * @param index der Displayorientierung
   */
  void DeviceConfigFragment::onDisplayOrientationChangedSlot( int index )
  {
    auto orientation = static_cast< DisplayOrientation >( index );
    if ( lg->getThreshold() == LgThreshold::LG_DEBUG )
    {
      QString orString;
      //
      if ( orientation == DisplayOrientation::LANDSCAPE )
      {
        orString = "landscape";
      }
      else
      {
        orString = "landscape 180°";
      }
      lg->debug( QString( "DeviceConfigFragment::onDisplayOrientationChangedSlot: orientation to %1" ).arg( orString ) );
    }
    spxConfig->setDisplayOrientation( static_cast< DisplayOrientation >( index ) );
  }

  /**
   * @brief Änderung der Temperatureinheit
   * @param index neuer Index der Einheit
   */
  void DeviceConfigFragment::onUnitsTemperatureChangedSlot( int index )
  {
    auto tUnit = static_cast< DeviceTemperaturUnit >( index );
    if ( lg->getThreshold() == LgThreshold::LG_DEBUG )
    {
      QString tmpString;
      //
      if ( tUnit == DeviceTemperaturUnit::CELSIUS )
      {
        tmpString = "celsius";
      }
      else
      {
        tmpString = "fahrenheid";
      }
      lg->debug( QString( "DeviceConfigFragment::onUnitsTemperatureChangedSlot: temperatur unit to %1" ).arg( tmpString ) );
    }
    spxConfig->setUnitsTemperatur( tUnit );
  }

  /**
   * @brief Änderung der Längeneinheit
   * @param index neuer Index der Einheit
   */
  void DeviceConfigFragment::onUnitsLengthChangedSlot( int index )
  {
    auto lUnit = static_cast< DeviceLenghtUnit >( index );
    if ( lg->getThreshold() == LgThreshold::LG_DEBUG )
    {
      QString lenString;
      //
      if ( lUnit == DeviceLenghtUnit::METRIC )
      {
        lenString = "metric";
      }
      else
      {
        lenString = "imperial";
      }
      lg->debug( QString( "DeviceConfigFragment::onUnitsLengthChangedSlot: lengt unit to %1" ).arg( lenString ) );
    }
    spxConfig->setUnitsLength( lUnit );
  }

  /**
   * @brief DeviceConfigFragment::unitsWatertypeChangedSlot
   * @param index
   */
  void DeviceConfigFragment::onUnitsWatertypeChangedSlot( int index )
  {
    auto wUnit = static_cast< DeviceWaterType >( index );
    if ( lg->getThreshold() == LgThreshold::LG_DEBUG )
    {
      QString typeString;
      //
      if ( wUnit == DeviceWaterType::FRESHWATER )
      {
        typeString = "freshwater";
      }
      else
      {
        typeString = "saltwater";
      }
      lg->debug( QString( "DeviceConfigFragment::onUnitsWatertypeChangedSlot: lengt unit to %1" ).arg( typeString ) );
    }
    spxConfig->setUnitsWaterType( wUnit );
  }

  /**
   * @brief Änderung des Auto setpoints
   * @param index neuer index des Autosetpoints
   */
  void DeviceConfigFragment::onSetpointAutoChangedSlot( int index )
  {
    // TODO: imperiale Maße auch berücksichtigen
    auto autoSP = static_cast< DeviceSetpointAuto >( index );
    if ( lg->getThreshold() == LgThreshold::LG_DEBUG )
    {
      int autoDepth;
      switch ( index )
      {
        case static_cast< int >( DeviceSetpointAuto::AUTO_06 ):
          autoDepth = 6;
          break;
        case static_cast< int >( DeviceSetpointAuto::AUTO_10 ):
          autoDepth = 10;
          break;
        case static_cast< int >( DeviceSetpointAuto::AUTO_15 ):
          autoDepth = 15;
          break;
        case static_cast< int >( DeviceSetpointAuto::AUTO_20 ):
          autoDepth = 20;
          break;
        case static_cast< int >( DeviceSetpointAuto::AUTO_OFF ):
        default:
          autoDepth = 0;
      }
      lg->debug( QString( "DeviceConfigFragment::onSetpointAutoChangedSlot: auto setpoint to %1" ).arg( autoDepth ) );
    }
    spxConfig->setSetpointAuto( autoSP );
  }

  /**
   * @brief Änderung des O2-Setpoints
   * @param index index des neuen Wertes für Setpoint
   */
  void DeviceConfigFragment::onSetpointValueChangedSlot( int index )
  {
    // TODO: imperiale Maße auch berücksichtigen
    auto setpointValue = static_cast< DeviceSetpointValue >( index );
    if ( lg->getThreshold() == LgThreshold::LG_DEBUG )
    {
      int setpoint;
      switch ( index )
      {
        case static_cast< int >( DeviceSetpointValue::SETPOINT_10 ):
          setpoint = 10;
          break;
        case static_cast< int >( DeviceSetpointValue::SETPOINT_11 ):
          setpoint = 11;
          break;
        case static_cast< int >( DeviceSetpointValue::SETPOINT_12 ):
          setpoint = 12;
          break;
        case static_cast< int >( DeviceSetpointValue::SETPOINT_13 ):
          setpoint = 13;
          break;
        case static_cast< int >( DeviceSetpointValue::SETPOINT_14 ):
        default:
          setpoint = 14;
      }
      lg->debug( QString( "DeviceConfigFragment::onSetpointValueChangedSlot: setpoint (ppo2) to %1" ).arg( setpoint ) );
    }
    spxConfig->setSetpointValue( setpointValue );
  }

  /**
   * @brief Sensoren an/aus
   * @param state neuer Status
   */
  void DeviceConfigFragment::onIndividualSensorsOnChangedSlot( int state )
  {
    DeviceIndividualSensors newState = DeviceIndividualSensors::SENSORS_OFF;
    //
    lg->debug( QString( "DeviceConfigFragment::onIndividualSensorsOnChangedSlot: changed to %1" ).arg( state ) );
    if ( state == static_cast< int >( Qt::CheckState::Checked ) )
      newState = DeviceIndividualSensors::SENSORS_ON;
    spxConfig->setIndividualSensorsOn( newState );
  }

  /**
   * @brief Passiv Semiclosed Rebreather Mode ein/aus
   * @param state neuer Status
   */
  void DeviceConfigFragment::onInIndividualPscrModeChangedSlot( int state )
  {
    DeviceIndividualPSCR newState = DeviceIndividualPSCR::PSCR_OFF;
    //
    lg->debug( QString( "DeviceConfigFragment::onInIndividualPscrModeChangedSlot: changed to %1" ).arg( state ) );
    if ( state == static_cast< int >( Qt::CheckState::Checked ) )
      newState = DeviceIndividualPSCR::PSCR_ON;
    spxConfig->setIndividualPscrMode( newState );
  }

  /**
   * @brief Änderung der Anzahl der genutzten Sensoren
   * @param index index der Anzahl der Sensoren
   */
  void DeviceConfigFragment::onIndividualSensorsCountChangedSlot( int index )
  {
    auto setpointValue = static_cast< DeviceIndividualSensorCount >( index );
    if ( lg->getThreshold() == LgThreshold::LG_DEBUG )
    {
      int sensorCount;
      switch ( index )
      {
        case static_cast< int >( DeviceIndividualSensorCount::SENSOR_COUNT_01 ):
          sensorCount = 1;
          break;
        case static_cast< int >( DeviceIndividualSensorCount::SENSOR_COUNT_02 ):
          sensorCount = 2;
          break;
        case static_cast< int >( DeviceIndividualSensorCount::SENSOR_COUNT_03 ):
        default:
          sensorCount = 3;
      }
      lg->debug( QString( "DeviceConfigFragment::onIndividualSensorsCountChangedSlot: count sensors to %1" ).arg( sensorCount ) );
    }
    spxConfig->setIndividualSensorsCount( setpointValue );
  }

  /**
   * @brief Änderung der akustischen Meldungen an/aus
   * @param state neuer Status
   */
  void DeviceConfigFragment::onIndividualAcousticChangedSlot( int state )
  {
    DeviceIndividualAcoustic newState = DeviceIndividualAcoustic::ACOUSTIC_OFF;
    //
    lg->debug( QString( "DeviceConfigFragment::onIndividualAcousticChangedSlot: changed to %1" ).arg( state ) );
    if ( state == static_cast< int >( Qt::CheckState::Checked ) )
      newState = DeviceIndividualAcoustic::ACOUSTIC_ON;
    spxConfig->setIndividualAcoustic( newState );
  }

  /**
   * @brief Änderung des Log intervals
   * @param index index des neuen Wertes
   */
  void DeviceConfigFragment::onIndividualLogIntervalChangedSlot( int index )
  {
    auto logInterval = static_cast< DeviceIndividualLogInterval >( index );
    if ( lg->getThreshold() == LgThreshold::LG_DEBUG )
    {
      int logInterval;
      switch ( index )
      {
        case static_cast< int >( DeviceIndividualLogInterval::INTERVAL_20 ):
          logInterval = 20;
          break;
        case static_cast< int >( DeviceIndividualLogInterval::INTERVAL_30 ):
          logInterval = 30;
          break;
        case static_cast< int >( DeviceIndividualLogInterval::INTERVAL_60 ):
        default:
          logInterval = 60;
      }
      lg->debug( QString( "DeviceConfigFragment::onIndividualLogIntervalChangedSlot: interrval to %1" ).arg( logInterval ) );
    }
    spxConfig->setIndividualLogInterval( logInterval );
  }

  /**
   * @brief DeviceConfigFragment::onConfigUpdateSlot
   */
  void DeviceConfigFragment::onConfigUpdateSlot()
  {
    //
    // es ist Zeit alle Einstellungen abzufragen
    // Seriennummer, Version, Lizenz wird im Objekt
    // SPX42ControlMainWin abgefragt...
    //
    // Beginnen wir mit Decompressionssachen
    //
    QByteArray sendCommand = spxCommands->askForConfig();
    lg->debug( "DeviceConfigFragment::onConfigUpdateSlot -> send cmd decoinfos..." );
    remoteSPX42->sendCommand( sendCommand );
    //
    // TODO: DISPLAY
    //
  }
}  // namespace spx
