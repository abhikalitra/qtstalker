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

#include <QLayout>
#include <QLabel>
#include <QGroupBox>

#include <QInputDialog>
#include <QMessageBox>

YahooDialog::YahooDialog ()
{
  setWindowTitle(tr("Configure Yahoo"));
  createMainPage();
  loadSettings();

  _thread = new YahooThread;
  connect(_thread, SIGNAL(signalMessage(QString)), _log, SLOT(append(const QString &)));
  connect(_thread, SIGNAL(signalDone()), this, SLOT(downloadDone()));

  _type = History;
  _symbolPos = -1;
  _cancelFlag = 0;
  _runningFlag = 0;
}

YahooDialog::~YahooDialog ()
{
  delete _thread;
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

  // adjustment
  _adjustment = new QCheckBox;
  grid->addWidget(_adjustment, row, col++);

  label = new QLabel(tr("Adjust for splits"));
  grid->addWidget(label, row++, col--);


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

  _infoButton = new QPushButton(tr("&Details"));
  _infoButton->setToolTip(tr("Download symbol details"));
  _buttonBox->addButton(_infoButton, QDialogButtonBox::ActionRole);
  connect(_infoButton, SIGNAL(clicked()), this, SLOT(startInfo()));

  _symbolsButton = new QPushButton(tr("&Symbols"));
  _symbolsButton->setToolTip(tr("Edit symbols to download"));
  _buttonBox->addButton(_symbolsButton, QDialogButtonBox::ActionRole);
  connect(_symbolsButton, SIGNAL(clicked()), this, SLOT(editSymbols()));

  _cancelButton = new QPushButton(tr("&Cancel"));
  _buttonBox->addButton(_cancelButton, QDialogButtonBox::ActionRole);
  connect(_cancelButton, SIGNAL(clicked()), this, SLOT(cancelButton()));
}

void YahooDialog::editSymbols ()
{
  YahooSymbolDialog *sdialog = new YahooSymbolDialog;
  sdialog->exec();
  delete sdialog;
}

void YahooDialog::cancelButton ()
{
  if (_runningFlag)
    _cancelFlag = 1;
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
  if (_cancelFlag)
  {
    _log->append("************* " + tr("Download cancelled") + " ***************");
    _cancelFlag = 0;
    _runningFlag = 0;
    
    _symbolsButton->setEnabled(TRUE);
    _histButton->setEnabled(TRUE);
    _infoButton->setEnabled(TRUE);
    return;
  }
  
  _symbolPos++;
  if (_symbolPos == _symbolList.count())
  {
    _runningFlag = 0;

    _symbolsButton->setEnabled(TRUE);
    _histButton->setEnabled(TRUE);
    _infoButton->setEnabled(TRUE);

    _log->append("************* " + tr("Download complete") + " ***************");
    return;
  }

  switch (_type)
  {
    case History:
      _thread->downloadHistory(_sdate->dateTime(), _edate->dateTime(), _adjustment->isChecked(), _symbolList[_symbolPos]);
      break;
    case Info:
      _thread->downloadInfo(_symbolList[_symbolPos]);
      break;
    default:
      break;
  }
}

void YahooDialog::loadSymbols ()
{
  _symbolList.clear();
  _symbolPos = -1;
  
  YahooDataBase db;
  db.getSymbols(_symbolList);
}

void YahooDialog::startHistory ()
{
  _type = History;
  loadSymbols();

  _symbolsButton->setEnabled(FALSE);
  _histButton->setEnabled(FALSE);
  _infoButton->setEnabled(FALSE);

  _cancelFlag = 0;
  _runningFlag = 1;
  
  _log->append("\n************** " + tr("Starting history download") + " ***************");

  downloadDone();
}

void YahooDialog::startInfo ()
{
  _type = Info;
  loadSymbols();

  _symbolsButton->setEnabled(FALSE);
  _histButton->setEnabled(FALSE);
  _infoButton->setEnabled(FALSE);
  
  _cancelFlag = 0;
  _runningFlag = 1;

  _log->append("\n************* " + tr("Starting details download") + " ***************");

  downloadDone();
}

