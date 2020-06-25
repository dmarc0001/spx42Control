#include "AboutDialog.hpp"

namespace spx
{
  QString AboutDialog::version = QString( "%1.%2.%3" ).arg( VMAJOR ).arg( VMINOR ).arg( VPATCH );

  AboutDialog::AboutDialog( QWidget *parent, AppConfigClass &conf, std::shared_ptr< Logger > logger )
      : QDialog( parent ), ui( new Ui::AboutDialog() ), lg( logger ), cf( conf )
  {
    if ( lg != Q_NULLPTR )
    {
      lg->debug( "AboutDialog::AboutDialog..." );
    }
    //
    // Hilfebutton ausblenden
    //
    this->setWindowFlags( this->windowFlags() & ~Qt::WindowContextHelpButtonHint );
    //
    // GUI initialisieren
    //
    ui->setupUi( this );
    // fixe Größe
    setFixedSize( this->width(), this->height() );
    if ( lg != Q_NULLPTR )
    {
      lg->debug( "AboutDialog::AboutDialog -> create and set LOGO..." );
    }
    QImage image( ":/images/logo_header450.png" );
    ui->logoLabel->setPixmap( QPixmap::fromImage( image ) );
    ui->buildNumberLabel->setText( AppConfigClass::getBuildDate() );
    ui->buildDateLabel->setText( AppConfigClass::getBuildNumStr() );
    ui->buildTypeLabel->setText( tr( "Version: %1 - %2" ).arg( AboutDialog::version ).arg( AppConfigClass::getBuldType() ) );
    QPalette pal = ui->logoLabel->palette();
    pal.setColor( QPalette::WindowText, Qt::darkRed );
    pal.setColor( QPalette::Text, Qt::darkRed );
    pal.setColor( QPalette::ButtonText, Qt::blue );
    ui->buildNumberLabel->setPalette( pal );
    ui->buildDateLabel->setPalette( pal );
    ui->buildNumberLabelLabel->setPalette( pal );
    ui->buildDateLabelLabel->setPalette( pal );
    ui->buildTypeLabel->setPalette( pal );
    QFont grFont = ui->closeButton->font();
    grFont.setBold( true );
    grFont.setPointSize( 17 );
    ui->closeButton->setPalette( pal );
    ui->closeButton->setFont( grFont );
  }
}  // namespace spx
