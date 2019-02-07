
#include "ControlMain.hpp"

int main( int argc, char *argv[] )
{
  // spx::AppConfigClass cf;
  QString baseName( "spx42Control" );
  QString sytemType( "unknown" );
  QString themeName( "Light" );
#ifdef WIN32
  sytemType = "Win";
#endif
#ifdef MACOS
  sytemType = "Mac";
#endif
#ifdef LINUX
  sytemType = "Lin";
#endif
  QApplication app( argc, argv );
  std::shared_ptr< spx::Logger > lg;  // das hier auch speichern damit das Objekt als letztes gelöscht wird
  //
  // Übersetzter...
  //
  QTranslator qtTranslator;
  QDir currDir = QDir::currentPath();
  QString fileName( baseName );
  QString prefix = "_";
  QString suffix = ".qm";
  QString destPath = currDir.absolutePath().append( "/" );
  QString destPath2 = QStandardPaths::writableLocation( QStandardPaths::AppDataLocation ).append( "/" );
  qDebug() << "App base name: " << fileName;
  qDebug() << "App locale: " << QLocale::system().name();
  qDebug() << "App current Path: " << currDir.absolutePath();
  qDebug() << "search files #1: " << destPath + fileName + prefix + QLocale::system().name() + suffix;
  qDebug() << "search files #2: " << destPath2 + fileName + prefix + QLocale::system().name() + suffix;
  qDebug() << "Translation load: " << qtTranslator.load( QLocale::system().name(), fileName, prefix, destPath, suffix );
  qDebug() << "Tanslator isEmpty: " << qtTranslator.isEmpty();
  QApplication::installTranslator( &qtTranslator );
  //
  // Hauptfenster erzeugen
  //
  spx::SPX42ControlMainWin w;
  w.setLocale( QLocale::system() );
  spx::AppConfigClass &cf = w.getConfig();
  themeName = cf.getGuiThemeName();
  //
  // Stylesheet laden, wenn vorhanden
  //
  QString styleSheetNameTemplate = "%1%2%3.css";
  // Der Name ist:
  QString styleSheetFile = styleSheetNameTemplate.arg( baseName ).arg( sytemType ).arg( themeName );

  qDebug().nospace().noquote() << "style sheet filename: " << styleSheetFile;
  qDebug().nospace() << "search style sheet at: " << destPath.append( styleSheetFile );
  // if ( readStylesheetFromFile( &app, destPath ) )
  if ( readStylesheetFromFile( &w, destPath ) )
  {
    qDebug() << "Stylesheet correct loadet...";
  }
  else
  {
    // if ( readStylesheetFromFile( &app, destPath2 ) )
    if ( readStylesheetFromFile( &w, destPath2 ) )
    {
      qDebug() << "Stylesheet correct loadet...";
    }
    else
    {
      qWarning() << "Can't load external stylesheet, try internal...";
      destPath = QString( ":/style/" ).append( styleSheetFile );
    }
    // if ( readStylesheetFromFile( &app, destPath ) )
    if ( readStylesheetFromFile( &w, destPath ) )
    {
      qDebug() << "internal Stylesheet correct loadet...";
    }
    else
    {
      qWarning() << "Can't load internal stylesheet, work without it...";
    }
  }
  //
  // und nun den logger (zeiger) holen
  // damit wird der logger NACH w gelöscht, es gibt keinen Absturz
  //
  lg = w.getLogger();
  //
  // Die Einstellugnen (für QT) auf Systemlocale setzen
  //
  w.setLocale( QLocale::system() );
  w.show();
  //
  // den eventloop starten
  //
  int retcode = QApplication::exec();
  lg->debug( "ControlMain -> app ends..." );
  qDebug() << "app ends with returncode <" << retcode << ">";
  return ( retcode );
}

bool readStylesheetFromFile( QMainWindow *app, QString &file )
{
  QFile styleFile( file );
  //
  if ( !styleFile.open( QIODevice::ReadOnly ) )
  {
    qDebug() << "Cant open stylesheet readonly: " << styleFile.errorString();
    return ( false );
  }
  //
  // die Datei einlesen
  //
  QString line = styleFile.readAll();
  // Zeilenvorschübe entfernen (global)
  line.replace( QRegularExpression( "(\n|\r)" ), " " );
  // Kommentare entfernen (globel)
  line.remove( QRegularExpression( "/\\*.*?\\*/" ) );
  app->setStyleSheet( line );
  return ( true );
}

bool readStylesheetFromFile( QApplication *app, QString &file )
{
  QFile styleFile( file );
  //
  if ( !styleFile.open( QIODevice::ReadOnly ) )
  {
    qDebug() << "Cant open stylesheet readonly: " << styleFile.errorString();
    return ( false );
  }
  //
  // die Datei einlesen
  //
  QString line = styleFile.readAll();
  // Zeilenvorschübe entfernen (global)
  line.replace( QRegularExpression( "(\n|\r)" ), " " );
  // Kommentare entfernen (globel)
  line.remove( QRegularExpression( "/\\*.*?\\*/" ) );
  app->setStyleSheet( line );
  return ( true );
}
