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

#include <QLayout>
#include <QLabel>
#include <QGroupBox>
#include <QDebug>

YahooDialog::YahooDialog ()
{
  _runningFlag = 0;
  setWindowTitle(tr("Configure Yahoo"));
  createMainPage();
  loadSettings();
}

void YahooDialog::createMainPage ()
{
  QVBoxLayout *vbox = new QVBoxLayout;
  vbox->setMargin(5);
  vbox->setSpacing(2);
  setLayout(vbox);

  QGridLayout *grid = new QGridLayout;
  grid->setSpacing(2);
  grid->setColumnStretch(1, 1);
  vbox->addLayout(grid);

  int row = 0;
  int col = 0;

  // start date parm
  QLabel *label = new QLabel(tr("Start Date"));
  grid->addWidget(label, row, col++);

  _sdate = new QDateTimeEdit(QDate::currentDate().addDays(-1));
  _sdate->setCalendarPopup(TRUE);
  _sdate->setMaximumDate(QDate::currentDate());
  _sdate->setDisplayFormat("yyyy.MM.dd");
  grid->addWidget(_sdate, row++, col--);


  // end date parm
  label = new QLabel(tr("End Date"));
  grid->addWidget(label, row, col++);
  
  _edate = new QDateTimeEdit(QDate::currentDate());
  _edate->setCalendarPopup(TRUE);
  _edate->setMaximumDate(QDate::currentDate());
  _edate->setDisplayFormat("yyyy.MM.dd");
  grid->addWidget(_edate, row++, col--);


  // symbols selection area
  _allSymbols = new QCheckBox;
  _allSymbols->setText(tr("Select All Symbols"));
  connect(_allSymbols, SIGNAL(toggled(bool)), this, SLOT(allSymbolsToggled(bool)));
  grid->addWidget(_allSymbols, row, col++);

  _selectSymbolsButton = new QPushButton;
  _selectSymbolsButton->setText(tr("Symbols..."));
  connect(_selectSymbolsButton, SIGNAL(clicked()), this, SLOT(selectSymbolsDialog()));
  grid->addWidget(_selectSymbolsButton, row++, col--);


  // adjustment
  _adjustment = new QCheckBox;
  _adjustment->setText(tr("Adjust for splits"));
  grid->addWidget(_adjustment, row++, col);


  
  // message log
  QGroupBox *gbox = new QGroupBox;
  gbox->setTitle(tr("Message Log"));
  vbox->addWidget(gbox);

  QHBoxLayout *hbox = new QHBoxLayout;
  hbox->setSpacing(2);
  gbox->setLayout(hbox);

  _log = new QTextEdit;
  _log->setReadOnly(TRUE);
  hbox->addWidget(_log);

  _buttonBox = new QDialogButtonBox;
  vbox->addWidget(_buttonBox);

  _histButton = new QPushButton(tr("&History"));
  _histButton->setToolTip(tr("Download historical quotes"));
  _buttonBox->addButton(_histButton, QDialogButtonBox::ActionRole);
  connect(_histButton, SIGNAL(clicked()), this, SLOT(startHistory()));

  _detailsButton = new QPushButton(tr("&Details"));
  _detailsButton->setToolTip(tr("Download symbol details"));
  _buttonBox->addButton(_detailsButton, QDialogButtonBox::ActionRole);
  connect(_detailsButton, SIGNAL(clicked()), this, SLOT(startDetails()));

  _cancelButton = new QPushButton(tr("&Cancel"));
  _buttonBox->addButton(_cancelButton, QDialogButtonBox::ActionRole);
  connect(_cancelButton, SIGNAL(clicked()), this, SLOT(cancelButton()));
}

void YahooDialog::cancelButton ()
{
  if (_runningFlag)
  {
    emit signalStop();
    _runningFlag = 0;
  }
  else
    accept();
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

  config.commit();
}

void YahooDialog::downloadDone ()
{
  _log->append("*** " + tr("Download finished") + " ***");

  _histButton->setEnabled(TRUE);
  _detailsButton->setEnabled(TRUE);
  _runningFlag = 0;

  emit signalChartRefresh();
}

void YahooDialog::startHistory ()
{
  _histButton->setEnabled(FALSE);
  _detailsButton->setEnabled(FALSE);

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

void YahooDialog::allSymbolsToggled (bool status)
{
  if (status)
    _selectSymbolsButton->setEnabled(FALSE);
  else
    _selectSymbolsButton->setEnabled(TRUE);
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



