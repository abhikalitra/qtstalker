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

#include "COLORDialog.h"
#include "Globals.h"
#include "Operator.h"
#include "ColorButton.h"

#include "../../../pics/add.xpm"
#include "../../../pics/delete.xpm"

#include <QtDebug>
#include <QStringList>
#include <QVBoxLayout>
#include <QToolBar>
#include <QToolButton>
#include <QLineEdit>
#include <QComboBox>
#include <QSettings>

COLORDialog::COLORDialog (QWidget *p, Setting *set) : QWidget (p)
{
  _settings = set;
  createGeneralPage();
  loadSettings();
}

void COLORDialog::createGeneralPage ()
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
  connect(b, SIGNAL(clicked(bool)), this, SLOT(addPattern()));
  tb->addWidget(b);

  // delete button
  b = new QToolButton;
  b->setIcon(QIcon(delete_xpm));
  b->setToolTip(tr("Delete Rule"));
  connect(b, SIGNAL(clicked(bool)), this, SLOT(deletePattern()));
  tb->addWidget(b);

  // list
  QStringList l;
  l << " " << tr("Color") << tr("Input 1") << tr("Operator") << tr("Input 2") << tr("Input 3");

  _plist = new QTreeWidget;
  _plist->setSortingEnabled(TRUE);
  _plist->setRootIsDecorated(FALSE);
  _plist->setHeaderLabels(l);
  _plist->setSelectionMode(QAbstractItemView::SingleSelection);
  _plist->setColumnWidth(0, 20);
  vbox->addWidget(_plist);

  l = _settings->data("RULES").split(",", QString::SkipEmptyParts);
  int loop = 0;
  for (; loop < l.count(); loop += 5)
    addPattern(l.at(loop), l.at(loop + 1), l.at(loop + 2), l.at(loop + 3), l.at(loop + 4));
}

void COLORDialog::save ()
{
  int loop = 0;
  QStringList l;
  for (; loop < _plist->topLevelItemCount(); loop++)
  {
    QTreeWidgetItem *item = _plist->topLevelItem(loop);

    int pos = 1;
    ColorButton *c = (ColorButton *) _plist->itemWidget(item, pos++);
    l << c->color().name();

    QLineEdit *le = (QLineEdit *) _plist->itemWidget(item, pos++);
    l << le->text();

    QComboBox *cb = (QComboBox *) _plist->itemWidget(item, pos++);
    l << cb->currentText();

    le = (QLineEdit *) _plist->itemWidget(item, pos++);
    l << le->text();

    le = (QLineEdit *) _plist->itemWidget(item, pos++);
    l << le->text();
  }
  _settings->setData("RULES", l.join(","));

  QSettings set(g_globalSettings);
  for (loop = 0; loop < _plist->columnCount(); loop++)
  {
    QString key = "COLORDialog_rule_column_width_" + QString::number(loop);
    set.setValue(key, _plist->columnWidth(loop));
  }
}

void COLORDialog::addPattern ()
{
  addPattern(QString(), QString(), QString(), QString(), QString());
}

void COLORDialog::addPattern (QString color, QString in, QString op, QString in2, QString in3)
{
  QTreeWidgetItem *item = new QTreeWidgetItem(_plist);

  int pos = 1;
  QColor col(Qt::red);
  if (! color.isEmpty())
    col.setNamedColor(color);

  ColorButton *c = new ColorButton(this, col);
  _plist->setItemWidget(item, pos++, c);

  QLineEdit *le = new QLineEdit(in);
  _plist->setItemWidget(item, pos++, le);

  Operator top;
  QComboBox *cb = new QComboBox;
  cb->addItems(top.list());
  cb->setCurrentIndex(cb->findText(op, Qt::MatchExactly));
  _plist->setItemWidget(item, pos++, cb);

  le = new QLineEdit(in2);
  _plist->setItemWidget(item, pos++, le);

  le = new QLineEdit(in3);
  _plist->setItemWidget(item, pos++, le);
}

void COLORDialog::deletePattern ()
{
  QList<QTreeWidgetItem *> l = _plist->selectedItems();
  if (! l.count())
    return;

  int loop = 0;
  for (; loop < l.count(); loop++)
    delete l.at(loop);
}

void COLORDialog::loadSettings ()
{
  QSettings set(g_globalSettings);

  int loop = 0;
  for (; loop < _plist->columnCount(); loop++)
  {
    QString key = "COLORDialog_rule_column_width_" + QString::number(loop);
    _plist->setColumnWidth(loop, set.value(key, 100).toInt());
  }
}
