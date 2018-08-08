#include "PinDialog.hpp"

namespace spx
{
  /**
   * @brief PinDialog::PinDialog
   * @param parent
   */
  PinDialog::PinDialog( QWidget *parent ) : QDialog( parent ), ex( "^\\d{4}$" )  // Vier Ziffern in einer Zeile
  {
    //
    // Hilfebutton ausblenden
    //
    this->setWindowFlags( this->windowFlags() & ~Qt::WindowContextHelpButtonHint );
    ui.setupUi( this );
    ui.lineEdit->setText( pin );
    ui.errorLabel->setVisible( false );
    ui.retryButton->setVisible( false );
    // Signal verbinden
    connect( ui.lineEdit, &QLineEdit::textEdited, this, &PinDialog::pinEdited );
    // Paletten vorbereiten
    palette_norm = ui.lineEdit->palette();
    palette_warn = ui.lineEdit->palette();
    palette_warn.setColor( QPalette::WindowText, Qt::red );
    palette_warn.setColor( QPalette::Text, Qt::red );
  }

  PinDialog::PinDialog( QWidget *parent, const QString &oldPin )
      : QDialog( parent ), pin( oldPin ), ex( "^\\d{4}$" )  // Vier Ziffern in einer Zeile
  {
    //
    // Hilfebutton ausblenden
    //
    this->setWindowFlags( this->windowFlags() & ~Qt::WindowContextHelpButtonHint );
    ui.setupUi( this );
    ui.lineEdit->setText( pin );
    ui.errorLabel->setVisible( false );
    ui.retryButton->setVisible( false );
    // Signal verbinden
    connect( ui.lineEdit, &QLineEdit::textEdited, this, &PinDialog::pinEdited );
    // Paletten vorbereiten
    palette_norm = ui.lineEdit->palette();
    palette_warn = ui.lineEdit->palette();
    palette_warn.setColor( QPalette::WindowText, Qt::red );
    palette_warn.setColor( QPalette::Text, Qt::red );
  }

  /**
   * @brief PinDialog::pinEdited
   * @param text
   */
  void PinDialog::pinEdited( const QString & )
  {
    //
    // test, ob der Text aus vier Ziffern besteht
    //
    if ( ui.lineEdit->text().length() == 4 && ui.lineEdit->text().contains( ex ) )
    {
      //
      // geht! normale Palette
      //
      ui.lineEdit->setPalette( palette_norm );
    }
    else
    {
      //
      // Warnung, der Text farbig
      //
      ui.lineEdit->setPalette( palette_warn );
    }
    if ( ui.errorLabel->isVisible() )
    {
      //
      // Fehlerlabel anzeigen, wenn nicht geschehen
      //
      ui.errorLabel->setVisible( false );
    }
  }

  /**
   * @brief PinDialog::done
   * @param res
   */
  void PinDialog::done( int res )
  {
    // Voreinstellung
    pin = "";
    //
    // war es akzeptiert?
    //
    if ( res == QDialog::Accepted || res == QDialogButtonBox::Retry )
    {
      if ( ui.lineEdit->text().length() == 4 && ui.lineEdit->text().contains( ex ) )
      {
        //
        // Wenn derPIN gültig ist
        //
        pin = ui.lineEdit->text();
        QDialog::done( res );
      }
      //
      // PIN ist ungültiges Format
      //
      ui.errorLabel->setText( tr( "not an PIN!" ) );
      ui.errorLabel->setPalette( palette_warn );
      ui.errorLabel->setVisible( true );
      return;
    }
    //
    // Voreingestellt weitergeben
    //
    QDialog::done( res );
  }

  /**
   * @brief PinDialog::getPin
   * @return
   */
  QByteArray PinDialog::getPin( void )
  {
    return ( pin.toUtf8() );
  }

  void PinDialog::showRetryButton( bool showButton )
  {
    ui.retryButton->setVisible( showButton );
    if ( showButton )
    {
      connect( ui.retryButton, &QPushButton::clicked, this, &PinDialog::retryButtonSlot );
    }
  }

  void PinDialog::retryButtonSlot( void )
  {
    done( QDialogButtonBox::Retry );
  }
}
