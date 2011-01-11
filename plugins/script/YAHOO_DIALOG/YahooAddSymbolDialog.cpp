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

#include "YahooAddSymbolDialog.h"
#include "Globals.h"
#include "Doc.h"
#include "ScriptPluginFactory.h"

#include <QtDebug>
#include <QDialogButtonBox>
#include <QLayout>
#include <QSettings>
#include <QFormLayout>

YahooAddSymbolDialog::YahooAddSymbolDialog ()
{
  _yexchange << "NYSE" << "AX" << "SA" << "TO" << "BO" << "NS" << "L" << "B";
  _helpFile = "Yahoo.html";

  QStringList l;
  l << "QtStalker" << g_session << ":" << "Yahoo " + tr("Add Symbols");
  setWindowTitle(l.join(" "));

  createGUI();

  loadSettings();

  buttonStatus();

  connect(this, SIGNAL(finished(int)), this, SLOT(deleteLater()));
}

void YahooAddSymbolDialog::createGUI ()
{
  QVBoxLayout *vbox = new QVBoxLayout;
  vbox->setSpacing(2);
  setLayout(vbox);

  QFormLayout *form = new QFormLayout;
  form->setSpacing(2);
  form->setMargin(5);
  vbox->addLayout(form);

  _symbols = new QLineEdit;
  _symbols->setToolTip(tr("Enter Yahoo symbols separated by a space"));
  connect(_symbols, SIGNAL(textChanged(const QString &)), this, SLOT(buttonStatus()));
  form->addRow(tr("Yahoo Symbols"), _symbols);

  // status message
  _message = new QLabel;
  vbox->addWidget(_message);

  // buttonbox
  QDialogButtonBox *bbox = new QDialogButtonBox(QDialogButtonBox::Help);
  connect(bbox, SIGNAL(accepted()), this, SLOT(done()));
  connect(bbox, SIGNAL(rejected()), this, SLOT(cancel()));
  vbox->addWidget(bbox);

  // ok button
  _okButton = bbox->addButton(QDialogButtonBox::Ok);
  _okButton->setDefault(TRUE);

  // cancel button
  _cancelButton = bbox->addButton(QDialogButtonBox::Cancel);

  // help button
  QPushButton *b = bbox->button(QDialogButtonBox::Help);
  connect(b, SIGNAL(clicked()), this, SLOT(help()));
}

void YahooAddSymbolDialog::buttonStatus ()
{
  int status = 0;
  if (_symbols->text().length())
    status++;

  _okButton->setEnabled(status);
}

void YahooAddSymbolDialog::help ()
{
  Doc *doc = new Doc;
  doc->showDocumentation(_helpFile);
}

void YahooAddSymbolDialog::done ()
{
  ScriptPluginFactory fac;
  ScriptPlugin *plug = fac.plugin("YAHOO_DATABASE");
  if (! plug)
  {
    qDebug() << "YahooAddSymbolDialog::done: no plugin";
    return;
  }

  QStringList cl;
  cl << "YAHOO_DATABASE" << "TRANSACTION";
  Command command(cl.join(","));

  if (plug->command(&command))
    qDebug() << "YahooAddSymbolDialog::done: command error";

  QString s = _symbols->text();
  s = s.trimmed();
  s = s.toUpper();
  QStringList l = s.split(" ");

  QStringList errorList;
  int loop = 0;
  for (; loop < l.count(); loop++)
  {
    QString symbol, exchange;
    if (getSymbolExchange(l[loop], symbol, exchange))
      errorList.append(l[loop]);
    else
    {
      cl.clear();
      cl << "YAHOO_DATABASE" << "SAVE" << l.at(loop) << exchange << symbol;
      command.parse(cl.join(","));

      if (plug->command(&command))
        qDebug() << "YahooAddSymbolDialog::done: command error";
    }
  }

  cl.clear();
  cl << "YAHOO_DATABASE" << "COMMIT";
  command.parse(cl.join(","));

  if (plug->command(&command))
    qDebug() << "YahooAddSymbolDialog::done: command error";

  delete plug;

  if (errorList.count())
  {
    errorList.prepend(tr("Invalid symbols found\n"));
    _message->setText(errorList.join("\n"));
    return;
  }

  saveSettings();

  emit signalNew();

  accept();
}

int YahooAddSymbolDialog::getSymbolExchange (QString &ysymbol, QString &symbol, QString &exchange)
{
  QString s;
  QStringList l = ysymbol.split(".");
  if (l.count() == 1)
    l.append("NYSE");

  int rc = 0;

  switch (_yexchange.indexOf(l[1]))
  {
    // unfortunately yahoo does not separate NYSE,NASDAQ,ASE quotes
    // so we lump them into the NYSE exchange
    case 0: // NYSE
      symbol = l.at(0);
      exchange = "XNYS";
      break;
    case 1: // AX
      symbol = l.at(0);
      exchange = "XASX";
      break;
    case 2: // SA
      symbol = l.at(0);
      exchange = "BVMF";
      break;
    case 3: // TO
      symbol = l.at(0);
      exchange = "XTSE";
      break;
    case 4: // BO
    case 5: // NS
      symbol = l.at(0);
      exchange = "XNSE";
      break;
    case 6: // L
      symbol = l.at(0);
      exchange = "XLON";
      break;
    case 7: // B, some XNYS symbols use .B eg BRK.B
      symbol = l.at(0) + ".B"; // we keep the .B
      exchange = "XNYS";
      break;
    default: // error
      rc = 1;
      break;
  }

  return rc;
}

void YahooAddSymbolDialog::cancel ()
{
  saveSettings();
  reject();
}

void YahooAddSymbolDialog::loadSettings ()
{
  QSettings settings;
  settings.beginGroup("main"); // global setting

  QSize sz = settings.value("yahoo_add_symbol_dialog_window_size", QSize(200,150)).toSize();
  resize(sz);

  // restore the position of the app
  QPoint p = settings.value("yahoo_add_symbol_dialog_window_position", QPoint(0,0)).toPoint();
  move(p);
}

void YahooAddSymbolDialog::saveSettings ()
{
  QSettings settings;
  settings.beginGroup("main");
  settings.setValue("yahoo_add_symbol_dialog_window_size", size());
  settings.setValue("yahoo_add_symbol_dialog_window_position", pos());
  settings.sync();
}
