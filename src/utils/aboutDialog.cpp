#include "aboutDialog.hpp"

namespace spx42
{
  AboutDialog::AboutDialog(QWidget *parent, AppConfigClass *conf, Logger *logger) :
    QDialog(parent),
    ui( new Ui::AboutDialog ),
    lg( logger ),
    cf( conf )
  {
    if( lg != Q_NULLPTR ) lg->debug("AboutDialog::AboutDialog...");
    //
    // Hilfebutton ausblenden
    //
    this->setWindowFlags( this->windowFlags() & ~Qt::WindowContextHelpButtonHint );
    //
    // GUI initialisieren
    //
    ui->setupUi(this);
    // fixe Größe
    setFixedSize(this->width(),this->height());
    if( lg != Q_NULLPTR ) lg->debug("AboutDialog::AboutDialog -> create and set LOGO...");
    QImage image(":/images/logo_header450.png");
    ui->logoLabel->setPixmap(QPixmap::fromImage(image));
    ui->buildNumberLabel->setText(cf->getBuildDate());
    ui->buildDateLabel->setText(cf->getBuildNumStr());
    QPalette pal= ui->logoLabel->palette();
    pal.setColor(QPalette::WindowText,Qt::darkRed);
    pal.setColor(QPalette::Text,Qt::darkRed);
    ui->buildNumberLabel->setPalette(pal);
    ui->buildDateLabel->setPalette(pal);
    ui->buildNumberLabelLabel->setPalette(pal);
    ui->buildDateLabelLabel->setPalette(pal);
    pal.setColor(QPalette::ButtonText,Qt::blue);
    ui->closeButton->setPalette(pal);
  }
}
