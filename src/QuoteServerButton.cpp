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

#include "QuoteServerButton.h"
#include "Config.h"
#include "../pics/quotes.xpm"
#include "QuoteServerDialog.h"
#include "QuoteServerRequest.h"

#include <QStringList>
#include <QDebug>
#include <QIcon>
#include <QProcess>

QuoteServerButton::QuoteServerButton ()
{
  setIcon(QIcon(quotes_xpm));
  setText(tr("Configure Quote Server..."));
  setStatusTip(tr("Configure Quote Server..."));
  setToolTip(tr("Configure Quote Server..."));
  connect(this, SIGNAL(clicked()), this, SLOT(dialog()));

  Config config;
  
  // set the quote server hostname
  QString name;
  config.getData(Config::QuoteServerName, name);
  if (name.isEmpty())
  {
    name = "127.0.0.1";
    config.setData(Config::QuoteServerName, name);
  }

  // set the quote server port number
  QString port;
  config.getData(Config::QuoteServerPort, port);
  if (port.isEmpty())
  {
    port = "5000";
    config.setData(Config::QuoteServerPort, port);
  }

  setupServer(name, port);
}

void QuoteServerButton::dialog ()
{
  QuoteServerDialog *dialog = new QuoteServerDialog;
  dialog->show();
}

void QuoteServerButton::setupServer (QString &, QString &port)
{
  // create malformed command to get ERROR response
  QStringList tl;
  tl << "Quotes" << "blah";
  QString command = tl.join(",") + "\n";

  QuoteServerRequest qsr;
  qsr.run(command);
  if (qsr.data() == "ERROR")
    return;

  command.clear();
  command.append("QuoteServer");
  command.append(" -p ");
  command.append(port);
  int rc = QProcess::startDetached(command);
  if (! rc)
    qDebug() << "Setup::setupQuoteServer: error starting the server";
}

