#include <QApplication>
#include "MainDialog.hpp"

/**
 * @brief start der anwendung
 * @param argc
 * @param argv
 * @return
 */
int main( int argc, char *argv[] )
{
  QApplication a( argc, argv );
  MainDialog w;
  QObject::connect( &w, &MainDialog::accepted, &a, &QApplication::quit );
  w.show();
  return a.exec();
}
