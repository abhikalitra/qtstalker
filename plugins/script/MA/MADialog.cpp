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

#include "MADialog.h"
#include "Globals.h"
#include "MAType.h"
#include "LineEdit.h"
#include "Command.h"

#include "../pics/add.xpm"
#include "../pics/delete.xpm"

#include <QtDebug>
#include <QToolBar>
#include <QSettings>
#include <QComboBox>
#include <QSpinBox>

MADialog::MADialog (QWidget *p) : PluginWidget (p)
{
  createGeneralPage();
  loadSettings();
}

void MADialog::createGeneralPage ()
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
  l << "" << "NAME" << "METHOD" << "INPUT" << "PERIOD";

  _tree = new QTreeWidget;
  _tree->setHeaderLabels(l);
  _tree->setSortingEnabled(TRUE);
  _tree->setRootIsDecorated(FALSE);
  _tree->setSelectionMode(QAbstractItemView::SingleSelection);
  connect(_tree, SIGNAL(itemSelectionChanged()), this, SLOT(selectionChanged()));
  _vbox->addWidget(_tree);

  selectionChanged();
}

void MADialog::setCommand (QString d)
{
  Command command(d);
  if (command.parm("PLUGIN") != "MA")
    return;

  addItem(command.parm("NAME"),
	  command.parm("METHOD"),
	  command.parm("INPUT"),
	  command.parm("PERIOD"));
}

void MADialog::selectionChanged ()
{
  bool status = TRUE;
  QList<QTreeWidgetItem *> l = _tree->selectedItems();
  if (! l.count())
    status = FALSE;
  _deleteButton->setEnabled(status);
}

void MADialog::addItem ()
{
  addItem(QString("MA"), QString("EMA"), QString("Close"), QString("10"));
}

void MADialog::addItem (QString name, QString method, QString input, QString period)
{
  QTreeWidgetItem *item = new QTreeWidgetItem(_tree);

  int col = 1;
  LineEdit *le = new LineEdit;
  le->setText(name);
  _tree->setItemWidget(item, col++, le);

  MAType mat;
  QComboBox *cb = new QComboBox;
  cb->addItems(mat.list());
  cb->setCurrentIndex(cb->findText(method, Qt::MatchExactly));
  _tree->setItemWidget(item, col++, cb);

  le = new LineEdit;
  le->setText(input);
  _tree->setItemWidget(item, col++, le);

  QSpinBox *sb = new QSpinBox;
  sb->setRange(2, 99999);
  sb->setValue(period.toInt());
  _tree->setItemWidget(item, col++, sb);
}

void MADialog::deleteItem ()
{
  QList<QTreeWidgetItem *> l = _tree->selectedItems();
  if (! l.count())
    return;

  int loop = 0;
  for (; loop < l.count(); loop++)
    delete l.at(loop);
}

void MADialog::loadSettings ()
{
  QSettings set(g_globalSettings);
  int loop = 0;
  for (; loop < _tree->columnCount(); loop++)
  {
    QString key = "MADialog_rule_column_width_" + QString::number(loop);
    _tree->setColumnWidth(loop, set.value(key, 75).toInt());
  }
}

void MADialog::commands (QStringList &l, int tab)
{
  int loop = 0;
  for (; loop < _tree->topLevelItemCount(); loop++)
  {
    QTreeWidgetItem *item = _tree->topLevelItem(loop);
    if (! item)
      continue;

    QStringList cl;
    cl << "PLUGIN=MA";

    int col = 1;
    LineEdit *le = (LineEdit *) _tree->itemWidget(item, col++);
    cl << "NAME=" + le->text();

    QComboBox *cb = (QComboBox *) _tree->itemWidget(item, col++);
    cl << "METHOD=" + cb->currentText();

    le = (LineEdit *) _tree->itemWidget(item, col++);
    cl << "INPUT=" + le->text();

    QSpinBox *sb = (QSpinBox *) _tree->itemWidget(item, col++);
    cl << "PERIOD=" + sb->text();

    cl << "TAB=" + QString::number(tab);

    l.append(cl.join(","));
  }

  QSettings set(g_globalSettings);
  for (loop = 0; loop < _tree->columnCount(); loop++)
  {
    QString key = "MADialog_rule_column_width_" + QString::number(loop);
    set.setValue(key, _tree->columnWidth(loop));
  }
  set.sync();
}
