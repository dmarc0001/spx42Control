/*
 * logger.hpp
 *
 *  Created on: 30.05.2016
 *      Author: dmarc
 */

#ifndef SRC_LOGGING_LOGGER_HPP_
#define SRC_LOGGING_LOGGER_HPP_
#include <QtGlobal>
#include <QString>
#include <QFile>
#include <QTextStream>
#include <QDateTime>
#include <QDebug>
#include <string>

#include "../config/AppConfigClass.hpp"

namespace spx42
{
  enum LgThreshold : quint8 { LG_NONE=0,LG_CRIT,LG_WARN,LG_INFO,LG_DEBUG };

  class Logger
  {
    private:
      LgThreshold threshold;                                   //! Logstatus
      QFile *logFile;                                           //! Zeiger auf das Logdateiobjekt
      QTextStream *textStream;                                  //! Zeiger auf einen Textstrom
      QDateTime dateTime;                                       //! das lokale Datum/Zeit objekt
      const AppConfigClass *configClass;                        //! Zeiger auf die Konfiguration
      static const QString dateTimeFormat;                      //! Format der Zeitausgabe
      static const QString DEBUG;                               //! String für Debuglevel
      static const QString INFO;                                //! String für Infolevel
      static const QString WARN;                                //! String für Warnlevel
      static const QString CRIT;                                //! String für Kritischen level

    public:
      Logger( const AppConfigClass *_config );                  //! Konstruktor mit Zeiger auf das Konfig-Objekt
      virtual ~Logger();                                        //! Destruktor
      int startLogging(LgThreshold th = LG_DEBUG);              //! Loggen beginnen
      void setThreshold(LgThreshold th);                        //! Setzte Loggingstufe
      LgThreshold getThreshold(void);                           //! lese Loggingstufe
      //
      void info( const QString& msg );                          //! INFO Ausgaben
      void info( const std::string& msg );                      //! INFO Ausgaben
      void info( const char *msg );                             //! INFO Ausgaben
      //
      void debug( const QString& msg );                         //! DEBUG Ausgaben
      void debug( const std::string& msg );                     //! DEBUG Ausgaben
      void debug( const char *msg );                            //! DEBUG Ausgaben
      //
      void warn( const QString& msg );                          //! WARN Ausgaben
      void warn( const std::string& msg );                      //! WARN Ausgaben
      void warn( const char *msg );                             //! WARN Ausgaben
      //
      void crit( const QString& msg );                          //! CRIT Ausgaben
      void crit( const std::string& msg );                      //! CRIT Ausgaben
      void crit( const char *msg );                             //! CRIT Ausgaben
      //
      void shutdown();                                          //! Logger explizit herunterfahren

    private:
      Logger();                                                 //! den Konstruktor verbergen/sperren
      QString getDateString();                                  //! interne Funktion für den Datumsstring
  };

} /* namespace vd */

#endif /* SRC_LOGGING_LOGGER_HPP_ */
