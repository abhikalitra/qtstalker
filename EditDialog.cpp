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
#include <qcolordialog.h>
#include <qmessagebox.h>
#include <qcolor.h>
#include <qfiledialog.h>
#include <qstringlist.h>
#include <qdatetime.h>

EditDialog::EditDialog (Config *c) : QDialog (0, "EditDialog", TRUE)
{
  config = c;
  settings = 0;

  ivalid = new QIntValidator(this);
  dvalid = new QDoubleValidator(this);

  QHBoxLayout *hbox = new QHBoxLayout(this);
  hbox->setSpacing(10);
  hbox->setMargin(10);

  baseBox = new QVBoxLayout(hbox);
  baseBox->setMargin(5);
  baseBox->setSpacing(10);

  topBox = new QVBoxLayout(baseBox);
  topBox->setSpacing(5);

  table = new QTable (0, 2, this);
  QHeader *head = table->horizontalHeader();
  head->setLabel(0, tr("Parameter"));
  head->setLabel(1, tr("Setting"));
  table->setSelectionMode(QTable::Single);
  connect(table, SIGNAL(clicked(int, int, int, const QPoint &)), this, SLOT(cellSelected(int, int)));
  connect(table, SIGNAL(valueChanged(int, int)), this, SLOT(valueChanged(int, int)));
  baseBox->addWidget(table);
  table->setMinimumWidth(200);

  grid = new QGridLayout(hbox, 3, 1);
  grid->setSpacing(2);

  okButton = new QPushButton (tr("OK"), this);
  connect (okButton, SIGNAL (clicked()), this, SLOT (accept()));
  grid->addWidget(okButton, 0, 0);

  cancelButton = new QPushButton (tr("Cancel"), this);
  connect (cancelButton, SIGNAL (clicked()), this, SLOT (reject()));
  grid->addWidget(cancelButton, 1, 0);
}

EditDialog::~EditDialog ()
{
  delete ivalid;
  delete dvalid;
}

void EditDialog::setItems (Setting *d)
{
  settings = d;

  QStringList key = settings->getKeyList();

  int loop;
  for (loop = 0; loop < (int) key.count(); loop++)
  {
    settingItem = settings->getItem(key[loop]);

    if (settingItem->type == Setting::None)
      continue;

    table->insertRows(table->numRows(), 1);
    table->setItem(table->numRows() - 1, 0, new QTableItem(table, QTableItem::Never, settingItem->key));
    table->adjustRow(table->numRows() - 1);

    switch (settingItem->type)
    {
      case Setting::Color:
        addColorButton();
        break;
      case Setting::Text:
      case Setting::Integer:
      case Setting::Float:
        table->setText(table->numRows() - 1, 1, settingItem->data);
        break;
      case Setting::List:
      case Setting::LineType:
      case Setting::MAType:
      case Setting::InputField:
        addList();
        break;
      case Setting::Bool:
        addCheck();
        break;
      default:
        table->setItem(table->numRows() - 1, 1, new QTableItem(table, QTableItem::Never, settingItem->data));
        break;
    }
  }

  table->adjustColumn(0);
  table->sortColumn(0, TRUE, TRUE);
}

void EditDialog::cellSelected (int row, int col)
{
  if (col != 1)
    return;

  settingItem = settings->getItem(table->text(row, 0));

  switch (settingItem->type)
  {
    case Setting::Color:
      colorDialog(row);
      break;
    case Setting::Symbol:
      symbolDialog(row);
      break;
    case Setting::Date:
      dateDialog(row);
      break;
    case Setting::Composite:
      compositeDialog(row);
      break;
    case Setting::FileList:
      fileDialog(row);
      break;
    default:
      break;
  }
}

void EditDialog::valueChanged (int row, int col)
{
  if (col != 1)
    return;

  settingItem = settings->getItem(table->text(row, 0));

  QString s = table->text(row, 1);
  int state = 0;
  int pos;
  QCheckTableItem *check;

  switch (settingItem->type)
  {
    case Setting::Text:
      while(s.contains(','))
      {
        pos = s.find(',', 0, TRUE);
        s = s.replace(pos, 1, " ");
      }
      settingItem->data = s;
      break;
    case Setting::Integer:
      ivalid->setRange(-99999999, 99999999);
      if (ivalid->validate(s, state) != QValidator::Acceptable)
        table->setText(row, col, settingItem->data);
      else
        settingItem->data = s;
      break;
    case Setting::Float:
      dvalid->setRange(-99999999, 99999999, 4);
      if (dvalid->validate(s, state) != QValidator::Acceptable)
        table->setText(row, col, settingItem->data);
      else
        settingItem->data = s;
      break;
    case Setting::Bool:
      check = (QCheckTableItem *) table->item(row, col);
      if (check->isChecked())
        settingItem->data = tr("True");
      else
        settingItem->data = tr("False");
      break;
    case Setting::List:
    case Setting::LineType:
    case Setting::MAType:
    case Setting::InputField:
      settingItem->data = table->text(row, 1);
      break;
    default:
      break;
  }
}

void EditDialog::addColorButton ()
{
  QRect rect = table->cellRect(table->numRows() - 1, 1);
  QPixmap pix(rect.width(), rect.height());
  QColor color(settingItem->data);
  pix.fill(color);
  table->setPixmap(table->numRows() - 1, 1, pix);
}

void EditDialog::addList ()
{
  QComboTableItem *combo = new QComboTableItem(table, settingItem->list, FALSE);
  combo->setCurrentItem(settingItem->data);
  table->setItem(table->numRows() - 1, 1, combo);
}

void EditDialog::addCheck ()
{
  QCheckTableItem *ch = new QCheckTableItem(table, QString::null);
  QString s = settingItem->data;
  if (! s.compare(tr("True")))
    ch->setChecked(TRUE);
  table->setItem(table->numRows() - 1, 1, ch);
}

void EditDialog::fileDialog (int row)
{
  QStringList selection = QFileDialog::getOpenFileNames("*", "*", this, "file dialog");
  if (selection.count())
  {
    table->setText(row, 1, selection.join(","));
    settingItem->list = selection;
  }
}

void EditDialog::symbolDialog (int row)
{
  QString selection = QFileDialog::getOpenFileName(config->getData(Config::DataPath), "*", this, "file dialog");
  if (! selection.isNull())
  {
    QStringList l = QStringList::split("/", selection, FALSE);
    QString symbol = l[l.count() - 1];
    table->setText(row, 1, symbol);

    if (settings)
      settingItem->data = symbol;
  }
}

void EditDialog::colorDialog (int row)
{
  QColor color(settingItem->data);
  color = QColorDialog::getColor(color, this, 0);
  if (color.isValid())
  {
    settingItem->data = color.name();

    QRect rect = table->cellRect(row, 1);
    QPixmap pix(rect.width(), rect.height());
    pix.fill(color);
    table->setPixmap(row, 1, pix);
  }
}

void EditDialog::dateDialog (int row)
{
  DateDialog *dialog = new DateDialog();
  dialog->setCaption(tr("Edit Date"));

  QDateTime dt = QDateTime::currentDateTime();
  QString s = table->text(row, 1);
  if (s.length())
  {
    s.insert(4, "-");
    s.insert(7, "-");
    s.append("00:00:00");
    dt = QDateTime::fromString(s, Qt::ISODate);
  }
  dialog->setDate(dt);

  int rc = dialog->exec();

  if (rc == QDialog::Accepted)
  {
    dt = dialog->getDate();
    table->setText(row, 1, dt.toString("yyyyMMdd"));
    settingItem->data = dt.toString("yyyyMMdd");
  }

  delete dialog;
}

void EditDialog::compositeDialog (int row)
{
  EditDialog *dialog = new EditDialog(config);
  dialog->setCaption(tr("Edit Composite"));

  Setting *set = new Setting;
  QStringList l = QStringList::split(" ", settingItem->data, FALSE);
  set->set(tr("Symbol"), l[0], Setting::Symbol);
  set->set(tr("Weight"), l[1], Setting::Float);

  dialog->setItems(set);

  int rc = dialog->exec();

  if (rc == QDialog::Accepted)
  {
    QString s = set->getData(tr("Symbol"));
    s.append(" ");
    s.append(set->getData(tr("Weight")));
    settingItem->data = s;
    table->setText(row, 1, s);
  }

  delete set;
  delete dialog;
}

//**********************************************************************
//*********************** DATE DIALOG **********************************
//**********************************************************************

DateDialog::DateDialog () : QDialog (0, "DateDialog", TRUE)
{
  QVBoxLayout *vbox = new QVBoxLayout(this);
  vbox->setSpacing(10);
  vbox->setMargin(10);

  date = new QDateTimeEdit(QDateTime::currentDateTime(), this);
  date->setAutoAdvance(TRUE);
  QDateEdit *de = date->dateEdit();
  de->setOrder(QDateEdit::YMD);
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

void DateDialog::setDate (QDateTime d)
{
  date->setDateTime(d);
}

QDateTime DateDialog::getDate ()
{
  return date->dateTime();
}






