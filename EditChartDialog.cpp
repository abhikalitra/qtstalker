/*
 *  Qtstalker stock charter
 *
 *  Copyright (C) 2001,2002 Stefan S. Stratigakos
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

#include "EditChartDialog.h"
#include "delete.xpm"
#include "ok.xpm"
#include "stop.xpm"
#include "export.xpm"
#include <qtooltip.h>

EditChartDialog::EditChartDialog (Config *c, QString cp) : QDialog (0, "EditChartDialog", TRUE)
{
  config = c;
  chartPath = cp;
  details = 0;
  record = 0;

  setCaption(tr("Qtstalker: Edit Chart"));
  
  QVBoxLayout *vbox = new QVBoxLayout(this);
  vbox->setMargin(5);
  vbox->setSpacing(5);
  
  toolbar = new QGridLayout(vbox, 1, 5);
  toolbar->setSpacing(1);

  okButton = new QToolButton(this);
  QToolTip::add(okButton, tr("OK"));
  okButton->setPixmap(QPixmap(ok));
  connect(okButton, SIGNAL(clicked()), this, SLOT(accept()));
  okButton->setMaximumWidth(30);
  okButton->setAutoRaise(TRUE);
  toolbar->addWidget(okButton, 0, 0);
  okButton->setEnabled(FALSE);

  cancelButton = new QToolButton(this);
  QToolTip::add(cancelButton, tr("Cancel"));
  cancelButton->setPixmap(QPixmap(stop));
  connect(cancelButton, SIGNAL(clicked()), this, SLOT(reject()));
  cancelButton->setMaximumWidth(30);
  cancelButton->setAutoRaise(TRUE);
  toolbar->addWidget(cancelButton, 0, 1);

  deleteButton = new QToolButton(this);
  QToolTip::add(deleteButton, tr("Delete Record"));
  deleteButton->setPixmap(QPixmap(deletefile));
  connect(deleteButton, SIGNAL(clicked()), this, SLOT(deleteRecord()));
  deleteButton->setMaximumWidth(30);
  deleteButton->setAutoRaise(TRUE);
  toolbar->addWidget(deleteButton, 0, 2);
  deleteButton->setEnabled(FALSE);

  saveButton = new QToolButton(this);
  QToolTip::add(saveButton, tr("Save Record"));
  saveButton->setPixmap(QPixmap(exportfile));
  connect(saveButton, SIGNAL(clicked()), this, SLOT(saveRecord()));
  saveButton->setMaximumWidth(30);
  saveButton->setAutoRaise(TRUE);
  toolbar->addWidget(saveButton, 0, 3);
  saveButton->setEnabled(FALSE);
  
  tab = new QTabWidget(this);
  vbox->addWidget(tab);

  // create details page

  QWidget *w = new QWidget(this);

  QVBoxLayout *vbox2 = new QVBoxLayout(w);
  vbox2->setMargin(5);

  detailList = new SettingView (w, config->getData(Config::DataPath));
  vbox2->addWidget(detailList);

  tab->addTab(w, tr("Details"));

  // create data page

  w = new QWidget(this);

  vbox2 = new QVBoxLayout(w);
  vbox2->setMargin(5);
  vbox2->setSpacing(5);

  date = new QDateEdit(QDate::currentDate(), w);
  date->setAutoAdvance(TRUE);
  date->setOrder(QDateEdit::YMD);
  connect(date, SIGNAL(valueChanged(const QDate &)), this, SLOT(dateChanged(const QDate &)));
  vbox2->addWidget(date);

  recordList = new SettingView (w, config->getData(Config::DataPath));
  vbox2->addWidget(recordList);

  tab->addTab(w, tr("Data"));

  db = new ChartDb();
  db->openChart(chartPath);
  details = db->getDetails();
  detailList->setItems(details);
}

EditChartDialog::~EditChartDialog ()
{
  db->saveDetails();
  delete db;
  
  if (record)
    delete record;
}

void EditChartDialog::deleteRecord ()
{
  db->deleteRecord(record);
  recordList->clear();
  delete record;
  record = 0;
  saveButton->setEnabled(FALSE);
  deleteButton->setEnabled(FALSE);
}

void EditChartDialog::saveRecord ()
{
  db->setRecord(record);
}

void EditChartDialog::dateChanged (const QDate &d)
{
  QString key = d.toString("yyyyMMdd");
  key.append("000000");

  recordList->clear();

  if (record)
  {
    delete record;
    record = 0;
  }

  QString data = db->getData(key);
  if (! data.length())
  {
    saveButton->setEnabled(FALSE);
    deleteButton->setEnabled(FALSE);
    return;
  }

  record = db->getRecord(key, data);
  if (! record)
    return;

  record->setType("Date", Setting::None);

  recordList->setItems(record);

  saveButton->setEnabled(TRUE);
  deleteButton->setEnabled(TRUE);
}



