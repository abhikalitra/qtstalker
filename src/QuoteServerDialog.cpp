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
#include "Globals.h"
#include "../pics/refresh.xpm"

#include <QtDebug>
#include <QProcess>
#include <QFormLayout>

QuoteServerDialog::QuoteServerDialog ()
{
  _oport = 5000;
  setWindowTitle("Qtstalker" + g_session + ": " + tr("Configure Quote Server"));

  createMainPage();
}

void QuoteServerDialog::createMainPage ()
{
  QWidget *w = new QWidget;

  QFormLayout *form = new QFormLayout;
  form->setSpacing(2);
  form->setMargin(5);
  w->setLayout(form);

  // hostname
  Config config;
  config.getData(Config::QuoteServerName, _ohostName);

  _hostName = new QLineEdit;
  _hostName->setText(_ohostName);
  _hostName->setToolTip(tr("Default: 127.0.0.1"));
  form->addRow(tr("Hostname"), _hostName);

  // port
  _oport = config.getInt(Config::QuoteServerPort);

  _port = new QSpinBox;
  _port->setRange(5000, 99999);
  _port->setValue(_oport);
  _port->setToolTip(tr("Default: 5000"));
  form->addRow(tr("Port #"), _port);

  // refresh button
  _refreshButton = new QPushButton;
  _refreshButton->setToolTip(tr("Restarts the server"));
  _refreshButton->setIcon(QIcon(refresh_xpm));
  _refreshButton->setText(tr("Restart"));
  connect(_refreshButton, SIGNAL(clicked()), this, SLOT(refreshServer()));
  form->addRow(tr("Restart"), _refreshButton);

  _tabs->addTab(w, tr("Settings"));
}

void QuoteServerDialog::done ()
{
  QString hostName = _hostName->text();
  if (hostName.isEmpty())
  {
    setMessage(tr("Hostname missing."));
    return;
  }

  int flag = 0;
  Config config;
  config.transaction();
  
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

  config.commit();

  if (flag)
  {
    // port # changed, restart the QuoteServer
    QString command("killall QuoteServer");
    int rc = QProcess::execute(command);
    if (rc)
    {
      setMessage(tr("Error stopping the server"));
      return;
    }

    command.clear();
    command.append("QuoteServer");
    command.append(" -p ");
    command.append(QString::number(_port->value()));
    rc = QProcess::startDetached(command);
    if (! rc)
    {
      setMessage(tr("Error starting the server"));
      return;
    }
  }

  accept();
}

void QuoteServerDialog::refreshServer ()
{
  // kill the QuoteServer
  QString command("killall QuoteServer");
  int rc = QProcess::execute(command);
  if (rc)
  {
    setMessage(tr("Error stopping the server"));
    return;
  }

  command.clear();
  command.append("QuoteServer");
  command.append(" -p ");
  command.append(QString::number(_port->value()));
  rc = QProcess::startDetached(command);
  if (! rc)
  {
    setMessage(tr("Error starting the server"));
    return;
  }

  setMessage(tr("Quote server restarted successfully"));
}
