/*
 *  Qtstalker stock charter
 *
 *  Copyright (C) 2001-2004 Stefan S. Stratigakos
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
#include "DbPlugin.h"
#include "../../../pics/newchart.xpm"
#include "HelpWindow.h"
#include "Config.h"
#include <qinputdialog.h>
#include <qdir.h>
#include <qmessagebox.h>
#include <qlayout.h>
#include <qlabel.h>
#include <qframe.h>
#include <qdir.h>


YahooDialog::YahooDialog (QWidget *p, QString &d) : QTabDialog (p, "YahooDialog", TRUE)
{
  helpFile = d;

  Config config;
  config.getData(Config::DataPath, dataPath);
  dataPath.append("/Stocks/Yahoo");

  QWidget *w = new QWidget(this);
  
  QVBoxLayout *vbox = new QVBoxLayout(w);
  vbox->setMargin(5);
  vbox->setSpacing(0);
  
  toolbar = new Toolbar(w, 30, 30, FALSE);
  vbox->addWidget(toolbar);

  QString s = "new";
  QString s2 = tr("New Symbol");
  toolbar->addButton(s, newchart, s2);
  QObject::connect(toolbar->getButton(s), SIGNAL(clicked()), this, SLOT(newStock()));
  
  vbox->addSpacing(5);
  
  QGridLayout *grid = new QGridLayout(vbox, 6, 2);
  grid->setSpacing(5);
  grid->setColStretch(1, 1);
  
  QLabel *label = new QLabel(tr("Method:"), w);
  grid->addWidget(label, 0, 0);
  
  method = new QComboBox(w);
  method->insertItem("History", 0);
  method->insertItem("Auto History", 1);
  method->insertItem("Quote", 2);
  method->insertItem("Fundamental", 3);
  QObject::connect(method, SIGNAL(activated(int)), this, SLOT(methodChanged(int)));
  grid->addWidget(method, 0, 1);
  
  label = new QLabel(tr("Start Date:"), w);
  grid->addWidget(label, 1, 0);

  date = new QDateEdit(QDate::currentDate(), w);
  date->setAutoAdvance(TRUE);
  date->setOrder(QDateEdit::YMD);
  grid->addWidget(date, 1, 1);

  label = new QLabel(tr("End Date:"), w);
  grid->addWidget(label, 2, 0);
  
  date2 = new QDateEdit(QDate::currentDate(), w);
  date2->setAutoAdvance(TRUE);
  date2->setOrder(QDateEdit::YMD);
  grid->addWidget(date2, 2, 1);
  
  label = new QLabel(tr("Error Retry"), w);
  grid->addWidget(label, 3, 0);
  
  retries = new QSpinBox(0, 99, 1, w);
  grid->addWidget(retries, 3, 1);
  
  label = new QLabel(tr("Timeout"), w);
  grid->addWidget(label, 4, 0);
  
  timeout = new QSpinBox(1, 99, 1, w);
  grid->addWidget(timeout, 4, 1);
  
  adjustment = new QCheckBox(tr("Adjustment"), w);
  grid->addWidget(adjustment, 5, 0);
  
  vbox->addSpacing(5);
  
  QFrame *sep = new QFrame(w);
  sep->setFrameShape(QFrame::HLine);
  sep->setFrameShadow(QFrame::Sunken);
  vbox->addWidget(sep);
  vbox->addSpacing(5);
  
  QHBoxLayout *hbox = new QHBoxLayout(vbox);
  hbox->setSpacing(5);
  
  allSymbols = new QCheckBox(tr("All Symbols"), w);
  connect(allSymbols, SIGNAL(toggled(bool)), this, SLOT(allSymbolsChecked(bool)));
  hbox->addWidget(allSymbols);

  QStringList l;  
  list = new FileButton(w, l, dataPath);
  hbox->addWidget(list);
  
  vbox->addStretch(1);

  addTab(w, tr("Yahoo"));

  setOkButton();
  setCancelButton();
  setHelpButton();
  QObject::connect(this, SIGNAL(helpButtonPressed()), this, SLOT(help()));
}

YahooDialog::~YahooDialog ()
{
}

void YahooDialog::newStock ()
{
  bool ok = FALSE;
  QString symbols = QInputDialog::getText(tr("New Yahoo Symbols"),
                                          tr("Enter symbols to add. Note: separate symbols with a space"),
				          QLineEdit::Normal,
				          QString::null,
				          &ok,
				          this);
  if (! symbols.length())
    return;

  QStringList l = QStringList::split(" ", symbols, FALSE);
  
  Config config;
  QString s;
  config.getData(Config::DataPath, s);
  s.append("/Stocks");
  QDir dir(s);
  if (! dir.exists(s))
  {
    if (! dir.mkdir(s))
    {
      qDebug("YahooDialog::newStock: Unable to create %s directory", s.latin1());
      return;
    }
  }
  
  if (! dir.exists(dataPath))
  {
    if (! dir.mkdir(dataPath))
    {
      qDebug("YahooDialog::newStock: Unable to create %s directory", dataPath.latin1());
      return;
    }
  }
  
  int loop;
  for (loop = 0; loop < (int) l.count(); loop++)
  {
    QString s = dataPath + "/";
    QFileInfo fi(l[loop]);
    if (fi.extension(FALSE).length())
    {
      s.append(fi.extension(FALSE).upper());
      if (! dir.exists(s, TRUE))
      {
        if (! dir.mkdir(s))
	{
          qDebug("YahooDialog::newStock: Unable to create %s directory", s.latin1());
          continue;
	}
      }
    }
    else
    {
      s.append("US");
      if (! dir.exists(s, TRUE))
      {
        if (! dir.mkdir(s))
	{
          qDebug("YahooDialog::newStock: Unable to create %s directory", s.latin1());
          continue;
	}
      }
    }
    s.append("/");
    s.append(l[loop]);
    if (dir.exists(s, TRUE))
      continue;

    DbPlugin db;
    if (db.openChart(s))
    {
      qDebug("YahooDialog::newStock: could not open db %s", s.latin1());
      db.close();
      continue;
    }
          
    db.createNew(DbPlugin::Stock);
    db.close();
  }
}

void YahooDialog::setList (QStringList &l)
{
  list->setFile(l);
}

void YahooDialog::getList (QStringList &l)
{
  l.clear();
  list->getFile(l);
}

void YahooDialog::setAdjustment (bool d)
{
  adjustment->setChecked(d);
}

bool YahooDialog::getAdjustment ()
{
  return adjustment->isChecked();
}

void YahooDialog::setStartDate (QDateTime dt)
{
  date->setDate(dt.date());
}

QDateTime YahooDialog::getStartDate ()
{
  QDateTime dt;
  dt.setDate(date->date());
  return dt;
}

void YahooDialog::setEndDate (QDateTime dt)
{
  date2->setDate(dt.date());
}

QDateTime YahooDialog::getEndDate ()
{
  QDateTime dt;
  dt.setDate(date2->date());
  return dt;
}

QString YahooDialog::getMethod ()
{
  return method->currentText();
}

void YahooDialog::setMethod (QString d)
{
  if (! d.compare("History"))
  {
    method->setCurrentItem(0);
    methodChanged(0);
  }
  else
  {
    if (! d.compare("Auto History"))
    {
      method->setCurrentItem(1);
      methodChanged(1);
    }
    else
    {
      if (! d.compare("Quote"))
      {
        method->setCurrentItem(2);
        methodChanged(2);
      }
      else
      {
        method->setCurrentItem(3);
        methodChanged(3);
      }
    }
  }
}

void YahooDialog::methodChanged (int d)
{
  switch (d)
  {
    case 0:
      adjustment->setEnabled(TRUE);
      date->setEnabled(TRUE);
      date2->setEnabled(TRUE);
      break;
    case 1:
      adjustment->setEnabled(TRUE);
      date->setEnabled(FALSE);
      date2->setEnabled(FALSE);
      break;
    case 2:
      adjustment->setEnabled(FALSE);
      date->setEnabled(FALSE);
      date2->setEnabled(FALSE);
      break;
    case 3:
      adjustment->setEnabled(FALSE);
      date->setEnabled(FALSE);
      date2->setEnabled(FALSE);
      break;
    default:
      break;
  }
}

void YahooDialog::setRetries (int d)
{
  retries->setValue(d);
}

int YahooDialog::getRetries ()
{
  return retries->value();
}

void YahooDialog::setTimeout (int d)
{
  timeout->setValue(d);
}

int YahooDialog::getTimeout ()
{
  return timeout->value();
}

void YahooDialog::help ()
{
  HelpWindow *hw = new HelpWindow(this, helpFile);
  hw->show();
}

void YahooDialog::allSymbolsChecked (bool d)
{
  if (d)
    list->setEnabled(FALSE);
  else
    list->setEnabled(TRUE);
}

void YahooDialog::setAllSymbols (bool d)
{
  allSymbols->setChecked(d);
}

bool YahooDialog::getAllSymbols ()
{
  return allSymbols->isChecked();
}
