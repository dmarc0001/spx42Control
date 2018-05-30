#include "DeviceEditDialog.hpp"

namespace spx
{
  DeviceEditDialog::DeviceEditDialog(QWidget *parent) :
    QDialog(parent),
    currentDevice(dummyRef),
    ex("^\\d{4}$")                                           // Vier Ziffern in einer Zeile
  {
    //
    // Hilfebutton ausblenden
    //
    this->setWindowFlags( this->windowFlags() & ~Qt::WindowContextHelpButtonHint );
    ui.setupUi(this);
    ui.errorLabel->setVisible(false);
    // Signal verbinden
    connect( ui.spxPinLineEdit, &QLineEdit::textEdited, this, &DeviceEditDialog::pinEdited );
    // Paletten vorbereiten
    palette_norm = ui.spxPinLineEdit->palette();
    palette_warn = ui.spxPinLineEdit->palette();
    palette_warn.setColor(QPalette::WindowText,Qt::red);
    palette_warn.setColor(QPalette::Text,Qt::red);
  }

  DeviceEditDialog::DeviceEditDialog(QWidget *parent, BluetoothDeviceDescriber &cDevice ) :
    QDialog(parent),
    currentDevice(cDevice),
    ex("^\\d{4}$")                                           // Vier Ziffern in einer Zeile
  {
    //
    // Hilfebutton ausblenden
    //
    this->setWindowFlags( this->windowFlags() & ~Qt::WindowContextHelpButtonHint );
    ui.setupUi(this);
    ui.spxNameLabel->setText(currentDevice.getDevName());
    ui.spxAliasLineEdit->setText(currentDevice.getAlias());
    ui.spxPinLineEdit->setText(currentDevice.getPin());
    ui.errorLabel->setVisible(false);
    // Signal verbinden
    connect( ui.spxPinLineEdit, &QLineEdit::textEdited, this, &DeviceEditDialog::pinEdited );
    // Paletten vorbereiten
    palette_norm = ui.spxPinLineEdit->palette();
    palette_warn = ui.spxPinLineEdit->palette();
    palette_warn.setColor(QPalette::WindowText,Qt::red);
    palette_warn.setColor(QPalette::Text,Qt::red);
  }

  /**
 * @brief DeviceEditDialog::pinEdited
 * @param text
 */
  #pragma GCC diagnostic ignored "-Wunused-parameter"
  void DeviceEditDialog::pinEdited(const QString &text)
  {
    //
    // test, ob der Text aus vier Ziffern besteht
    //
    if( ui.spxPinLineEdit->text().length() == 4 && ui.spxPinLineEdit->text().contains(ex) )
    {
      //
      // geht! normale Palette
      //
      ui.spxPinLineEdit->setPalette(palette_norm);
    }
    else
    {
      //
      // Warnung, der Text farbig
      //
      ui.spxPinLineEdit->setPalette(palette_warn);
    }
    if( ui.errorLabel->isVisible() )
    {
      //
      // Fehlerlabel anzeigen, wenn nicht geschehen
      //
      ui.errorLabel->setVisible(false);
    }
  }

  /**
 * @brief DeviceEditDialog::done
 * @param res
 */
  void DeviceEditDialog::done(int res)
  {
    //
    // war es akzeptiert?
    //
    if (res == QDialog::Accepted )
    {
      if( ui.spxPinLineEdit->text().length() == 4 && ui.spxPinLineEdit->text().contains(ex))
      {
        //
        // Wenn derPIN gültig ist
        //
        currentDevice.setAlias(ui.spxAliasLineEdit->text());
        currentDevice.setPin(ui.spxPinLineEdit->text());
        QDialog::done(res);
      }
      //
      // PIN ist ungültiges Format
      //
      ui.errorLabel->setText(tr("not an PIN!"));
      ui.errorLabel->setPalette(palette_warn);
      ui.errorLabel->setVisible(true);
      return;
    }
    //
    // Voreingestellt weitergeben
    //
    QDialog::done(res);
  }

}
