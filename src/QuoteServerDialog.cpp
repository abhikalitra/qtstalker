/*
 *  Qtstalker stock charter
 *
 *  Copyright (C) 2001-2010 Stefan S. Stratigakos
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307,
 *  USA.
 */

#include "QuoteServerDialog.h"
#include "Config.h"

#include <QtDebug>
#include <QMessageBox>
#include <QProcess>
#include <QLayout>
#include <QLabel>

QuoteServerDialog::QuoteServerDialog (QWidget *w) : QDialog (w, 0)
{
  _oport = 5000;
  setAttribute(Qt::WA_DeleteOnClose);
  setWindowTitle(tr("Configure Quote Server"));

  QVBoxLayout *vbox = new QVBoxLayout;
  vbox->setSpacing(10);
  vbox->setMargin(5);
  setLayout(vbox);

  QGridLayout *grid = new QGridLayout;
  grid->setSpacing(2);
  grid->setColumnStretch(1, 1);
  vbox->addLayout(grid);

  int row = 0;
  int col = 0;

  // hostname
  QLabel *label = new QLabel(tr("Quote server hostname"));
  grid->addWidget(label, row, col++);
  
  Config config;
  config.getData(Config::QuoteServerName, _ohostName);

  _hostName = new QLineEdit;
  _hostName->setText(_ohostName);
  _hostName->setToolTip(tr("default 127.0.0.1"));
  grid->addWidget(_hostName, row++, col--);
  

  // port
  label = new QLabel(tr("Quote server port #"));
  grid->addWidget(label, row, col++);

  _oport = config.getInt(Config::QuoteServerPort);

  _port = new QSpinBox;
  _port->setRange(5000, 99999);
  _port->setValue(_oport);
  grid->addWidget(_port, row++, col--);


  _buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel | QDialogButtonBox::Help);
  connect(_buttonBox, SIGNAL(accepted()), this, SLOT(done()));
  connect(_buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
  vbox->addWidget(_buttonBox);

  _refreshButton = _buttonBox->addButton(tr("Refresh"), QDialogButtonBox::ActionRole);
  _refreshButton->setToolTip(tr("Restarts the server"));
  connect(_refreshButton, SIGNAL(clicked()), this, SLOT(refreshServer()));

  vbox->addStretch(1);
}

void QuoteServerDialog::done ()
{
  QString hostName = _hostName->text();
  if (hostName.isEmpty())
  {
    QMessageBox::information(this, tr("Qtstalker: Error"), tr("Hostname missing."));
    return;
  }

  int flag = 0;
  Config config;
  if (hostName != _ohostName)
  {
    flag = TRUE;
    config.setData(Config::QuoteServerName, hostName);
  }

  if (_port->value() != _oport)
  {
    flag = TRUE;
    config.setData(Config::QuoteServerPort, _port->value());
  }

  if (flag)
  {
    // port # changed, restart the QuoteServer
    QString command("killall QuoteServer");
    int rc = QProcess::execute(command);
    if (rc)
    {
      QMessageBox::information(this, tr("Qtstalker: Error"), tr("Error stopping the server"));
      accept();
      return;
    }

    command.clear();
    command.append("QuoteServer");
    command.append(" -p ");
    command.append(QString::number(_port->value()));
    rc = QProcess::startDetached(command);
    if (! rc)
    {
      QMessageBox::information(this, tr("Qtstalker: Error"), tr("Error starting the server"));
    }
  }

  accept();
}

void QuoteServerDialog::refreshServer ()
{
  int rc = QMessageBox::warning(this,
                                tr("Qtstalker: Warning"),
                                tr("Are you sure you want to restart the server?"),
                                QMessageBox::Yes,
                                QMessageBox::No,
                                QMessageBox::NoButton);

  if (rc == QMessageBox::No)
    return;

  // kill the QuoteServer
  QString command("killall QuoteServer");
  rc = QProcess::execute(command);
  if (rc)
  {
    QMessageBox::information(this, tr("Qtstalker: Error"), tr("Error stopping the server"));
    return;
  }

  command.clear();
  command.append("QuoteServer");
  command.append(" -p ");
  command.append(QString::number(_port->value()));
  rc = QProcess::startDetached(command);
  if (! rc)
  {
    QMessageBox::information(this, tr("Qtstalker: Error"), tr("Error starting the server"));
    return;
  }
}

