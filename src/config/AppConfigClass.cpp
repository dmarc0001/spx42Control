#include <QApplication>
#include <QDebug>
#include <QSettings>

#include "AppConfigClass.hpp"
#include "CurrBuildDef.hpp"

namespace spx
{
  const QString AppConfigClass::constBuildDate{static_cast< const char * >( &SPX_BUILDTIME[ 0 ] )};
  const QString AppConfigClass::constBuildNumStr{static_cast< const char * >( &SPX_BUILDCOUNT[ 0 ] )};
  const QString AppConfigClass::constBuildTypeStr{static_cast< const char * >( &SPX_BUILDTYPE[ 0 ] )};
  const QString AppConfigClass::constLogGroupName{"logger"};
  const QString AppConfigClass::constLogFileKey{"logFileName"};
  const QString AppConfigClass::constNoData{"-"};
  const QString AppConfigClass::constAppGroupName{"application"};
  const QString AppConfigClass::constAppTimeoutKey{"watchdogTimeout"};
  const QString AppConfigClass::constAppDatabaseNameKey{"databaseFile"};
  const QString AppConfigClass::constAppDatabasePathKey{"databasePath"};
  const int AppConfigClass::defaultWatchdogTimerVal{20};
  const QString AppConfigClass::constAppThresholdKey{"loggingThreshold"};
  const qint8 AppConfigClass::defaultAppThreshold{4};
  const QString AppConfigClass::defaultDatabaseName{"spx42Database.sqlite"};
  const QString AppConfigClass::defaultDatabasePath{
      QStandardPaths::writableLocation( QStandardPaths::AppDataLocation ).append( "/spx42Control" )};

  /**
   * @brief LoggerClass::LoggerClass Der Konstruktor mit Name der Konfigdatei im Programmverzeichnis
   * @param cfg
   */
  AppConfigClass::AppConfigClass()
      : configFile( QApplication::applicationName() + ".ini" )
      , watchdogTimer( 0 )
      , logThreshold( 0 )
      , databasePath( defaultDatabasePath )
  {
  }

  AppConfigClass::~AppConfigClass()
  {
    qDebug().noquote() << "AppConfigClass::~AppConfigClass()";
    saveSettings();
  }

  /**
   * @brief AppConfigClass::getConfigFile Gibt das konfigurierte CONFIG File zurück (aus loadSettings)
   * @return
   */
  QString AppConfigClass::getConfigFile() const
  {
    return ( configFile );
  }

  /**
   * @brief LoggerClass::loadSettings Lade Einstellungen aus der Datei
   * @return
   */
  bool AppConfigClass::loadSettings()
  {
    qDebug().noquote() << "AppConfigClass::loadSettings(void) CONFIG: <" + configFile + ">";
    QSettings settings( configFile, QSettings::IniFormat );
    if ( !loadLogSettings( settings ) )
    {
      makeDefaultLogSettings( settings );
    }
    if ( !loadAppSettings( settings ) )
    {
      makeAppDefaultSettings( settings );
    }
    return ( true );
  }

  /**
   * @brief AppConfigClass::loadSettings Lade Einstellungen aus CONFIG Datei
   * @param cFile
   * @return
   */
  bool AppConfigClass::loadSettings( QString &cFile )
  {
    qDebug().noquote() << "AppConfigClass::loadSettings(" << cFile << ")";
    configFile = cFile;
    QSettings settings( configFile, QSettings::IniFormat );
    if ( !loadLogSettings( settings ) )
    {
      makeDefaultLogSettings( settings );
    }
    if ( !loadAppSettings( settings ) )
    {
      makeAppDefaultSettings( settings );
    }
    return ( true );
  }

  /**
   * @brief LoggerClass::saveSettings sichere Einstellunge in Datei
   * @return
   */
  bool AppConfigClass::saveSettings()
  {
    qDebug().noquote() << "AppConfigClass::saveSettings()";
    bool retVal = true;
    qDebug().noquote() << "AppConfigClass::saveSettings(void) CONFIG: <" + configFile + ">";
    // QSettings settings(configFile, QSettings::NativeFormat);
    QSettings settings( configFile, QSettings::IniFormat );
    //
    // die Logeinstellungen sichern
    //
    if ( !saveLogSettings( settings ) )
    {
      retVal = false;
    }
    if ( !saveAppSettings( settings ) )
    {
      retVal = false;
    }
    return ( retVal );
  }

  /*###########################################################################
   ############################################################################
   #### Logger Einstellungen                                               ####
   ############################################################################
  ###########################################################################*/

  /**
   * @brief AppConfigClass::getLogfileName Gib den Namen der LOGDATEI zurück
   * @return
   */
  QString AppConfigClass::getLogfileName() const
  {
    return logfileName;
  }

  /**
   * @brief AppConfigClass::setLogfileName Setze den Dateinamen des LOGFILES
   * @param value
   */
  void AppConfigClass::setLogfileName( const QString &value )
  {
    logfileName = value;
  }

  /**
   * @brief AppConfigClass::loadLogSettings Private Funktion zu m laden der Logeinstelungen
   * @param settings
   * @return
   */
  bool AppConfigClass::loadLogSettings( QSettings &settings )
  {
    bool retval = true;
    qDebug().noquote() << "AppConfigClass::loadLogSettings()";
    //
    // Öffne die Gruppe Logeinstellungen
    //
    settings.beginGroup( constLogGroupName );
    //
    // Lese den Dateinamen aus
    //
    logfileName = settings.value( constLogFileKey, AppConfigClass::constNoData ).toString();
    if ( QString::compare( logfileName, constNoData ) == 0 )
    {
      // Nicht gefunden -> Fehler markieren
      retval = false;
    }
    qDebug().noquote().nospace() << "AppConfigClass::loadLogSettings(): Logfile: <" + logfileName + ">";
    //
    // Ende der Gruppe
    //
    settings.endGroup();
    //
    // Ergebnis kommunizieren
    //
    return ( retval );
  }

  /**
   * @brief AppConfigClass::makeDefaultLogSettings Erzeuge VORGABE Einstellungen für LOGGING
   * @param settings
   */
  void AppConfigClass::makeDefaultLogSettings( QSettings &settings )
  {
    qDebug().noquote() << "AppConfigClass::makeDefaultLogSettings()";
    //
    // Öffne die Gruppe Logeinstellungen
    //
    settings.beginGroup( constLogGroupName );
    //
    // defaultwerte setzten
    //
    logfileName = QApplication::applicationName() + ".log";
    settings.setValue( constLogFileKey, logfileName );
    //
    // Ende der Gruppe
    //
    settings.endGroup();
  }

  /**
   * @brief AppConfigClass::saveLogSettings Sichere Einstellungen
   * @param settings
   * @return
   */
  bool AppConfigClass::saveLogSettings( QSettings &settings )
  {
    qDebug().noquote().nospace() << "AppConfigClass::saveLogSettings() DATEI: <" + configFile + ">";
    //
    // Öffne die Gruppe Logeinstellungen
    //
    settings.beginGroup( constLogGroupName );
    //
    settings.setValue( constLogFileKey, logfileName );
    //
    // Ende der Gruppe
    //
    settings.endGroup();
    return ( true );
  }

  /*###########################################################################
   ############################################################################
   #### Application settings                                               ####
   ############################################################################
  ###########################################################################*/

  int AppConfigClass::getWatchdogTime()
  {
    return ( watchdogTimer );
  }
  void AppConfigClass::setLogThreshold( qint8 th )
  {
    logThreshold = th;
  }

  qint8 AppConfigClass::getLogTreshold()
  {
    return ( logThreshold );
  }

  bool AppConfigClass::loadAppSettings( QSettings &settings )
  {
    bool retval = true;
    qDebug().noquote() << "AppConfigClass::loadAppSettings()";
    //
    // Öffne die Gruppe app
    //
    settings.beginGroup( constAppGroupName );
    //
    // Lese den Timeoutwert
    //
    watchdogTimer = settings.value( constAppTimeoutKey, 0 ).toInt();
    if ( watchdogTimer == 0 )
    {
      // Nicht gefunden -> Fehler markieren
      retval = false;
    }
    qDebug().noquote().nospace() << "AppConfigClass::loadAppSettings(): watchdog: <" << watchdogTimer << ">";
    //
    // Lese die Loggerstufe
    //
    logThreshold = static_cast< qint8 >( settings.value( constAppThresholdKey, 0 ).toInt() & 0xff );
    if ( logThreshold == 0 )
    {
      // Nicht gefunden -> default DEBUG
      logThreshold = defaultAppThreshold;
    }
    qDebug().noquote().nospace() << "AppConfigClass::loadAppSettings(): logThreshold: <" << logThreshold << ">";
    //
    // lese den Namen der Datenbank
    //
    databaseName = settings.value( constAppDatabaseNameKey, defaultDatabaseName ).toString();
    databasePath = settings.value( constAppDatabasePathKey, defaultDatabasePath ).toString();
    qDebug().noquote().nospace() << "AppConfigClass::loadAppSettings(): database: <" << databasePath << "/" << databaseName << ">";
    //
    // Ende der Gruppe
    //
    settings.endGroup();
    //
    // Ergebnis kommunizieren
    //
    return ( retval );
  }

  void AppConfigClass::makeAppDefaultSettings( QSettings &settings )
  {
    qDebug().noquote() << "AppConfigClass::makeAppDefaultSettings()";
    //
    // Öffne die Gruppe App
    //
    settings.beginGroup( constAppGroupName );
    //
    // defaultwerte setzten
    //
    watchdogTimer = AppConfigClass::defaultWatchdogTimerVal;
    settings.setValue( constAppTimeoutKey, watchdogTimer );
    logThreshold = defaultAppThreshold;
    settings.setValue( constAppThresholdKey, static_cast< int >( logThreshold ) );
    databaseName = defaultDatabaseName;
    settings.setValue( constAppDatabaseNameKey, databaseName );
    databasePath = defaultDatabasePath;
    settings.setValue( constAppDatabasePathKey, databasePath );
    //
    // Ende der Gruppe
    //
    settings.endGroup();
  }

  bool AppConfigClass::saveAppSettings( QSettings &settings )
  {
    //
    // Öffne die Gruppe Logeinstellungen
    //
    settings.beginGroup( constAppGroupName );
    //
    qDebug().noquote().nospace() << "AppConfigClass::saveAppSettings watchdog timer: <" << watchdogTimer << ">";
    settings.setValue( constAppTimeoutKey, watchdogTimer );
    qDebug().noquote().nospace() << "AppConfigClass::saveAppSettings threshold: <" << logThreshold << ">";
    settings.setValue( constAppThresholdKey, static_cast< int >( logThreshold ) );
    qDebug().noquote().nospace() << "AppConfigClass::saveAppSettings databasename: <" << databaseName << ">";
    settings.setValue( constAppDatabaseNameKey, databaseName );
    qDebug().noquote().nospace() << "AppConfigClass::saveAppSettings databasepath: <" << databasePath << ">";
    settings.setValue( constAppDatabasePathKey, databasePath );
    //
    // Ende der Gruppe
    //
    settings.endGroup();
    return ( true );
  }

  QString AppConfigClass::getBuildDate()
  {
    return constBuildDate;
  }

  QString AppConfigClass::getBuildNumStr()
  {
    return constBuildNumStr;
  }

  QString AppConfigClass::getBuldType()
  {
    return constBuildTypeStr;
  }

  QString AppConfigClass::getDatabaseName() const
  {
    return databaseName;
  }

  void AppConfigClass::setDatabaseName( const QString &value )
  {
    databaseName = value;
  }

  QString AppConfigClass::getDatabasePath() const
  {
    return databasePath;
  }

  void AppConfigClass::setDatabasePath( const QString &value )
  {
    databasePath = value;
  }

  QString AppConfigClass::getFullDatabaseLocation() const
  {
    // um korrekte Pfade zu erzeugen ein QFileobjekt
    QFile dbFile( QString().append( databasePath ).append( "/" ).append( databaseName ) );
    // Fileinfo zur korrekten erzeugung des kompletten Pfdades
    QFileInfo dbFileInfo( dbFile );
    return ( dbFileInfo.filePath() );
  }
}  // namespace spx
