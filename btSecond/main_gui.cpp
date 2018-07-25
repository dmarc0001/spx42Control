#include <QApplication>
#include "BtDiscoveringDialog.hpp"

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
  BtDiscoveringDialog w;
  QObject::connect( &w, &BtDiscoveringDialog::accepted, &a, &QApplication::quit );
  w.show();
  return a.exec();
}
