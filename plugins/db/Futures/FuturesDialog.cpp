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

#include "FuturesDialog.h"
#include "Bar.h"
#include "BarDate.h"
#include "../../../src/delete.xpm"
#include "../../../src/export.xpm"
#include "../../../src/search.xpm"
#include "HelpWindow.h"
#include <qlabel.h>
#include <qlayout.h>
#include <qvalidator.h>
#include <qmessagebox.h>
#include <qpushbutton.h>
#include <qtooltip.h>
#include <qpixmap.h>
#include <qdir.h>

FuturesDialog::FuturesDialog (QString d, QString p) : QTabDialog (0, "FuturesDialog", TRUE)
{
  helpFile = p;

  saveRecordFlag = FALSE;
  ignoreSaveRecordFlag = FALSE;
  setCaption(tr("Qtstalker: Edit Futures"));
  
  db = new ChartDb;
  db->openChart(d);
  
  createDetailsPage();
  createDataPage();
  
  setOkButton(tr("&OK"));
  setCancelButton(tr("&Cancel"));
  connect(this, SIGNAL(applyButtonPressed()), this, SLOT(saveChart()));
  setHelpButton();
  QObject::connect(this, SIGNAL(helpButtonPressed()), this, SLOT(help()));
}

FuturesDialog::~FuturesDialog ()
{
  delete db;
}

void FuturesDialog::createDetailsPage ()
{
  QWidget *w = new QWidget(this);
    
  QGridLayout *grid = new QGridLayout(w);
  grid->setMargin(5);
  grid->setSpacing(5);
  
  QLabel *label = new QLabel(tr("Symbol"), w);
  grid->addWidget(label, 0, 0);
  
  QLineEdit *edit = new QLineEdit(db->getData("Symbol"), w);
  edit->setReadOnly(TRUE);
  grid->addWidget(edit, 0, 1);
  
  label = new QLabel(tr("Name"), w);
  grid->addWidget(label, 1, 0);
  
  title = new QLineEdit(db->getData("Title"), w);
  grid->addWidget(title, 1, 1);
  
  label = new QLabel(tr("Type"), w);
  grid->addWidget(label, 2, 0);
  
  edit = new QLineEdit(db->getData("Type"), w);
  edit->setReadOnly(TRUE);
  grid->addWidget(edit, 2, 1);
  
  label = new QLabel(tr("Futures Type"), w);
  grid->addWidget(label, 3, 0);
  
  edit = new QLineEdit(db->getData("FuturesType"), w);
  edit->setReadOnly(TRUE);
  grid->addWidget(edit, 3, 1);

  label = new QLabel(tr("Futures Month"), w);
  grid->addWidget(label, 4, 0);
  
  edit = new QLineEdit(db->getData("FuturesMonth"), w);
  edit->setReadOnly(TRUE);
  grid->addWidget(edit, 4, 1);
  
  grid->expand(grid->numRows() + 1, grid->numCols());
  grid->setColStretch(1, 1);
  
  addTab(w, tr("Details"));  
}

void FuturesDialog::createDataPage ()
{
  QWidget *w = new QWidget(this);
    
  QVBoxLayout *vbox = new QVBoxLayout(w);
  vbox->setMargin(5);
  vbox->setSpacing(0);

  toolbar = new Toolbar(w, 30, 30, FALSE);
  vbox->addWidget(toolbar);
  vbox->addSpacing(10);
  
  toolbar->addButton("delete", QPixmap(deleteitem), tr("Delete Record"));
  connect(toolbar->getButton("delete"), SIGNAL(clicked()), this, SLOT(deleteRecord()));
  toolbar->setButtonStatus("delete", FALSE);
  toolbar->getButton("delete")->setAccel(CTRL+Key_D);

  toolbar->addButton("save", QPixmap(exportfile), tr("Save Record"));
  connect(toolbar->getButton("save"), SIGNAL(clicked()), this, SLOT(saveRecord()));
  toolbar->setButtonStatus("save", FALSE);
  toolbar->getButton("save")->setAccel(CTRL+Key_S);

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
  QObject::connect(button, SIGNAL(clicked()), this, SLOT(slotDateSearch()));
  QToolTip::add(button, tr("Search"));
  button->setPixmap(search);
  grid->addWidget(button, 0, 2);
  button->setAccel(CTRL+Key_R);
  
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
  QIntValidator *iv = new QIntValidator(0, 999999999, this);
  oi->setValidator(iv);
  connect(oi, SIGNAL(textChanged(const QString &)), this, SLOT(textChanged(const QString &)));
  grid->addWidget(oi, 7, 1);
  
  grid->expand(grid->numRows() + 1, grid->numCols());
  grid->setColStretch(1, 1);
  
  addTab(w, tr("Data"));  
}

void FuturesDialog::deleteRecord ()
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

  db->deleteData(dateSearch->dateTime().toString("yyyyMMddmmhhss"));
  
  clearRecordFields();
  
  toolbar->setButtonStatus("delete", FALSE);
  toolbar->setButtonStatus("save", FALSE);
  saveRecordFlag = FALSE;
}

void FuturesDialog::saveRecord ()
{
  Bar *bar = new Bar;
  bar->setDate(date->text());
  bar->setOpen(open->text().toDouble());
  bar->setHigh(high->text().toDouble());
  bar->setLow(low->text().toDouble());
  bar->setClose(close->text().toDouble());
  bar->setVolume(volume->text().toDouble());
  bar->setOI(oi->text().toInt());
  db->setBar(bar);
  delete bar;
  
  toolbar->setButtonStatus("save", FALSE);
  saveRecordFlag = FALSE;
}

void FuturesDialog::slotDateSearch ()
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

  QString key = dateSearch->dateTime().toString("yyyyMMddmmhhss");

  clearRecordFields();
  
  QString data = db->getData(key);
  if (! data.length())
  {
    toolbar->setButtonStatus("delete", FALSE);
    toolbar->setButtonStatus("save", FALSE);
    return;
  }

  Bar *record = db->getBar(key, data);

  ignoreSaveRecordFlag = TRUE;
  date->setText(record->getDate().getDateTimeString(TRUE));
  open->setText(QString::number(record->getOpen()));
  high->setText(QString::number(record->getHigh()));
  low->setText(QString::number(record->getLow()));
  close->setText(QString::number(record->getClose()));
  volume->setText(QString::number(record->getVolume(), 'f', 0));
  oi->setText(QString::number(record->getOI(), 'f', 0));
  ignoreSaveRecordFlag = FALSE;
  
  delete record;
  
  toolbar->setButtonStatus("delete", TRUE);
  toolbar->setButtonStatus("save", FALSE);
}

void FuturesDialog::saveChart ()
{
  db->setData("Title", title->text());

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

void FuturesDialog::clearRecordFields ()
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

void FuturesDialog::textChanged (const QString &)
{
  if (! ignoreSaveRecordFlag)
  {
    saveRecordFlag = TRUE;
    toolbar->setButtonStatus("save", TRUE);
  }
}

void FuturesDialog::help ()
{
  HelpWindow *hw = new HelpWindow(this, helpFile);
  hw->show();
  reject();
}

