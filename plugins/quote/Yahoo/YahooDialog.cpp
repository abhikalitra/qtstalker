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
#include "ChartDb.h"
#include <qinputdialog.h>
#include <qdir.h>
#include <qmessagebox.h>
#include <qlayout.h>
#include <qlabel.h>

YahooDialog::YahooDialog () : QTabDialog (0, "YahooDialog", TRUE)
{
  dataPath = QDir::homeDirPath();
  dataPath.append("/Qtstalker/data/Stocks");

  QWidget *w = new QWidget(this);
  
  QHBoxLayout *hbox = new QHBoxLayout(w);
  
  QVBoxLayout *vbox = new QVBoxLayout(hbox);
  
  QGridLayout *grid = new QGridLayout(vbox, 5, 2);
  grid->setMargin(5);
  grid->setSpacing(5);
  grid->setColStretch(1, 1);
  
  QLabel *label = new QLabel(tr("Quote Type"), w);
  grid->addWidget(label, 0, 0);
  
  method = new QComboBox(w);
  method->insertItem(tr("History"), -1);
  method->insertItem(tr("Quote"), -1);
  grid->addWidget(method, 0, 1);
  
  label = new QLabel(tr("Start Date"), w);
  grid->addWidget(label, 1, 0);

  date = new QDateEdit(QDate::currentDate(), w);
  date->setAutoAdvance(TRUE);
  date->setOrder(QDateEdit::YMD);
  grid->addWidget(date, 1, 1);

  label = new QLabel(tr("End Date"), w);
  grid->addWidget(label, 2, 0);
  
  date2 = new QDateEdit(QDate::currentDate(), w);
  date2->setAutoAdvance(TRUE);
  date2->setOrder(QDateEdit::YMD);
  grid->addWidget(date2, 2, 1);
  
  adjustment = new QCheckBox(tr("Adjustment"), w);
  grid->addWidget(adjustment, 3, 0);
  
  list = new QListView(w);
  list->addColumn(tr("Symbol"), 200);
  list->setMultiSelection(TRUE);
  vbox->addWidget(list);

  grid = new QGridLayout(hbox, 4, 1);
  grid->setMargin(5);
  grid->setSpacing(5);
  
  newButton = new QPushButton(tr("New"), w);
  QObject::connect(newButton, SIGNAL(pressed()), this, SLOT(newStock()));
  grid->addWidget(newButton, 0, 0);
  
  selectButton = new QPushButton(tr("Select All"), w);
  QObject::connect(selectButton, SIGNAL(pressed()), this, SLOT(selectAll()));
  grid->addWidget(selectButton, 1, 0);
  
  unselectButton = new QPushButton(tr("Unselect All"), w);
  QObject::connect(unselectButton, SIGNAL(pressed()), this, SLOT(unselectAll()));
  grid->addWidget(unselectButton, 2, 0);
  
  addTab(w, tr("Yahoo"));

  setOkButton();
  setCancelButton();
    
  updateList();
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
      qDebug("Yahoo plugin: Unable to create directory");
      return;
    }
  }
  
  int loop;
  for (loop = 0; loop < (int) l.count(); loop++)
  {
    QString s = dataPath;
    s.append("/");
    s.append(l[loop]);
    if (dir.exists(s, TRUE))
      continue;

    ChartDb *db = new ChartDb();
    db->openChart(s);
    Setting *details = db->getDetails();
    details->set("Format", "Open|High|Low|Close|Volume", Setting::None);
    details->set("Chart Type", "Stock", Setting::None);
    details->set("Symbol", l[loop], Setting::None);
    details->set("Source", "Yahoo", Setting::None);
    details->set("Title", l[loop], Setting::Text);
    db->saveDetails();
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
    new QListViewItem(list, dir[loop]);
}

QStringList YahooDialog::getList ()
{
  QStringList l;
  QListViewItem *item = list->firstChild();
  while (item)
  {
    if (item->isSelected())
      l.append(item->text(0));
      
    item = item->nextSibling();
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
  method->setCurrentText(d);
}

