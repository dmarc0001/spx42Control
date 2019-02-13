#ifndef OPTIONSDIALOG_HPP
#define OPTIONSDIALOG_HPP

#include <QFileDialog>
#include <QObject>
#include "config/AppConfigClass.hpp"
#include "logging/Logger.hpp"
#include "ui_OptionsDialog.h"

namespace spx
{
  class OptionsDialog : public QDialog
  {
    private:
    Q_OBJECT
    //! Das GUI-Objekt
    std::unique_ptr< Ui::OptionsDialog > ui;
    //! Zeiger auf den Logger
    std::shared_ptr< Logger > lg;
    //! Referenz auf die Konfiguration
    AppConfigClass &cf;
    //! Log Schwelle / Loglevel
    qint8 logThreshold;
    //! datenbank dateipfad
    QString databaseFileDirName;
    //! logdaten dateipfad
    QString logfileDirName;
    //! Name des Theme (Dark, Light oder Custom)
    QString themeName;

    public:
    //! der Konstruktor
    explicit OptionsDialog( QWidget *parent, AppConfigClass &conf, std::shared_ptr< Logger > logger = Q_NULLPTR );
    //! initialisiere die GUI mit den Einstellungen aus der Konfig
    void init( void );
    //! die Funktion überschreiben um bei OK Daten zu beackern
    virtual void accept( void ) override;

    private slots:
    //! Datenbank Pfad einstellen
    void onSelectDatabasePushBottonSlot( void );
    //! Logdatei Pfad einstellen
    void onSelectLogfilePushBottonSlot( void );
  };
}  // namespace spx
#endif  // OPTIONSDIALOG_HPP
