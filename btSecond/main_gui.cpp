#include <QApplication>
#include <QStandardPaths>
#include "BtDiscoverDialog.hpp"

using namespace spx;

/**
 * @brief start der anwendung
 * @param argc
 * @param argv
 * @return
 */
int main( int argc, char *argv[] )
{
  QApplication a( argc, argv );
  QString stdPath = QStandardPaths::writableLocation( QStandardPaths::AppDataLocation );
  BtDiscoverDialog w;
  w.debugSetDatabase( stdPath );
  QObject::connect( &w, &BtDiscoverDialog::accepted, &a, &QApplication::quit );
  w.show();
  return a.exec();
}
