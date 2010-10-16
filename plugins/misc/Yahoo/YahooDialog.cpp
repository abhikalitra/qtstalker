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
#include "YahooDataBase.h"
#include "YahooConfig.h"
#include "YahooThread.h"
#include "Globals.h"
#include "../pics/disable.xpm"
#include "../pics/quotes.xpm"

#include <QLayout>
#include <QDialogButtonBox>
#include <QDebug>
#include <QFormLayout>
#include <QGroupBox>

YahooDialog::YahooDialog ()
{
  _runningFlag = 0;
  
  setWindowTitle("QtStalker" + g_session + ": Yahoo ");
  
  QVBoxLayout *vbox = new QVBoxLayout;
  vbox->setSpacing(5);
  vbox->setMargin(5);
  setLayout(vbox);

  _tabs = new QTabWidget;
  vbox->addWidget(_tabs);

  // buttonbox
  QDialogButtonBox *bbox = new QDialogButtonBox(QDialogButtonBox::Help);

  QPushButton *b = bbox->addButton(QDialogButtonBox::Close);
  connect(b, SIGNAL(clicked()), this, SLOT(closeDialog()));
  vbox->addWidget(bbox);

  createMainPage();
  
  loadSettings();
}

void YahooDialog::createMainPage ()
{
  QWidget *w = new QWidget;

  QVBoxLayout *vbox = new QVBoxLayout;
  vbox->setMargin(5);
  vbox->setSpacing(10);
  w->setLayout(vbox);

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
  QGroupBox *gbox = new QGroupBox;
  gbox->setTitle(tr("Log"));
  vbox->addWidget(gbox);

  QHBoxLayout *hbox = new QHBoxLayout;
  hbox->setSpacing(2);
  hbox->setMargin(5);
  gbox->setLayout(hbox);

  _log = new QTextEdit;
  _log->setReadOnly(TRUE);
  hbox->addWidget(_log);

  QVBoxLayout *bbox = new QVBoxLayout;
  bbox->setMargin(0);
  bbox->setSpacing(2);
  hbox->addLayout(bbox);

  _histButton = new QPushButton;
  _histButton->setToolTip(tr("Download historical quotes"));
  _histButton->setText(tr("&History"));
  _histButton->setIcon(QIcon(quotes_xpm));
  connect(_histButton, SIGNAL(clicked()), this, SLOT(startHistory()));
  bbox->addWidget(_histButton);

  _detailsButton = new QPushButton;
  _detailsButton->setToolTip(tr("Download symbol details"));
  _detailsButton->setText(tr("&Details"));
  _detailsButton->setIcon(QIcon(quotes_xpm));
  connect(_detailsButton, SIGNAL(clicked()), this, SLOT(startDetails()));
  bbox->addWidget(_detailsButton);

  _cancelButton = new QPushButton;
  _cancelButton->setToolTip(tr("Cancel download"));
  _cancelButton->setText(tr("&Cancel"));
  _cancelButton->setIcon(QIcon(disable_xpm));
  connect(_cancelButton, SIGNAL(clicked()), this, SLOT(cancelButton()));
  _cancelButton->setEnabled(FALSE);
  bbox->addWidget(_cancelButton);

  bbox->addStretch();

  _tabs->addTab(w, tr("Yahoo"));
}

void YahooDialog::cancelButton ()
{
  emit signalStop();
  _runningFlag = 0;
}

void YahooDialog::closeDialog ()
{
  saveSettings();
  hide();
}

void YahooDialog::loadSettings ()
{
  YahooConfig config;

  // restore the size of the window
  QSize sz;
  config.getData(YahooConfig::Size, sz);
  if (! sz.isNull())
    resize(sz);

  // restore the position of the window
  QPoint p;
  config.getData(YahooConfig::Pos, p);
  if (! p.isNull())
    move(p);

  _adjustment->setChecked(config.getInt(YahooConfig::Adjustment));

  _allSymbols->setChecked(config.getInt(YahooConfig::AllSymbols));
}

void YahooDialog::saveSettings ()
{
  YahooConfig config;
  config.transaction();

  // save app size and position
  QSize sz = size();
  config.setData(YahooConfig::Size, sz);
  
  QPoint pt = pos();
  config.setData(YahooConfig::Pos, pt);

  config.setData(YahooConfig::Adjustment, _adjustment->isChecked());

  config.setData(YahooConfig::AllSymbols, _allSymbols->isChecked());

  config.commit();
}

void YahooDialog::downloadDone ()
{
  _log->append("*** " + tr("Download finished") + " ***");

  _histButton->setEnabled(TRUE);
  _detailsButton->setEnabled(TRUE);
  _cancelButton->setEnabled(FALSE);
  
  _runningFlag = 0;

  emit signalChartRefresh();
}

void YahooDialog::startHistory ()
{
  _histButton->setEnabled(FALSE);
  _detailsButton->setEnabled(FALSE);
  _cancelButton->setEnabled(TRUE);

  _log->append("\n*** " + tr("Starting history download") + " ***");

  _runningFlag = 1;

  // get the symbols to download
  QStringList sl;
  if (_allSymbols->isChecked())
  {
    YahooDataBase db;
    db.getSymbols(sl);
  }
  else
    sl = _symbolList;

  QString type = "H";
  YahooThread *thread = new YahooThread(this, type, sl, _sdate->dateTime(), _edate->dateTime());
  connect(thread, SIGNAL(signalMessage(QString)), _log, SLOT(append(const QString &)));
  connect(thread, SIGNAL(finished()), this, SLOT(downloadDone()));
  connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));
  connect(this, SIGNAL(signalStop()), thread, SLOT(stop()));
  thread->start();
}

void YahooDialog::startDetails ()
{
  _histButton->setEnabled(FALSE);
  _detailsButton->setEnabled(FALSE);
  _cancelButton->setEnabled(TRUE);

  _log->append("\n*** " + tr("Starting details download") + " ***");

  _runningFlag = 1;

  // get the symbols to download
  QStringList sl;
  if (_allSymbols->isChecked())
  {
    YahooDataBase db;
    db.getSymbols(sl);
  }
  else
    sl = _symbolList;

  QString type = "D";
  YahooThread *thread = new YahooThread(this, type, sl, _sdate->dateTime(), _edate->dateTime());
  connect(thread, SIGNAL(signalMessage(QString)), _log, SLOT(append(const QString &)));
  connect(thread, SIGNAL(finished()), this, SLOT(downloadDone()));
  connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));
  connect(this, SIGNAL(signalStop()), thread, SLOT(stop()));
  thread->start();
}

void YahooDialog::selectSymbolsDialog ()
{
  YahooSymbolDialog *sdialog = new YahooSymbolDialog;
  connect(sdialog, SIGNAL(signalSymbols(QStringList)), this, SLOT(setSymbols(QStringList)));
  connect(sdialog, SIGNAL(finished(int)), sdialog, SLOT(deleteLater()));
  sdialog->show();
}

void YahooDialog::setSymbols (QStringList list)
{
  _symbolList = list;
}



