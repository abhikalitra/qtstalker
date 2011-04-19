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

#include "IPLOTWidget.h"
#include "Globals.h"
#include "Command.h"
#include "LineEdit.h"
#include "ColorButton.h"

#include "../pics/add.xpm"
#include "../pics/delete.xpm"

#include <QtDebug>
#include <QToolBar>
#include <QSettings>
#include <QComboBox>
#include <QCheckBox>
#include <QSpinBox>

IPLOTWidget::IPLOTWidget (QWidget *p) : PluginWidget (p)
{
  createGeneralPage();
  loadSettings();
}

void IPLOTWidget::createGeneralPage ()
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
  l << "" << "NAME" << tr("Use Color") << "COLOR" << tr("Use Style") << "STYLE" << "Z" << tr("Pen");
  
  _tree = new QTreeWidget;
  _tree->setHeaderLabels(l);
  _tree->setSortingEnabled(TRUE);
  _tree->setRootIsDecorated(FALSE);
  _tree->setSelectionMode(QAbstractItemView::SingleSelection);
  connect(_tree, SIGNAL(itemSelectionChanged()), this, SLOT(selectionChanged()));
  _vbox->addWidget(_tree);

  selectionChanged();
}

void IPLOTWidget::setCommand (QString d)
{
  Command command(d);
  if (command.parm("PLUGIN") != "IPLOT")
    return;

  addItem(command.parm("NAME"),
	  command.parm("COLOR"),
	  command.parm("STYLE"),
	  command.parm("Z"),
	  command.parm("PEN"));
}

void IPLOTWidget::selectionChanged ()
{
  bool status = TRUE;
  QList<QTreeWidgetItem *> l = _tree->selectedItems();
  if (! l.count())
    status = FALSE;
  _deleteButton->setEnabled(status);
}

void IPLOTWidget::addItem ()
{
  addItem(QString("Close"), QString("#FF0000"), QString("Line"), QString("0"), QString("1"));
}

void IPLOTWidget::addItem (QString name, QString color, QString style, QString z, QString pen)
{
  QTreeWidgetItem *item = new QTreeWidgetItem(_tree);

  int col = 1;
  LineEdit *le = new LineEdit;
  le->setText(name);
  _tree->setItemWidget(item, col++, le);

  QCheckBox *ch = new QCheckBox;
  if (! color.isEmpty())
    ch->setChecked(TRUE);
  _tree->setItemWidget(item, col++, ch);

  QColor c(color);
  ColorButton *colb = new ColorButton(this, c);
  _tree->setItemWidget(item, col++, colb);
    
  ch = new QCheckBox;
  if (! style.isEmpty())
    ch->setChecked(TRUE);
  _tree->setItemWidget(item, col++, ch);

  Curve curve;
  QComboBox *cb = new QComboBox;
  cb->addItems(curve.list());
  if (! style.isEmpty())
    cb->setCurrentIndex(cb->findText(style, Qt::MatchExactly));
  _tree->setItemWidget(item, col++, cb);

  QSpinBox *sb = new QSpinBox;
  sb->setRange(-1, 99);
  sb->setValue(z.toInt());
  _tree->setItemWidget(item, col++, sb);

  sb = new QSpinBox;
  sb->setRange(1, 99);
  sb->setValue(pen.toInt());
  _tree->setItemWidget(item, col++, sb);
}

void IPLOTWidget::deleteItem ()
{
  QList<QTreeWidgetItem *> l = _tree->selectedItems();
  if (! l.count())
    return;

  int loop = 0;
  for (; loop < l.count(); loop++)
    delete l.at(loop);
}

void IPLOTWidget::loadSettings ()
{
  QSettings set(g_globalSettings);
  int loop = 0;
  for (; loop < _tree->columnCount(); loop++)
  {
    QString key = "IPLOTWidget_rule_column_width_" + QString::number(loop);
    _tree->setColumnWidth(loop, set.value(key, 75).toInt());
  }
}

void IPLOTWidget::commands (QStringList &l, int tab)
{
  int loop = 0;
  for (; loop < _tree->topLevelItemCount(); loop++)
  {
    QTreeWidgetItem *item = _tree->topLevelItem(loop);
    if (! item)
      continue;

    QStringList cl;
    cl << "PLUGIN=IPLOT";

    int col = 1;
    LineEdit *le = (LineEdit *) _tree->itemWidget(item, col++);
    cl << "NAME=" + le->text();

    QCheckBox *ch = (QCheckBox *) _tree->itemWidget(item, col++);
    if (ch->isChecked())
    {
      ColorButton *colb = (ColorButton *) _tree->itemWidget(item, col++);
      cl << "COLOR=" + colb->color().name();
    }
    else
      col++;
      
    ch = (QCheckBox *) _tree->itemWidget(item, col++);
    if (ch->isChecked())
    {
      QComboBox *cb = (QComboBox *) _tree->itemWidget(item, col++);
      cl << "STYLE=" + cb->currentText();
    }
    else
      col++;
    
    QSpinBox *sb = (QSpinBox *) _tree->itemWidget(item, col++);
    cl << "Z=" + sb->text();

    sb = (QSpinBox *) _tree->itemWidget(item, col++);
    cl << "PEN=" + sb->text();

    cl << "TAB=" + QString::number(tab);

    l.append(cl.join(","));
  }

  QSettings set(g_globalSettings);
  for (loop = 0; loop < _tree->columnCount(); loop++)
  {
    QString key = "IPLOTWidget_rule_column_width_" + QString::number(loop);
    set.setValue(key, _tree->columnWidth(loop));
  }
  set.sync();
}
