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

#include "ScannerEditDialog.h"
#include "Globals.h"
#include "ScannerDataBase.h"
#include "DateRange.h"
#include "ScannerConfig.h"

#include <QtDebug>
#include <QComboBox>
#include <QDoubleSpinBox>
#include <QToolButton>
#include <QIcon>
#include <QFormLayout>
#include <QGroupBox>

ScannerEditDialog::ScannerEditDialog (QString name)
{
  _scanner.setName(name);
//  _helpFile = "Scanner.html";

  QString s = "Qtstalker" + g_session + ": " + tr("Scanner") + " - " + name;
  setWindowTitle(s);
  
  createMainPage();

  setSettings();

  _saveFlag = FALSE;
  _okButton->setEnabled(FALSE);
}

ScannerEditDialog::~ScannerEditDialog ()
{
  saveSettings();
}

void ScannerEditDialog::saveSettings ()
{
  ScannerConfig config;
  config.transaction();

  // save app size and position
  QString k = "size" + _scanner.name();
  QSize sz = size();
  config.setData(k, sz);

  k = "position" + _scanner.name();
  QPoint pt = pos();
  config.setData(k, pt);

  config.commit();
}

void ScannerEditDialog::createMainPage ()
{
  QWidget *w = new QWidget;

  QVBoxLayout *vbox = new QVBoxLayout;
  vbox->setMargin(5);
  vbox->setSpacing(2);
  w->setLayout(vbox);

  QFormLayout *form = new QFormLayout;
  form->setMargin(0);
  form->setSpacing(2);
  vbox->addLayout(form);

  // symbol
  _symbols = new SymbolButton(w);
  connect(_symbols, SIGNAL(signalChanged()), this, SLOT(ruleChanged()));
  form->addRow(tr("Symbols"), _symbols);

  // bar length
  BarData bd;
  QStringList l;
  bd.getBarLengthList(l);
  
  _barLength = new QComboBox;
  _barLength->addItems(l);
  connect(_barLength, SIGNAL(activated(int)), this, SLOT(ruleChanged()));
  form->addRow(tr("Bar Length"), _barLength);

  // date range
  DateRange dr;
  l.clear();
  dr.list(l);
  
  _dateRange = new QComboBox;
  _dateRange->addItems(l);
  _dateRange->setToolTip(tr("The amount of bars to use for the indicator"));
  connect(_dateRange, SIGNAL(activated(int)), this, SLOT(ruleChanged()));
  form->addRow(tr("Date Range"), _dateRange);

  // group name
  _groupName = new QLineEdit;
  _groupName->setToolTip(tr("Scan results group name"));
  connect(_groupName, SIGNAL(textEdited(const QString &)), this, SLOT(ruleChanged()));
  form->addRow(tr("Group Name"), _groupName);

  // plot list
  QGroupBox *gbox = new QGroupBox;
  gbox->setTitle(tr("Rule"));
  vbox->addWidget(gbox);
  
  QVBoxLayout *tvbox = new QVBoxLayout;
  tvbox->setMargin(0);
  tvbox->setSpacing(0);
  gbox->setLayout(tvbox);
  
  _list = new IndicatorPlotList;
  connect(_list, SIGNAL(signalIndicatorChanged()), this, SLOT(ruleChanged()));
  connect(_list, SIGNAL(signalItemChanged()), this, SLOT(ruleChanged()));
  tvbox->addWidget(_list);
  
  _tabs->addTab(w, tr("Settings"));
}

void ScannerEditDialog::setSettings ()
{
  ScannerConfig config;

  // restore the size of the window
  QString k = "size" + _scanner.name();
  QSize sz;
  config.getData(k, sz);
  if (! sz.isNull())
    resize(sz);

  // restore the position of the window
  k = "position" + _scanner.name();
  QPoint p;
  config.getData(k, p);
  if (! p.isNull())
    move(p);

  ScannerDataBase db;
  db.getScanner(_scanner);

  _list->setIndicator(_scanner.indicator());
  _list->setSettings(_scanner.settings());
  _list->setList(_scanner.plots());

  _barLength->setCurrentIndex(_scanner.barLength());
  
  _dateRange->setCurrentIndex(_scanner.dateRange());

  _groupName->setText(_scanner.groupName());

  _symbols->setSymbols(_scanner.group());
}

void ScannerEditDialog::ruleChanged ()
{
  _saveFlag = TRUE;
  _okButton->setEnabled(_saveFlag);
}

void ScannerEditDialog::confirmYes ()
{
  _okButton->setEnabled(TRUE);
  _cancelButton->setEnabled(TRUE);

  done();
}

void ScannerEditDialog::cancel ()
{
  if (_saveFlag)
  {
    setMessage(tr("Settings modified. Save changes?"));
    setConfirm();
    _okButton->setEnabled(FALSE);
    _cancelButton->setEnabled(FALSE);
  }
  else
    reject();
}

void ScannerEditDialog::done ()
{
  // remove any forbidden sql characters
  QString gn = _groupName->text();
  gn = gn.remove(QString("'"), Qt::CaseSensitive);

  if (! gn.length())
  {
    setMessage(tr("Group name empty or invalid"));
    return;
  }

  _scanner.setGroupName(gn);

  _scanner.setGroup(_symbols->getSymbols());

  _scanner.setBarLength(_barLength->currentIndex());

  _scanner.setDateRange(_dateRange->currentIndex());

  _scanner.setIndicator(_list->indicator());
  _list->list(_scanner.plots());
  _scanner.setSettings(_list->settings());

  ScannerDataBase db;
  db.transaction();
  db.setScanner(_scanner);
  db.commit();
  
  accept();
}

