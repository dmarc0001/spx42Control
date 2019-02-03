#include "OptionsDialog.hpp"

namespace spx
{
  OptionsDialog::OptionsDialog( QWidget *parent, AppConfigClass &conf, std::shared_ptr< Logger > logger )
      : QDialog( parent ), ui( new Ui::OptionsDialog() ), lg( logger ), cf( conf )
  {
    if ( lg != Q_NULLPTR )
    {
      lg->debug( "AboutDialog::AboutDialog..." );
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
    connect( ui->databaseSelectPushButton, &QPushButton::clicked, this, &OptionsDialog::onSelectDatabasePushBottonSlot );
    connect( ui->logfileSelectPushButton, &QPushButton::clicked, this, &OptionsDialog::onSelectLogfilePushBottonSlot );

  }  // namespace spx

  /**
   * @brief OptionsDialog::init
   */
  void OptionsDialog::init()
  {
    lg->debug( "OptionsDialog::init..." );
    //
    // initialisiere die Einstellungen
    //
    lg->debug( "OptionsDialog::init -> read and init database and log path..." );
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
        lg->debug( "OptionsDialog::init -> log threshold is warning..." );
        ui->warningRadioButton->setChecked( true );
        break;
      case LgThreshold::LG_INFO:
        lg->debug( "OptionsDialog::init -> log threshold is info..." );
        ui->infoRadioButton->setChecked( true );
        break;
      case LgThreshold::LG_DEBUG:
        lg->debug( "OptionsDialog::init -> log threshold is debug..." );
        ui->debugRadioButton->setChecked( true );
        break;
    }
  }

  /**
   * @brief OptionsDialog::accept
   */
  void OptionsDialog::accept()
  {
    lg->info( "OptionsDialog::accept -> set options accepting..." );
    //
    if ( !logfileDirName.isEmpty() )
      cf.setLogFilePath( logfileDirName );
    if ( !databaseFileDirName.isEmpty() )
      cf.setDatabasePath( databaseFileDirName );
    cf.setLogThreshold( logThreshold );
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
        lg->info( QString( "OptionsDialog::onSelectDatabasePushBottonSlot -> %1..." ).arg( databaseFileDirName ) );
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
        lg->info( QString( "OptionsDialog::onSelectLogfilePushBottonSlot -> %1..." ).arg( logfileDirName ) );
      }
      else
      {
        logfileDirName.clear();
      }
    }
  }
}  // namespace spx
