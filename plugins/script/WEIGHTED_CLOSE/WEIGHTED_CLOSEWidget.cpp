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

#include "WEIGHTED_CLOSEWidget.h"
#include "Globals.h"
#include "LineEdit.h"
#include "Command.h"

#include "../pics/add.xpm"
#include "../pics/delete.xpm"

#include <QtDebug>
#include <QToolBar>
#include <QSettings>

WEIGHTED_CLOSEWidget::WEIGHTED_CLOSEWidget (QWidget *p) : PluginWidget (p)
{
  createGeneralPage();
  loadSettings();
}

void WEIGHTED_CLOSEWidget::createGeneralPage ()
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
  l << "" << "NAME" << "INPUT_HIGH" << "INPUT_LOW" << "INPUT_CLOSE";

  _tree = new QTreeWidget;
  _tree->setHeaderLabels(l);
  _tree->setSortingEnabled(TRUE);
  _tree->setRootIsDecorated(FALSE);
  _tree->setSelectionMode(QAbstractItemView::SingleSelection);
  connect(_tree, SIGNAL(itemSelectionChanged()), this, SLOT(selectionChanged()));
  _vbox->addWidget(_tree);

  selectionChanged();
}

void WEIGHTED_CLOSEWidget::setCommand (QString d)
{
  Command command(d);
  if (command.parm("PLUGIN") != "WEIGHTED_CLOSE")
    return;

  addItem(command.parm("NAME"),
	  command.parm("INPUT_HIGH"),
	  command.parm("INPUT_LOW"),
	  command.parm("INPUT_CLOSE"));
}

void WEIGHTED_CLOSEWidget::selectionChanged ()
{
  bool status = TRUE;
  QList<QTreeWidgetItem *> l = _tree->selectedItems();
  if (! l.count())
    status = FALSE;
  _deleteButton->setEnabled(status);
}

void WEIGHTED_CLOSEWidget::addItem ()
{
  addItem(QString("AP"), QString("High"), QString("Low"), QString("Close"));
}

void WEIGHTED_CLOSEWidget::addItem (QString name, QString high, QString low, QString close)
{
  QTreeWidgetItem *item = new QTreeWidgetItem(_tree);

  int col = 1;
  LineEdit *le = new LineEdit;
  le->setText(name);
  _tree->setItemWidget(item, col++, le);

  le = new LineEdit;
  le->setText(high);
  _tree->setItemWidget(item, col++, le);

  le = new LineEdit;
  le->setText(low);
  _tree->setItemWidget(item, col++, le);

  le = new LineEdit;
  le->setText(close);
  _tree->setItemWidget(item, col++, le);
}

void WEIGHTED_CLOSEWidget::deleteItem ()
{
  QList<QTreeWidgetItem *> l = _tree->selectedItems();
  if (! l.count())
    return;

  int loop = 0;
  for (; loop < l.count(); loop++)
    delete l.at(loop);
}

void WEIGHTED_CLOSEWidget::loadSettings ()
{
  QSettings set(g_globalSettings);
  int loop = 0;
  for (; loop < _tree->columnCount(); loop++)
  {
    QString key = "WEIGHTED_CLOSEWidget_rule_column_width_" + QString::number(loop);
    _tree->setColumnWidth(loop, set.value(key, 75).toInt());
  }
}

void WEIGHTED_CLOSEWidget::commands (QStringList &l, int tab)
{
  int loop = 0;
  for (; loop < _tree->topLevelItemCount(); loop++)
  {
    QTreeWidgetItem *item = _tree->topLevelItem(loop);
    if (! item)
      continue;

    QStringList cl;
    cl << "PLUGIN=WEIGHTED_CLOSE";

    int col = 1;
    LineEdit *le = (LineEdit *) _tree->itemWidget(item, col++);
    cl << "NAME=" + le->text();

    le = (LineEdit *) _tree->itemWidget(item, col++);
    cl << "INPUT_HIGH=" + le->text();

    le = (LineEdit *) _tree->itemWidget(item, col++);
    cl << "INPUT_LOW=" + le->text();

    le = (LineEdit *) _tree->itemWidget(item, col++);
    cl << "INPUT_CLOSE=" + le->text();

    cl << "TAB=" + QString::number(tab);

    l.append(cl.join(","));
  }

  QSettings set(g_globalSettings);
  for (loop = 0; loop < _tree->columnCount(); loop++)
  {
    QString key = "WEIGHTED_CLOSEWidget_rule_column_width_" + QString::number(loop);
    set.setValue(key, _tree->columnWidth(loop));
  }
  set.sync();
}
