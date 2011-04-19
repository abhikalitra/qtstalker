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

#include "HLINEWidget.h"
#include "Globals.h"
#include "Command.h"
#include "ColorButton.h"

#include "../pics/add.xpm"
#include "../pics/delete.xpm"

#include <QtDebug>
#include <QToolBar>
#include <QSettings>
#include <QSpinBox>
#include <QDoubleSpinBox>

HLINEWidget::HLINEWidget (QWidget *p) : PluginWidget (p)
{
  createGeneralPage();
  loadSettings();
}

void HLINEWidget::createGeneralPage ()
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
  l << "" << tr("Color") << tr("Price") << tr("Plot");

  _tree = new QTreeWidget;
  _tree->setHeaderLabels(l);
  _tree->setSortingEnabled(TRUE);
  _tree->setRootIsDecorated(FALSE);
  _tree->setSelectionMode(QAbstractItemView::SingleSelection);
  connect(_tree, SIGNAL(itemSelectionChanged()), this, SLOT(selectionChanged()));
  _vbox->addWidget(_tree);

  selectionChanged();
}

void HLINEWidget::setCommand (QString d)
{
  Command command(d);
  if (command.parm("PLUGIN") != "HLINE")
    return;

  addItem(command.parm("COLOR"),
	  command.parm("PRICE"),
	  command.parm("Z"));
}

void HLINEWidget::selectionChanged ()
{
  bool status = TRUE;
  QList<QTreeWidgetItem *> l = _tree->selectedItems();
  if (! l.count())
    status = FALSE;
  _deleteButton->setEnabled(status);
}

void HLINEWidget::addItem ()
{
  addItem(QString("white"), QString("0"), QString("-1"));
}

void HLINEWidget::addItem (QString color, QString price, QString z)
{
  QTreeWidgetItem *item = new QTreeWidgetItem(_tree);

  int col = 1;
  ColorButton *c = new ColorButton(this, QColor(color));
  _tree->setItemWidget(item, col++, c);

  QDoubleSpinBox *dsb = new QDoubleSpinBox;
  dsb->setRange(-99999999, 99999999);
  dsb->setValue(price.toDouble());
  _tree->setItemWidget(item, col++, dsb);
  
  QSpinBox *sb = new QSpinBox;
  sb->setRange(-1, 99);
  sb->setValue(z.toInt());
  _tree->setItemWidget(item, col++, sb);
}

void HLINEWidget::deleteItem ()
{
  QList<QTreeWidgetItem *> l = _tree->selectedItems();
  if (! l.count())
    return;

  int loop = 0;
  for (; loop < l.count(); loop++)
    delete l.at(loop);
}

void HLINEWidget::loadSettings ()
{
  QSettings set(g_globalSettings);
  int loop = 0;
  for (; loop < _tree->columnCount(); loop++)
  {
    QString key = "HLINEWidget_rule_column_width_" + QString::number(loop);
    _tree->setColumnWidth(loop, set.value(key, 75).toInt());
  }
}

void HLINEWidget::commands (QStringList &l, int tab)
{
  int loop = 0;
  for (; loop < _tree->topLevelItemCount(); loop++)
  {
    QTreeWidgetItem *item = _tree->topLevelItem(loop);
    if (! item)
      continue;

    QStringList cl;
    cl << "PLUGIN=HLINE";

    int col = 1;
    ColorButton *c = (ColorButton *) _tree->itemWidget(item, col++);
    cl << "COLOR=" + c->color().name();

    QDoubleSpinBox *dsb = (QDoubleSpinBox *) _tree->itemWidget(item, col++);
    cl << "PRICE=" + dsb->text();

    QSpinBox *sb = (QSpinBox *) _tree->itemWidget(item, col++);
    cl << "Z=" + sb->text();

    cl << "TYPE=RO";
    
    cl << "TAB=" + QString::number(tab);

    l.append(cl.join(","));
  }

  QSettings set(g_globalSettings);
  for (loop = 0; loop < _tree->columnCount(); loop++)
  {
    QString key = "HLINEWidget_rule_column_width_" + QString::number(loop);
    set.setValue(key, _tree->columnWidth(loop));
  }
  set.sync();
}
