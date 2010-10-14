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
  l << tr("Enable") << tr("Plot") << tr("Operator") << tr("Value");

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
  QInputDialog *dialog = new QInputDialog;
  QString s = "Qtstalker: " + tr("Add Plot Item");
  dialog->setWindowTitle(s);
  dialog->setLabelText(tr("Enter required CUS variables seperated by a space"));
  dialog->setInputMode(QInputDialog::TextInput);
  connect(dialog, SIGNAL(textValueSelected(const QString &)), this, SLOT(addPlot2(QString)));
  connect(dialog, SIGNAL(finished(int)), dialog, SLOT(deleteLater()));
  dialog->show();
}

void IndicatorPlotList::addPlot2 (QString d)
{
  if (d.isEmpty())
    return;

  QStringList plotNames = d.split(" ");

  int loop = 0;
  for (; loop < plotNames.count(); loop++)
    addPlotItem(plotNames, QString("0"), plotNames.at(loop), QString("0"), QString("Close"));

  buttonStatus();

  emit signalItemChanged();
}

void IndicatorPlotList::deletePlot ()
{
  QStringList l;
  int loop = 0;
  for (; loop < _list->topLevelItemCount(); loop++)
  {
    QTreeWidgetItem *item = _list->topLevelItem(loop);
    l << item->text(1);
  }

  QInputDialog *dialog = new QInputDialog;
  QString s = "Qtstalker: " + tr("Delete Plot Item");
  dialog->setWindowTitle(s);
  dialog->setLabelText(tr("Plot Item"));
  dialog->setInputMode(QInputDialog::TextInput);
  dialog->setComboBoxEditable(FALSE);
  dialog->setComboBoxItems(l);
  dialog->setTextValue(_indicator->text());
  connect(dialog, SIGNAL(textValueSelected(const QString &)), this, SLOT(deletePlot2(QString)));
  connect(dialog, SIGNAL(finished(int)), dialog, SLOT(deleteLater()));
  dialog->show();
}

void IndicatorPlotList::deletePlot2 (QString d)
{
  int loop = 0;
  for (; loop < _list->topLevelItemCount(); loop++)
  {
    QTreeWidgetItem *item = _list->topLevelItem(loop);

    if (item->text(1) == d)
    {
      delete item;
      buttonStatus();
      emit signalItemChanged();
      break;
    }
  }
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

  QStringList plotNames;
  plugin->plotNames(i, plotNames);

  _list->clear();

  int loop = 0;
  for (; loop < plotNames.count(); loop++)
    addPlotItem(plotNames, QString("0"), plotNames.at(loop), QString("0"), QString("Close"));

  buttonStatus();

  emit signalIndicatorChanged();
}

void IndicatorPlotList::addPlotItem (QStringList &plotNames, QString enable, QString pn, QString oper, QString val)
{
  QTreeWidgetItem *item = new QTreeWidgetItem(_list);

  QCheckBox *check = new QCheckBox;
  check->setCheckState((Qt::CheckState) enable.toInt());
  _list->setItemWidget(item, 0, check);
  connect(check, SIGNAL(stateChanged(int)), this, SIGNAL(signalItemChanged()));

  item->setText(1, pn);

  QComboBox *cb = new QComboBox;
  Operator op;
  cb->addItems(op.list());
  cb->setCurrentIndex(oper.toInt());
  _list->setItemWidget(item, 2, cb);
  connect(cb, SIGNAL(currentIndexChanged(int)), this, SIGNAL(signalItemChanged()));

  QComboBox *vc = new QComboBox;
  vc->setEditable(TRUE);
  vc->addItems(plotNames);

  QStringList l;
  l << "Open" << "High" << "Low" << "Close" << "Volume" << "OI";
  vc->addItems(l);

  vc->clearEditText();
  vc->setCurrentIndex(vc->findText(val));
  _list->setItemWidget(item, 3, vc);
  connect(vc, SIGNAL(currentIndexChanged(int)), this, SIGNAL(signalItemChanged()));

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
  _indicator->setText(d);
  buttonStatus();
}

QString IndicatorPlotList::indicator ()
{
  return _indicator->text();
}

void IndicatorPlotList::setList (QStringList plotNames, QStringList l)
{
  _list->clear();
  
  if (! l.count())
    return;

  int loop = 0;
  for (; loop < l.count(); loop++)
  {
    QStringList l2 = l.at(loop).split(",");
    if (l2.count() != 4)
      continue;
    
    addPlotItem(plotNames, l2.at(0), l2.at(1), l2.at(2), l2.at(3));
  }
}

void IndicatorPlotList::list (QStringList &plotNames, QStringList &items)
{
  plotNames.clear();
  items.clear();

  int loop = 0;
  for (; loop < _list->topLevelItemCount(); loop++)
  {
    QTreeWidgetItem *item = _list->topLevelItem(loop);

    QStringList l;
    QCheckBox *check = (QCheckBox *) _list->itemWidget(item, 0);
    l << QString::number(check->checkState());

    l << item->text(1);
    plotNames.append(item->text(1));

    QComboBox *cb = (QComboBox *) _list->itemWidget(item, 2);
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
  if (_indicator->text() == "CUS")
    status = TRUE;
  
  _addAction->setEnabled(status);

  if (! _list->topLevelItemCount())
    status = FALSE;
  _deleteAction->setEnabled(status);
}

