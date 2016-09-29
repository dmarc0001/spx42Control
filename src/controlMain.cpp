#include <QApplication>
#include <QTranslator>
#include <QLibraryInfo>
#include <QLocale>
#include <QDebug>
#include <QDir>

#include "Spx42ControlMainWin.hpp"

int main(int argc, char *argv[])
{
  QApplication app(argc, argv);
  //
  // Übersetzter...
  //
  QTranslator qtTranslator;
  QDir currDir = QDir::currentPath( );
  QString fileName = "spx42Control";
  QString prefix = "_";
  QString suffix = ".qm";
  QString destPath = currDir.absolutePath( ) + "/";


  qDebug( ) << "File: " << fileName;
  qDebug( ) << "Locale: " << QLocale::system( ).name( );
  qDebug( ) << "Path: " << currDir.absolutePath( );
  qDebug( ) << "searchFile: " << destPath + fileName + prefix + QLocale::system( ).name( ) + suffix;

  qDebug( ) << "Translation load: " << qtTranslator.load( QLocale::system( ).name( ), fileName, prefix, destPath, suffix );
  qDebug( ) << "Tanslator isEmpty: " << qtTranslator.isEmpty( );
  app.installTranslator( &qtTranslator );
  //
  // Hauptfenster erzeugen, Fenster zeigen
  //
  spx42::SPX42ControlMainWin w;
  //
  // Die Einstellugnen (für QT) auf Systemlocale setzen
  //
  w.setLocale( QLocale::system() );
  w.show();
  //
  // den eventloop starten
  //
  return app.exec();
}
