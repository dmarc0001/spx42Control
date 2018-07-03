#include <QtCore/QCoreApplication>
#include "ConsoleMainObject.hpp"

int main( int argc, char *argv[] )
{
  //
  // Core Applikation für event loop
  //
  QCoreApplication a( argc, argv );
  //
  // mein Hauptobjekt machen
  //
  spx::ConsoleMainObject mainObject( &a );
  //
  // Ende Signal verbinden
  //
  QObject::connect( &mainObject, &spx::ConsoleMainObject::sigQuit, &a, &QCoreApplication::quit );
  //
  // initialisieren
  //
  mainObject.init();
  //
  // event loop
  //
  return ( a.exec() );
}
