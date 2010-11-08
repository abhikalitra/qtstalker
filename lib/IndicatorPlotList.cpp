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

#include "IndicatorPlotList.h"
#include "Config.h"
#include "IndicatorPluginFactory.h"
#include "IndicatorPlugin.h"
#include "Operator.h"
#include "../pics/indicator.xpm"
#include "../pics/configure.xpm"
#include "../pics/add.xpm"
#include "../pics/delete.xpm"

#include <QDebug>
#include <QLayout>
#include <QInputDialog>
#include <QTreeWidgetItem>
#include <QComboBox>
#include <QCheckBox>
#include <QFormLayout>
#include <QToolBar>

IndicatorPlotList::IndicatorPlotList ()
{
  QVBoxLayout *vbox = new QVBoxLayout;
  vbox->setMargin(5);
  vbox->setSpacing(2);
  setLayout(vbox);

  QFormLayout *form = new QFormLayout;
  form->setMargin(0);
  form->setSpacing(2);
  vbox->addLayout(form);

  _indicator = new QLabel;
  QString s = tr("Indicator") + ": ";
  form->addRow(s, _indicator);

  QToolBar *tb = new QToolBar;
  vbox->addWidget(tb);

  // indicator
  _indicatorAction = new QAction(QIcon(indicator_xpm), tr("Indicator..."), this);
  _indicatorAction->setStatusTip(tr("Select indicator..."));
  _indicatorAction->setToolTip(tr("Select indicator..."));
  connect(_indicatorAction, SIGNAL(activated()), this, SLOT(selectIndicator()));
  tb->addAction(_indicatorAction);
  
  // indicator settings
  _indicatorSettingsAction = new QAction(QIcon(configure_xpm), tr("Indicator Settings..."), this);
  _indicatorSettingsAction->setStatusTip(tr("Edit indicator settings..."));
  _indicatorSettingsAction->setToolTip(tr("Edit indicator settings..."));
  connect(_indicatorSettingsAction, SIGNAL(activated()), this, SLOT(editIndicator()));
  tb->addAction(_indicatorSettingsAction);

  // plot add
  _addAction = new QAction(QIcon(add_xpm), tr("Add Plot..."), this);
  _addAction->setStatusTip(tr("Add plot..."));
  _addAction->setToolTip(tr("Add plot..."));
  connect(_addAction, SIGNAL(activated()), this, SLOT(addPlot()));
  tb->addAction(_addAction);

  // plot delete
  _deleteAction = new QAction(QIcon(delete_xpm), tr("Delete Plot..."), this);
  _deleteAction->setStatusTip(tr("Delete plot..."));
  _deleteAction->setToolTip(tr("Delete plot..."));
  connect(_deleteAction, SIGNAL(activated()), this, SLOT(deletePlot()));
  tb->addAction(_deleteAction);

  QStringList l;
  l << "#" << tr("Plot") << tr("Operator") << tr("Value");

  _list = new QTreeWidget;
  _list->setSortingEnabled(FALSE);
  _list->setRootIsDecorated(FALSE);
  _list->setHeaderLabels(l);
  _list->setSelectionMode(QAbstractItemView::SingleSelection);
  vbox->addWidget(_list);

  buttonStatus();
}

void IndicatorPlotList::selectIndicator ()
{
  Config config;
  QStringList l;
  config.getData(Config::IndicatorPluginList, l);

  QInputDialog *dialog = new QInputDialog;
  QString s = "Qtstalker: " + tr("Select Indicator");
  dialog->setWindowTitle(s);
  dialog->setLabelText(tr("Indicator"));
  dialog->setInputMode(QInputDialog::TextInput);
  dialog->setComboBoxEditable(FALSE);
  dialog->setComboBoxItems(l);
  dialog->setTextValue(_indicator->text());
  connect(dialog, SIGNAL(textValueSelected(const QString &)), this, SLOT(selectIndicator2(QString)));
  connect(dialog, SIGNAL(finished(int)), dialog, SLOT(deleteLater()));
  dialog->show();
}

void IndicatorPlotList::selectIndicator2 (QString s)
{
  _indicator->setText(s);
  indicatorChanged();
}

void IndicatorPlotList::addPlot ()
{
  if (_indicator->text() == "CUS")
  {
    QInputDialog *dialog = new QInputDialog;
    QString s = "Qtstalker: " + tr("Add Plot Item");
    dialog->setWindowTitle(s);
    dialog->setLabelText(tr("Enter required CUS variables seperated by a space"));
    dialog->setInputMode(QInputDialog::TextInput);
    connect(dialog, SIGNAL(textValueSelected(const QString &)), this, SLOT(addPlot2(QString)));
    connect(dialog, SIGNAL(finished(int)), dialog, SLOT(deleteLater()));
    dialog->show();
    return;
  }

  addPlotItem(_plotNames.at(0), QString("0"), QString("Close"));

  buttonStatus();

  emit signalItemChanged();
}

void IndicatorPlotList::addPlot2 (QString d)
{
  if (d.isEmpty())
    return;

  _plotNames = d.split(" ");

  addPlotItem(_plotNames.at(0), QString("0"), QString("Close"));

  buttonStatus();

  emit signalItemChanged();
}

void IndicatorPlotList::deletePlot ()
{
  QList<QTreeWidgetItem *> l = _list->selectedItems();
  if (! l.count())
    return;

  delete l.at(0);
  
  int loop = 0;
  for (; loop < _list->topLevelItemCount(); loop++)
  {
    QTreeWidgetItem *item = _list->topLevelItem(loop);
    item->setText(0, QString::number(loop + 1));
  }

  buttonStatus();
  emit signalItemChanged();
}

void IndicatorPlotList::indicatorChanged ()
{
  IndicatorPluginFactory fac;
  IndicatorPlugin *plugin = fac.plugin(_indicator->text());
  if (! plugin)
  {
    qDebug() << "IndicatorPlotList::indicatorChanged: no plugin";
    return;
  }

  Indicator i;
  plugin->defaults(i);
  _settings = i.settings();

  plugin->plotNames(i, _plotNames);
  
  _list->clear();

  buttonStatus();

  emit signalIndicatorChanged();
}

void IndicatorPlotList::addPlotItem (QString pn, QString oper, QString val)
{
  QTreeWidgetItem *item = new QTreeWidgetItem(_list);

  QString s = QString::number(_list->topLevelItemCount());
  item->setText(0, s);

  QStringList l = _plotNames;
  l << "Open" << "High" << "Low" << "Close" << "Volume" << "OI";
  if (l.indexOf(val) == -1)
    l << val;

  QComboBox *cb = new QComboBox;
  cb->setEditable(TRUE);
  cb->addItems(l);
  cb->clearEditText();
  if (_indicator->text() == "CUS")
    cb->setEditText(pn);
  else    
    cb->setCurrentIndex(cb->findText(pn));
  _list->setItemWidget(item, 1, cb);
  connect(cb, SIGNAL(currentIndexChanged(int)), this, SIGNAL(signalItemChanged()));
  connect(cb, SIGNAL(editTextChanged(const QString &)), this, SIGNAL(signalItemChanged()));

  cb = new QComboBox;
  cb->setEditable(FALSE);
  Operator op;
  cb->addItems(op.list());
  cb->setCurrentIndex(oper.toInt());
  _list->setItemWidget(item, 2, cb);
  connect(cb, SIGNAL(currentIndexChanged(int)), this, SIGNAL(signalItemChanged()));

  cb = new QComboBox;
  cb->setEditable(TRUE);
  cb->addItems(l);
  cb->clearEditText();
  if (_indicator->text() == "CUS")
    cb->setEditText(val);
  else
    cb->setCurrentIndex(cb->findText(val));
  _list->setItemWidget(item, 3, cb);
  connect(cb, SIGNAL(currentIndexChanged(int)), this, SIGNAL(signalItemChanged()));
  connect(cb, SIGNAL(editTextChanged(const QString &)), this, SIGNAL(signalItemChanged()));

  int loop = 0;
  for (; loop < _list->columnCount(); loop++)
    _list->resizeColumnToContents(loop);
}

void IndicatorPlotList::editIndicator ()
{
  IndicatorPluginFactory fac;
  IndicatorPlugin *plug = fac.plugin(_indicator->text());
  if (! plug)
  {
    qDebug() << "IndicatorPlotList::editIndicator: no plugin";
    return;
  }

  Indicator i;
  if (! _settings.count())
  {
    plug->defaults(i);
    _settings = i.settings();
  }
  else
    i.setSettings(_settings);

  IndicatorPluginDialog *dialog = plug->dialog(i);
  if (! dialog)
  {
    qDebug() << "IndicatorPlotList::editIndicator: no dialog";
    return;
  }

  connect(dialog, SIGNAL(signalDone(Indicator)), this, SLOT(editIndicator2(Indicator)));
  connect(dialog, SIGNAL(finished(int)), dialog, SLOT(deleteLater()));
  dialog->show();
}

void IndicatorPlotList::editIndicator2 (Indicator i)
{
  _settings = i.settings();
  emit signalIndicatorChanged();
}

void IndicatorPlotList::setIndicator (QString d)
{
  if (d.isEmpty())
    return;
  
  _indicator->setText(d);

  IndicatorPluginFactory fac;
  IndicatorPlugin *plugin = fac.plugin(_indicator->text());
  if (! plugin)
  {
    qDebug() << "IndicatorPlotList::setIndicator: no plugin";
    return;
  }

  Indicator i;
  plugin->defaults(i);

  plugin->plotNames(i, _plotNames);
}

QString IndicatorPlotList::indicator ()
{
  return _indicator->text();
}

void IndicatorPlotList::setList (QStringList l)
{
  _list->clear();
  
  if (! l.count())
    return;

  int loop = 0;
  for (; loop < l.count(); loop++)
  {
    QStringList l2 = l.at(loop).split(",");
    if (l2.count() != 3)
      continue;
    
    addPlotItem(l2.at(0), l2.at(1), l2.at(2));
  }

  buttonStatus();
}

void IndicatorPlotList::list (QStringList &items)
{
  items.clear();

  int loop = 0;
  for (; loop < _list->topLevelItemCount(); loop++)
  {
    QTreeWidgetItem *item = _list->topLevelItem(loop);

    QStringList l;
    QComboBox *cb = (QComboBox *) _list->itemWidget(item, 1);
    l << cb->currentText();

    cb = (QComboBox *) _list->itemWidget(item, 2);
    l << QString::number(cb->currentIndex());

    cb = (QComboBox *) _list->itemWidget(item, 3);
    l << cb->currentText();

    items << l.join(",");
  }
}

void IndicatorPlotList::setSettings (Setting d)
{
  _settings = d;
}

Setting IndicatorPlotList::settings ()
{
  return _settings;
}

void IndicatorPlotList::buttonStatus ()
{
  bool status = FALSE;
  if (! _indicator->text().isEmpty())
    status = TRUE;
  
  _addAction->setEnabled(status);

  if (! _list->topLevelItemCount())
    status = FALSE;
  _deleteAction->setEnabled(status);
}

