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
#include "Config.h"
#include "ChartDb.h"
#include "../../../src/newchart.xpm"
#include "../../../src/selectall.xpm"
#include "../../../src/unselectall.xpm"
#include "HelpWindow.h"
#include <qinputdialog.h>
#include <qdir.h>
#include <qmessagebox.h>
#include <qlayout.h>
#include <qlabel.h>
#include <qframe.h>
#include <qdir.h>

YahooDialog::YahooDialog (QString d) : QTabDialog (0, "YahooDialog", TRUE)
{
  Config config;
  QString s = config.getData(Config::HelpFilePath) + "/" + d;
  QDir dir;
  if (dir.exists(s))
    helpFile = s;

  dataPath = QDir::homeDirPath();
  dataPath.append("/Qtstalker/data/Stocks");

  QWidget *w = new QWidget(this);
  
  QVBoxLayout *vbox = new QVBoxLayout(w);
  vbox->setMargin(5);
  vbox->setSpacing(0);
  
  toolbar = new Toolbar(w, 30, 30, FALSE);
  vbox->addWidget(toolbar);
  
  toolbar->addButton("new", newchart, tr("New Symbol"));
  QObject::connect(toolbar->getButton("new"), SIGNAL(clicked()), this, SLOT(newStock()));
  
  toolbar->addButton("selectAll", selectall, tr("Select All Symbols"));
  QObject::connect(toolbar->getButton("selectAll"), SIGNAL(clicked()), this, SLOT(selectAll()));
  
  toolbar->addButton("unselectAll", unselectall, tr("Unselect All Symbols"));
  QObject::connect(toolbar->getButton("unselectAll"), SIGNAL(clicked()), this, SLOT(unselectAll()));
  
  vbox->addSpacing(10);
  
  QGridLayout *grid = new QGridLayout(vbox, 4, 2);
  grid->setSpacing(5);
  grid->setColStretch(1, 1);
  
  QLabel *label = new QLabel(tr("Method:"), w);
  grid->addWidget(label, 0, 0);
  
  method = new QComboBox(w);
  method->insertItem(tr("History"), 0);
  method->insertItem(tr("Quote"), 1);
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
  
  vbox->addSpacing(5);
  
  adjustment = new QCheckBox(tr("Adjustment"), w);
  vbox->addWidget(adjustment);
  vbox->addSpacing(10);
  
  QFrame *sep = new QFrame(w);
  sep->setFrameShape(QFrame::HLine);
  sep->setFrameShadow(QFrame::Sunken);
  vbox->addWidget(sep);
  vbox->addSpacing(10);
  
  label = new QLabel(tr("Download Symbols:"), w);
  vbox->addWidget(label);
  
  list = new QListBox(w);
  list->setMultiSelection(TRUE);
  vbox->addWidget(list);

  addTab(w, tr("Yahoo"));

  setOkButton();
  setCancelButton();
  setHelpButton();
  QObject::connect(this, SIGNAL(helpButtonPressed()), this, SLOT(help()));

  resize(300, 400);
      
  updateList();
  selectAll();
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
  
  QDir dir;
  if (! dir.exists(dataPath))
  {
    if (! dir.mkdir(dataPath))
    {
      qDebug("YahooDialog::newStock: Unable to create directory");
      return;
    }
  }
  
  int loop;
  Config config;
  for (loop = 0; loop < (int) l.count(); loop++)
  {
    QString s = dataPath;
    s.append("/");
    s.append(l[loop]);
    if (dir.exists(s, TRUE))
      continue;

    ChartDb *db = new ChartDb;
    db->setPlugin("Stocks");
    if (db->openChart(s))
      qDebug("YahooDialog::newStock: could not open db %s", s.latin1());
    delete db;
  }
  
  updateList();
}

void YahooDialog::updateList ()
{
  list->clear();
  
  QDir dir(dataPath);
  int loop;
  for (loop = 2; loop < (int) dir.count(); loop++)
    list->insertItem(dir[loop]);
}

QStringList YahooDialog::getList ()
{
  QStringList l;
  int loop;
  for (loop = 0; loop < (int) list->count(); loop++)
  {
    if (list->isSelected(loop))
      l.append(list->text(loop));
  }
  
  return l;
}

void YahooDialog::selectAll ()
{
  list->selectAll(TRUE);
}

void YahooDialog::unselectAll ()
{
  list->selectAll(FALSE);
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
  if (! d.compare(tr("History")))
    method->setCurrentItem(0);
  else
    method->setCurrentItem(1);
    
  methodChanged(0);
}

void YahooDialog::methodChanged (int)
{
  if (method->currentItem() == 1)
  {
    adjustment->setEnabled(FALSE);
    date->setEnabled(FALSE);
    date2->setEnabled(FALSE);
  }
  else
  {
    adjustment->setEnabled(TRUE);
    date->setEnabled(TRUE);
    date2->setEnabled(TRUE);
  }
}

void YahooDialog::help ()
{
  if (! helpFile.length())
    return;
    
  HelpWindow *hw = new HelpWindow(this, helpFile);
  hw->show();
}

