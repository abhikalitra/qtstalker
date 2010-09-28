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

#include "AlertEditDialog.h"
#include "Globals.h"
#include "AlertDataBase.h"
#include "IndicatorPluginFactory.h"
#include "IndicatorPlugin.h"
#include "Setting.h"
#include "../../../pics/indicator.xpm"
#include "../../../pics/search.xpm"
#include "AlertConfig.h"
#include "SymbolDialog.h"
#include "AlertOperator.h"
#include "DateRange.h"

#include <QtDebug>
#include <QComboBox>
#include <QDoubleSpinBox>
#include <QToolButton>
#include <QIcon>

AlertEditDialog::AlertEditDialog (AlertItem item)
{
  _item = item;
  
  createMainPage();

  setSettings();
}

void AlertEditDialog::createMainPage ()
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
  QLabel *label = new QLabel(tr("Symbol"));
  grid->addWidget(label, row, col++);

  _symbol = new QLineEdit;
  _symbol->setReadOnly(TRUE);
  grid->addWidget(_symbol, row, col++);

  QToolButton *tb = new QToolButton;
  tb->setIcon(QIcon(search_xpm));
  tb->setToolTip(tr("Symbol search..."));
  connect(tb, SIGNAL(clicked()), this, SLOT(symbolSearch()));
  grid->addWidget(tb, row++, col);
  col -= 2;

  // indicator
  label = new QLabel(tr("Indicator"));
  grid->addWidget(label, row, col++);

  _indicator = new QLineEdit;
  _indicator->setReadOnly(TRUE);
  grid->addWidget(_indicator, row, col++);

  tb = new QToolButton;
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
  label = new QLabel(tr("Bars"));
  grid->addWidget(label, row, col++);

  DateRange dr;
  l.clear();
  dr.list(l);
  
  _bars = new QComboBox;
  _bars->addItems(l);
  _bars->setToolTip(tr("The amount of bars to use for the indicator"));
  grid->addWidget(_bars, row++, col--);

  // mail notify
  QHBoxLayout *thbox = new QHBoxLayout;
  thbox->setMargin(0);
  thbox->setSpacing(2);
  vbox->addLayout(thbox);

  _mailNotify = new QCheckBox;
  _mailNotify->setText(tr("Mail"));
  _mailNotify->setToolTip(tr("Notify by mail"));
  thbox->addWidget(_mailNotify);
  
  // sound notify
  _soundNotify = new QCheckBox;
  _soundNotify->setText(tr("Sound"));
  _soundNotify->setToolTip(tr("Notify by playing a sound file"));
  thbox->addWidget(_soundNotify);

  // popup notify
  _popupNotify = new QCheckBox;
  _popupNotify->setText(tr("Popup"));
  _popupNotify->setToolTip(tr("Notify by popup message"));
  thbox->addWidget(_popupNotify);

  thbox->addStretch();

  // alert list
  l.clear();
  l << tr("Enable") << tr("Plot") << tr("Operator") << tr("Value");
  
  _alertList = new QTreeWidget;
  _alertList->setSortingEnabled(FALSE);
  _alertList->setRootIsDecorated(FALSE);
  _alertList->setHeaderLabels(l);
  _alertList->setSelectionMode(QAbstractItemView::SingleSelection);
  vbox->addWidget(_alertList);
  
  _tabs->addTab(w, tr("Settings"));
}

void AlertEditDialog::indicatorSettings ()
{
  QString s = _indicator->text();

  IndicatorPluginFactory fac;
  IndicatorPlugin *plugin = fac.plugin(s);
  if (! plugin)
    return;

  Setting set;
  set.parse(_item.settings());
  
  Indicator i;
  i.setSettings(set);
  
  IndicatorPluginDialog *dialog = plugin->dialog(i);
  if (! dialog)
  {
    qDebug() << "AlertEditDialog::indicatorSettings: no dialog";
    return;
  }

  connect(dialog, SIGNAL(signalDone(Indicator)), this, SLOT(indicatorSettings2(Indicator)));
  connect(dialog, SIGNAL(finished(int)), dialog, SLOT(deleteLater()));
  dialog->show();
}

void AlertEditDialog::indicatorSettings2 (Indicator i)
{
  QString s;
  i.settings().getString(s);
  _item.setSettings(s);
}

void AlertEditDialog::setSettings ()
{
  if (_item.id() != -1)
  {
    QString s = _item.exchange() + ":" + _item.symbol();
    _symbol->setText(s);
  }
  
  _indicator->setText(_item.indicator());

  _barLength->setCurrentIndex(_item.barLength());
  
  _bars->setCurrentIndex(_item.bars());

  _mailNotify->setChecked(_item.mail());
  
  _soundNotify->setChecked(_item.sound());
  
  _popupNotify->setChecked(_item.popup());

  _alertList->clear();

  QString s = _indicator->text();

  IndicatorPluginFactory fac;
  IndicatorPlugin *plugin = fac.plugin(s);
  if (! plugin)
    return;

  Indicator i;
  if (_item.id() == -1)
    plugin->defaults(i);
  else
  {
    Setting set;
    set.parse(_item.settings());
    i.setSettings(set);
  }

  Setting set = i.settings();
  set.getString(s);
  _item.setSettings(s);

  QStringList plotNames;
  plugin->plotNames(i, plotNames);

  int loop = 0;
  for (; loop < plotNames.count(); loop++)
  {
    QTreeWidgetItem *item = new QTreeWidgetItem(_alertList);

    item->setCheckState(0, (Qt::CheckState) _item.enable(plotNames.at(loop)));

    item->setText(1, plotNames.at(loop));

    QComboBox *cb = new QComboBox;
    AlertOperator op;
    cb->addItems(op.list());
    cb->setCurrentIndex(_item.op(plotNames.at(loop)));
    _alertList->setItemWidget(item, 2, cb);

    QDoubleSpinBox *sb = new QDoubleSpinBox;
    sb->setRange(-99999999, 99999999);
    sb->setValue(_item.value(plotNames.at(loop)));
    _alertList->setItemWidget(item, 3, sb);
  }

  for (loop = 0; loop < _alertList->topLevelItemCount(); loop++)
    _alertList->resizeColumnToContents(loop);
}

void AlertEditDialog::done ()
{
  if (_item.id() == -1)
  {
    AlertConfig db;
    int id = db.getInt(AlertConfig::_LastId);
    _item.setId(id);

    id++;
    db.transaction();
    db.setData(AlertConfig::_LastId, id);
    db.commit();
  }

  QStringList l = _symbol->text().split(":");
  if (l.count())
  {
    _item.setExchange(l.at(0));
    _item.setSymbol(l.at(1));
  }

  _item.setBarLength(_barLength->currentIndex());

  _item.setBars(_bars->currentIndex());

  _item.setMail(_mailNotify->isChecked());

  _item.setSound(_soundNotify->isChecked());

  _item.setPopup(_popupNotify->isChecked());

  int loop = 0;
  for (; loop < _alertList->topLevelItemCount(); loop++)
  {
    QTreeWidgetItem *item = _alertList->topLevelItem(loop);

    _item.setEnable(item->text(1), item->checkState(0));

    QComboBox *cb = (QComboBox *) _alertList->itemWidget(item, 2);
    _item.setOp(item->text(1), cb->currentIndex());

    QDoubleSpinBox *sb = (QDoubleSpinBox *) _alertList->itemWidget(item, 3);
    _item.setValue(item->text(1), sb->value());
  }

  AlertDataBase db;
  db.transaction();
  db.setAlert(_item);
  db.commit();
  
  emit signalEdit(_item);

  accept();
}

void AlertEditDialog::symbolSearch ()
{
  SymbolDialog *dialog = new SymbolDialog;
  connect(dialog, SIGNAL(signalSymbols(Group)), this, SLOT(symbolSearch2(Group)));
  dialog->show();
}

void AlertEditDialog::symbolSearch2 (Group g)
{
  if (! g.count())
    return;
  
  QStringList l;
  g.getStringList(l);

  BarData bd;
  g.getSymbol(l.at(0), bd);
  _symbol->setText(bd.getKey());
}

