#include "OptionsDialog.hpp"

namespace spx
{
  OptionsDialog::OptionsDialog( QWidget *parent, AppConfigClass &conf, std::shared_ptr< Logger > logger )
      : QDialog( parent ), ui( new Ui::OptionsDialog() ), lg( logger ), cf( conf )
  {
    if ( lg != Q_NULLPTR )
    {
      *lg << LDEBUG << "AboutDialog::AboutDialog..." << Qt::endl;
    }
    //
    // Hilfebutton ausblenden
    //
    this->setWindowFlags( this->windowFlags() & ~Qt::WindowContextHelpButtonHint );
    //
    // GUI initialisieren
    //
    ui->setupUi( this );
    // fixe Größe
    setFixedSize( this->width(), this->height() );
    setWindowTitle( tr( "APPLICATION OPTIONS" ) );
    connect( ui->warningRadioButton, &QRadioButton::toggled, [=]( bool checked ) {
      if ( checked )
        logThreshold = LgThreshold::LG_WARN;
    } );
    connect( ui->infoRadioButton, &QRadioButton::toggled, [=]( bool checked ) {
      if ( checked )
        logThreshold = LgThreshold::LG_INFO;
    } );
    connect( ui->debugRadioButton, &QRadioButton::toggled, [=]( bool checked ) {
      if ( checked )
        logThreshold = LgThreshold::LG_DEBUG;
    } );
    connect( ui->darkRadioButton, &QRadioButton::toggled, [=]( bool checked ) {
      if ( checked )
        themeName = AppConfigClass::darktStr;
    } );
    connect( ui->lightRadioButton, &QRadioButton::toggled, [=]( bool checked ) {
      if ( checked )
        themeName = AppConfigClass::lightStr;
    } );
    connect( ui->customRadioButton, &QRadioButton::toggled, [=]( bool checked ) {
      if ( checked )
        themeName = AppConfigClass::customtStr;
    } );
    connect( ui->databaseSelectPushButton, &QPushButton::clicked, this, &OptionsDialog::onSelectDatabasePushBottonSlot );
    connect( ui->logfileSelectPushButton, &QPushButton::clicked, this, &OptionsDialog::onSelectLogfilePushBottonSlot );

  }  // namespace spx

  /**
   * @brief OptionsDialog::init
   */
  void OptionsDialog::init()
  {
    *lg << LDEBUG << "OptionsDialog::init..." << Qt::endl;
    //
    // initialisiere die Einstellungen
    //
    *lg << LDEBUG << "OptionsDialog::init -> read and init database and log path..." << Qt::endl;
    ui->databasePathLineEdit->setText( cf.getDatabasePath() );
    ui->logfilePathLineEdit->setText( cf.getLogfilePath() );
    //
    // Loglevel darstellen
    //
    logThreshold = cf.getLogTreshold();
    switch ( logThreshold )
    {
      default:
      case LgThreshold::LG_NONE:
      case LgThreshold::LG_CRIT:
      case LgThreshold::LG_WARN:
        *lg << LDEBUG << "OptionsDialog::init -> log threshold is warning..." << Qt::endl;
        ui->warningRadioButton->setChecked( true );
        break;
      case LgThreshold::LG_INFO:
        *lg << LDEBUG << "OptionsDialog::init -> log threshold is info..." << Qt::endl;
        ui->infoRadioButton->setChecked( true );
        break;
      case LgThreshold::LG_DEBUG:
        *lg << LDEBUG << "OptionsDialog::init -> log threshold is debug..." << Qt::endl;
        ui->debugRadioButton->setChecked( true );
        break;
    }
    themeName = cf.getGuiThemeName();
    if ( themeName == AppConfigClass::customtStr )
    {
      ui->customRadioButton->setChecked( true );
    }
    else if ( themeName == AppConfigClass::darktStr )
    {
      ui->darkRadioButton->setChecked( true );
    }
    else
    {
      themeName = AppConfigClass::lightStr;
      ui->lightRadioButton->setChecked( true );
    }
  }

  /**
   * @brief OptionsDialog::accept
   */
  void OptionsDialog::accept()
  {
    *lg << LINFO << "OptionsDialog::accept -> set options accepting..." << Qt::endl;
    //
    if ( !logfileDirName.isEmpty() )
      cf.setLogFilePath( logfileDirName );
    if ( !databaseFileDirName.isEmpty() )
      cf.setDatabasePath( databaseFileDirName );
    cf.setLogThreshold( logThreshold );
    cf.setGuiThemeName( themeName );
    QDialog::accept();
  }

  /**
   * @brief OptionsDialog::onSelectDatabasePushBottonSlot
   */
  void OptionsDialog::onSelectDatabasePushBottonSlot()
  {
    QFileDialog fileDial( this, tr( "SELECT DATABASE DIR" ), ui->databasePathLineEdit->text(), nullptr );
    fileDial.setFileMode( QFileDialog::Directory );
    fileDial.setViewMode( QFileDialog::Detail );
    if ( fileDial.exec() )
    {
      databaseFileDirName = fileDial.selectedFiles().first();
      QDir fileDir( databaseFileDirName );
      if ( fileDir.exists() )
      {
        *lg << LINFO << "OptionsDialog::onSelectDatabasePushBottonSlot -> " << databaseFileDirName << "..." << Qt::endl;
      }
      else
      {
        databaseFileDirName.clear();
      }
    }
  }

  /**
   * @brief OptionsDialog::onSelectLogfilePushBottonSlot
   */
  void OptionsDialog::onSelectLogfilePushBottonSlot()
  {
    QFileDialog fileDial( this, tr( "SELECT LOGFILE DIR" ), ui->logfilePathLineEdit->text(), nullptr );
    fileDial.setFileMode( QFileDialog::Directory );
    fileDial.setViewMode( QFileDialog::Detail );
    if ( fileDial.exec() )
    {
      logfileDirName = fileDial.selectedFiles().first();
      QDir fileDir( logfileDirName );
      if ( fileDir.exists() )
      {
        *lg << LINFO << "OptionsDialog::onSelectLogfilePushBottonSlot -> " << logfileDirName << "..." << Qt::endl;
      }
      else
      {
        logfileDirName.clear();
      }
    }
  }
}  // namespace spx
