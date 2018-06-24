#include <QtCore/QCoreApplication>
#include "ConsoleMainObject.hpp"

int main( int argc, char *argv[] )
{
  QCoreApplication a( argc, argv );
  spx::ConsoleMainObject mainObject( &a );
  return ( mainObject.execute( &a ) );
}
