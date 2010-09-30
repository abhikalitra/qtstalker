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

#include "ScannerEditDialog.h"
#include "Globals.h"
#include "ScannerDataBase.h"
#include "IndicatorPluginFactory.h"
#include "IndicatorPlugin.h"
#include "Setting.h"
#include "../../../pics/indicator.xpm"
#include "Operator.h"
#include "DateRange.h"

#include <QtDebug>
#include <QComboBox>
#include <QDoubleSpinBox>
#include <QToolButton>
#include <QIcon>

ScannerEditDialog::ScannerEditDialog (ScannerItem scanner)
{
  _scanner = scanner;

  createMainPage();

  setSettings();
}

void ScannerEditDialog::createMainPage ()
{
  QWidget *w = new QWidget;

  QVBoxLayout *vbox = new QVBoxLayout;
  vbox->setMargin(5);
  vbox->setSpacing(2);
  w->setLayout(vbox);

  QGridLayout *grid = new QGridLayout;
  grid->setMargin(0);
  grid->setSpacing(2);
  grid->setColumnStretch(3, 1);
  vbox->addLayout(grid);

  int row = 0;
  int col = 0;

  // symbol
  QLabel *label = new QLabel(tr("Symbols"));
  grid->addWidget(label, row, col++);

  _symbols = new SymbolButton(w);
  grid->addWidget(_symbols, row++, col--);

  // indicator
  label = new QLabel(tr("Indicator"));
  grid->addWidget(label, row, col++);

  _indicator = new QLineEdit;
  _indicator->setReadOnly(TRUE);
  grid->addWidget(_indicator, row, col++);

  QToolButton *tb = new QToolButton;
  tb->setIcon(QIcon(indicator_xpm));
  tb->setToolTip(tr("Indicator settings..."));
  connect(tb, SIGNAL(clicked()), this, SLOT(indicatorSettings()));
  grid->addWidget(tb, row++, col);
  col -= 2;

  // bar length
  label = new QLabel(tr("Bar Length"));
  grid->addWidget(label, row, col++);

  BarData bd;
  QStringList l;
  bd.getBarLengthList(l);
  
  _barLength = new QComboBox;
  _barLength->addItems(l);
  grid->addWidget(_barLength, row++, col--);

  // bar range
  label = new QLabel(tr("Date Range"));
  grid->addWidget(label, row, col++);

  DateRange dr;
  l.clear();
  dr.list(l);
  
  _dateRange = new QComboBox;
  _dateRange->addItems(l);
  _dateRange->setToolTip(tr("The amount of bars to use for the indicator"));
  grid->addWidget(_dateRange, row++, col--);

  // group name
  label = new QLabel(tr("Group Name"));
  grid->addWidget(label, row, col++);

  _groupName = new QLineEdit;
  _groupName->setToolTip(tr("Scan results group name"));
  grid->addWidget(_groupName, row++, col--);

  // alert list
  l.clear();
  l << tr("Enable") << tr("Plot") << tr("Operator") << tr("Value");
  
  _list = new QTreeWidget;
  _list->setSortingEnabled(FALSE);
  _list->setRootIsDecorated(FALSE);
  _list->setHeaderLabels(l);
  _list->setSelectionMode(QAbstractItemView::SingleSelection);
  vbox->addWidget(_list);
  
  _tabs->addTab(w, tr("Settings"));
}

void ScannerEditDialog::indicatorSettings ()
{
  QString s = _indicator->text();

  IndicatorPluginFactory fac;
  IndicatorPlugin *plugin = fac.plugin(s);
  if (! plugin)
    return;

  Setting set;
  set.parse(_scanner.settings());
  
  Indicator i;
  i.setSettings(set);
  
  IndicatorPluginDialog *dialog = plugin->dialog(i);
  if (! dialog)
  {
    qDebug() << "ScannerEditDialog::indicatorSettings: no dialog";
    return;
  }

  connect(dialog, SIGNAL(signalDone(Indicator)), this, SLOT(indicatorSettings2(Indicator)));
  connect(dialog, SIGNAL(finished(int)), dialog, SLOT(deleteLater()));
  dialog->show();
}

void ScannerEditDialog::indicatorSettings2 (Indicator i)
{
  QString s;
  i.settings().getString(s);
  _scanner.setSettings(s);
}

void ScannerEditDialog::setSettings ()
{
  _indicator->setText(_scanner.indicator());

  _barLength->setCurrentIndex(_scanner.barLength());
  
  _dateRange->setCurrentIndex(_scanner.dateRange());

  _groupName->setText(_scanner.groupName());

  _symbols->setSymbols(_scanner.group());
  
  _list->clear();

  QString s = _indicator->text();

  IndicatorPluginFactory fac;
  IndicatorPlugin *plugin = fac.plugin(s);
  if (! plugin)
    return;

  Indicator i;
  if (! _scanner.settings().length())
    plugin->defaults(i);
  else
  {
    Setting set;
    set.parse(_scanner.settings());
    i.setSettings(set);
  }

  Setting set = i.settings();
  set.getString(s);
  _scanner.setSettings(s);

  QStringList plotNames;
  plugin->plotNames(i, plotNames);

  int loop = 0;
  for (; loop < plotNames.count(); loop++)
  {
    QTreeWidgetItem *item = new QTreeWidgetItem(_list);

    item->setCheckState(0, (Qt::CheckState) _scanner.enable(plotNames.at(loop)));

    item->setText(1, plotNames.at(loop));

    QComboBox *cb = new QComboBox;
    Operator op;
    cb->addItems(op.list());
    cb->setCurrentIndex(_scanner.op(plotNames.at(loop)));
    _list->setItemWidget(item, 2, cb);

    QDoubleSpinBox *sb = new QDoubleSpinBox;
    sb->setRange(-99999999, 99999999);
    sb->setValue(_scanner.value(plotNames.at(loop)));
    _list->setItemWidget(item, 3, sb);
  }

  for (loop = 0; loop < _list->topLevelItemCount(); loop++)
    _list->resizeColumnToContents(loop);
}

void ScannerEditDialog::done ()
{
  // remove any forbidden sql characters
  QString gn = _groupName->text();
  gn = gn.remove(QString("'"), Qt::CaseSensitive);

  if (! gn.length())
  {
    setMessage(tr("Group name empty or invalid"));
    return;
  }

  _scanner.setGroupName(gn);

  _scanner.setGroup(_symbols->getSymbols());

  _scanner.setBarLength(_barLength->currentIndex());

  _scanner.setDateRange(_dateRange->currentIndex());

  int loop = 0;
  for (; loop < _list->topLevelItemCount(); loop++)
  {
    QTreeWidgetItem *item = _list->topLevelItem(loop);

    _scanner.setEnable(item->text(1), item->checkState(0));

    QComboBox *cb = (QComboBox *) _list->itemWidget(item, 2);
    _scanner.setOp(item->text(1), cb->currentIndex());

    QDoubleSpinBox *sb = (QDoubleSpinBox *) _list->itemWidget(item, 3);
    _scanner.setValue(item->text(1), sb->value());
  }

  ScannerDataBase db;
  db.transaction();
  db.setScanner(_scanner);
  db.commit();
  
  emit signalEdit(_scanner);

  accept();
}

