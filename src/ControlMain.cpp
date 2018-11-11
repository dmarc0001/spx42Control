
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
  QString destPath = currDir.absolutePath() + "/";
  qDebug() << "File: " << fileName;
  qDebug() << "Locale: " << QLocale::system().name();
  qDebug() << "Path: " << currDir.absolutePath();
  qDebug() << "searchFile: " << destPath + fileName + prefix + QLocale::system().name() + suffix;
  qDebug() << "Translation load: " << qtTranslator.load( QLocale::system().name(), fileName, prefix, destPath, suffix );
  qDebug() << "Tanslator isEmpty: " << qtTranslator.isEmpty();
  app.installTranslator( &qtTranslator );
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
    qWarning() << "Can't load external stylesheet, try internal...";
    destPath = ":/style/defaultStyle.css";
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
  int retcode = app.exec();
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
