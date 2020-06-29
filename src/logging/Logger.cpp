#include "Logger.hpp"

#include <QDateTime>
#include <QFileInfo>

namespace spx
{
  const QString Logger::dateTimeFormat{"[yyyy-MM-dd hh:mm:ss.z] "};
  const QString Logger::NONE_STR{"NONE "};
  const QString Logger::DEBUG_STR{"DEBUG "};
  const QString Logger::INFO_STR{"INFO  "};
  const QString Logger::WARN_STR{"WARN  "};
  const QString Logger::CRIT_STR{"CRIT  "};

  //#######################################################################################
  //#### Spezialfälle des << Operators, die nicht per template gehen                   ####
  //#######################################################################################

  /**
   * @brief operator <<
   * @param lg
   * @param th
   * @return
   */
  Logger &operator<<( Logger &lg, const LgThreshold th )
  {
    lg.setCurrentThreshold( th );
    return lg;
  }

  /**
   * @brief operator <<
   * @param lg
   * @param endl
   * @return
   */
  Logger &operator<<( Logger &lg, QTextStream &( QTextStream &s ) )
  {
    lg.lineEnd();
    return lg;
  }
  // Logger &operator<<( Logger &lg, QTextStream &endl( QTextStream &s ) )

  //#######################################################################################
  //#### der Logger                                                                    ####
  //#######################################################################################

  /**
   * @brief Logger::Logger Konstruktor mit Konfigurationsdatei Übergabe
   * @param lFile
   */
  Logger::Logger() : threshold( LG_DEBUG ), currentThreshold( LG_DEBUG ), logToConsole( false ), logFile( nullptr )
  {
  }

  /**
   * @brief Logger::~Logger
   */
  Logger::~Logger()
  {
    qDebug().noquote().nospace() << "SHUTDOWN LOGGING...";
    // shutdown();
  }

  /**
   * @brief Logger::startLogging
   * @param th
   * @param fileName
   * @return
   */
  int Logger::startLogging( LgThreshold th, const QString &fileName, bool consoleLog )
  {
    threshold = th;
    bool consoleOk = true;
    logToConsole = consoleLog;
    consoleStream = std::unique_ptr< QTextStream >( new QTextStream( stdout ) );
    if ( !consoleStream || consoleStream->status() != QTextStream::Ok )
    {
      //
      // das geht nicht auf der Konsole!
      // Loggen in Datei wäre noch machbar
      //
      qCritical() << "can't open log stream to console!";
      logToConsole = false;
      consoleOk = false;
    }
    //
    // gibt es einen Lognamen
    //
    if ( fileName != nullptr )
    {
      if ( fileName.length() > 4 )
      {
        logFile = std::unique_ptr< QFile >( new QFile( fileName ) );
        logFile->open( QIODevice::WriteOnly | QIODevice::Append );
        textStream = std::unique_ptr< QTextStream >( new QTextStream( logFile.get() ) );
        if ( logFile && logFile->isOpen() && textStream && consoleStream )
        {
          if ( consoleOk )
            *consoleStream << "START CONSOLE LOGGING (" << fileName << "), console logging: " << consoleLog << Qt::endl;
          *textStream << "START FILE LOGGING (" << fileName << ")" << Qt::endl;
          return ( 1 );
        }
      }
    }
    else
    {
      //
      // es gib keinen Namen fürs Logging,also keine Logdatei
      //
      qCritical() << "there is no log file name, no file logging!";
      //
      // Konsole auch nicht?
      //
      if ( !consoleOk )
      {
        qCritical() << "FAIL! No log to console an no file logging possible!";
        return 0;
      }
      else
      {
        logToConsole = true;
      }
    }
    //
    // Oh, da lief was falsch, LOGGEN nicht möglich
    //
    qDebug().noquote().nospace() << "START LOGGING...FAILED";
    return ( 0 );
  }

  /**
   * @brief Logger::lineEnd
   */
  void Logger::lineEnd()
  {
    //
    // nur innerhalb der logstufen loggen
    //
    if ( threshold >= currentThreshold )
    {
      //
      // Zeilenende ausgeben und Ausgabe flushen
      //
      *textStream << Qt::endl;
      textStream->flush();
      //
      // ist log zur Konsole aktiv, auch da neue Zeile
      //
      if ( logToConsole )
      {
        *consoleStream << Qt::endl;
      }
      wasNewline = true;
    }
  }

  /**
   * @brief Logger::setThreshold
   * @param th
   */
  void Logger::setThreshold( LgThreshold th )
  {
    threshold = th;
  }

  /**
   * @brief Logger::getThreshold
   * @return
   */
  LgThreshold Logger::getThreshold()
  {
    return ( threshold );
  }

  /**
   * @brief Logger::shutdown
   */
  void Logger::shutdown()
  {
    //
    // Logen stoppen
    //
    if ( textStream )
    {
      textStream->flush();
    }
    if ( consoleStream )
    {
      consoleStream->flush();
    }
    //
    // Das logfile dann auch schliessen
    //
    if ( logFile != nullptr )
    {
      logFile->flush();
      logFile->close();
    }
  }

  /**
   * @brief Logger::getDateString
   * @return
   */
  QString Logger::getDateString()
  {
    dateTime = QDateTime::currentDateTime();
    return ( dateTime.toString( dateTimeFormat ) );
  }

  /**
   * @brief Logger::getThresholdString
   * @param th
   * @return
   */
  const QString Logger::getThresholdString( LgThreshold th )
  {
    switch ( th )
    {
      case LG_NONE:
        return Logger::NONE_STR;
      case LG_CRIT:
        return Logger::CRIT_STR.trimmed();
      case LG_WARN:
        return Logger::WARN_STR.trimmed();
      case LG_INFO:
        return Logger::INFO_STR.trimmed();
      case LG_DEBUG:
        return Logger::DEBUG_STR.trimmed();
    }
    return QString( "unknown" );
  }

  /**
   * @brief Logger::getThresholdFromString
   * @param th
   * @return
   */
  LgThreshold Logger::getThresholdFromString( const QString &th )
  {
    if ( th.toUpper().compare( Logger::NONE_STR.trimmed() ) == 0 )
      return ( LG_NONE );
    else if ( th.toUpper().compare( Logger::CRIT_STR.trimmed() ) == 0 )
      return ( LG_CRIT );
    else if ( th.toUpper().compare( Logger::WARN_STR.trimmed() ) == 0 )
      return ( LG_WARN );
    else if ( th.toUpper().compare( Logger::INFO_STR.trimmed() ) == 0 )
      return ( LG_INFO );
    else if ( th.toUpper().compare( Logger::DEBUG_STR.trimmed() ) == 0 )
      return ( LG_DEBUG );
    return ( LG_DEBUG );
  }

}  // namespace spx
