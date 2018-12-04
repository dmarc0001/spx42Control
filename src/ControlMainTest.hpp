#ifndef CONTROLMAIN_HPP
#define CONTROLMAIN_HPP

#include <QApplication>
#include <QDebug>
#include <QDir>
#include <QLibraryInfo>
#include <QLocale>
#include <QRegularExpression>
#include <QTranslator>

#include "SPX42ControlMainWin.hpp"

int main( int argc, char *argv[] );

bool readStylesheetFromFile( QApplication *app, QString &file );

#endif  // CONTROLMAIN_HPP
