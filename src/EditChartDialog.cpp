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

#include "EditChartDialog.h"
#include "delete.xpm"
#include "export.xpm"

EditChartDialog::EditChartDialog (Config *c, QString cp) : EditDialog (c)
{
  chartPath = cp;
  details = 0;
  record = 0;

  setCaption(tr("Qtstalker: Edit Chart"));

  hideSettingView(TRUE);

  setButtonStatus(0, FALSE);

  setButton(QPixmap(deletefile), tr("Delete Record"), 2);
  connect(getButton(2), SIGNAL(clicked()), this, SLOT(deleteRecord()));
  setButtonStatus(2, FALSE);

  setButton(QPixmap(exportfile), tr("Save Record"), 3);
  connect(getButton(3), SIGNAL(clicked()), this, SLOT(saveRecord()));
  setButtonStatus(3, FALSE);

  tab = new QTabWidget(this);
  basebox->addWidget(tab);

  // create details page

  QWidget *w = new QWidget(this);

  QVBoxLayout *vbox = new QVBoxLayout(w);
  vbox->setMargin(5);

  detailList = new SettingView (w, config->getData(Config::DataPath));
  vbox->addWidget(detailList);

  tab->addTab(w, tr("Details"));

  // create data page

  w = new QWidget(this);

  vbox = new QVBoxLayout(w);
  vbox->setMargin(5);
  vbox->setSpacing(5);

  date = new QDateEdit(QDate::currentDate(), w);
  date->setAutoAdvance(TRUE);
  date->setOrder(QDateEdit::YMD);
  connect(date, SIGNAL(valueChanged(const QDate &)), this, SLOT(dateChanged(const QDate &)));
  vbox->addWidget(date);

  recordList = new SettingView (w, config->getData(Config::DataPath));
  vbox->addWidget(recordList);

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
  setButtonStatus(2, FALSE);
  setButtonStatus(3, FALSE);
}

void EditChartDialog::saveRecord ()
{
  recordList->updateSettings();
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
    setButtonStatus(2, FALSE);
    setButtonStatus(3, FALSE);
    return;
  }

  record = db->getRecord(key, data);
  if (! record)
    return;

  record->setType("Date", Setting::None);

  recordList->setItems(record);

  setButtonStatus(2, TRUE);
  setButtonStatus(3, TRUE);
}



