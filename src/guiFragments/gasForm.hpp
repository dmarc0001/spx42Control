#ifndef GASFORM_HPP
#define GASFORM_HPP

#include <QWidget>

#include "../logging/Logger.hpp"

namespace Ui {
  class GasForm;
}

namespace spx42
{
  class GasForm : public QWidget
  {
    private:
      Q_OBJECT
      Ui::GasForm *ui;
      QWidget *parentWidget;
      Logger *lg;

    public:
      explicit GasForm(QWidget *parent, Logger *logger);
      ~GasForm();

  };
}
#endif // GASFORM_HPP
