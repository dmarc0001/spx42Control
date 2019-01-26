#include "DeviceInfoFragment.hpp"
#include "ui_DeviceInfoFragment.h"

namespace spx
{
  DeviceInfoFragment::DeviceInfoFragment( QWidget *parent,
                                          std::shared_ptr< Logger > logger,
                                          std::shared_ptr< SPX42Database > spx42Database,
                                          std::shared_ptr< SPX42Config > spxCfg,
                                          std::shared_ptr< SPX42RemotBtDevice > remSPX42 )
      : QWidget( parent )
      , IFragmentInterface( logger, spx42Database, spxCfg, remSPX42 )
      , ui( new Ui::DeviceInfoFragment )
<<<<<<< HEAD
      , spxPic( ":/images/SPX42.jpg" )
=======
      , spxPic( ":/images/spx42-normal" )
>>>>>>> dev/stabilizing
  {
    lg->debug( "DeviceInfoFragment::DeviceInfoFragment..." );
    ui->setupUi( this );
    ui->transferProgressBar->setVisible( false );
    titleTemplate = tr( "DEVICE INFO Serial [%1] Lic: %2" );
    deviceModelTemplate = tr( "DEVICE MODEL: %1" );
    deviceSerialNumberTemplate = tr( "DEVICE SERIAL NUMBER: %1" );
    deviceFirmwareVersionTemplate = tr( "DEVICE FIRMWARE VERSION: %1" );
    unknownString = tr( "- UNKNOWN -" );
    notString = tr( "NOT " );
    fahrenheidBugTemplate = tr( "HAS %1FAHRENHEID BUG" );
    setDateTemplate = tr( "CAN %1SET DATE" );
    hasSixParamsTemplate = tr( "HAS %1SIX PARAMS INDIVIDUAL" );
    sixMetersAutoSetpointTemplate = tr( "AUTOSETPOINT IS %1" );
    firmwareSupportetTemplate = tr( "FIRMWARE IS %1SUPPORTED" );
    olderParamsSortTemplate = tr( "HAS OLDER PARAMS SORT" );
    newerParamsSortTemplate = tr( "HAS NEWER PARAMS SORT" );
    olderDisplayBrightnessTemplate = tr( "HAS OLDER DISPLAY BRIGHTNESS" );
    newerDisplayBrightnessTemplate = tr( "HAS NEWER DISPLAY BRIGHTNESS" );
    onConfLicChangedSlot();
    connect( spxConfig.get(), &SPX42Config::licenseChangedSig, this, &DeviceInfoFragment::onConfLicChangedSlot );
    connect( remoteSPX42.get(), &SPX42RemotBtDevice::onStateChangedSig, this, &DeviceInfoFragment::onOnlineStatusChangedSlot );
    prepareGuiWithConfig();
  }

  DeviceInfoFragment::~DeviceInfoFragment()
  {
    lg->debug( "DeviceInfoFragment::~DeviceInfoFragment..." );
    deactivateTab();
  }

  void DeviceInfoFragment::prepareGuiWithConfig()
  {
    //
    // stelle Verbindungsstatus fest
    //
    bool isConnected = remoteSPX42->getConnectionStatus() == SPX42RemotBtDevice::SPX42_CONNECTED;
#ifdef DEBUG
    ui->spx42ImageLabel->setVisible( false );
    ui->firmwarePartGroupBox->setVisible( true );
#else
    ui->spx42ImageLabel->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding );
    ui->spx42ImageLabel->setAlignment( Qt::AlignCenter );
    ui->spx42ImageLabel->setMinimumSize( 400, 250 );
    ui->spx42ImageLabel->setPixmap( QPixmap::fromImage( spxPic, Qt::AutoColor ) );
    ui->firmwarePartGroupBox->setVisible( false );
    ui->spx42ImageLabel->setVisible( true );
#endif
    //
    // da ist nur wichtig online/offline
    //
#ifdef DEBUG
    ui->hasSixParametersLabel->setEnabled( isConnected );
    ui->isOlderParametersSortLabel->setEnabled( isConnected );
    ui->isNewerDisplayBightnessLabel->setEnabled( isConnected );
    ui->isSixMetersAutoSetpointLabel->setEnabled( isConnected );
#endif
    //
    if ( isConnected )
    {
      //
      // TODO: Wenn sich das jemals ändert, hier noch die Config machen
      //
      ui->deviceModelLabel->setText( deviceModelTemplate.arg( "SPX42" ) );
      ui->deviceSerialNumberLabel->setText( deviceSerialNumberTemplate.arg( spxConfig->getSerialNumber() ) );
      ui->deviceFirmwareVersionNumber->setText( deviceFirmwareVersionTemplate.arg( spxConfig->getSpxFirmwareVersionString() ) );
#ifdef DEBUG
      ui->fahrenheidBugLabel->setEnabled( spxConfig->getHasFahrenheidBug() );
      ui->fahrenheidBugLabel->setText( fahrenheidBugTemplate.arg( spxConfig->getHasFahrenheidBug() ? "" : notString ) );
      ui->canSetDateLabel->setEnabled( spxConfig->getCanSetDate() );
      ui->canSetDateLabel->setText( setDateTemplate.arg( spxConfig->getCanSetDate() ? "" : notString ) );
      ui->hasSixParametersLabel->setText( hasSixParamsTemplate.arg( spxConfig->getHasSixValuesIndividual() ? "" : notString ) );
      ui->isFirmwareSupportedLabel->setEnabled( spxConfig->getIsFirmwareSupported() );
      ui->isFirmwareSupportedLabel->setText( firmwareSupportetTemplate.arg( spxConfig->getIsFirmwareSupported() ? "" : notString ) );
      ui->isOlderParametersSortLabel->setText( spxConfig->getIsOldParamSorting() ? olderParamsSortTemplate : newerParamsSortTemplate );
      ui->isNewerDisplayBightnessLabel->setText( spxConfig->getIsNewerDisplayBrightness() ? newerDisplayBrightnessTemplate
                                                                                          : olderDisplayBrightnessTemplate );
      ui->isSixMetersAutoSetpointLabel->setText(
          sixMetersAutoSetpointTemplate.arg( spxConfig->getIsSixMetersAutoSetpoint() ? "6m" : "5m" ) );
#endif
    }
    else
    {
      ui->deviceModelLabel->setText( deviceModelTemplate.arg( unknownString ) );
      //
      ui->deviceSerialNumberLabel->setText( deviceSerialNumberTemplate.arg( unknownString ) );
      ui->deviceFirmwareVersionNumber->setText( deviceFirmwareVersionTemplate.arg( unknownString ) );
      //
      // alle deaktiviert
      //
#ifdef DEBUG
      ui->fahrenheidBugLabel->setEnabled( false );
      ui->canSetDateLabel->setEnabled( false );
      ui->isFirmwareSupportedLabel->setEnabled( false );
      // texte dazu
      ui->fahrenheidBugLabel->setText( fahrenheidBugTemplate.arg( "" ).append( unknownString ) );
      ui->canSetDateLabel->setText( setDateTemplate.arg( "" ).append( unknownString ) );
      ui->hasSixParametersLabel->setText( hasSixParamsTemplate.arg( "" ).append( unknownString ) );
      ui->isFirmwareSupportedLabel->setText( firmwareSupportetTemplate.arg( "" ).append( unknownString ) );
      ui->isOlderParametersSortLabel->setText( olderParamsSortTemplate.append( unknownString ) );
      ui->isNewerDisplayBightnessLabel->setText( olderDisplayBrightnessTemplate.append( unknownString ) );
      ui->isSixMetersAutoSetpointLabel->setText( sixMetersAutoSetpointTemplate.arg( unknownString ) );
#endif
    }
  }

  void DeviceInfoFragment::deactivateTab()
  {
    disconnect( spxConfig.get(), nullptr, this, nullptr );
    disconnect( remoteSPX42.get(), nullptr, this, nullptr );
  }

  void DeviceInfoFragment::onOnlineStatusChangedSlot( bool )
  {
    lg->debug( "DeviceInfoFragment::onOnlineStatusChangedSlot..." );
    prepareGuiWithConfig();
  }

  void DeviceInfoFragment::onSocketErrorSlot( QBluetoothSocket::SocketError )
  {
    // TODO: implementieren
  }

  void DeviceInfoFragment::onConfLicChangedSlot()
  {
    lg->debug( QString( "DeviceInfoFragment::onConfLicChangedSlot -> set: %1" )
                   .arg( static_cast< int >( spxConfig->getLicense().getLicType() ) ) );
    ui->tabHeaderLabel->setText( QString( titleTemplate.arg( spxConfig->getSerialNumber() ).arg( spxConfig->getLicName() ) ) );
  }

  void DeviceInfoFragment::onCloseDatabaseSlot()
  {
    // TODO: implementieren
  }

  void DeviceInfoFragment::onCommandRecivedSlot()
  {
    spSingleCommand recCommand;
    QDateTime nowDateTime;
    char kdo;
    //
    lg->debug( "ConnectFragment::onDatagramRecivedSlot..." );
    //
    // alle abholen...
    //
    while ( ( recCommand = remoteSPX42->getNextRecCommand() ) )
    {
      // ja, es gab ein Datagram zum abholen
      kdo = recCommand->getCommand();
      switch ( kdo )
      {
        case SPX42CommandDef::SPX_ALIVE:
          // Kommando ALIVE liefert zurück:
          // ~03:PW
          // PX => Angabe HEX in Milivolt vom Akku
          lg->debug( "ConnectFragment::onDatagramRecivedSlot -> alive/acku..." );
          ackuVal = ( recCommand->getValueFromHexAt( SPXCmdParam::ALIVE_POWER ) / 100.0 );
          emit onAkkuValueChangedSig( ackuVal );
          break;
        case SPX42CommandDef::SPX_APPLICATION_ID:
          // Kommando APPLICATION_ID (VERSION)
          // ~04:NR -> VERSION als String
          lg->debug( "ConnectFragment::onDatagramRecivedSlot -> firmwareversion..." );
          // Setzte die Version in die Config
          spxConfig->setSpxFirmwareVersion( recCommand->getParamAt( SPXCmdParam::FIRMWARE_VERSION ) );
          spxConfig->freezeConfigs( SPX42ConfigClass::CF_CLASS_SPX );
          // Geht das Datum zu setzen?
          if ( spxConfig->getCanSetDate() )
          {
            // ja der kann das Datum online setzten
            nowDateTime = QDateTime::currentDateTime();
            // sende das Datum an den SPX
            remoteSPX42->setDateTime( nowDateTime );
          }
          break;
        case SPX42CommandDef::SPX_SERIAL_NUMBER:
          // Kommando SERIAL NUMBER
          // ~07:XXX -> Seriennummer als String
          lg->debug( "ConnectFragment::onDatagramRecivedSlot -> serialnumber..." );
          spxConfig->setSerialNumber( recCommand->getParamAt( SPXCmdParam::SERIAL_NUMBER ) );
          spxConfig->freezeConfigs( SPX42ConfigClass::CF_CLASS_SPX );
          ui->deviceSerialNumberLabel->setText( deviceSerialNumberTemplate.arg( spxConfig->getSerialNumber() ) );
          break;
        case SPX42CommandDef::SPX_LICENSE_STATE:
          // Kommando SPX_LICENSE_STATE
          // komplett: <~45:LS:CE>
          // übergeben LS,CE
          // LS : License State 0=Nitrox,1=Normoxic Trimix,2=Full Trimix
          // CE : Custom Enabled 0= disabled, 1=enabled
          lg->debug( "ConnectFragment::onDatagramRecivedSlot -> license state..." );
          spxConfig->setLicense( recCommand->getParamAt( SPXCmdParam::LICENSE_STATE ),
                                 recCommand->getParamAt( SPXCmdParam::LICENSE_INDIVIDUAL ) );
          spxConfig->freezeConfigs( SPX42ConfigClass::CF_CLASS_SPX );
          onConfLicChangedSlot();
          break;
      }
    }
    //
    // falls es mehr gibt, lass dem Rest der App auch eine Chance
    //
    QCoreApplication::processEvents();
  }
}  // namespace spx
