#ifndef SRC_LOGGING_LOGGER_HPP_
#define SRC_LOGGING_LOGGER_HPP_
#include <QDateTime>
#include <QDebug>
#include <QFile>
#include <QMutex>
#include <QMutexLocker>
#include <QString>
#include <QTextStream>
#include <QtGlobal>
#include <memory>
#include <string>

namespace spx
{
  enum LgThreshold : quint8
  {
    LG_NONE = 0,
    LG_CRIT,  // 1
    LG_WARN,  // 2
    LG_INFO,  // 3
    LG_DEBUG  // 4
  };
  // um das schreiben abzukürzen
  constexpr LgThreshold LNONE = LgThreshold::LG_NONE;
  constexpr LgThreshold LDEBUG = LgThreshold::LG_DEBUG;
  constexpr LgThreshold LINFO = LgThreshold::LG_INFO;
  constexpr LgThreshold LWARN = LgThreshold::LG_WARN;
  constexpr LgThreshold LCRIT = LgThreshold::LG_CRIT;
  //

  class Logger
  {
    private:
    //! Logs level
    LgThreshold threshold;
    LgThreshold currentThreshold;
    bool logToConsole;
    bool wasNewline{true};
    //! Zeiger auf das Logdateiobjekt
    std::unique_ptr< QFile > logFile;
    //! Zeiger auf einen Textstrom
    std::unique_ptr< QTextStream > textStream;
    std::unique_ptr< QTextStream > consoleStream;
    //! das lokale Datum/Zeit objekt
    QDateTime dateTime;
    //! Mutex zum locken der Queue
    QMutex logMutex;
    //! Format der Zeitausgabe
    static const QString dateTimeFormat;
    //! String für keinen level
    static const QString NONE_STR;
    //! String für Debuglevel
    static const QString DEBUG_STR;
    //! String für Infolevel
    static const QString INFO_STR;
    //! String für Warnlevel
    static const QString WARN_STR;
    //! String für Kritischen level
    static const QString CRIT_STR;

    public:
    explicit Logger();
    // Logger( const Logger &lg );
    virtual ~Logger();  //! Destruktor
    int startLogging( LgThreshold th = LG_DEBUG, const QString &fn = "logging.log",
                      bool consoleLog = false );       //! Loggen beginnen
    void setThreshold( LgThreshold th );               //! Setzte Loggingstufe
    inline void setCurrentThreshold( LgThreshold th )  //! für << flag setzten
    {
      currentThreshold = th;
    };
    inline LgThreshold getCurrentThreshold()
    {
      return currentThreshold;
    }
    LgThreshold getThreshold( void );  //! lese Loggingstufe

    //
    // die Ausgaben als templates machen
    //
    template < typename T >
    void debug_op( const T msg )
    {
      //! Serialisieren...
      QMutexLocker locker( &logMutex );
      // qDebug() << "THRESHOLD: " << threshold << " CURRENT: " << currentThreshold;
      if ( threshold >= currentThreshold )
      {
        if ( wasNewline )
        {
          *textStream << getDateString() << DEBUG_STR << msg;
          if ( logToConsole )
            *consoleStream << DEBUG_STR << msg;
          wasNewline = false;
        }
        else
        {
          *textStream << msg;
          if ( logToConsole )
            *consoleStream << msg;
        }
      }
    }

    template < typename T >
    void info_op( const T msg )
    {
      //! Serialisieren...
      QMutexLocker locker( &logMutex );
      if ( threshold >= currentThreshold )
      {
        if ( wasNewline )
        {
          *textStream << getDateString() << INFO_STR << msg;
          if ( logToConsole )
            *consoleStream << INFO_STR << msg;
          wasNewline = false;
        }
        else
        {
          *textStream << msg;
          if ( logToConsole )
            *consoleStream << msg;
        }
      }
    }

    template < typename T >
    void warn_op( const T msg )
    {
      //! Serialisieren...
      QMutexLocker locker( &logMutex );
      if ( threshold >= currentThreshold )
      {
        if ( wasNewline )
        {
          *textStream << getDateString() << WARN_STR << msg;
          if ( logToConsole )
            *consoleStream << WARN_STR << msg;
          wasNewline = false;
        }
        else
        {
          *textStream << msg;
          if ( logToConsole )
            *consoleStream << msg;
        }
      }
    }

    template < typename T >
    void crit_op( const T msg )
    {
      //! Serialisieren...
      QMutexLocker locker( &logMutex );
      if ( threshold >= currentThreshold )
      {
        if ( wasNewline )
        {
          *textStream << getDateString() << CRIT_STR << msg;
          if ( logToConsole )
            *consoleStream << CRIT_STR << msg;
          wasNewline = false;
        }
        else
        {
          *textStream << msg;
          if ( logToConsole )
            *consoleStream << msg;
        }
      }
    }

    //! der Spezialfall
    friend Logger &operator<<( Logger &lg, const LgThreshold th );
    friend Logger &operator<<( Logger &lg, QTextStream &endl( QTextStream &s ) );
    //

    template < typename T >
    friend Logger &operator<<( Logger &lg, const T msg )
    {
      switch ( lg.getCurrentThreshold() )
      {
        case LNONE:
          break;
        case LDEBUG:
          lg.debug_op( msg );
          break;
        case LINFO:
          lg.info_op( msg );
          break;
        case LWARN:
          lg.warn_op( msg );
          break;
        case LCRIT:
          lg.crit_op( msg );
          break;
      }
      return lg;
    }
    void lineEnd();
    //! Logger explizit herunterfahren
    void shutdown();
    //! string zum level erfahren
    static const QString getThresholdString( LgThreshold th );
    static LgThreshold getThresholdFromString( const QString &th );

    private:
    QString getDateString();  //! interne Funktion für den Datumsstring
  };
}  // namespace spx
#endif /* SRC_LOGGING_LOGGER_HPP_ */
