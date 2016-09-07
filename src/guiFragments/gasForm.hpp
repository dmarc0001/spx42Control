﻿#ifndef GASFORM_HPP
#define GASFORM_HPP

#include <QWidget>

#include "../logging/Logger.hpp"

namespace Ui {
  class GasForm;
}

namespace spx42
{
  class GasFragment : public QWidget
  {
    private:
      Q_OBJECT
      Ui::GasForm *ui;
      QWidget *parentWidget;
      Logger *lg;
      static int counter;
      static int count;

    public:
      explicit GasFragment(QWidget *parent, Logger *logger);
      ~GasFragment();

  };
}
#endif // GASFORM_HPP
