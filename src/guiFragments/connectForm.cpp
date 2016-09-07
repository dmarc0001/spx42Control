#include "connectForm.hpp"
#include "ui_connectForm.h"

namespace spx42
{
  ConnectForm::ConnectForm(QWidget *parent, Logger *logger) :
    QWidget(parent),
    ui(new Ui::ConnectForm),
    parentWidget(parent),
    lg(logger)
  {
    ui->setupUi(parent);
    connect(ui->connectButton, &QPushButton::clicked, this, &ConnectForm::connectButtonSlot );
  }

  ConnectForm::~ConnectForm()
  {
    delete ui;
  }

  void ConnectForm::connectButtonSlot(void)
  {
    lg->debug("ConnectForm::connectButtonSlot -> connect button clicked.");
  }
}
