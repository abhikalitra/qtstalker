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

#include "StocksDialog.h"
#include "Bar.h"
#include "HelpWindow.h"
#include <qlabel.h>
#include <qlayout.h>
#include <qmessagebox.h>

StocksDialog::StocksDialog (QString p, DbPlugin *d) : QTabDialog (0, "StocksDialog", TRUE)
{
  helpFile = p;
  db = d;

  setCaption(tr("Qtstalker: Edit Stock"));
  
  createDetailsPage();
  createDataPage();
  
  setOkButton(tr("&OK"));
  setCancelButton(tr("&Cancel"));
  connect(this, SIGNAL(applyButtonPressed()), this, SLOT(saveChart()));
  
  setHelpButton();
  QObject::connect(this, SIGNAL(helpButtonPressed()), this, SLOT(help()));
}

StocksDialog::~StocksDialog ()
{
}

void StocksDialog::createDetailsPage ()
{
  QWidget *w = new QWidget(this);
  
  QVBoxLayout *vbox = new QVBoxLayout(w);
  vbox->setMargin(5);
  vbox->setSpacing(5);
    
  QGridLayout *grid = new QGridLayout(vbox);
  grid->setMargin(0);
  grid->setSpacing(5);
  
  QLabel *label = new QLabel(tr("Symbol"), w);
  grid->addWidget(label, 0, 0);

  QString s;
  db->getHeaderField(DbPlugin::Symbol, s);
  label = new QLabel(s, w);
  label->setFrameStyle(QFrame::WinPanel | QFrame::Sunken);
  grid->addWidget(label, 0, 1);

  label = new QLabel(tr("Name"), w);
  grid->addWidget(label, 1, 0);
  
  db->getHeaderField(DbPlugin::Title, s);
  title = new QLineEdit(s, w);
  grid->addWidget(title, 1, 1);
  
  label = new QLabel(tr("Type"), w);
  grid->addWidget(label, 2, 0);
  
  db->getHeaderField(DbPlugin::Type, s);
  label = new QLabel(s, w);
  label->setFrameStyle(QFrame::WinPanel | QFrame::Sunken);
  grid->addWidget(label, 2, 1);
  
  label = new QLabel(tr("First Date"), w);
  grid->addWidget(label, 3, 0);
  
  Bar *bar = db->getFirstBar();
  if (bar)
  {
    bar->getDate().getDateTimeString(TRUE, s);
    label = new QLabel(s, w);
    label->setFrameStyle(QFrame::WinPanel | QFrame::Sunken);
    grid->addWidget(label, 3, 1);
    delete bar;
  }
  
  label = new QLabel(tr("Last Date"), w);
  grid->addWidget(label, 4, 0);
  
  bar = db->getLastBar();
  if (bar)
  {
    bar->getDate().getDateTimeString(TRUE, s);
    label = new QLabel(s, w);
    label->setFrameStyle(QFrame::WinPanel | QFrame::Sunken);
    grid->addWidget(label, 4, 1);
    delete bar;
  }
  
  grid->expand(grid->numRows() + 1, grid->numCols());
  grid->setColStretch(1, 1);
  
  // fundamentals section
    
  Setting fund;
  s = "Fundamentals";
  QString s2;
  db->getData(s, s2); 
  fund.parse(s2);
  
  s = tr("Fundamentals: last updated ");
  s.append(fund.getData("updateDate"));
  fund.remove("updateDate");
  QStringList key;
  fund.getKeyList(key);
  key.sort();
  
  vbox->addSpacing(10);
  label = new QLabel(s, w);
  vbox->addWidget(label);
  
  fundView = new QListView(w);
  fundView->addColumn(tr("Description"));
  fundView->addColumn(tr("Value"));
  vbox->addWidget(fundView);
  
  int loop;
  for (loop = 0; loop < (int) key.count(); loop++)
    new QListViewItem(fundView, key[loop], fund.getData(key[loop]));
  
  if (! key.count())
    new QListViewItem(fundView, tr("No data available."));
    
  addTab(w, tr("Details"));  
}

void StocksDialog::createDataPage ()
{
  QWidget *w = new QWidget(this);
    
  QVBoxLayout *vbox = new QVBoxLayout(w);
  vbox->setMargin(5);
  vbox->setSpacing(0);

  barEdit = new BarEdit(w);
  QString s = tr("Open");  
  QString s2 = "Open";  
  barEdit->createField(s, s2, FALSE);
  s = tr("High");  
  s2 = "High";  
  barEdit->createField(s, s2, FALSE);
  s = tr("Low");  
  s2 = "Low";  
  barEdit->createField(s, s2, FALSE);
  s = tr("Close");  
  s2 = "Close";  
  barEdit->createField(s, s2, FALSE);
  s = tr("Volume");  
  s2 = "Volume";  
  barEdit->createField(s, s2, FALSE);
  connect(barEdit, SIGNAL(signalDeleteRecord(QString)), this, SLOT(deleteRecord(QString)));
  connect(barEdit, SIGNAL(signalSaveRecord()), this, SLOT(saveRecord()));
  connect(barEdit, SIGNAL(signalSearch(QString)), this, SLOT(slotDateSearch(QString)));
  vbox->addWidget(barEdit);
  
  addTab(w, tr("Data"));  
}

void StocksDialog::deleteRecord (QString k)
{
  db->deleteData(k);
}

void StocksDialog::saveRecord ()
{
  Bar bar;
  QString s = barEdit->getDate();
  bar.setDate(s);
  s = "Open";
  bar.setOpen(barEdit->getField(s).toDouble());
  s = "High";
  bar.setHigh(barEdit->getField(s).toDouble());
  s = "Low";
  bar.setLow(barEdit->getField(s).toDouble());
  s = "Close";
  bar.setClose(barEdit->getField(s).toDouble());
  s = "Volume";
  bar.setVolume(barEdit->getField(s).toDouble());
  db->setBar(bar);
}

void StocksDialog::slotDateSearch (QString k)
{
  Bar *record = db->getBar(k);
  if (! record)
    return;

  QString s;
  record->getDate().getDateTimeString(TRUE, s);
  barEdit->setDate(s, record->getTickFlag());
  
  s = "Open";
  QString s2 = QString::number(record->getOpen());
  barEdit->setField(s, s2);
  
  s = "High";
  s2 = QString::number(record->getHigh());
  barEdit->setField(s, s2);
  
  s = "Low";
  s2 = QString::number(record->getLow());
  barEdit->setField(s, s2);
  
  s = "Close";
  s2 = QString::number(record->getClose());
  barEdit->setField(s, s2);
  
  s = "Volume";
  s2 = QString::number(record->getVolume(), 'f', 0);
  barEdit->setField(s, s2);
  
  s = "OI";
  s2 = QString::number(record->getOI(), 'f', 0);
  barEdit->setField(s, s2);
  
  delete record;
}

void StocksDialog::saveChart ()
{
  QString s = title->text();
  db->setHeaderField(DbPlugin::Title, s);

  if (barEdit->getSaveFlag())
  {  
    int rc = QMessageBox::warning(this,
    			          tr("Warning"),
			          tr("Record has been modified.\nSave changes?"),
			          QMessageBox::Yes,
			          QMessageBox::No,
			          QMessageBox::NoButton);

    if (rc == QMessageBox::Yes)
      saveRecord();
  }
  
  accept();
}

void StocksDialog::help ()
{
  HelpWindow *hw = new HelpWindow(this, helpFile);
  hw->show();
  reject();
}

