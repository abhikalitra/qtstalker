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

#include "YahooDialog.h"
#include "YahooSymbolDialog.h"
#include "Globals.h"
#include "Doc.h"
#include "YahooDataBase.h"
#include "YahooHistory.h"

#include <QLayout>
#include <QDialogButtonBox>
#include <QDebug>
#include <QFormLayout>
#include <QGroupBox>
#include <QSettings>

YahooDialog::YahooDialog (Command *c)
{
  _command = c;
  _helpFile = "Yahoo.html";

  QStringList l;
  l << "QtStalker" << g_session << ":" << "Yahoo";
  setWindowTitle(l.join(" "));

  createGUI();

  loadSettings();

  connect(this, SIGNAL(finished(int)), this, SLOT(deleteLater()));
}

void YahooDialog::createGUI ()
{
  QVBoxLayout *vbox = new QVBoxLayout;
  vbox->setMargin(5);
  vbox->setSpacing(10);
  setLayout(vbox);

  QFormLayout *form = new QFormLayout;
  form->setMargin(0);
  form->setSpacing(2);
  vbox->addLayout(form);

  // start date parm
  _sdate = new QDateTimeEdit(QDate::currentDate().addDays(-1));
  _sdate->setCalendarPopup(TRUE);
  _sdate->setMaximumDate(QDate::currentDate());
  _sdate->setDisplayFormat("yyyy.MM.dd");
  form->addRow(tr("Start Date"), _sdate);

  // end date parm
  _edate = new QDateTimeEdit(QDate::currentDate());
  _edate->setCalendarPopup(TRUE);
  _edate->setMaximumDate(QDate::currentDate());
  _edate->setDisplayFormat("yyyy.MM.dd");
  form->addRow(tr("End Date"), _edate);

  // symbols select
  _selectSymbolsButton = new QPushButton;
  _selectSymbolsButton->setText(tr("Symbols..."));
  connect(_selectSymbolsButton, SIGNAL(clicked()), this, SLOT(selectSymbolsDialog()));
  form->addRow(tr("Select Symbols"), _selectSymbolsButton);

  // all symbols
  _allSymbols = new QCheckBox;
  form->addRow(tr("Select All Symbols"), _allSymbols);

  // adjustment
  _adjustment = new QCheckBox;
  _adjustment->setToolTip(tr("Uses the yahoo adjusted close instead of the actual close"));
  form->addRow(tr("Adjust for splits"), _adjustment);

  // message log
  _log = new QTextEdit;
  _log->setReadOnly(TRUE);
  vbox->addWidget(_log);

  // buttonbox
  QDialogButtonBox *bbox = new QDialogButtonBox(QDialogButtonBox::Help);
  connect(bbox, SIGNAL(accepted()), this, SLOT(done()));
  connect(bbox, SIGNAL(rejected()), this, SLOT(cancel()));
  vbox->addWidget(bbox);

  // ok button
  _okButton = bbox->addButton(QDialogButtonBox::Ok);

  // cancel button
  _cancelButton = bbox->addButton(QDialogButtonBox::Cancel);
  _cancelButton->setDefault(TRUE);

  // help button
  QPushButton *b = bbox->button(QDialogButtonBox::Help);
  connect(b, SIGNAL(clicked()), this, SLOT(help()));
}

void YahooDialog::cancel ()
{
  if (_okButton->isEnabled())
  {
    saveSettings();
    reject();
  }
  else
    emit signalStop();
}

void YahooDialog::loadSettings ()
{
  QSettings settings;
  settings.beginGroup("main"); // global setting

  QSize sz = settings.value("yahoo_dialog_window_size", QSize(200,150)).toSize();
  resize(sz);

  // restore the position of the app
  QPoint p = settings.value("yahoo_dialog_window_position", QPoint(0,0)).toPoint();
  move(p);

  _adjustment->setChecked(settings.value("yahoo_dialog_adjustment", TRUE).toBool());

  _allSymbols->setChecked(settings.value("yahoo_dialog_all_symbols", TRUE).toBool());
}

void YahooDialog::saveSettings ()
{
  QSettings settings;
  settings.beginGroup("main");
  settings.setValue("yahoo_dialog_window_size", size());
  settings.setValue("yahoo_dialog_window_position", pos());
  settings.setValue("yahoo_dialog_adjustment", _adjustment->isChecked());
  settings.setValue("yahoo_dialog_all_symbols", _allSymbols->isChecked());
  settings.sync();
}

void YahooDialog::done ()
{
  _sdate->setEnabled(FALSE);
  _edate->setEnabled(FALSE);
  _adjustment->setEnabled(FALSE);
  _allSymbols->setEnabled(FALSE);
  _selectSymbolsButton->setEnabled(FALSE);
  _okButton->setEnabled(FALSE);

  _log->append("\n*** " + tr("Starting history download") + " ***");

  YahooDataBase db;

  if (_allSymbols->isChecked())
    db.symbols(_symbolList);

  QList<Setting> symbols;
  int loop = 0;
  for (; loop < _symbolList.count(); loop++)
  {
    Setting symbol;
    symbol.setData("YSYMBOL", _symbolList.at(loop));

    if (db.load(symbol))
    {
      qDebug() << "YahooDialog::done error loading symbol from YahooDataBase" << _symbolList.at(loop);
      continue;
    }

    symbol.setData("DATE_START", _sdate->dateTime().toString(Qt::ISODate));
    symbol.setData("DATE_END", _sdate->dateTime().toString(Qt::ISODate));
    symbol.setData("ADJUSTMENT", _adjustment->isChecked());

    symbols.append(symbol);
  }
  
  YahooHistory *thread = new YahooHistory(this, symbols);
  connect(thread, SIGNAL(signalMessage(QString)), _log, SLOT(append(const QString &)));
  connect(thread, SIGNAL(finished()), this, SLOT(downloadDone()));
  connect(this, SIGNAL(signalStop()), thread, SLOT(stop()));
  thread->start();
}

void YahooDialog::selectSymbolsDialog ()
{
  YahooSymbolDialog *sdialog = new YahooSymbolDialog;
  connect(sdialog, SIGNAL(signalSymbols(QStringList)), this, SLOT(setSymbols(QStringList)));
  sdialog->show();
}

void YahooDialog::setSymbols (QStringList list)
{
  _symbolList = list;
}

void YahooDialog::help ()
{
  Doc *doc = new Doc;
  doc->showDocumentation(_helpFile);
}

void YahooDialog::downloadDone ()
{
  _log->append("*** " + tr("Download finished") + " ***");

  _sdate->setEnabled(TRUE);
  _edate->setEnabled(TRUE);
  _adjustment->setEnabled(TRUE);
  _allSymbols->setEnabled(TRUE);
  _selectSymbolsButton->setEnabled(TRUE);
  _okButton->setEnabled(TRUE);
  
  g_middleMan->chartPanelRefresh();
}

