#ifndef MAINOBJECT_HPP
#define MAINOBJECT_HPP

#include <QCoreApplication>

class MainObject : public QCoreApplication
{
  public:
  explicit MainObject( int argc, char *argv[] );
  ~MainObject();
};

#endif  // MAINOBJECT_HPP
