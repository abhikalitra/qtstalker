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

#include "AlertDialog.h"
#include "AlertDataBase.h"
#include "AlertConfig.h"
#include "Globals.h"
#include "AlertEditDialog.h"
#include "AlertThread.h"
#include "Config.h"

#include "../pics/edit.xpm"
#include "../pics/newchart.xpm"
#include "../pics/delete.xpm"
#include "../pics/refresh.xpm"
#include "../pics/que.xpm"
#include "../pics/configure.xpm"

#include <QLayout>
#include <QDialogButtonBox>
#include <QToolButton>
#include <QInputDialog>

AlertDialog::AlertDialog ()
{
  setWindowTitle("QtStalker" + g_session + ": Alerts ");

  QVBoxLayout *vbox = new QVBoxLayout;
  vbox->setSpacing(5);
  vbox->setMargin(5);
  setLayout(vbox);

  _tabs = new QTabWidget;
  vbox->addWidget(_tabs);

  // buttonbox
  QDialogButtonBox *bbox = new QDialogButtonBox(QDialogButtonBox::Help);

  QPushButton *b = bbox->addButton(QDialogButtonBox::Close);
  connect(b, SIGNAL(clicked()), this, SLOT(closeDialog()));
  vbox->addWidget(bbox);

  connect(&_timer, SIGNAL(timeout()), this, SLOT(run()));
  
  createMainPage();

  loadSettings();
}

void AlertDialog::createMainPage ()
{
  QWidget *w = new QWidget;
  
  QVBoxLayout *vbox = new QVBoxLayout;
  vbox->setMargin(5);
  vbox->setSpacing(5);
  w->setLayout(vbox);

  QHBoxLayout *hbox = new QHBoxLayout;
  hbox->setMargin(0);
  hbox->setSpacing(5);
  vbox->addLayout(hbox);

  QStringList l;
  l << tr("ID") << tr("Symbol") << tr("Indicator") << tr("Status");
  
  _alerts = new QTreeWidget;
  _alerts->setSortingEnabled(TRUE);
  _alerts->setRootIsDecorated(FALSE);
  _alerts->setHeaderLabels(l);
  _alerts->setSelectionMode(QAbstractItemView::SingleSelection);
  connect(_alerts, SIGNAL(itemSelectionChanged()), this, SLOT(selectionChanged()));
  hbox->addWidget(_alerts);

  // create button box
  QVBoxLayout *bbox = new QVBoxLayout;
  bbox->setMargin(0);
  bbox->setSpacing(2);
  hbox->addLayout(bbox);

  QPushButton *b = new QPushButton;
  b->setText(tr("&New..."));
  b->setIcon(QPixmap(newchart_xpm));
  b->setToolTip(tr("Create a new alert..."));
  connect(b, SIGNAL(clicked()), this, SLOT(newAlert()));
  bbox->addWidget(b);

  _editButton = new QPushButton;
  _editButton->setText(tr("&Edit..."));
  _editButton->setIcon(QPixmap(edit_xpm));
  _editButton->setToolTip(tr("Edit alert..."));
  connect(_editButton, SIGNAL(clicked()), this, SLOT(editAlert()));
  bbox->addWidget(_editButton);

  _deleteButton = new QPushButton;
  _deleteButton->setText(tr("&Delete"));
  _deleteButton->setToolTip(tr("Delete alert"));
  _deleteButton->setIcon(QPixmap(delete_xpm));
  connect(_deleteButton, SIGNAL(clicked()), this, SLOT(deleteAlert()));
  bbox->addWidget(_deleteButton);

  _resetButton = new QPushButton;
  _resetButton->setText(tr("&Reset"));
  _resetButton->setToolTip(tr("Reset alert back to monitor"));
  _resetButton->setIcon(QPixmap(refresh_xpm));
  connect(_resetButton, SIGNAL(clicked()), this, SLOT(resetAlert()));
  bbox->addWidget(_resetButton);

  b = new QPushButton;
  b->setText(tr("&Configure"));
  b->setToolTip(tr("Configure settings for alerts"));
  b->setIcon(QPixmap(configure_xpm));
  connect(b, SIGNAL(clicked()), this, SLOT(configureDialog()));
  bbox->addWidget(b);

  bbox->addStretch();

  _tabs->addTab(w, tr("Alerts"));
}

void AlertDialog::loadSettings ()
{
  AlertConfig config;

  // restore the size of the window
  QSize sz;
  config.getData(AlertConfig::Size, sz);
  if (! sz.isNull())
    resize(sz);

  // restore the position of the window
  QPoint p;
  config.getData(AlertConfig::Pos, p);
  if (! p.isNull())
    move(p);

  int t = config.getInt(AlertConfig::Timer);
  if (! t)
    t = 1;
  _timer.setInterval(60000 * t);
  _timer.start();

  loadAlerts();
}

void AlertDialog::saveSettings ()
{
  AlertConfig config;
  config.transaction();
  
  // save app size and position
  QSize sz = size();
  config.setData(AlertConfig::Size, sz);

  QPoint pt = pos();
  config.setData(AlertConfig::Pos, pt);

  config.setData(AlertConfig::Timer, (int) _timer.interval() / 60000);

  config.commit();
}

void AlertDialog::newAlert ()
{
  Config config;
  QStringList l;
  config.getData(Config::IndicatorPluginList, l);
  
  QInputDialog *dialog = new QInputDialog;
  dialog->setWindowTitle("Qtstalker" + g_session + ": " + tr("New Alert Indicator"));
  dialog->setLabelText(tr("Select indicator for alert"));
  dialog->setComboBoxEditable(FALSE);
  dialog->setComboBoxItems(l);
  connect(dialog, SIGNAL(textValueSelected(const QString &)), this, SLOT(newAlert2(QString)));
  connect(dialog, SIGNAL(finished(int)), dialog, SLOT(deleteLater()));
  dialog->show();
}

void AlertDialog::newAlert2 (QString indicator)
{
  AlertItem item;
  item.setIndicator(indicator);
  
  AlertEditDialog *dialog = new AlertEditDialog(item);
  connect(dialog, SIGNAL(signalMessage(QString)), this, SIGNAL(signalMessage(QString)));
  connect(dialog, SIGNAL(signalEdit(int)), this, SLOT(newAlert3(int)));
  dialog->show();
}

void AlertDialog::newAlert3 (int id)
{
  AlertItem alert;
  alert.setId(id);
  
  AlertDataBase db;
  db.getAlert(alert);

  QTreeWidgetItem *item = new QTreeWidgetItem(_alerts);
  _treeItems.insert(id, item);

  item->setText(0, QString::number(id));
  
  QString symbol = alert.exchange() + ":" + alert.symbol();
  item->setText(1, symbol);
  
  item->setText(2, alert.indicator());
  
  item->setText(3, alert.statusToString(alert.status()));
}

void AlertDialog::editAlert ()
{
  QList<QTreeWidgetItem *> l = _alerts->selectedItems();
  
  AlertItem alert;
  alert.setId(l.at(0)->text(0).toInt());

  AlertDataBase db;
  db.getAlert(alert);
  
  AlertEditDialog *dialog = new AlertEditDialog(alert);
  connect(dialog, SIGNAL(signalMessage(QString)), this, SIGNAL(signalMessage(QString)));
  connect(dialog, SIGNAL(signalEdit(int)), this, SLOT(editAlert2(int)));
  dialog->show();
}

void AlertDialog::editAlert2 (int id)
{
  QTreeWidgetItem *item = _treeItems.value(id);
  if (! item)
    return;
  
  AlertItem alert;
  alert.setId(id);

  AlertDataBase db;
  db.getAlert(alert);

  QString s = alert.exchange() + ":" + alert.symbol();
  item->setText(1, s);

  item->setText(2, alert.indicator());
}

void AlertDialog::deleteAlert ()
{
  QList<QTreeWidgetItem *> l = _alerts->selectedItems();
  QTreeWidgetItem *item = l.at(0);

  AlertItem alert;
  alert.setId(item->text(0).toInt());

  AlertDataBase db;
  db.transaction();
  db.deleteAlert(alert);
  db.commit();
  
  delete item;
}

void AlertDialog::run ()
{
  AlertDataBase db;
  QList<AlertItem> l;
  db.getAlerts(l);
  
  int loop = 0;
  for (; loop < l.count(); loop++)
  {
    AlertItem alert = l.at(loop);
    
    if (alert.status() != AlertItem::_Waiting)
      continue;
    
    qRegisterMetaType<AlertItem>("AlertItem");
    AlertThread *thread = new AlertThread(this, alert);
    connect(thread, SIGNAL(signalDone(AlertItem)), this, SLOT(done(AlertItem)));
    thread->start();
  }
}

void AlertDialog::done (AlertItem alert)
{
  QTreeWidgetItem *item = _treeItems.value(alert.id());
  if (! item)
    return;

  item->setText(3, alert.statusToString(alert.status()));

  AlertDataBase db;
  db.transaction();
  db.setAlert(alert);
  db.commit();

  if (alert.status() == AlertItem::_Notify)
    notify(alert);
}

void AlertDialog::closeDialog ()
{
  saveSettings();
  hide();
}

void AlertDialog::selectionChanged ()
{
  int status = 0;
  QList<QTreeWidgetItem *> l = _alerts->selectedItems();
  if (l.count())
    status = 1;

  _editButton->setEnabled(status);
  _deleteButton->setEnabled(status);
  _resetButton->setEnabled(status);
}

void AlertDialog::loadAlerts ()
{
  _alerts->clear();

  AlertDataBase db;
  QList<AlertItem> l;
  db.getAlerts(l);

  int loop = 0;
  for (; loop < l.count(); loop++)
  {
    AlertItem alert = l.at(loop);
    newAlert3(alert.id());
  }

  selectionChanged();
}

void AlertDialog::resetAlert ()
{
  QList<QTreeWidgetItem *> l = _alerts->selectedItems();
  
  AlertItem alert;
  alert.setId(l.at(0)->text(0).toInt());

  AlertDataBase db;
  db.getAlert(alert);

  alert.setStatus(AlertItem::_Waiting);

  db.transaction();
  db.setAlert(alert);
  db.commit();

  l.at(0)->setText(3, alert.statusToString(AlertItem::_Waiting));
}

void AlertDialog::notify (AlertItem alert)
{
  if (alert.popup())
  {
//    Dialog *dialog = new Dialog;
//    dialog->setMessage();
  }

  if (alert.sound())
  {
  }

  if (alert.mail())
  {
  }
}

void AlertDialog::configureDialog ()
{
  AlertConfigureDialog *dialog = new AlertConfigureDialog;
  connect(dialog, SIGNAL(signalMessage(QString)), this, SIGNAL(signalMessage(QString)));
  dialog->show();
}

