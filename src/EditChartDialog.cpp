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
#include <qlabel.h>

EditChartDialog::EditChartDialog (Config *c, QString cp) : EditDialog (c)
{
  chartPath = cp;
  details = 0;
  record = 0;

  setCaption(tr("Qtstalker: Edit Chart"));

  hideSettingView(TRUE);

  setButton("delete", QPixmap(deleteitem), tr("Delete Record"));
  connect(getButton("delete"), SIGNAL(clicked()), this, SLOT(deleteRecord()));
  setButtonStatus("delete", FALSE);

  setButton("save", QPixmap(exportfile), tr("Save Record"));
  connect(getButton("save"), SIGNAL(clicked()), this, SLOT(saveRecord()));
  setButtonStatus("save", FALSE);
  
  createDetailsPage();
  createDataPage();

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

void EditChartDialog::createDetailsPage ()
{
  QWidget *w = new QWidget(this);
  
  QVBoxLayout *vbox = new QVBoxLayout(w);
  vbox->setMargin(5);
  vbox->setSpacing(0);
  
  detailList = new SettingView (w, config->getData(Config::DataPath));
  vbox->addWidget(detailList);
  
  tabs->addTab(w, tr("Details"));  
}

void EditChartDialog::createDataPage ()
{
  QWidget *w = new QWidget(this);
  
  QVBoxLayout *vbox = new QVBoxLayout(w);
  vbox->setMargin(5);
  vbox->setSpacing(0);
  
  date = new QDateEdit(QDate::currentDate(), w);
  date->setAutoAdvance(TRUE);
  date->setOrder(QDateEdit::YMD);
  connect(date, SIGNAL(valueChanged(const QDate &)), this, SLOT(dateChanged(const QDate &)));
  vbox->addWidget(date);
  vbox->addSpacing(5);

  recordList = new SettingView (w, config->getData(Config::DataPath));
  vbox->addWidget(recordList);
  
  tabs->addTab(w, tr("Data"));  
}

void EditChartDialog::deleteRecord ()
{
  db->deleteRecord(record);
  recordList->clear();
  delete record;
  record = 0;
  setButtonStatus("delete", FALSE);
  setButtonStatus("save", FALSE);
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
    setButtonStatus("delete", FALSE);
    setButtonStatus("save", FALSE);
    return;
  }

  record = db->getRecord(key, data);
  if (! record)
    return;

  record->setType("Date", Setting::None);

  recordList->setItems(record);

  setButtonStatus("delete", TRUE);
  setButtonStatus("save", TRUE);
}



