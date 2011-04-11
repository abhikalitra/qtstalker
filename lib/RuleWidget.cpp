/*
 *  Qtstalker stock charter
 *
 *  Copyright (C) 2001-2010 Stefan S. Stratigakos
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

#include "RuleWidget.h"
#include "Globals.h"
#include "ColorButton.h"
#include "LineEdit.h"
#include "Operator.h"
#include "InputType.h"
#include "MAType.h"
#include "CandleType.h"

#include "../pics/add.xpm"
#include "../pics/delete.xpm"

#include <QtDebug>
#include <QStringList>
#include <QVBoxLayout>
#include <QToolBar>
#include <QComboBox>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QSettings>
#include <QDateTimeEdit>

RuleWidget::RuleWidget (QWidget *p, QString key) : QWidget (p)
{
  _saveKey = key;
  createGeneralPage();
}

void RuleWidget::createGeneralPage ()
{
  QVBoxLayout *vbox = new QVBoxLayout;
  setLayout(vbox);

  // toolbar
  QToolBar *tb = new QToolBar;
  vbox->addWidget(tb);

  // add button
  QToolButton *b = new QToolButton;
  b->setIcon(QIcon(add_xpm));
  b->setToolTip(tr("Add Rule"));
  connect(b, SIGNAL(clicked(bool)), this, SLOT(addRule()));
  tb->addWidget(b);

  // delete button
  _deleteButton = new QToolButton;
  _deleteButton->setIcon(QIcon(delete_xpm));
  _deleteButton->setToolTip(tr("Delete Rule"));
  connect(_deleteButton, SIGNAL(clicked(bool)), this, SLOT(deleteRule()));
  tb->addWidget(_deleteButton);

  // list
  _plist = new QTreeWidget;
  _plist->setSortingEnabled(TRUE);
  _plist->setRootIsDecorated(FALSE);
  _plist->setSelectionMode(QAbstractItemView::SingleSelection);
  connect(_plist, SIGNAL(itemSelectionChanged()), this, SLOT(selectionChanged()));
  vbox->addWidget(_plist);

  selectionChanged();
}

void RuleWidget::addRule (QStringList &l)
{
  if (l.count() != 4)
    return;

  int row = l.at(0).toInt();
  QTreeWidgetItem *item = _plist->topLevelItem(row);
  if (! item)
    item = new QTreeWidgetItem(_plist);

  int col = l.at(1).toInt();
  col++;
  int type = l.at(2).toInt();
  QString data = l.at(3);
  
  switch (type)
  {
    case _OUTPUT:
    case _TEXT:
    case _INPUT:
    {
      LineEdit *le = new LineEdit;
      le->setText(data);
      _plist->setItemWidget(item, col, le);
      break;
    }
    case _INTEGER:
    {
      QSpinBox *sb = new QSpinBox;
      sb->setRange(-999999, 999999);
      sb->setValue(data.toInt());
      _plist->setItemWidget(item, col, sb);
      break;
    }
    case _DOUBLE:
    {
      QDoubleSpinBox *sb = new QDoubleSpinBox;
      sb->setRange(-99999999, 99999999);
      sb->setDecimals(4);
      sb->setValue(data.toDouble());
      _plist->setItemWidget(item, col, sb);
      break;
    }
    case _COLOR:
    {
      QColor c(data);
      ColorButton *cb = new ColorButton(this, c);
      _plist->setItemWidget(item, col, cb);
      break;
    }
    case _OPERATOR:
    {
      Operator op;
      QComboBox *cb = new QComboBox;
      cb->addItems(op.list());
      cb->setCurrentIndex(cb->findText(data, Qt::MatchExactly));
      _plist->setItemWidget(item, col, cb);
      break;
    }
    case _MA:
    {
      MAType mat;
      QComboBox *cb = new QComboBox;
      cb->addItems(mat.list());
      cb->setCurrentIndex(cb->findText(data, Qt::MatchExactly));
      _plist->setItemWidget(item, col, cb);
      break;
    }
    case _CANDLE:
    {
      CandleType ct;
      QComboBox *cb = new QComboBox;
      cb->addItems(ct.list());
      cb->setCurrentIndex(cb->findText(data, Qt::MatchExactly));
      _plist->setItemWidget(item, col, cb);
      break;
    }
    case _STYLE:
    {
      Curve c;
      QComboBox *cb = new QComboBox;
      cb->addItems(c.list());
      cb->setCurrentIndex(cb->findText(data, Qt::MatchExactly));
      _plist->setItemWidget(item, col, cb);
      break;
    }
    case _PLOT:
    {
      QSpinBox *sb = new QSpinBox;
      sb->setRange(-1, 99);
      sb->setValue(data.toInt());
      _plist->setItemWidget(item, col, sb);
      break;
    }
    case _LIST:
    {
      QComboBox *cb = new QComboBox;
      cb->addItems(_list);
      cb->setCurrentIndex(cb->findText(data, Qt::MatchExactly));
      _plist->setItemWidget(item, col, cb);
      break;
    }
    case _DATE:
    {
      QDateTimeEdit *d = new QDateTimeEdit(QDateTime::currentDateTime());
      d->setCalendarPopup(TRUE);
      d->setDisplayFormat("yyyy.MM.dd HH:mm:ss");
      d->setDateTime(QDateTime::fromString(data, Qt::ISODate));
      _plist->setItemWidget(item, col, d);
      break;
    }
    default:
      break;
  }
}

void RuleWidget::save ()
{
  QString s = _settings->data("PLUGIN");
  _settings->clear();
  _settings->setData("PLUGIN", s);
  _settings->setData("ROWS", _plist->topLevelItemCount());

  int loop = 0;
  for (; loop < _plist->topLevelItemCount(); loop++)
  {
    QTreeWidgetItem *item = _plist->topLevelItem(loop);
    if (! item)
      continue;

    int loop2 = 0;
    for (; loop2 < _format.count(); loop2++)
    {
      QString key = QString::number(loop) + "," + QString::number(loop2) + ",TYPE";
      _settings->setData(key, _format.at(loop2));
      
      key = QString::number(loop) + "," + QString::number(loop2) + ",DATA";

      switch (_format.at(loop2))
      {
	case _OUTPUT:
	case _TEXT:
	case _INPUT:
	{
          LineEdit *le = (LineEdit *) _plist->itemWidget(item, loop2 + 1);
	  if (! le)
	    break;
	  _settings->setData(key, le->text());
	  break;
	}
	case _OPERATOR:
	case _MA:
	case _CANDLE:
	case _STYLE:
	case _LIST:
	{
          QComboBox *cb = (QComboBox *) _plist->itemWidget(item, loop2 + 1);
	  if (! cb)
	    break;
	  _settings->setData(key, cb->currentText());
	  break;
	}
	case _INTEGER:
	case _PLOT:
	{
          QSpinBox *sb = (QSpinBox *) _plist->itemWidget(item, loop2 + 1);
	  if (! sb)
	    break;
	  _settings->setData(key, sb->text());
	  break;
	}
	case _DOUBLE:
	{
          QDoubleSpinBox *sb = (QDoubleSpinBox *) _plist->itemWidget(item, loop2 + 1);
	  if (! sb)
	    break;
	  _settings->setData(key, sb->value());
	  break;
	}
	case _COLOR:
	{
          ColorButton *c = (ColorButton *) _plist->itemWidget(item, loop2 + 1);
	  if (! c)
	    break;
	  _settings->setData(key, c->color().name());
	  break;
	}
	case _DATE:
	{
          QDateTimeEdit *d = (QDateTimeEdit *) _plist->itemWidget(item, loop2 + 1);
	  if (! d)
	    break;
	  _settings->setData(key, d->dateTime());
	  break;
	}
	default:
	  break;
      }
    }
  }

  QSettings set(g_globalSettings);
  for (loop = 0; loop < _plist->columnCount(); loop++)
  {
    QString key = _saveKey + "_rule_column_width_" + QString::number(loop);
    set.setValue(key, _plist->columnWidth(loop));
  }
  set.sync();
}

void RuleWidget::deleteRule ()
{
  QList<QTreeWidgetItem *> l = _plist->selectedItems();
  if (! l.count())
    return;

  int loop = 0;
  for (; loop < l.count(); loop++)
    delete l.at(loop);
}

void RuleWidget::setRules (Setting *d, QList<int> format, QStringList header)
{
  _plist->clear();
  _settings = d;
  _format = format;

  header.prepend("");
  _plist->setHeaderLabels(header);

  int rows = _settings->getInt("ROWS");
  int loop = 0;
  for (; loop < rows; loop++)
  {
    int loop2 = 0;
    for (; loop2 < _format.count(); loop2++)
    {
      QStringList l;
      l << QString::number(loop);
      l << QString::number(loop2);
      QString key = QString::number(loop) + "," + QString::number(loop2) + ",TYPE";
      l << _settings->data(key);
      key = QString::number(loop) + "," + QString::number(loop2) + ",DATA";
      l << _settings->data(key);
      addRule(l);
    }
  }
}

void RuleWidget::addRule ()
{
  int rowFlag = FALSE;
  int loop = 0;
  for (; loop < _format.count(); loop++)
  {
    QStringList l;
    if (! rowFlag)
    {
      l << QString::number(_plist->topLevelItemCount());
      rowFlag++;
    }
    else
      l << QString::number(_plist->topLevelItemCount() - 1);
      
    l << QString::number(loop);
    l << QString::number(_format.at(loop));
    
    switch (_format.at(loop))
    {
      case _OUTPUT:
      case _TEXT:
	l << "Name";
        break;
      case _INPUT:
        l << "Close";
	break;
      case _OPERATOR:
        l << "EQ";
	break;
      case _MA:
	l << "EMA";
	break;
      case _CANDLE:
	l << "DOJI";
	break;
      case _INTEGER:
      case _DOUBLE:
	l << "0";
	break;
      case _COLOR:
        l << "red";
	break;
      case _STYLE:
        l << "Line";
	break;
      case _PLOT:
        l << "-1";
	break;
      case _LIST:
        l << "NONE";
	break;
      case _DATE:
        l << QDateTime::currentDateTime().toString(Qt::ISODate);
	break;
      default:
	break;
    }

    addRule(l);
  }
}

void RuleWidget::loadSettings ()
{
  QSettings set(g_globalSettings);
  int loop = 0;
  for (; loop < _plist->columnCount(); loop++)
  {
    QString key = _saveKey + "_rule_column_width_" + QString::number(loop);
    _plist->setColumnWidth(loop, set.value(key, 75).toInt());
  }
}

void RuleWidget::setList (QStringList l)
{
  _list = l;
}

void RuleWidget::selectionChanged ()
{
  bool status = TRUE;
  QList<QTreeWidgetItem *> l = _plist->selectedItems();
  if (! l.count())
    status = FALSE;
  _deleteButton->setEnabled(status);  
}
