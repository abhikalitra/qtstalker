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

#include "CandlePatternDialog.h"
#include "Globals.h"
#include "CandleType.h"

#include "../../../pics/add.xpm"
#include "../../../pics/delete.xpm"

#include <QtDebug>
#include <QStringList>
#include <QToolBar>
#include <QToolButton>
#include <QComboBox>
#include <QFormLayout>
#include <QSettings>

CandlePatternDialog::CandlePatternDialog (QWidget *p, Setting *set) : QWidget (p)
{
  _settings = set;
  createGeneralPage();
  createPatternPage();
  loadSettings();
}

void CandlePatternDialog::createGeneralPage ()
{
  QVBoxLayout *vbox = new QVBoxLayout;
  setLayout(vbox);

  _tabs = new QTabWidget;
  vbox->addWidget(_tabs);

  QWidget *w = new QWidget;

  QFormLayout *form = new QFormLayout;
  w->setLayout(form);

  // candle color
  _color = new ColorButton(this, QColor(_settings->data("COLOR")));
  _color->setColorButton();
  form->addRow(tr("Color"), _color);

  // penetration
  _pen = new QDoubleSpinBox;
  _pen->setRange(0, 100);
  _pen->setValue(_settings->getDouble("PEN"));
  form->addRow(tr("Penetration"), _pen);

  // z
  _z = new QSpinBox;
  _z->setRange(-1, 99);
  _z->setValue(_settings->getInt("Z"));
  form->addRow(tr("Plot Order"), _z);

  _tabs->addTab(w, tr("General"));
}

void CandlePatternDialog::createPatternPage ()
{
  QWidget *w = new QWidget;

  QVBoxLayout *vbox = new QVBoxLayout;
  w->setLayout(vbox);

  // toolbar
  QToolBar *tb = new QToolBar;
  vbox->addWidget(tb);

  // add button
  QToolButton *b = new QToolButton;
  b->setIcon(QIcon(add_xpm));
  b->setToolTip(tr("Add Pattern"));
  connect(b, SIGNAL(clicked(bool)), this, SLOT(addPattern()));
  tb->addWidget(b);

  // delete button
  b = new QToolButton;
  b->setIcon(QIcon(delete_xpm));
  b->setToolTip(tr("Delete Pattern"));
  connect(b, SIGNAL(clicked(bool)), this, SLOT(deletePattern()));
  tb->addWidget(b);

  // list
  QStringList l;
  l << " " << tr("Pattern") << tr("Color");
  
  _plist = new QTreeWidget;
  _plist->setSortingEnabled(TRUE);
  _plist->setRootIsDecorated(FALSE);
  _plist->setHeaderLabels(l);
  _plist->setSelectionMode(QAbstractItemView::SingleSelection);
  _plist->setColumnWidth(0, 20);
  _plist->setColumnWidth(1, 150);
  vbox->addWidget(_plist);

  l = _settings->data("PATTERN").split(",", QString::SkipEmptyParts);
  int loop = 0;
  for (; loop < l.count(); loop += 2)
    addPattern(l.at(loop), l.at(loop + 1));

  _tabs->addTab(w, tr("Pattern"));
}

void CandlePatternDialog::addPattern ()
{
  addPattern(QString(), QString());
}

void CandlePatternDialog::addPattern (QString pattern, QString color)
{
  QTreeWidgetItem *item = new QTreeWidgetItem(_plist);

  CandleType ct;
  QComboBox *cb = new QComboBox;
  cb->addItems(ct.list());
  cb->setCurrentIndex(cb->findText(pattern, Qt::MatchExactly));
  _plist->setItemWidget(item, 1, cb);

  QColor col(Qt::red);
  if (! color.isEmpty())
    col.setNamedColor(color);
  
  ColorButton *c = new ColorButton(this, col);
  _plist->setItemWidget(item, 2, c);
}

void CandlePatternDialog::deletePattern ()
{
  QList<QTreeWidgetItem *> l = _plist->selectedItems();
  if (! l.count())
    return;

  int loop = 0;
  for (; loop < l.count(); loop++)
    delete l.at(loop);
}

void CandlePatternDialog::save ()
{
  _settings->setData("PEN", _pen->value());
  _settings->setData("COLOR", _color->color().name());
  _settings->setData("Z", _z->text());

  int loop = 0;
  QStringList l;
  for (; loop < _plist->topLevelItemCount(); loop++)
  {
    QTreeWidgetItem *item = _plist->topLevelItem(loop);

    QComboBox *cb = (QComboBox *) _plist->itemWidget(item, 1);
    l << cb->currentText();

    ColorButton *c = (ColorButton *) _plist->itemWidget(item, 2);
    l << c->color().name();
  }
  _settings->setData("PATTERN", l.join(","));

  QSettings set(g_globalSettings);
  for (loop = 0; loop < _plist->columnCount(); loop++)
  {
    QString key = "CandlePatternDialog_rule_column_width_" + QString::number(loop);
    set.setValue(key, _plist->columnWidth(loop));
  }
}

void CandlePatternDialog::loadSettings ()
{
  QSettings set(g_globalSettings);

  int loop = 0;
  for (; loop < _plist->columnCount(); loop++)
  {
    QString key = "CandlePatternDialog_rule_column_width_" + QString::number(loop);
    _plist->setColumnWidth(loop, set.value(key, 100).toInt());
  }
}
