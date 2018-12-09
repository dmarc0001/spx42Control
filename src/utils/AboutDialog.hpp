#ifndef ABOUTDIALOG_HPP
#define ABOUTDIALOG_HPP

#include <QDialog>
#include <QPalette>
#include <QString>
#include <memory>
#include "config/AppConfigClass.hpp"
#include "logging/Logger.hpp"
#include "ui_AboutDialog.h"

namespace spx
{
  class AboutDialog : public QDialog
  {
    private:
    Q_OBJECT
    std::unique_ptr< Ui::AboutDialog > ui;
    std::shared_ptr< Logger > lg;
    AppConfigClass &cf;

    public:
    explicit AboutDialog( QWidget *parent, AppConfigClass &conf, std::shared_ptr< Logger > logger = Q_NULLPTR );
  };
}  // namespace spx

#endif  // ABOUTDIALOG_HPP
