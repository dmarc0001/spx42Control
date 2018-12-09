#include "HelpDialog.hpp"
#include "ui_HelpDialog.h"

HelpDialog::HelpDialog(QWidget *parent) :
  QDialog(parent),
  ui(new Ui::HelpDialog)
{
  ui->setupUi(this);
}

HelpDialog::~HelpDialog()
{
  delete ui;
}

void HelpDialog::changeEvent(QEvent *e)
{
  QDialog::changeEvent(e);
  switch (e->type()) {
    case QEvent::LanguageChange:
      ui->retranslateUi(this);
      break;
    default:
      break;
  }
}
