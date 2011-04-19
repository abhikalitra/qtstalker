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

#include "COMPAREWidget.h"
#include "Globals.h"
#include "LineEdit.h"
#include "Command.h"
#include "Operator.h"

#include "../pics/add.xpm"
#include "../pics/delete.xpm"

#include <QtDebug>
#include <QToolBar>
#include <QSettings>
#include <QComboBox>

COMPAREWidget::COMPAREWidget (QWidget *p) : PluginWidget (p)
{
  createGeneralPage();
  loadSettings();
}

void COMPAREWidget::createGeneralPage ()
{
  // toolbar
  QToolBar *tb = new QToolBar;
  _vbox->addWidget(tb);

  // add button
  QToolButton *b = new QToolButton;
  b->setIcon(QIcon(add_xpm));
  b->setToolTip(tr("Add Item"));
  connect(b, SIGNAL(clicked(bool)), this, SLOT(addItem()));
  tb->addWidget(b);

  // delete button
  _deleteButton = new QToolButton;
  _deleteButton->setIcon(QIcon(delete_xpm));
  _deleteButton->setToolTip(tr("Delete Item"));
  connect(_deleteButton, SIGNAL(clicked(bool)), this, SLOT(deleteItem()));
  tb->addWidget(_deleteButton);

  // list
  QStringList l;
  l << "" << "NAME" << "OP" << "NAME2" << "NAME3" << "NAME4";

  _tree = new QTreeWidget;
  _tree->setHeaderLabels(l);
  _tree->setSortingEnabled(TRUE);
  _tree->setRootIsDecorated(FALSE);
  _tree->setSelectionMode(QAbstractItemView::SingleSelection);
  connect(_tree, SIGNAL(itemSelectionChanged()), this, SLOT(selectionChanged()));
  _vbox->addWidget(_tree);

  selectionChanged();
}

void COMPAREWidget::setCommand (QString d)
{
  Command command(d);
  if (command.parm("PLUGIN") != "COMPARE")
    return;

  addItem(command.parm("NAME"),
	  command.parm("OP"),
	  command.parm("NAME2"),
	  command.parm("NAME3"),
	  command.parm("NAME4"));
}

void COMPAREWidget::selectionChanged ()
{
  bool status = TRUE;
  QList<QTreeWidgetItem *> l = _tree->selectedItems();
  if (! l.count())
    status = FALSE;
  _deleteButton->setEnabled(status);
}

void COMPAREWidget::addItem ()
{
  addItem(QString("comp"), QString("EQ"), QString("Close"), QString("Close"), QString("Close"));
}

void COMPAREWidget::addItem (QString name, QString op, QString name2, QString name3, QString name4)
{
  QTreeWidgetItem *item = new QTreeWidgetItem(_tree);

  int col = 1;
  LineEdit *le = new LineEdit;
  le->setText(name);
  _tree->setItemWidget(item, col++, le);

  Operator oper;
  QComboBox *cb = new QComboBox;
  cb->addItems(oper.list());
  cb->setCurrentIndex(cb->findText(op, Qt::MatchExactly));
  _tree->setItemWidget(item, col++, cb);

  le = new LineEdit;
  le->setText(name2);
  _tree->setItemWidget(item, col++, le);

  le = new LineEdit;
  le->setText(name3);
  _tree->setItemWidget(item, col++, le);

  le = new LineEdit;
  le->setText(name4);
  _tree->setItemWidget(item, col++, le);
}

void COMPAREWidget::deleteItem ()
{
  QList<QTreeWidgetItem *> l = _tree->selectedItems();
  if (! l.count())
    return;

  int loop = 0;
  for (; loop < l.count(); loop++)
    delete l.at(loop);
}

void COMPAREWidget::loadSettings ()
{
  QSettings set(g_globalSettings);
  int loop = 0;
  for (; loop < _tree->columnCount(); loop++)
  {
    QString key = "COMPAREWidget_rule_column_width_" + QString::number(loop);
    _tree->setColumnWidth(loop, set.value(key, 75).toInt());
  }
}

void COMPAREWidget::commands (QStringList &l, int tab)
{
  int loop = 0;
  for (; loop < _tree->topLevelItemCount(); loop++)
  {
    QTreeWidgetItem *item = _tree->topLevelItem(loop);
    if (! item)
      continue;

    QStringList cl;
    cl << "PLUGIN=COMPARE";

    int col = 1;
    LineEdit *le = (LineEdit *) _tree->itemWidget(item, col++);
    cl << "NAME=" + le->text();

    QComboBox *cb = (QComboBox *) _tree->itemWidget(item, col++);
    cl << "OP=" + cb->currentText();

    le = (LineEdit *) _tree->itemWidget(item, col++);
    cl << "NAME2=" + le->text();

    le = (LineEdit *) _tree->itemWidget(item, col++);
    cl << "NAME3=" + le->text();

    le = (LineEdit *) _tree->itemWidget(item, col++);
    cl << "NAME4=" + le->text();

    cl << "TAB=" + QString::number(tab);

    l.append(cl.join(","));
  }

  QSettings set(g_globalSettings);
  for (loop = 0; loop < _tree->columnCount(); loop++)
  {
    QString key = "COMPAREWidget_rule_column_width_" + QString::number(loop);
    set.setValue(key, _tree->columnWidth(loop));
  }
  set.sync();
}
