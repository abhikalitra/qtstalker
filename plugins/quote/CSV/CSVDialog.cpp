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

#include "CSVDialog.h"
#include <qdir.h>
#include <qmessagebox.h>
#include <qlayout.h>
#include <qlabel.h>

CSVDialog::CSVDialog () : QTabDialog (0, "CSVDialog", TRUE)
{
  createMainPage();
  createDatePage();
  typeChanged(0);
  setOkButton();
  setCancelButton();
  resize(325, 325);
}

CSVDialog::~CSVDialog ()
{
}

void CSVDialog::createMainPage ()
{
  QWidget *w = new QWidget(this);
  
  QGridLayout *grid = new QGridLayout(w, 8, 1);
  grid->setMargin(5);
  grid->setSpacing(5);
  grid->setColStretch(1, 1);
  
  QLabel *label = new QLabel(tr("Input:"), w);
  grid->addWidget(label, 0, 0);
  
  file = new FileButton(w);
  grid->addWidget(file, 0, 1);

  label = new QLabel(tr("Symbol:"), w);
  grid->addWidget(label, 1, 0);
  
  symbol = new QLineEdit(w);
  grid->addWidget(symbol, 1, 1);
  
  label = new QLabel(tr("Chart Type:"), w);
  grid->addWidget(label, 2, 0);
  
  type = new QComboBox(w);
  type->insertItem(tr(tr("Stock")), -1);
  type->insertItem(tr(tr("Futures")), -1);
  QObject::connect(type, SIGNAL(activated(int)), this, SLOT(typeChanged(int)));
  grid->addWidget(type, 2, 1);
  
  label = new QLabel(tr("Futures Symbol:"), w);
  grid->addWidget(label, 3, 0);
  
  futuresSymbol = new QComboBox(w);
  grid->addWidget(futuresSymbol, 3, 1);
    
  label = new QLabel(tr("Futures Month:"), w);
  grid->addWidget(label, 4, 0);
  
  futuresMonth = new QComboBox(w);
  grid->addWidget(futuresMonth, 4, 1);
  
  label = new QLabel(tr("Format:"), w);
  grid->addWidget(label, 5, 0);
  
  format = new QComboBox(w);
  grid->addWidget(format, 5, 1);
  
  label = new QLabel(tr("Delimiter:"), w);
  grid->addWidget(label, 6, 0);
  
  delimiter = new QComboBox(w);
  grid->addWidget(delimiter, 6, 1);
  
  addTab(w, tr("General"));
}

void CSVDialog::createDatePage ()
{
  // date page    
  QWidget *w = new QWidget(this);
  
  QVBoxLayout *vbox = new QVBoxLayout(w);
  vbox->setMargin(5);
  vbox->setSpacing(0);
  
  dateRange = new QCheckBox(tr("Select Date Range"), w);
  QObject::connect(dateRange, SIGNAL(toggled(bool)), this, SLOT(dateRangeChanged(bool)));
  vbox->addWidget(dateRange);
  vbox->addSpacing(5);
  
  QGridLayout *grid = new QGridLayout(vbox, 4, 1);
  grid->setSpacing(5);
  grid->setColStretch(1, 1);

  QLabel *label = new QLabel(tr("Date Start:"), w);
  grid->addWidget(label, 0, 0);
  
  sdate = new QDateEdit(QDate::currentDate(), w);
  sdate->setAutoAdvance(TRUE);
  sdate->setOrder(QDateEdit::YMD);
  grid->addWidget(sdate, 0, 1);
  
  label = new QLabel(tr("Date End:"), w);
  grid->addWidget(label, 1, 0);
  
  edate = new QDateEdit(QDate::currentDate(), w);
  edate->setAutoAdvance(TRUE);
  edate->setOrder(QDateEdit::YMD);
  grid->addWidget(edate, 1, 1);
  
  label = new QLabel(tr("Date Format:"), w);
  grid->addWidget(label, 2, 0);
  
  dateFormat = new QComboBox(w);
  grid->addWidget(dateFormat, 2, 1);
  
  addTab(w, tr("Date"));
}

void CSVDialog::typeChanged (int)
{
  int i = type->currentItem();
  if (i)
  {
    futuresSymbol->setEnabled(TRUE);
    futuresMonth->setEnabled(TRUE);
  }
  else
  {
    futuresSymbol->setEnabled(FALSE);
    futuresMonth->setEnabled(FALSE);
  }
}

void CSVDialog::dateRangeChanged (bool d)
{
  sdate->setEnabled(d);
  edate->setEnabled(d);
}

QString CSVDialog::getType ()
{
  return type->currentText();
}

QStringList CSVDialog::getFiles ()
{
  return file->getFile();
}

QString CSVDialog::getSymbol ()
{
  return symbol->text();
}

void CSVDialog::setFuturesSymbol (QStringList l, QString d)
{
  futuresSymbol->insertStringList(l, -1);
  futuresSymbol->setCurrentText(d);
}

QString CSVDialog::getFuturesSymbol ()
{
  return futuresSymbol->currentText();
}

void CSVDialog::setFuturesMonth (QStringList l, QString d)
{
  futuresMonth->insertStringList(l, -1);
  futuresMonth->setCurrentText(d);
}

QString CSVDialog::getFuturesMonth ()
{
  return futuresMonth->currentText();
}

void CSVDialog::setFormat (QStringList l, QString d)
{
  format->insertStringList(l, -1);
  format->setCurrentText(d);
}

QString CSVDialog::getFormat ()
{
  return format->currentText();
}

void CSVDialog::setDelimiter (QStringList l, QString d)
{
  delimiter->insertStringList(l, -1);
  delimiter->setCurrentText(d);
}

QString CSVDialog::getDelimiter ()
{
  return delimiter->currentText();
}

void CSVDialog::setDateFormat (QStringList l, QString d)
{
  dateFormat->insertStringList(l, -1);
  dateFormat->setCurrentText(d);
}

QString CSVDialog::getDateFormat ()
{
  return dateFormat->currentText();
}

void CSVDialog::setStartDate (QDateTime dt)
{
  sdate->setDate(dt.date());
}

QDateTime CSVDialog::getStartDate ()
{
  QDateTime dt;
  dt.setDate(sdate->date());
  return dt;
}

void CSVDialog::setEndDate (QDateTime dt)
{
  edate->setDate(dt.date());
}

QDateTime CSVDialog::getEndDate ()
{
  QDateTime dt;
  dt.setDate(edate->date());
  return dt;
}

void CSVDialog::setDateRange (bool d)
{
  dateRange->setChecked(d);
  dateRangeChanged(d);
}

bool CSVDialog::getDateRange ()
{
  return dateRange->isChecked();
}


