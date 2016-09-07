#include <QSettings>
#include <QApplication>
#include <QDebug>

#include "AppConfigClass.hpp"


namespace spx42
{
  const QString AppConfigClass::constBuildDate = SPX_BUILDTIME;
  const QString AppConfigClass::constBuildNumStr = SPX_BUILDCOUNT;
  const QString AppConfigClass::constLogGroupName = "logger";
  const QString AppConfigClass::constLogFileKey = "logFileName";
  const QString AppConfigClass::constNoData = "-";
  const QString AppConfigClass::constAppGroupName = "application";
  const QString AppConfigClass::constAppTimeoutKey = "watchdogTimeout";
  const int AppConfigClass::defaultWatchdogTimerVal = 20 ;
  const QString AppConfigClass::constAppThresholdKey = "loggingThreshold";
  const qint8 AppConfigClass::defaultAppThreshold = 4;

  /**
   * @brief LoggerClass::LoggerClass Der Konstruktor mit Name der Konfigdatei im Programmverzeichnis
   * @param cfg
   */
  AppConfigClass::AppConfigClass(void)
    : configFile(QApplication::applicationName() + ".ini")
  {
  }

  AppConfigClass::~AppConfigClass(void)
  {
    qDebug().noquote() << "AppConfigClass::~AppConfigClass()";
    saveSettings();
  }

  /**
   * @brief AppConfigClass::getConfigFile Gibt das konfigurierte CONFIG File zurück (aus loadSettings)
   * @return
   */
  QString AppConfigClass::getConfigFile(void) const
  {
    return( configFile );
  }

  /**
  * @brief LoggerClass::loadSettings Lade Einstellungen aus der Datei
  * @return
  */
  bool AppConfigClass::loadSettings(void)
  {
    qDebug().noquote() << "AppConfigClass::loadSettings(void) CONFIG: <" + configFile + ">";
    QSettings settings(configFile, QSettings::IniFormat);
    if( !loadLogSettings(settings) )
    {
      makeDefaultLogSettings( settings );
    }
    if( ! loadAppSettings( settings ) )
    {
      makeAppDefaultSettings( settings );
    }
    return( true );
  }

  /**
   * @brief AppConfigClass::loadSettings Lade Einstellungen aus CONFIG Datei
   * @param cFile
   * @return
   */
  bool AppConfigClass::loadSettings( QString &cFile)
  {
    qDebug().noquote() << "AppConfigClass::loadSettings(" << cFile << ")";
    configFile = cFile;
    QSettings settings(configFile, QSettings::IniFormat);
    if( !loadLogSettings(settings) )
    {
      makeDefaultLogSettings( settings );
    }
    if( ! loadAppSettings( settings ) )
    {
      makeAppDefaultSettings( settings );
    }
    return( true );
  }


  /**
   * @brief LoggerClass::saveSettings sichere Einstellunge in Datei
   * @return
   */
  bool AppConfigClass::saveSettings(void)
  {
    qDebug().noquote() << "AppConfigClass::saveSettings()";
    bool retVal = true;
    qDebug().noquote() << "AppConfigClass::saveSettings(void) CONFIG: <" + configFile + ">";
    //QSettings settings(configFile, QSettings::NativeFormat);
    QSettings settings(configFile, QSettings::IniFormat);
    //
    // die Logeinstellungen sichern
    //
    if( !saveLogSettings( settings ) )
    {
      retVal = false;
    }
    if( ! saveAppSettings( settings ))
    {
      retVal = false;
    }
    return( retVal );
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
  QString AppConfigClass::getLogfileName(void) const
  {
    return logfileName;
  }

  /**
   * @brief AppConfigClass::setLogfileName Setze den Dateinamen des LOGFILES
   * @param value
   */
  void AppConfigClass::setLogfileName(const QString &value)
  {
    logfileName = value;
  }

  /**
   * @brief AppConfigClass::loadLogSettings Private Funktion zu m laden der Logeinstelungen
   * @param settings
   * @return
   */
  bool AppConfigClass::loadLogSettings(QSettings &settings)
  {
    bool retval = true;
    qDebug().noquote() << "AppConfigClass::loadLogSettings()";
    //
    // Öffne die Gruppe Logeinstellungen
    //
    settings.beginGroup(constLogGroupName);
    //
    // Lese den Dateinamen aus
    //
    logfileName = settings.value(constLogFileKey, AppConfigClass::constNoData).toString();
    if( QString::compare(logfileName, constNoData) == 0 )
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
    return( retval );
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
    settings.beginGroup(constLogGroupName);
    //
    // defaultwerte setzten
    //
    logfileName = QApplication::applicationName() + ".log";
    settings.setValue(constLogFileKey, logfileName );
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
  bool AppConfigClass::saveLogSettings(QSettings &settings)
  {
    qDebug().noquote().nospace() << "AppConfigClass::saveLogSettings() DATEI: <" + configFile + ">";
    //
    // Öffne die Gruppe Logeinstellungen
    //
    settings.beginGroup(constLogGroupName);
    //
    settings.setValue(constLogFileKey, logfileName );
    //
    // Ende der Gruppe
    //
    settings.endGroup();
    return( true );
  }


  /*###########################################################################
   ############################################################################
   #### Application settings                                               ####
   ############################################################################
  ###########################################################################*/

  int AppConfigClass::getWatchdogTime(void)
  {
    return( watchdogTimer );
  }
  void AppConfigClass::setLogThreshold( qint8 th )
  {
    logThreshold = th;
  }

  qint8 AppConfigClass::getLogTreshold(void)
  {
    return( logThreshold );
  }

  bool AppConfigClass::loadAppSettings( QSettings &settings)
  {
    bool retval = true;
    qDebug().noquote() << "AppConfigClass::loadAppSettings()";
    //
    // Öffne die Gruppe app
    //
    settings.beginGroup(constAppGroupName);
    //
    // Lese den Timeoutwert
    //
    watchdogTimer = settings.value(constAppTimeoutKey, 0).toInt();
    if( watchdogTimer == 0 )
    {
      // Nicht gefunden -> Fehler markieren
      retval = false;
    }
    qDebug().noquote().nospace() << "AppConfigClass::loadAppSettings(): watchdog: <" << watchdogTimer << ">";
    //
    // Lese die Loggerstufe
    //
    logThreshold = (qint8)(settings.value(constAppThresholdKey, 0).toInt() & 0xff);
    if( logThreshold == 0 )
    {
      // Nicht gefunden -> default DEBUG
      logThreshold = defaultAppThreshold;
    }
    qDebug().noquote().nospace() << "AppConfigClass::loadAppSettings(): logThreshold: <" << logThreshold << ">";
    //
    // Ende der Gruppe
    //
    settings.endGroup();
    //
    // Ergebnis kommunizieren
    //
    return( retval );
  }

  void AppConfigClass::makeAppDefaultSettings( QSettings &settings )
  {
    qDebug().noquote() << "AppConfigClass::makeAppDefaultSettings()";
    //
    // Öffne die Gruppe App
    //
    settings.beginGroup(constAppGroupName);
    //
    // defaultwerte setzten
    //
    watchdogTimer = AppConfigClass::defaultWatchdogTimerVal;
    settings.setValue(constAppTimeoutKey, watchdogTimer );
    logThreshold = defaultAppThreshold;
    settings.setValue(constAppThresholdKey, (int)logThreshold );
    //
    // Ende der Gruppe
    //
    settings.endGroup();
  }

  bool AppConfigClass::saveAppSettings(QSettings &settings)
  {
    //
    // Öffne die Gruppe Logeinstellungen
    //
    settings.beginGroup(constAppGroupName);
    //
    qDebug().noquote().nospace() << "AppConfigClass::saveAppSettings watchdog timer: <" << watchdogTimer << ">";
    settings.setValue(constAppTimeoutKey, watchdogTimer );
    qDebug().noquote().nospace() << "AppConfigClass::saveAppSettings threshold: <" << logThreshold << ">";
    settings.setValue(constAppThresholdKey, (int)logThreshold );
    //
    // Ende der Gruppe
    //
    settings.endGroup();
    return( true );
  }

  QString AppConfigClass::getBuildDate()
  {
    return constBuildDate;
  }

  QString AppConfigClass::getBuildNumStr()
  {
    return constBuildNumStr;
  }


}
