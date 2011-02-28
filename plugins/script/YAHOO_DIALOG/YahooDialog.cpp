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
#include "YahooDataBase.h"
#include "YahooHistory.h"
#include "QuoteDataBase.h"
#include "DateRange.h"
#include "YahooSymbol.h"

#include <QDebug>
#include <QGroupBox>
#include <QApplication>

YahooDialog::YahooDialog (QWidget *p, Command *c) : Dialog (p)
{
  _command = c;
  _helpFile = "Yahoo.html";
  _keySize = "yahoo_dialog_window_size";
  _keyPos = "yahoo_dialog_window_position";

  QStringList l;
  l << "QtStalker" << g_session << ":" << "Yahoo";
  setWindowTitle(l.join(" "));

  createGUI();

  loadSettings();
}

void YahooDialog::createGUI ()
{
  // start date parm
  _sdate = new QDateTimeEdit(QDate::currentDate().addDays(-1));
  _sdate->setCalendarPopup(TRUE);
  _sdate->setMaximumDate(QDate::currentDate());
  _sdate->setDisplayFormat("yyyy.MM.dd");
  _form->addRow(tr("Start Date"), _sdate);

  // end date parm
  _edate = new QDateTimeEdit(QDate::currentDate());
  _edate->setCalendarPopup(TRUE);
  _edate->setMaximumDate(QDate::currentDate());
  _edate->setDisplayFormat("yyyy.MM.dd");
  _form->addRow(tr("End Date"), _edate);

  // symbols select
  _selectSymbolsButton = new QPushButton;
  _selectSymbolsButton->setText(QString("0 ") + tr("Selected"));
  connect(_selectSymbolsButton, SIGNAL(clicked()), this, SLOT(selectSymbolsDialog()));
  _form->addRow(tr("Select Symbols"), _selectSymbolsButton);

  // all symbols
  _allSymbols = new QCheckBox;
  _form->addRow(tr("Select All Symbols"), _allSymbols);
  connect(_allSymbols, SIGNAL(toggled(bool)), this, SLOT(allSymbolsToggled(bool)));

  // adjustment
  _adjustment = new QCheckBox;
  _adjustment->setToolTip(tr("Uses the yahoo adjusted close instead of the actual close"));
  _form->addRow(tr("Adjust for splits"), _adjustment);

  // auto date update
  _autoDate = new QCheckBox;
  _autoDate->setToolTip(tr("Updates from last date on disk to now."));
  _form->addRow(tr("Auto dates"), _autoDate);
  connect(_autoDate, SIGNAL(toggled(bool)), this, SLOT(autoDateToggled(bool)));

  int pos = 1;
  
  // message log area
  QGroupBox *gbox = new QGroupBox;
  gbox->setTitle(tr("Message Log"));
  _vbox->insertWidget(pos++, gbox);

  QVBoxLayout *tvbox = new QVBoxLayout;
  gbox->setLayout(tvbox);
  
  // message log
  _log = new QTextEdit;
  _log->setReadOnly(TRUE);
  tvbox->addWidget(_log);
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
  Dialog::loadSettings();

  QSettings settings(g_globalSettings);
  _adjustment->setChecked(settings.value("yahoo_dialog_adjustment", TRUE).toBool());
  _allSymbols->setChecked(settings.value("yahoo_dialog_all_symbols", TRUE).toBool());
  _autoDate->setChecked(settings.value("yahoo_dialog_auto_date", TRUE).toBool());
}

void YahooDialog::saveSettings ()
{
  Dialog::saveSettings();
  
  QSettings settings(g_globalSettings);
  settings.setValue("yahoo_dialog_adjustment", _adjustment->isChecked());
  settings.setValue("yahoo_dialog_all_symbols", _allSymbols->isChecked());
  settings.setValue("yahoo_dialog_auto_date", _autoDate->isChecked());
  settings.sync();
}

void YahooDialog::done ()
{
  _log->append("\n*** " + tr("Starting history download") + " ***");
  _log->update();
  qApp->processEvents();

  _sdate->setEnabled(FALSE);
  _edate->setEnabled(FALSE);
  _adjustment->setEnabled(FALSE);
  _allSymbols->setEnabled(FALSE);
  _selectSymbolsButton->setEnabled(FALSE);
  _okButton->setEnabled(FALSE);
  _autoDate->setEnabled(FALSE);

  YahooDataBase db;
  QuoteDataBase qdb;

  if (_allSymbols->isChecked())
    db.symbols(_symbolList);

  YahooSymbol ys;
  QList<Setting> symbols;
  int loop = 0;
  for (; loop < _symbolList.count(); loop++)
  {
    Setting symbol;
    symbol.setData("SYMBOL", _symbolList.at(loop));

    ys.data(symbol);

    if (_autoDate->isChecked())
    {
      BarData bd;
      bd.setExchange(symbol.data("EXCHANGE"));
      bd.setSymbol(symbol.data("SYMBOL"));
      bd.setBarLength(BarData::DailyBar);
      bd.setRange(DateRange::Day);

      if (qdb.getBars(&bd))
      {
        symbol.setData("DATE_START", QString("1950-01-01 00:00:00"));
        symbol.setData("DATE_END", QDateTime::currentDateTime().toString(Qt::ISODate));
      }
      else
      {
        Bar *bar = bd.bar(0);
        if (! bar)
          continue;

        symbol.setData("DATE_START", bar->date().toString(Qt::ISODate));
        symbol.setData("DATE_END", QDateTime::currentDateTime().toString(Qt::ISODate));
      }
    }
    else
    {
      symbol.setData("DATE_START", _sdate->dateTime().toString(Qt::ISODate));
      symbol.setData("DATE_END", _edate->dateTime().toString(Qt::ISODate));
    }
    
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
  YahooSymbolDialog *sdialog = new YahooSymbolDialog(this);
  connect(sdialog, SIGNAL(signalSymbols(QStringList)), this, SLOT(setSymbols(QStringList)));
  sdialog->show();
}

void YahooDialog::setSymbols (QStringList list)
{
  _symbolList = list;

  QString s = QString::number(list.count()) + " " + tr("Selected");
  _selectSymbolsButton->setText(s);
}

void YahooDialog::downloadDone ()
{
  _log->append("*** " + tr("Download finished") + " ***");

  allSymbolsToggled(_allSymbols->isChecked());
  autoDateToggled(_autoDate->isChecked());
  _adjustment->setEnabled(TRUE);
  _allSymbols->setEnabled(TRUE);
  _okButton->setEnabled(TRUE);
  _autoDate->setEnabled(TRUE);
  
  g_middleMan->chartPanelRefresh();
}

void YahooDialog::allSymbolsToggled (bool d)
{
  bool status = FALSE;
  if (! d)
    status = TRUE;
  
  _selectSymbolsButton->setEnabled(status);
}

void YahooDialog::autoDateToggled (bool d)
{
  bool status = FALSE;
  if (! d)
    status = TRUE;

  _sdate->setEnabled(status);
  _edate->setEnabled(status);
}
