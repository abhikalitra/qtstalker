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

#include "EditDialog.h"
#include "ChartDb.h"
#include "ok.xpm"
#include "stop.xpm"
#include "up.xpm"
#include "dirclosed.xpm"
#include <qcolordialog.h>
#include <qmessagebox.h>
#include <qcolor.h>
#include <qfiledialog.h>
#include <qstringlist.h>
#include <qtooltip.h>
#include <qpushbutton.h>
#include <qinputdialog.h>
#include <qpixmap.h>

EditDialog::EditDialog (Config *c) : QDialog (0, "EditDialog", TRUE)
{
  config = c;
  settings = 0;
  item = 0;
  currentDir.setPath(config->getData(Config::DataPath));

  baseBox = new QVBoxLayout(this);
  baseBox->setMargin(5);
  baseBox->setSpacing(5);

  toolbar = new QGridLayout(baseBox, 1, 4);
  toolbar->setSpacing(1);

  okButton = new QToolButton(this);
  QToolTip::add(okButton, tr("OK"));
  okButton->setPixmap(QPixmap(ok));
  connect(okButton, SIGNAL(clicked()), this, SLOT(saveData()));
  okButton->setMaximumWidth(30);
  okButton->setAutoRaise(TRUE);
  toolbar->addWidget(okButton, 0, 0);

  cancelButton = new QToolButton(this);
  QToolTip::add(cancelButton, tr("Cancel"));
  cancelButton->setPixmap(QPixmap(stop));
  connect(cancelButton, SIGNAL(clicked()), this, SLOT(reject()));
  cancelButton->setMaximumWidth(30);
  cancelButton->setAutoRaise(TRUE);
  toolbar->addWidget(cancelButton, 0, 1);

  upButton = new QToolButton(this);
  QToolTip::add(upButton, tr("Parent Directory"));
  upButton->setPixmap(QPixmap(up));
  connect(upButton, SIGNAL(clicked()), this, SLOT(upDirectory()));
  upButton->setMaximumWidth(30);
  upButton->setAutoRaise(TRUE);
  upButton->hide();
  toolbar->addWidget(upButton, 0, 2);

//  QFrame *sep = new QFrame(this);
//  sep->setFrameStyle(QFrame::HLine | QFrame::Sunken);
//  baseBox->addWidget(sep);

  topBox = new QVBoxLayout(baseBox);
  
  fileBox = new QHBoxLayout(topBox);

  fileList = new QListView(this);
  fileList->addColumn(tr("Symbol"), 200);
  fileList->setSelectionMode(QListView::Single);
  connect(fileList, SIGNAL(clicked(QListViewItem *)), this, SLOT(fileSelection(QListViewItem *)));
  fileBox->addWidget(fileList);

  gbox = new QGroupBox(this);
  gbox->setColumnLayout(2, Horizontal);
  fileBox->addWidget(gbox);

  QLabel *label = new QLabel(tr("Symbol"), gbox);
  symbol = new QLabel(0, gbox);

  label = new QLabel(tr("Title"), gbox);
  title = new QLabel(0, gbox);

  label = new QLabel(tr("Type"), gbox);
  type = new QLabel(0, gbox);

  label = new QLabel(tr("First Date"), gbox);
  firstDate = new QLabel(0, gbox);

  label = new QLabel(tr("Last Date"), gbox);
  lastDate = new QLabel(0, gbox);

  fileList->hide();
  gbox->hide();

  list = new QListView(this);
  list->setSelectionMode(QListView::Single);
  list->addColumn(tr("Setting"), -1);
  list->addColumn(tr("Value"), 150);
  list->setColumnWidthMode(0, QListView::Maximum);
  connect(list, SIGNAL(clicked(QListViewItem *)), this, SLOT(cellSelected(QListViewItem *)));
  baseBox->addWidget(list);
}

EditDialog::~EditDialog ()
{
}

void EditDialog::setItems (Setting *d)
{
  settings = d;
  makeSettings();
}

void EditDialog::makeSettings ()
{
  QStringList key = settings->getKeyList();

  int loop;
  for (loop = 0; loop < (int) key.count(); loop++)
  {
    if (settings->getType(key[loop]) != Setting::None)
    {
      item = new QListViewItem(list, key[loop], settings->getData(key[loop]));

      if (settings->getType(key[loop]) == Setting::Color)
      {
        QPixmap pix;
	pix.resize(25, item->height());
	pix.fill(QColor(item->text(1)));
	item->setPixmap(1, pix);
      }
    }
  }
}

void EditDialog::cellSelected (QListViewItem *i)
{
  if (! i)
    return;

  item = list->selectedItem();

  switch (settings->getType(item->text(0)))
  {
    case Setting::Color:
      colorDialog();
      break;
    case Setting::Text:
      textDialog();
      break;
    case Setting::Integer:
      intDialog();
      break;
    case Setting::Float:
      floatDialog();
      break;
    case Setting::List:
    case Setting::LineType:
    case Setting::MAType:
    case Setting::InputField:
      listDialog();
      break;
    case Setting::Bool:
      checkDialog();
      break;
    case Setting::Date:
      dateDialog();
      break;
    case Setting::FileList:
      fileDialog();
      break;
    case Setting::Symbol:
      symbolDialog();
      break;
    default:
      break;
  }
}

void EditDialog::colorDialog ()
{
  QColor color = QColorDialog::getColor(QColor(item->text(1)), this, 0);
  if (color.isValid())
  {
    item->setText(1, color.name());
    
    QPixmap pix;
    pix.resize(25, item->height());
    pix.fill(QColor(item->text(1)));
    item->setPixmap(1, pix);
  }
}

void EditDialog::dateDialog ()
{
  DateDialog *dialog = new DateDialog();
  dialog->setCaption(tr("Edit Date"));

  QDate dt = QDate::currentDate();
  QString s = item->text(1);
  if (s.length())
  {
    s.insert(4, "-");
    s.insert(7, "-");
    dt = QDate::fromString(s, Qt::ISODate);
  }
  dialog->setDate(dt);

  int rc = dialog->exec();

  if (rc == QDialog::Accepted)
  {
    dt = dialog->getDate();
    item->setText(1, dt.toString("yyyyMMdd"));
  }

  delete dialog;
}

void EditDialog::textDialog ()
{
  bool ok = FALSE;

  QString s = tr("Edit ");
  s.append(item->text(0));

  QString text = QInputDialog::getText(s,
  				       s,
				       QLineEdit::Normal,
				       item->text(1),
				       &ok,
				       this,
				       s);
  if (ok == FALSE)
    return;
    
  while(text.contains(','))
  {
    int pos = text.find(',', 0, TRUE);
    text = text.replace(pos, 1, " ");
  }

  item->setText(1, text);
}

void EditDialog::intDialog ()
{
  bool ok = FALSE;

  QString s = tr("Edit ");
  s.append(item->text(0));

  int n = QInputDialog::getInteger(s,
  				   s,
				   settings->getInt(item->text(0)),
				   -99999999,
				   99999999,
				   1,
				   &ok,
				   this,
				   s);
  if (ok == FALSE)
    return;

  item->setText(1, QString::number(n));
}

void EditDialog::floatDialog ()
{
  bool ok = FALSE;
  
  QString s = tr("Edit ");
  s.append(item->text(0));

  double n = QInputDialog::getDouble(s,
  				     s,
				     settings->getFloat(item->text(0)),
				     -99999999,
				     99999999,
				     1,
				     &ok,
				     this,
				     s);
  if (ok == FALSE)
    return;

  item->setText(1, QString::number(n));
}

void EditDialog::fileDialog ()
{
  QStringList selection = QFileDialog::getOpenFileNames("*", "*", this, "file dialog");
  if (selection.count())
  {
    settings->setList(item->text(0), selection);

    QString s = QString::number(selection.count());
    s.append(tr(" files selected"));
    item->setText(1, s);
  }
}

void EditDialog::symbolDialog ()
{
  QString selection = QFileDialog::getOpenFileName(config->getData(Config::DataPath), "*", this, "file dialog");
  if (! selection.isNull())
  {
    QString s = config->getData(Config::DataPath);
    selection.remove(0, s.length() + 1);
    item->setText(1, selection);
  }
}

void EditDialog::checkDialog ()
{
  bool ok = FALSE;

  QStringList l;
  l.append(tr("False"));
  l.append(tr("True"));

  int index = l.findIndex(settings->getData(item->text(0)));

  QString s = tr("Edit ");
  s.append(item->text(0));

  s = QInputDialog::getItem(s,
  			    s,
			    l,
			    index,
			    FALSE,
			    &ok,
			    this,
			    s);

  if (ok == FALSE)
    return;

  item->setText(1, s);
}

void EditDialog::listDialog ()
{
  bool ok = FALSE;

  QStringList l = settings->getList(item->text(0));
  int index = l.findIndex(settings->getData(item->text(0)));

  QString s = tr("Edit ");
  s.append(item->text(0));

  s = QInputDialog::getItem(s,
  			    s,
			    l,
			    index,
			    FALSE,
			    &ok,
			    this,
			    s);

  if (ok == FALSE)
    return;

  item->setText(1, s);
}

void EditDialog::saveData ()
{
  QListViewItemIterator it(list);
  for (; it.current(); ++it)
  {
    item = it.current();
    settings->setData(item->text(0), item->text(1));
  }

  accept();
}

//***********************************************************
// *********** file list stuff ******************************
//***********************************************************

void EditDialog::updateFileList ()
{
  fileList->clear();

  int loop;
  for (loop = 2; loop < (int) currentDir.count(); loop++)
  {
    item = new QListViewItem(fileList, currentDir[loop]);

    QString s = currentDir.absPath();
    s.append("/");
    s.append(currentDir[loop]);
    QFileInfo info(s);
    if (info.isDir())
      item->setPixmap(0, QPixmap(dirclosed));
  }
  
  item = 0;
}

void EditDialog::upDirectory ()
{
  QString s = config->getData(Config::DataPath);
  if (s.compare(currentDir.absPath()))
  {
    currentDir.cdUp();
    updateFileList();
  }
}

void EditDialog::clearFileInfo ()
{
  symbol->setText("");
  title->setText("");
  type->setText("");
  firstDate->setText("");
  lastDate->setText("");
}

void EditDialog::setFileInfo ()
{
  item = fileList->selectedItem();
  if (! item)
    return;

  QString s = currentDir.absPath();
  s.append("/");
  s.append(item->text(0));

  ChartDb *db = new ChartDb;
  if (db->openChart(s))
  {
    delete db;
    clearFileInfo();
    return;
  }

  Setting *set = db->getDetails();

  symbol->setText(set->getData("Symbol"));

  title->setText(set->getData("Title"));

  type->setText(set->getData("Chart Type"));

  s = set->getData("First Date");
  s.truncate(s.length() - 6);
  firstDate->setText(s);

  s = set->getData("Last Date");
  s.truncate(s.length() - 6);
  lastDate->setText(s);

  delete db;
}

void EditDialog::fileSelection (QListViewItem *item)
{
  if (! item)
    return;

  if (item->pixmap(0))
  {
    QString s = currentDir.absPath();
    s.append("/");
    s.append(item->text(0));
    currentDir.setPath(s);
    updateFileList();
    clearFileInfo();
  }
  else
    setFileInfo();
}

//**********************************************************************
//*********************** DATE DIALOG **********************************
//**********************************************************************

DateDialog::DateDialog () : QDialog (0, "DateDialog", TRUE)
{
  QVBoxLayout *vbox = new QVBoxLayout(this);
  vbox->setSpacing(10);
  vbox->setMargin(10);

  date = new QDateEdit(QDate::currentDate(), this);
  date->setAutoAdvance(TRUE);
  date->setOrder(QDateEdit::YMD);
  vbox->addWidget(date);

  QHBoxLayout *hbox = new QHBoxLayout(vbox);
  hbox->setSpacing(5);

  QPushButton *button = new QPushButton (tr("OK"), this);
  connect (button, SIGNAL (clicked()), this, SLOT (accept()));
  hbox->addWidget(button);

  button = new QPushButton (tr("Cancel"), this);
  connect (button, SIGNAL (clicked()), this, SLOT (reject()));
  hbox->addWidget(button);
}

DateDialog::~DateDialog ()
{
}

void DateDialog::setDate (QDate d)
{
  date->setDate(d);
}

QDate DateDialog::getDate ()
{
  return date->date();
}


