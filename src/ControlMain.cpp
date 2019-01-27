
#include "ControlMain.hpp"

int main( int argc, char *argv[] )
{
  QApplication app( argc, argv );
  //
  // Übersetzter...
  //
  QTranslator qtTranslator;
  QDir currDir = QDir::currentPath();
  QString fileName = "spx42Control";
  QString prefix = "_";
  QString suffix = ".qm";
  QString destPath = currDir.absolutePath().append( "/" );
  QString destPath2 = QStandardPaths::writableLocation( QStandardPaths::AppDataLocation ).append( "/" );
  qDebug() << "File: " << fileName;
  qDebug() << "Locale: " << QLocale::system().name();
  qDebug() << "Path: " << currDir.absolutePath();
  qDebug() << "searchFile: " << destPath + fileName + prefix + QLocale::system().name() + suffix;
  qDebug() << "searchFile: " << destPath2 + fileName + prefix + QLocale::system().name() + suffix;
  qDebug() << "Translation load: " << qtTranslator.load( QLocale::system().name(), fileName, prefix, destPath, suffix );
  qDebug() << "Tanslator isEmpty: " << qtTranslator.isEmpty();
  QApplication::installTranslator( &qtTranslator );
  //
  // Stylesheet laden, wenn vorhanden
  //
  suffix = ".css";
  qDebug().nospace().noquote() << "File: " << fileName << suffix;
  qDebug() << "Path: " << destPath;
  qDebug().nospace() << "searchFile: " << destPath.append( fileName ).append( suffix );
  if ( readStylesheetFromFile( &app, destPath ) )
  {
    qDebug() << "Stylesheet correct loadet...";
  }
  else
  {
    if ( readStylesheetFromFile( &app, destPath2 ) )
    {
      qDebug() << "Stylesheet correct loadet...";
    }
    else
    {
      qWarning() << "Can't load external stylesheet, try internal...";
#ifdef UNIX
      destPath = ":/style/defaultStyleUx.css";
#else
#ifdef TARGET_OS_MAC
      destPath = ":/style/defaultStyleMac.css";
#else
      destPath = ":/style/defaultStyle.css";
#endif
#endif
    }
    if ( readStylesheetFromFile( &app, destPath ) )
    {
      qDebug() << "internal Stylesheet correct loadet...";
    }
    else
    {
      qWarning() << "Can't load internal stylesheet, work without it...";
    }
  }
  //
  // Hauptfenster erzeugen, Fenster zeigen
  //
  spx::SPX42ControlMainWin w;
  //
  // Die Einstellugnen (für QT) auf Systemlocale setzen
  //
  w.setLocale( QLocale::system() );
  w.show();
  //
  // den eventloop starten
  //
  int retcode = QApplication::exec();
  qDebug() << "app ends with returncode <" << retcode << ">";
  return ( retcode );
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
