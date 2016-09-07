
#include "Logger.hpp"
#include <QDateTime>
#include <QFileInfo>


namespace spx42
{
  const QString Logger::dateTimeFormat = "[yyyy-MM-dd hh:mm:ss.z] ";
  const QString Logger::DEBUG = "DEBUG ";
  const QString Logger::INFO  = "INFO  ";
  const QString Logger::WARN  = "WARN  ";
  const QString Logger::CRIT  = "CRIT  ";

  /**
   * @brief Logger::Logger Konstruktor mit Konfigurationsdatei Übergabe
   * @param lFile
   */
  Logger::Logger( const AppConfigClass *_config = nullptr  )
    : threshold(LG_DEBUG),logFile( nullptr ), textStream( nullptr) , configClass(_config)
  {
  }

  Logger::Logger() :
    threshold(LG_DEBUG),
    configClass( nullptr )
  {}

  Logger::~Logger()
  {
    shutdown();
  }

   /**
   * @brief Logger::startLogging begine Logging, öffne Logfile
   * @return
   */
  int Logger::startLogging(LgThreshold th)
  {
    threshold = th;
    //
    // gibt es eine Einstellung?
    //
    if( configClass != nullptr )
    {
      if( configClass->getLogfileName().length() > 4 )
      {
        // Super, das Logfile ist benannt!
        qDebug().noquote().nospace() << "START LOGGING...";
        logFile = new QFile( configClass->getLogfileName() );
        logFile->open(QIODevice::WriteOnly | QIODevice::Append);
        textStream = new QTextStream( logFile );
        *textStream << getDateString() << "START LOGGING" << endl;
        if( logFile != nullptr && logFile->isOpen() && textStream != nullptr )
        {
          qDebug().noquote().nospace() << "START LOGGING...OK";
          return( 1 );
        }
      }
    }
    //
    // Oh, da lief was falsch, LOGGEN nicht möglich
    //
    qDebug().noquote().nospace() << "START LOGGING...FAILED";
    return( 0 );
  }

  /**
   * @brief Logger::setThreshold
   * @param th
   */
  void Logger::setThreshold(LgThreshold th)
  {
    threshold = th;
  }

  /**
   * @brief Logger::getThreshold
   * @return
   */
  LgThreshold Logger::getThreshold(void)
  {
    return( threshold );
  }

  /**
   * @brief Logger::warn Ausgabe(n) für WARNUNG
   * @param msg
   */
  void Logger::warn( const QString& msg )
  {
    qWarning().noquote().nospace() << msg;
    if( textStream != nullptr && threshold >= LG_WARN )
    {
      *textStream << getDateString() << WARN << msg << endl;
    }
  }

  void Logger::warn( const char *msg )
  {
    qWarning().noquote().nospace() << msg;
    if( textStream != nullptr && threshold >= LG_WARN )
    {
      *textStream << getDateString() << WARN << msg << endl;
    }
  }

  void Logger::warn( const std::string& msg )
  {
    qWarning().noquote().nospace() << msg.c_str();
    if( textStream != nullptr && threshold >= LG_WARN )
    {
      *textStream << getDateString() << WARN << msg.c_str() << endl;
    }
  }

  /**
   * @brief Logger::info Ausgabe(n) für Info
   * @param msg
   */
  void Logger::info( const QString& msg )
  {
    qInfo().noquote().nospace() << msg;
    if( textStream != nullptr && threshold >= LG_INFO )
    {
      *textStream << getDateString() << INFO << msg << endl;
    }
  }

  void Logger::info( const char *msg )
  {
    qInfo().noquote().nospace() << msg;
    if( textStream != nullptr && threshold >= LG_INFO )
    {
      *textStream << getDateString() << INFO << msg << endl;
    }
  }
  void Logger::info( const std::string& msg )
  {
    qInfo().noquote().nospace() << msg.c_str();
    if( textStream != nullptr && threshold >= LG_INFO )
    {
      *textStream << getDateString() << INFO << msg.c_str() << endl;
    }
  }

  /**
   * @brief Logger::debug Ausgaben für Debugging
   * @param msg
   */
  void Logger::debug( const QString& msg )
  {
      qDebug().noquote().nospace() << msg;
      if( textStream != nullptr && threshold >= LG_DEBUG )
      {
        *textStream << getDateString() << DEBUG << msg << endl;
      }
  }

  void Logger::debug( const char *msg )
  {
    qDebug().noquote().nospace() << msg;
    if( textStream != nullptr && threshold >= LG_DEBUG )
    {
      *textStream << getDateString() << DEBUG << msg << endl;
    }
  }

  void Logger::debug( const std::string& msg )
  {
    qDebug().noquote().nospace() << msg.c_str();
    if( textStream != nullptr && threshold >= LG_DEBUG )
    {
      *textStream << getDateString() << DEBUG << msg.c_str() << endl;
    }
  }

  /**
   * @brief Logger::crit Ausgaben für Kritische Fehler
   * @param msg
   */
  void Logger::crit( const QString& msg )
  {
      qCritical().noquote().nospace() << msg;
      if( textStream != nullptr && threshold >= LG_DEBUG )
      {
        *textStream << getDateString() << CRIT << msg << endl;
      }
  }

  void Logger::crit( const char *msg )
  {
      qCritical().noquote().nospace() << msg;
      if( textStream != nullptr && threshold >= LG_DEBUG )
      {
        *textStream << getDateString() << CRIT << msg << endl;
      }
  }

  void Logger::crit( const std::string& msg )
  {
      qCritical().noquote().nospace() << msg.c_str();
      if( textStream != nullptr && threshold >= LG_DEBUG )
      {
        *textStream << getDateString() << CRIT << msg.c_str() << endl;
      }
  }

  void Logger::shutdown()
  {
    if( textStream != nullptr )
    {
      textStream->flush();
      delete(textStream);
      textStream = nullptr;
    }
    if( logFile != nullptr )
    {
      logFile->flush();
      logFile->close();
      delete( logFile );
      logFile = nullptr;
    }
  }


  QString Logger::getDateString()
  {
    dateTime = QDateTime::currentDateTime();
    return( dateTime.toString(dateTimeFormat) );
  }
} /* namespace spx */
