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
#include "BarDate.h"
#include "HelpWindow.h"
#include "../../../src/delete.xpm"
#include "../../../src/export.xpm"
#include "../../../src/search.xpm"
#include <qlabel.h>
#include <qlayout.h>
#include <qvalidator.h>
#include <qmessagebox.h>
#include <qpushbutton.h>
#include <qtooltip.h>
#include <qpixmap.h>
#include <qdir.h>

StocksDialog::StocksDialog (QString d, QString p) : QTabDialog (0, "StocksDialog", TRUE)
{
  helpFile = p;

  saveRecordFlag = FALSE;
  ignoreSaveRecordFlag = FALSE;
  setCaption(tr("Qtstalker: Edit Stock"));
  
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

StocksDialog::~StocksDialog ()
{
  delete db;
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
  
  QLineEdit *edit = new QLineEdit(db->getHeaderField(DbPlugin::Symbol), w);
  edit->setReadOnly(TRUE);
  grid->addWidget(edit, 0, 1);
  
  label = new QLabel(tr("Name"), w);
  grid->addWidget(label, 1, 0);
  
  title = new QLineEdit(db->getHeaderField(DbPlugin::Title), w);
  grid->addWidget(title, 1, 1);
  
  label = new QLabel(tr("Type"), w);
  grid->addWidget(label, 2, 0);
  
  edit = new QLineEdit(db->getHeaderField(DbPlugin::Type), w);
  edit->setReadOnly(TRUE);
  grid->addWidget(edit, 2, 1);
  
  grid->expand(grid->numRows() + 1, grid->numCols());
  grid->setColStretch(1, 1);
  
  // fundamentals section
    
  Setting fund;
  fund.parse(db->getHeaderField(DbPlugin::Lvar1));
  QStringList key = fund.getKeyList();
  key.sort();
  
  vbox->addSpacing(10);
  label = new QLabel(tr("Fundamentals"), w);
  vbox->addWidget(label);
  
  fundView = new QTextEdit(w);
  fundView->setTextFormat(Qt::LogText);
  fundView->setReadOnly(TRUE);
  vbox->addWidget(fundView);
  
  int loop;
  for (loop = 0; loop < (int) key.count(); loop++)
  {
    QString s = key[loop] + " = " + fund.getData(key[loop]);
    fundView->append(s);
  }
  
  if (! key.count())
    fundView->append(tr("No data available."));
    
  addTab(w, tr("Details"));  
}

void StocksDialog::createDataPage ()
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
  if (! db->getHeaderField(DbPlugin::BarType).toInt())
    dateSearch->timeEdit()->setEnabled(FALSE);
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

  grid->expand(grid->numRows() + 1, grid->numCols());
  grid->setColStretch(1, 1);
  
  addTab(w, tr("Data"));  
}

void StocksDialog::deleteRecord ()
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

  db->deleteBar(dateSearch->dateTime().toString("yyyyMMddmmhhss"));
  
  clearRecordFields();
  
  toolbar->setButtonStatus("delete", FALSE);
  toolbar->setButtonStatus("save", FALSE);
  saveRecordFlag = FALSE;
}

void StocksDialog::saveRecord ()
{
  Bar *bar = new Bar;
  bar->setDate(date->text());
  bar->setOpen(open->text().toDouble());
  bar->setHigh(high->text().toDouble());
  bar->setLow(low->text().toDouble());
  bar->setClose(close->text().toDouble());
  bar->setVolume(volume->text().toDouble());
  db->setBar(bar);
  delete bar;
  
  toolbar->setButtonStatus("save", FALSE);
  saveRecordFlag = FALSE;
}

void StocksDialog::slotDateSearch ()
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
  
  Bar *record = db->getBar(key);
  if (! record)
  {
    toolbar->setButtonStatus("delete", FALSE);
    toolbar->setButtonStatus("save", FALSE);
    return;
  }

  ignoreSaveRecordFlag = TRUE;
  date->setText(record->getDate().getDateTimeString(TRUE));
  open->setText(QString::number(record->getOpen()));
  high->setText(QString::number(record->getHigh()));
  low->setText(QString::number(record->getLow()));
  close->setText(QString::number(record->getClose()));
  volume->setText(QString::number(record->getVolume(), 'f', 0));
  ignoreSaveRecordFlag = FALSE;
  
  delete record;
  
  toolbar->setButtonStatus("delete", TRUE);
  toolbar->setButtonStatus("save", FALSE);
}

void StocksDialog::saveChart ()
{
  db->setHeaderField(DbPlugin::Title, title->text());

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

void StocksDialog::clearRecordFields ()
{
  ignoreSaveRecordFlag = TRUE;
  date->clear();
  open->clear();
  high->clear();
  low->clear();
  close->clear();
  volume->clear();
  ignoreSaveRecordFlag = FALSE;
}

void StocksDialog::textChanged (const QString &)
{
  if (! ignoreSaveRecordFlag)
  {
    saveRecordFlag = TRUE;
    toolbar->setButtonStatus("save", TRUE);
  }
}

void StocksDialog::help ()
{
  HelpWindow *hw = new HelpWindow(this, helpFile);
  hw->show();
  reject();
}

