/*
 *  Qtstalker stock charter
 *
 *  Copyright (C) 2001-2007 Stefan S. Stratigakos
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

#include "TestRuleDialog.h"
#include "Operator.h"
#include "Globals.h"
#include "../pics/add.xpm"
#include "../pics/delete.xpm"

#include <QDebug>
#include <QToolButton>
#include <QToolBar>
#include <QComboBox>
#include <QSpinBox>

TestRuleDialog::TestRuleDialog (QWidget *p, Setting *set) : Dialog (p)
{
  _settings = set;
  _keySize = "TestRuleDialog_window_size";
  _keyPos = "TestRuleDialog_window_position";

  QStringList l;
  l << "QtStalker" + g_session + ":" << tr("Test Rule");
  setWindowTitle(l.join(" "));

  createGUI();

  loadSettings();

  loadRule();
}

void TestRuleDialog::createGUI ()
{
  int row = 0;
  
  // toolbar
  QToolBar *tb = new QToolBar;
  _vbox->insertWidget(row++, tb);

  // add button
  QToolButton *b = new QToolButton;
  b->setIcon(QIcon(add_xpm));
  b->setToolTip(tr("Add Rule"));
  connect(b, SIGNAL(clicked(bool)), this, SLOT(addRule()));
  tb->addWidget(b);

  // delete button
  b = new QToolButton;
  b->setIcon(QIcon(delete_xpm));
  b->setToolTip(tr("Delete Rule"));
  connect(b, SIGNAL(clicked(bool)), this, SLOT(deleteRule()));
  tb->addWidget(b);
 
  // tree
  QStringList l;
  l << " " << tr("Enable") << tr("Value") << tr("Offset") << tr("Op") << tr("Value") << tr("Offset");
  
  _tree = new QTreeWidget;
  _tree->setSortingEnabled(FALSE);
  _tree->setRootIsDecorated(FALSE);
  _tree->setHeaderLabels(l);
  _tree->setSelectionMode(QAbstractItemView::SingleSelection);
  _tree->setColumnWidth(0, 20);
  _vbox->insertWidget(row++, _tree);
}

void TestRuleDialog::addRule ()
{
  addRule(QString::number(Qt::Checked),
	  "Close",
	  "0",
	  "GT",
	  "Close",
	  "0");
}

void TestRuleDialog::addRule (QString check, QString val, QString offset, QString op, QString val2, QString offset2)
{
  QTreeWidgetItem *item = new QTreeWidgetItem(_tree);
  int col = 1;

  // enable column
  item->setCheckState(col++, (Qt::CheckState) check.toInt());

  // value column
  QComboBox *cb = new QComboBox;
  cb->addItems(_settings->data("INPUT").split(",", QString::SkipEmptyParts));
  cb->setCurrentIndex(cb->findText(val, Qt::MatchExactly));
  _tree->setItemWidget(item, col++, cb);

  // offset column
  QSpinBox *sb = new QSpinBox;
  sb->setRange(0, 1000);
  sb->setValue(offset.toInt());
  _tree->setItemWidget(item, col++, sb);
  
  // op column
  Operator oper;
  cb = new QComboBox;
  cb->addItems(oper.list());
  cb->setCurrentIndex(cb->findText(op, Qt::MatchExactly));
  _tree->setItemWidget(item, col++, cb);

  // value2 column
  cb = new QComboBox;
  cb->addItems(_settings->data("INPUT").split(",", QString::SkipEmptyParts));
  cb->setCurrentIndex(cb->findText(val2, Qt::MatchExactly));
  _tree->setItemWidget(item, col++, cb);

  // offset2 column
  sb = new QSpinBox;
  sb->setRange(0, 1000);
  sb->setValue(offset2.toInt());
  _tree->setItemWidget(item, col++, sb);

  int loop = 0;
  for (; loop < _tree->columnCount(); loop++)
    _tree->resizeColumnToContents(loop);
}

void TestRuleDialog::deleteRule ()
{
  QList<QTreeWidgetItem *> l = _tree->selectedItems();
  if (! l.count())
    return;

  int loop = 0;
  for (; loop < l.count(); loop++)
    delete l.at(loop);
}

void TestRuleDialog::loadRule ()
{
  int loop = 1;
  int size = _settings->getInt("COUNT");
  for (; loop <= size; loop++)
  {
    QString key = "RULE" + QString::number(loop);
    QStringList l = _settings->data(key).split(",", QString::SkipEmptyParts);
    if (l.count() != 6)
      continue;

    addRule(l.at(0), l.at(1), l.at(2), l.at(3), l.at(4), l.at(5));
  }
}

void TestRuleDialog::saveRule ()
{
  int loop = 0;
  for (; loop < _tree->topLevelItemCount() ; loop++)
  {
    QStringList l;
    int col = 1;
    
    QTreeWidgetItem *item = _tree->topLevelItem(loop);

    // get enable
    l << QString::number(item->checkState(col++));

    // get value
    QComboBox *cb = (QComboBox *) _tree->itemWidget(item, col++);
    l << cb->currentText();

    // get offset
    QSpinBox *sb = (QSpinBox *) _tree->itemWidget(item, col++);
    l << sb->text();

    // get operator
    cb = (QComboBox *) _tree->itemWidget(item, col++);
    l << cb->currentText();

    // get value2
    cb = (QComboBox *) _tree->itemWidget(item, col++);
    l << cb->currentText();

    // get offset2
    sb = (QSpinBox *) _tree->itemWidget(item, col++);
    l << sb->text();

    // save rule
    QString key = "RULE" + QString::number(loop + 1);
    _settings->setData(key, l.join(","));

    // save rule count
    _settings->setData("COUNT", loop + 1);
  }
}

void TestRuleDialog::done ()
{
  saveRule();
  
  saveSettings();

  accept();
}
