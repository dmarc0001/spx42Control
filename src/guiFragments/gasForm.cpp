#include "gasForm.hpp"
#include "ui_gasForm.h"

namespace spx42
{
  GasForm::GasForm(QWidget *parent, Logger *logger) :
    QWidget(parent),
    ui(new Ui::GasForm),
    parentWidget( parent ),
    lg(logger)
  {
    ui->setupUi(this);
  }

  GasForm::~GasForm()
  {
    delete ui;
  }
}
