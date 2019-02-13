#ifndef CONTROLMAIN_HPP
#define CONTROLMAIN_HPP

#include <QApplication>
#include <QDebug>
#include <QDir>
#include <QLibraryInfo>
#include <QLocale>
#include <QRegularExpression>
#include <QTranslator>
#include <memory>
#include "config/AppConfigClass.hpp"
#include "logging/Logger.hpp"

#include "SPX42ControlMainWin.hpp"

int main( int argc, char *argv[] );

bool readStylesheetFromFile( QApplication *app, QString &file );
bool readStylesheetFromFile( QMainWindow *app, QString &file );

#endif  // CONTROLMAIN_HPP
