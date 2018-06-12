#include <QDebug>
#include "MainObject.hpp"

int main( int argc, char *argv[] )
{
  qDebug() << "main.cpp: start...";
  MainObject m( argc, argv );
  return m.exec();
  qDebug() << "main.cpp: end...";
}
