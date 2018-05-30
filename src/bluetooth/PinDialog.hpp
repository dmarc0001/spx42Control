#ifndef PINDIALOG_HPP
#define PINDIALOG_HPP

#include <QRegExp>
#include <QDialogButtonBox>

#include "ui_PinDialog.h"

namespace spx
{
  class PinDialog : public QDialog
  {
    private:
      Q_OBJECT
      QString pin;                                           //! Speicher für die PIN
      const QRegExp ex;                                      //! Ausdruck, der die PIN beschreibt
      QPalette palette_norm;                                 //! Normale Palette des Labels
      QPalette palette_warn;                                 //! Palete für ROT

    public:
      explicit PinDialog(QWidget *parent = 0);               //! Konstruktor
      PinDialog(QWidget *parent, const QString &oldPin );    //! Konstruktor mit oldPin
      virtual void done(int res);                            //! wenn Box geschlosen werden soll
      QByteArray getPin(void);                               //! Pin zurück, wenn formal gültig
      void showRetryButton( bool showButton = true );        //! Soll der RETRY Button gezeigt werden?

    private:
      Ui::PinDialog ui;                                      //! GUI Objekt

    private slots:
      void pinEdited(const QString &text);                   //! Wenn die Eingabe bearbeitet wird
      void retryButtonSlot( void );                          //! Wenn RETRY gefordert wurde
  };
}
#endif // PINDIALOG_HPP
