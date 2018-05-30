#ifndef DEVICEEDITDIALOG_HPP
#define DEVICEEDITDIALOG_HPP

#include <QObject>
#include <QWidget>
#include <QRegExp>

#include "BtDeviceDescriber.hpp"
#include "ui_DeviceEditDialog.h"

namespace spx
{
  class DeviceEditDialog : public QDialog
  {
    private:
      Q_OBJECT
      BluetoothDeviceDescriber &currentDevice;               //! Referenz auf current Device
      const QRegExp ex;                                      //! Ausdruck, der die PIN beschreibt
      QPalette palette_norm;                                 //! Normale Palette des Labels
      QPalette palette_warn;                                 //! Palete für ROT
      BluetoothDeviceDescriber dummyRef;                     //! Dummy für Konstruktor ohne Ref

    public:
      DeviceEditDialog();
      explicit DeviceEditDialog(QWidget *parent = 0);        //! Konstruktor
      DeviceEditDialog(QWidget *parent, BluetoothDeviceDescriber& cDevice );//! Konstruktor
      virtual void done(int res);                            //! wenn Box geschlosen werden soll

    private:
      Ui::DeviceEditDialog ui;                               //! GUI Objekt

    private slots:
      void pinEdited(const QString &text);                   //! Wenn die Eingabe bearbeitet wird
  };
}
#endif // DEVICEEDITDIALOG_HPP
