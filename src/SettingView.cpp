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

#include "SettingView.h"
#include "SymbolDialog.h"
#include <qcolordialog.h>
#include <qmessagebox.h>
#include <qcolor.h>
#include <qfiledialog.h>
#include <qstringlist.h>
#include <qinputdialog.h>
#include <qpixmap.h>
#include <qstring.h>
#include <qpushbutton.h>
#include <qtooltip.h>
#include <qfontdialog.h>
#include <qfont.h>
#include <qpalette.h>
#include <qvalidator.h>

SettingView::SettingView (QWidget *w, QString dp) : QWidget (w)
{
  dataPath = dp;
  settings = 0;

  QVBoxLayout *box = new QVBoxLayout(this);
  box->setMargin(0);

  list = new QTable(0, 2, this);
  list->setSelectionMode(QTable::Single);
  list->horizontalHeader()->setLabel(0, tr("Setting"));
  list->horizontalHeader()->setLabel(1, tr("Value"));
  list->setColumnReadOnly(0, TRUE);
  list->setShowGrid(FALSE);
  list->setSorting(TRUE);
  connect(list, SIGNAL(doubleClicked(int, int, int, const QPoint &)), this, SLOT(doubleClick(int, int, int, const QPoint &)));
  connect(list, SIGNAL(valueChanged(int, int)), this, SLOT(itemChanged(int, int)));
  box->addWidget(list);
}

SettingView::~SettingView ()
{
}

void SettingView::setItems (Setting *d)
{
  settings = d;
  makeSettings();
}

void SettingView::makeSettings ()
{
  clearRows();

  QStringList key = settings->getKeyList();
  
  int row = 0;
  int loop;
  for (loop = 0; loop < (int) key.count(); loop++)
  {
    if (settings->getType(key[loop]) != Setting::None)
    {
      list->setNumRows(row + 1);

      list->setText(row, 0, key[loop]);

      while (1)
      {
        if (settings->getType(key[loop]) == Setting::Color)
	{
          ColorItem *item = new ColorItem(list, QTableItem::Never, QString::null);
	  item->setColor(settings->getData(key[loop]));
	  list->setItem(row, 1, item);
          break;
	}

        if (settings->getType(key[loop]) == Setting::Text ||
	    settings->getType(key[loop]) == Setting::Float)
	{
	  list->setText(row, 1, settings->getData(key[loop]));
          break;
	}

        if (settings->getType(key[loop]) == Setting::Integer)
	{
          IntegerItem *item = new IntegerItem(list, QTableItem::Always, settings->getData(key[loop]).toInt());
	  list->setItem(row, 1, item);
          break;
	}

        if (settings->getType(key[loop]) == Setting::List ||
	    settings->getType(key[loop]) == Setting::LineType ||
	    settings->getType(key[loop]) == Setting::MAType ||
	    settings->getType(key[loop]) == Setting::InputField)
	{

          QComboTableItem *item = new QComboTableItem(list, settings->getList(key[loop]), FALSE);
	  list->setItem(row, 1, item);
	  item->setCurrentItem(settings->getData(key[loop]));
          break;
	}

        if (settings->getType(key[loop]) == Setting::Bool)
	{
          QCheckTableItem *item = new QCheckTableItem(list, QString::null);
	  if (! settings->getData(key[loop]).compare(tr("True")))
            item->setChecked(TRUE);
	  else
            item->setChecked(FALSE);
	  list->setItem(row, 1, item);
          break;
	}

        if (settings->getType(key[loop]) == Setting::Symbol)
	{
          QTableItem *item = new QTableItem(list, QTableItem::Never, settings->getData(key[loop]));
	  list->setItem(row, 1, item);
          break;
	}

          if (settings->getType(key[loop]) == Setting::Date)
	{
          DateItem *item = new DateItem(list, QTableItem::Always, settings->getData(key[loop]));
	  list->setItem(row, 1, item);
          break;
	}

        if (settings->getType(key[loop]) == Setting::FileList)
	{
          QTableItem *item = new QTableItem(list, QTableItem::Never, QString::null);
	  item->setText(tr("0 files selected"));
	  list->setItem(row, 1, item);
          break;
	}

        if (settings->getType(key[loop]) == Setting::Font)
	{
          QTableItem *item = new QTableItem(list, QTableItem::Never, QString::null);
	  item->setText(settings->getData(key[loop]));
	  list->setItem(row, 1, item);
          break;
	}

	break;
      }

      row++;
    }
  }
  
  list->sortColumn(0, TRUE, TRUE);
}

void SettingView::doubleClick (int row, int, int, const QPoint &)
{
  if (row < 0)
    return;

  QString s = list->text(row, 0);

  switch (settings->getType(s))
  {
    case Setting::Color:
      colorDialog(row);
      break;
    case Setting::FileList:
      fileDialog(row);
      break;
    case Setting::Symbol:
      symbolDialog(row);
      break;
    case Setting::Font:
      fontDialog(row);
      break;
    default:
      break;
  }
}

void SettingView::itemChanged (int row, int)
{
  QString s = list->text(row, 0);

  switch (settings->getType(s))
  {
    case Setting::Float:
      floatChanged(row);
      break;
    case Setting::Text:
    case Setting::List:
    case Setting::LineType:
    case Setting::MAType:
    case Setting::InputField:
      textChanged(row);
      break;
    default:
      break;
  }
}

void SettingView::colorDialog (int row)
{
  ColorItem *item = (ColorItem *) list->item(row, 1);
  QColor color = QColorDialog::getColor(QColor(settings->getData(list->text(row, 0))), this, 0);
  if (color.isValid())
  {
    item->setColor(color.name());
    settings->setData(list->text(row, 0), color.name());
  }
}

void SettingView::floatChanged (int row)
{
  int rc;
  QDoubleValidator dv(-99999999, 99999999, 4, this, 0);
  QString s = list->text(row, 1);
  if (dv.validate(s, rc) == QValidator::Invalid)
    list->setText(row, 1, settings->getData(list->text(row, 0)));
  else
    settings->setData(list->text(row, 0), list->text(row, 1));
}

void SettingView::textChanged (int row)
{
  settings->setData(list->text(row, 0), list->text(row, 1));
}

void SettingView::fileDialog (int row)
{
  QString path = "*";

  QStringList selection = settings->getList(list->text(row, 0));
  if (selection.count())
  {
    path = selection[0];
    path.truncate(selection[0].findRev("/"));
  }

  selection = QFileDialog::getOpenFileNames("*", path, this, "file dialog");
  if (selection.count())
  {
    settings->setList(list->text(row, 0), selection);

    QString s = QString::number(selection.count());
    s.append(tr(" files selected"));
    list->setText(row, 1, s);

    settings->setData(list->text(row, 0), list->text(row, 1));
  }
}

void SettingView::symbolDialog (int row)
{
  SymbolDialog *dialog = new SymbolDialog(this,
  							   dataPath,
							   "*");
  dialog->setCaption(tr("Select Chart"));

  int rc = dialog->exec();

  if (rc == QDialog::Accepted)
  {
    list->setText(row, 1, dialog->selectedFile());
    settings->setData(list->text(row, 0), list->text(row, 1));
  }

  delete dialog;
}

void SettingView::fontDialog (int row)
{
  bool ok;
  QStringList l = QStringList::split(" ", list->text(row, 1), FALSE);
  QFont font = QFontDialog::getFont(&ok, QFont(l[0], l[1].toInt(), l[2].toInt()), this);
  if (ok)
  {
    QString s = font.family();
    s.append(" ");
    s.append(QString::number(font.pointSize()));
    s.append(" ");
    s.append(QString::number(font.weight()));

    list->setText(row, 1, s);

    settings->setData(list->text(row, 0), list->text(row, 1));
  }
}

void SettingView::clear ()
{
  settings = 0;
  clearRows();
}

void SettingView::clearRows ()
{
  int loop;
  for (loop = list->numRows() - 1; loop > -1; loop--)
    list->removeRow(loop);
}

void SettingView::updateSettings ()
{
  int loop;
  ColorItem *color;
  QCheckTableItem *check;
  QTableItem *item;

  for (loop = 0; loop < (int) list->numRows(); loop++)
  {
    switch(settings->getType(list->text(loop, 0)))
    {
      case Setting::Color:
        color = (ColorItem *) list->item(loop, 1);
        settings->setData(list->text(loop, 0), color->getColor());
	break;
      case Setting::Bool:
        check = (QCheckTableItem *) list->item(loop, 1);
          if (check->isChecked())
          settings->setData(list->text(loop, 0), tr("True"));
          else
          settings->setData(list->text(loop, 0), tr("False"));
         break;
      default:
        item = list->item(loop, 1);
        settings->setData(list->text(loop, 0), item->text());
	break;
    }
  }
}

//**********************************************************************
//*********************** Color Item *************************************
//**********************************************************************

ColorItem::ColorItem (QTable *t, EditType et, QString s) : QTableItem (t, et, s)
{
}

ColorItem::~ColorItem ()
{
}

void ColorItem::paint (QPainter *p, const QColorGroup &cg, const QRect &cr, bool selected)
{
  QColorGroup g(cg);
  g.setColor(QColorGroup::Base, QColor(color));
  QTableItem::paint(p, g, cr, selected);
}

void ColorItem::setColor (QString col)
{
  color = col;
}

QString ColorItem::getColor ()
{
  return color;
}

//**********************************************************************
//*********************** Integer Item ***********************************
//**********************************************************************

IntegerItem::IntegerItem (QTable *t, EditType et, int v) : QTableItem (t, et, QString::null)
{
  setReplaceable( FALSE );
  val = v;
}

IntegerItem::~IntegerItem ()
{
}

QWidget * IntegerItem::createEditor () const
{
  ((IntegerItem*)this )->spinner = new QSpinBox (-999999, 999999, 1, table()->viewport());
  spinner->setValue(val);
  return spinner;
}

QString IntegerItem::text () const
{
  return spinner->text();
}

//**********************************************************************
//*********************** Date Item *************************************
//**********************************************************************

DateItem::DateItem (QTable *t, EditType et, QString d) : QTableItem (t, et, QString::null)
{
  setReplaceable( FALSE );
  startDate = d;
  startDate.insert(6, "-");
  startDate.insert(4, "-");
}

DateItem::~DateItem ()
{
}

QWidget * DateItem::createEditor () const
{
  ((DateItem*)this )->dateEdit = new QDateEdit (QDate::currentDate(), table()->viewport());
  if (startDate.length())
    dateEdit->setDate(QDate::fromString(startDate, Qt::ISODate));
  dateEdit->setAutoAdvance(TRUE);
  dateEdit->setOrder(QDateEdit::YMD);
  return dateEdit;
}

QString DateItem::text () const
{
  return dateEdit->date().toString("yyyyMMdd");
}

