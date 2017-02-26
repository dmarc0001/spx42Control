#ifndef CONTROLMAIN_HPP
#define CONTROLMAIN_HPP

#include <QApplication>
#include <QTranslator>
#include <QLibraryInfo>
#include <QLocale>
#include <QDebug>
#include <QDir>
#include <QRegularExpression>

#include "Spx42ControlMainWin.hpp"

int main(int argc, char *argv[]);

bool readStylesheetFromFile(QApplication *app, QString& file );

#endif // CONTROLMAIN_HPP
