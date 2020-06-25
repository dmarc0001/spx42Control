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
    QWebEngineView *view = new QWebEngineView( parent );
    QString localFileName = tr( "%1/helpsystem/en/help.html" ).arg( QCoreApplication::applicationDirPath() );
    QUrl url = QUrl::fromLocalFile( localFileName );
    view->load( url );
    *lg << LDEBUG << "HelpDialog::HelpDialog -> open url: <" << url.toString() << ">" << Qt::endl;
    //
    // zur Marke springen, wenn das Dokument geladen ist (callback)
    //
    connect( view, &QWebEngineView::loadFinished, [=]( bool fin ) {
      if ( fin )
      {
        QString jumpMark = QString( "document.getElementById(\"mark_%1\").scrollIntoView();" )
                               .arg( static_cast< int >( currentTab ), 2, 10, QChar( '0' ) );
        view->page()->runJavaScript( jumpMark );
        *lg << LDEBUG << "HelpDialog::HelpDialog -> try jump to <" << jumpMark << ">" << Qt::endl;
      }
    } );
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
