#ifndef ABOUTDIALOG_HPP
#define ABOUTDIALOG_HPP

#include <QDialog>
#include <QString>
#include <QPalette>

#include "ui_aboutdialog.h"
#include "../config/AppConfigClass.hpp"
#include "../logging/Logger.hpp"

namespace spx42
{
  class AboutDialog : public QDialog
  {
    private:
      Q_OBJECT
      Ui::AboutDialog *ui;
      Logger *lg;
      AppConfigClass *cf;

    public:
      explicit AboutDialog(QWidget *parent, AppConfigClass *conf, Logger *logger = Q_NULLPTR );

  };
}
#endif // ABOUTDIALOG_HPP
