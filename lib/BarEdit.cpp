/*
 *  Qtstalker stock charter
 *
 *  Copyright (C) 2001-2005 Stefan S. Stratigakos
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

#include "BarEdit.h"
#include "../pics/delete.xpm"
#include "../pics/export.xpm"
#include "../pics/search.xpm"
#include <qlabel.h>
#include <qvalidator.h>
#include <qmessagebox.h>
#include <qpushbutton.h>
#include <qtooltip.h>
#include <qpixmap.h>

BarEdit::BarEdit (QWidget *w) : QWidget (w)
{
  saveRecordFlag = FALSE;
  ignoreSaveRecordFlag = FALSE;
  editList.setAutoDelete(TRUE);
  saveLabel = "save";
  deleteLabel = "delete";

  QVBoxLayout *vbox = new QVBoxLayout(this);
  vbox->setMargin(5);
  vbox->setSpacing(0);

  toolbar = new Toolbar(this, 30, 30, FALSE);
  vbox->addWidget(toolbar);
  vbox->addSpacing(10);
  
  QString s = tr("Delete Record");
  toolbar->addButton(deleteLabel, QPixmap(deleteitem), s);
  connect(toolbar->getButton(deleteLabel), SIGNAL(clicked()), this, SLOT(deleteRecord()));
  toolbar->setButtonStatus(deleteLabel, FALSE);
  toolbar->getButton(deleteLabel)->setAccel(CTRL+Key_D);

  s = tr("Save Record");
  toolbar->addButton(saveLabel, QPixmap(exportfile), s);
  connect(toolbar->getButton(saveLabel), SIGNAL(clicked()), this, SLOT(saveRecord()));
  toolbar->setButtonStatus(saveLabel, FALSE);
  toolbar->getButton(saveLabel)->setAccel(CTRL+Key_S);

  grid = new QGridLayout(vbox);
  grid->setSpacing(2);
    
  QLabel *label = new QLabel(tr("Search"), this);
  grid->addWidget(label, 0, 0);
  
  QDateTime dt = QDateTime::currentDateTime();
  dt.setTime(QTime(0, 0, 0));
  dateSearch = new QDateTimeEdit(dt, this);
  dateSearch->setAutoAdvance(TRUE);
  dateSearch->dateEdit()->setOrder(QDateEdit::YMD);
  grid->addWidget(dateSearch, 0, 1);
  
  QPushButton *button = new QPushButton(tr("Search"), this);
  QObject::connect(button, SIGNAL(clicked()), this, SLOT(slotDateSearch()));
  QToolTip::add(button, tr("Search"));
  button->setPixmap(search);
  grid->addWidget(button, 0, 2);
  button->setAccel(CTRL+Key_R);
  
  label = new QLabel(tr("Date"), this);
  grid->addWidget(label, 1, 0);
  
  date = new QLineEdit(this);
  date->setReadOnly(TRUE);
  grid->addWidget(date, 1, 1);

  grid->setColStretch(1, 1);
}

BarEdit::~BarEdit ()
{
}

void BarEdit::createField (QString &l, QString &d, bool f)
{
  QLabel *label = new QLabel(l, this);
  grid->addWidget(label, grid->numRows() - 1, 0);
  
  QLineEdit *edit = new QLineEdit(this);
  editList.replace(d, edit);
  
  if (f)
  {
    QIntValidator *iv = new QIntValidator(0, 999999999, this);
    edit->setValidator(iv);
    connect(edit, SIGNAL(textChanged(const QString &)), this, SLOT(textChanged(const QString &)));
    grid->addWidget(edit, grid->numRows() - 1, 1);
  }
  else
  {
    QDoubleValidator *dv = new QDoubleValidator(0, 99999999999.0, 4, this, 0);
    edit->setValidator(dv);
    connect(edit, SIGNAL(textChanged(const QString &)), this, SLOT(textChanged(const QString &)));
    grid->addWidget(edit, grid->numRows() - 1, 1);
  }
  
  grid->expand(grid->numRows() + 1, grid->numCols());
}

void BarEdit::deleteRecord ()
{
  int rc = QMessageBox::warning(this,
  			        tr("Delete record."),
			        tr("Are you sure you want to delete record?"),
			        QMessageBox::Yes,
			        QMessageBox::No,
			        QMessageBox::NoButton);

  if (rc == QMessageBox::No)
    return;

  QString s = dateSearch->dateTime().toString("yyyyMMddmmhhss");
  
  emit signalDeleteRecord(s);
  
  clearRecordFields();
  
  toolbar->setButtonStatus(deleteLabel, FALSE);
  toolbar->setButtonStatus(saveLabel, FALSE);
  
  saveRecordFlag = FALSE;
}

void BarEdit::saveRecord ()
{
  emit signalSaveRecord();
  toolbar->setButtonStatus(saveLabel, FALSE);
  saveRecordFlag = FALSE;
}

void BarEdit::slotDateSearch ()
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
  
  emit signalSearch(key);
  
  toolbar->setButtonStatus(deleteLabel, TRUE);
  toolbar->setButtonStatus(saveLabel, FALSE);
}

void BarEdit::clearRecordFields ()
{
  ignoreSaveRecordFlag = TRUE;
  
  date->clear();
  
  QDictIterator<QLineEdit> it(editList);
  for(; it.current(); ++it)
    it.current()->clear();
  
  ignoreSaveRecordFlag = FALSE;
  
  saveRecordFlag = FALSE;
}

void BarEdit::textChanged (const QString &)
{
  if (! ignoreSaveRecordFlag)
  {
    saveRecordFlag = TRUE;
    toolbar->setButtonStatus(saveLabel, TRUE);
  }
}

void BarEdit::setField (QString &k, QString &d)
{
  ignoreSaveRecordFlag = TRUE;
  
  QLineEdit *edit = editList[k];
  if (edit)
    edit->setText(d);
  
  ignoreSaveRecordFlag = FALSE;
}

void BarEdit::setDate (QString &d, bool f)
{
  if (! f)
    dateSearch->timeEdit()->setEnabled(FALSE);
  else
    dateSearch->timeEdit()->setEnabled(TRUE);
    
  date->setText(d);
}

QString BarEdit::getField (QString &d)
{
  QString s;
  QLineEdit *edit = editList[d];
  if (edit)
    s = edit->text();
  return s;
}

QString BarEdit::getDate ()
{
  return date->text();
}

bool BarEdit::getSaveFlag ()
{
  return saveRecordFlag;
}


