#include "HelpDialog.hpp"
#include "ui_HelpDialog.h"

namespace spx
{
  HelpDialog::HelpDialog( ApplicationTab tabIdx, QWidget *parent, std::shared_ptr< Logger > logger )
      : QDialog( parent ), ui( new Ui::HelpDialog() ), lg( logger ), currentTab( tabIdx )
  {
    ui->setupUi( this );
    // setFixedSize( this->width(), this->height() );
    //
    // Hilfebutton ausblenden
    //
    this->setWindowFlags( this->windowFlags() & ~Qt::WindowContextHelpButtonHint );
    //
    // den Browser auf den richtrigen Abschnitt
    // und das Sprachenabhängig
    //
    // QString url = tr( "qrc:/help/help_en.html" );
    QWebEngineView *view = new QWebEngineView( parent );
    // QString localFileName = tr( "%1/helpsystem/en/help.html#mark_%2" )
    //                            .arg( QDir::currentPath() )
    //                            .arg( static_cast< int >( currentTab ), 2, 10, QChar( '0' ) );

    QString localFileName = tr( "%1/helpsystem/en/help.html" ).arg( QDir::currentPath() );
    QUrl url = QUrl::fromLocalFile( localFileName );
    view->load( url );
    //
    // TODO: zur Marke springen
    //
    view->page()->runJavaScript(
        QString( "location.hash = \"#mark_%1\";" ).arg( static_cast< int >( currentTab ), 2, 10, QChar( '0' ) ) );
    lg->debug( QString( "HelpDialog::HelpDialog -> open url: <%1>" ).arg( url.toString() ) );
    //
    // Widget tauschen
    //
    QLayoutItem *old = this->layout()->replaceWidget( ui->helpPlainTextEdit, view, Qt::FindChildrenRecursively );
    delete old;
    // aktualisieren
    this->update();
  }

  HelpDialog::~HelpDialog()
  {
  }

  void HelpDialog::changeEvent( QEvent *e )
  {
    QDialog::changeEvent( e );
    switch ( e->type() )
    {
      case QEvent::LanguageChange:
        ui->retranslateUi( this );
        break;
      default:
        break;
    }
  }
}  // namespace spx
