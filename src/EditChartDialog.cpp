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
#include "search.xpm"
#include <qlabel.h>
#include <qlayout.h>
#include <qvalidator.h>
#include <qmessagebox.h>
#include <qpushbutton.h>
#include <qtooltip.h>
#include <qpixmap.h>

EditChartDialog::EditChartDialog (QString cp) : QTabDialog (0, "EditChartDialog", TRUE)
{
  saveRecordFlag = FALSE;
  ignoreSaveRecordFlag = FALSE;
  record = 0;
  setCaption(tr("Qtstalker: Edit Chart"));
  
  db = new ChartDb();
  db->openChart(cp);
  
  createDetailsPage();
  createDataPage();
  
  setOkButton();
  setCancelButton();
  connect(this, SIGNAL(applyButtonPressed()), this, SLOT(saveChart()));
}

EditChartDialog::~EditChartDialog ()
{
  delete db;
  
  if (record)
    delete record;
}

void EditChartDialog::createDetailsPage ()
{
  QWidget *w = new QWidget(this);
    
  QGridLayout *grid = new QGridLayout(w);
  grid->setMargin(5);
  grid->setSpacing(5);
  
  QLabel *label = new QLabel(tr("Symbol"), w);
  grid->addWidget(label, 0, 0);
  
  QLineEdit *edit = new QLineEdit(db->getDetail(ChartDb::Symbol), w);
  edit->setReadOnly(TRUE);
  grid->addWidget(edit, 0, 1);
  
  label = new QLabel(tr("Name"), w);
  grid->addWidget(label, 1, 0);
  
  title = new QLineEdit(db->getDetail(ChartDb::Title), w);
  grid->addWidget(title, 1, 1);
  
  label = new QLabel(tr("Type"), w);
  grid->addWidget(label, 2, 0);
  
  edit = new QLineEdit(db->getDetail(ChartDb::Type), w);
  edit->setReadOnly(TRUE);
  grid->addWidget(edit, 2, 1);
  
  QString s = db->getDetail(ChartDb::FuturesType);
  if (s.length())
  {
    label = new QLabel(tr("Futures Symbol"), w);
    grid->addWidget(label, 3, 0);
  
    edit = new QLineEdit(s, w);
    edit->setReadOnly(TRUE);
    grid->addWidget(edit, 3, 1);
    
    label = new QLabel(tr("Futures Month"), w);
    grid->addWidget(label, 4, 0);
  
    edit = new QLineEdit(db->getDetail(ChartDb::FuturesMonth), w);
    edit->setReadOnly(TRUE);
    grid->addWidget(edit, 4, 1);
  }
  
  grid->expand(grid->numRows() + 1, grid->numCols());
  grid->setColStretch(1, 1);
  
  addTab(w, tr("Details"));  
}

void EditChartDialog::createDataPage ()
{
  QWidget *w = new QWidget(this);
    
  QVBoxLayout *vbox = new QVBoxLayout(w);
  vbox->setMargin(5);
  vbox->setSpacing(0);

  toolbar = new Toolbar(w, 30, 30);
  vbox->addWidget(toolbar);
  vbox->addSpacing(10);
  
  toolbar->addButton("delete", QPixmap(deleteitem), tr("Delete Record"));
  connect(toolbar->getButton("delete"), SIGNAL(clicked()), this, SLOT(deleteRecord()));
  toolbar->setButtonStatus("delete", FALSE);

  toolbar->addButton("save", QPixmap(exportfile), tr("Save Record"));
  connect(toolbar->getButton("save"), SIGNAL(clicked()), this, SLOT(saveRecord()));
  toolbar->setButtonStatus("save", FALSE);

  QGridLayout *grid = new QGridLayout(vbox);
  grid->setSpacing(5);
    
  QLabel *label = new QLabel(tr("Search"), w);
  grid->addWidget(label, 0, 0);
  
  QDateTime dt = QDateTime::currentDateTime();
  dt.setTime(QTime(0, 0, 0));
  dateSearch = new QDateTimeEdit(dt, w);
  dateSearch->setAutoAdvance(TRUE);
  dateSearch->dateEdit()->setOrder(QDateEdit::YMD);
  grid->addWidget(dateSearch, 0, 1);
  
  QPushButton *button = new QPushButton(tr("Search"), w);
  QObject::connect(button, SIGNAL(pressed()), this, SLOT(slotDateSearch()));
  QToolTip::add(button, tr("Search"));
  button->setPixmap(search);
  grid->addWidget(button, 0, 2);
  
  label = new QLabel(tr("Date"), w);
  grid->addWidget(label, 1, 0);
  
  date = new QLineEdit(w);
  date->setReadOnly(TRUE);
  grid->addWidget(date, 1, 1);

  label = new QLabel(tr("Open"), w);
  grid->addWidget(label, 2, 0);
  
  open = new QLineEdit(w);
  QDoubleValidator *dv = new QDoubleValidator(0, 99999999999.0, 4, this, 0);
  open->setValidator(dv);
  connect(open, SIGNAL(textChanged(const QString &)), this, SLOT(textChanged(const QString &)));
  grid->addWidget(open, 2, 1);

  label = new QLabel(tr("High"), w);
  grid->addWidget(label, 3, 0);
  
  high = new QLineEdit(w);
  dv = new QDoubleValidator(0, 99999999999.0, 4, this, 0);
  high->setValidator(dv);
  connect(high, SIGNAL(textChanged(const QString &)), this, SLOT(textChanged(const QString &)));
  grid->addWidget(high, 3, 1);
  
  label = new QLabel(tr("Low"), w);
  grid->addWidget(label, 4, 0);
  
  low = new QLineEdit(w);
  dv = new QDoubleValidator(0, 99999999999.0, 4, this, 0);
  low->setValidator(dv);
  connect(low, SIGNAL(textChanged(const QString &)), this, SLOT(textChanged(const QString &)));
  grid->addWidget(low, 4, 1);

  label = new QLabel(tr("Close"), w);
  grid->addWidget(label, 5, 0);
  
  close = new QLineEdit(w);
  dv = new QDoubleValidator(0, 99999999999.0, 4, this, 0);
  close->setValidator(dv);
  connect(close, SIGNAL(textChanged(const QString &)), this, SLOT(textChanged(const QString &)));
  grid->addWidget(close, 5, 1);

  label = new QLabel(tr("Volume"), w);
  grid->addWidget(label, 6, 0);
  
  volume = new QLineEdit(w);
  dv = new QDoubleValidator(0, 99999999999.0, 0, this, 0);
  volume->setValidator(dv);
  connect(volume, SIGNAL(textChanged(const QString &)), this, SLOT(textChanged(const QString &)));
  grid->addWidget(volume, 6, 1);

  label = new QLabel(tr("OI"), w);
  grid->addWidget(label, 7, 0);
  
  oi = new QLineEdit(w);
  dv = new QDoubleValidator(0, 99999999999.0, 0, this, 0);
  oi->setValidator(dv);
  connect(oi, SIGNAL(textChanged(const QString &)), this, SLOT(textChanged(const QString &)));
  grid->addWidget(oi, 7, 1);
        
  grid->expand(grid->numRows() + 1, grid->numCols());
  grid->setColStretch(1, 1);
  
  addTab(w, tr("Data"));  
}

void EditChartDialog::deleteRecord ()
{
  if (saveRecordFlag)
  {  
    int rc = QMessageBox::warning(this,
    			          tr("Delete record."),
			          tr("Are you sure you want to delete record?"),
			          QMessageBox::Yes,
			          QMessageBox::No,
			          QMessageBox::NoButton);

    if (rc == QMessageBox::No)
      return;
  }

  db->deleteData(record->getDate().getDateTimeString(TRUE));
  
  clearRecordFields();
  
  delete record;
  record = 0;
  
  toolbar->setButtonStatus("delete", FALSE);
  toolbar->setButtonStatus("save", FALSE);
  saveRecordFlag = FALSE;
}

void EditChartDialog::saveRecord ()
{
  record->setOpen(open->text().toDouble());
  record->setHigh(high->text().toDouble());
  record->setLow(low->text().toDouble());
  record->setClose(close->text().toDouble());
  record->setVolume(volume->text().toDouble());
  record->setOI(oi->text().toInt());
  
  db->setBar(record);
  
  toolbar->setButtonStatus("save", FALSE);
  saveRecordFlag = FALSE;
}

void EditChartDialog::slotDateSearch ()
{
  if (saveRecordFlag)
  {  
    int rc = QMessageBox::warning(this,
    			          tr("Warning"),
			          tr("Record has been modified.\nSave changes?"),
			          QMessageBox::Yes,
			          QMessageBox::No,
			          QMessageBox::NoButton);

    if (rc == QMessageBox::Yes)
      saveRecord();
    else
      saveRecordFlag = FALSE;
  }

  QString key = dateSearch->dateTime().toString("yyyy-MM-ddmm:hh:ss");

  clearRecordFields();
  
  if (record)
  {
    delete record;
    record = 0;
  }

  QString data = db->getData(key);
  if (! data.length())
  {
    toolbar->setButtonStatus("delete", FALSE);
    toolbar->setButtonStatus("save", FALSE);
    return;
  }

  record = db->getBar(key, data);
  if (! record)
    return;

  ignoreSaveRecordFlag = TRUE;
  date->setText(record->getDate().getDateTimeString(TRUE));
  open->setText(QString::number(record->getOpen()));
  high->setText(QString::number(record->getHigh()));
  low->setText(QString::number(record->getLow()));
  close->setText(QString::number(record->getClose()));
  volume->setText(QString::number(record->getVolume(), 'f', 0));
  oi->setText(QString::number(record->getOI()));
  ignoreSaveRecordFlag = FALSE;
  
  toolbar->setButtonStatus("delete", TRUE);
  toolbar->setButtonStatus("save", FALSE);
}

void EditChartDialog::saveChart ()
{
  db->setDetail(ChartDb::Title, title->text());

  if (saveRecordFlag)
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

void EditChartDialog::clearRecordFields ()
{
  ignoreSaveRecordFlag = TRUE;
  date->clear();
  open->clear();
  high->clear();
  low->clear();
  close->clear();
  volume->clear();
  oi->clear();
  ignoreSaveRecordFlag = FALSE;
}

void EditChartDialog::textChanged (const QString &)
{
  if (! ignoreSaveRecordFlag)
  {
    saveRecordFlag = TRUE;
    toolbar->setButtonStatus("save", TRUE);
  }
}


