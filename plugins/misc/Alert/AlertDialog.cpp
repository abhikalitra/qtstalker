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
#include "AlertConfigureDialog.h"

#include "../pics/edit.xpm"
#include "../pics/newchart.xpm"
#include "../pics/delete.xpm"
#include "../pics/refresh.xpm"
#include "../pics/configure.xpm"
#include "../pics/ok.xpm"
#include "../pics/que.xpm"
#include "../pics/disable.xpm"

#include <QLayout>
#include <QDialogButtonBox>
#include <QToolButton>
#include <QInputDialog>
#include <QProcess>
#include <phonon/mediaobject.h>

AlertDialog::AlertDialog ()
{
//  _helpFile = "Alert.html";
  
  setWindowTitle("QtStalker" + g_session + ": " + tr("Alerts"));

  // buttons
  _buttonBox->removeButton(_okButton);
  _buttonBox->removeButton(_cancelButton);

  QPushButton *b = new QPushButton;
  b->setText(tr("Close"));
  b->setIcon(QIcon(disable_xpm));
  _buttonBox->addButton(b, QDialogButtonBox::ActionRole);
  connect(b, SIGNAL(clicked()), this, SLOT(closeDialog()));

  connect(&_timer, SIGNAL(timeout()), this, SLOT(run()));
  
  createMainPage();

  loadSettings();
}

void AlertDialog::createMainPage ()
{
  QWidget *w = new QWidget;
  
  QHBoxLayout *hbox = new QHBoxLayout;
  hbox->setMargin(0);
  hbox->setSpacing(5);
  w->setLayout(hbox);

  QStringList l;
  l << tr("ID") << tr("Symbol") << tr("Indicator") << tr("Status") << tr("Last Update");
  
  _alerts = new QTreeWidget;
  _alerts->setSortingEnabled(TRUE);
  _alerts->setRootIsDecorated(FALSE);
  _alerts->setHeaderLabels(l);
  _alerts->setSelectionMode(QAbstractItemView::ExtendedSelection);
  connect(_alerts, SIGNAL(itemSelectionChanged()), this, SLOT(selectionChanged()));
  connect(_alerts, SIGNAL(itemDoubleClicked(QTreeWidgetItem *, int)), this, SLOT(editAlert()));
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
  _resetButton->setToolTip(tr("Reset alert back to waiting status"));
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
  config.getData(AlertConfig::_Size, sz);
  if (! sz.isNull())
    resize(sz);

  // restore the position of the window
  QPoint p;
  config.getData(AlertConfig::_Pos, p);
  if (! p.isNull())
    move(p);

  int t = config.getInt(AlertConfig::_TimerInterval);
  if (! t)
    t = 1;
  _timer.setInterval(60000 * t);

  if (config.getInt(AlertConfig::_AlertsEnable))
    _timer.start();

  loadAlerts();
}

void AlertDialog::saveSettings ()
{
  AlertConfig config;
  config.transaction();
  
  // save app size and position
  QSize sz = size();
  config.setData(AlertConfig::_Size, sz);

  QPoint pt = pos();
  config.setData(AlertConfig::_Pos, pt);

  config.commit();
}

void AlertDialog::newAlert ()
{
  AlertItem item;
  AlertEditDialog *dialog = new AlertEditDialog(item);
  dialog->setWindowTitle("QtStalker" + g_session + ": " + tr("New Alert"));
  connect(dialog, SIGNAL(signalMessage(QString)), this, SIGNAL(signalMessage(QString)));
  connect(dialog, SIGNAL(signalEdit(AlertItem)), this, SLOT(newAlert2(AlertItem)));
  dialog->show();
}

void AlertDialog::newAlert2 (AlertItem alert)
{
  QTreeWidgetItem *item = new QTreeWidgetItem(_alerts);
  _treeItems.insert(alert.id(), item);

  item->setText(0, QString::number(alert.id()));
  item->setText(1, alert.symbol());
  item->setText(2, alert.indicator());
  item->setText(3, alert.statusToString(alert.status()));
  item->setText(4, alert.lastUpdate().toString(Qt::ISODate));

  if (alert.status() == AlertItem::_Waiting)
    item->setIcon(3, QIcon(que_xpm));
  else
    item->setIcon(3, QIcon(ok_xpm));
  
  resizeColumns();
}

void AlertDialog::editAlert ()
{
  QList<QTreeWidgetItem *> l = _alerts->selectedItems();

  int loop = 0;
  for (; loop < l.count(); loop++)
  {
    AlertItem alert;
    alert.setId(l.at(0)->text(0).toInt());

    AlertDataBase db;
    db.getAlert(alert);
  
    AlertEditDialog *dialog = new AlertEditDialog(alert);
    connect(dialog, SIGNAL(signalMessage(QString)), this, SIGNAL(signalMessage(QString)));
    connect(dialog, SIGNAL(signalEdit(AlertItem)), this, SLOT(editAlert2(AlertItem)));
    dialog->show();
  }
}

void AlertDialog::editAlert2 (AlertItem alert)
{
  QTreeWidgetItem *item = _treeItems.value(alert.id());
  if (! item)
    return;

  item->setText(1, alert.symbol());
  item->setText(2, alert.indicator());

  resizeColumns();
}

void AlertDialog::deleteAlert ()
{
  setMessage(tr("Delete selected alerts?"));
  setConfirm();
}

void AlertDialog::confirmYes ()
{
  QList<QTreeWidgetItem *> l = _alerts->selectedItems();

  int loop = 0;
  for (; loop < l.count(); loop++)
  {
    QTreeWidgetItem *item = l.at(loop);

    AlertItem alert;
    alert.setId(item->text(0).toInt());

    AlertDataBase db;
    db.transaction();
    db.deleteAlert(alert);
    db.commit();
  
    delete item;
  }

  resizeColumns();

  unsetConfirm();
}

void AlertDialog::confirmNo ()
{
  unsetConfirm();
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

  item->setText(4, alert.lastUpdate().toString(Qt::ISODate));

  AlertDataBase db;
  
  if (alert.status() == AlertItem::_Waiting)
  {
    db.transaction();
    db.setAlert(alert);
    db.commit();
    return;
  }
  
  alert.setStatus(AlertItem::_Hit);

  item->setIcon(3, QIcon(ok_xpm));
  item->setText(3, alert.statusToString(alert.status()));

  db.transaction();
  db.setAlert(alert);
  db.commit();

  if (alert.popup())
  {
    Dialog *dialog = new Dialog;
    dialog->setWindowTitle("Qtstalker" + g_session + ": " + tr("Alert Triggered"));
    
    QStringList l;
    l << tr("ID")  + ": " + QString::number(alert.id());
    l << tr("Symbol") + ": " + alert.symbol();
    l << tr("Indicator")  + ": " + alert.indicator();
    l << tr("Time")  + ": " + alert.hitDate().toString(Qt::ISODate);
    
    dialog->setMessage(l.join("\n"));
    
    QFont f = dialog->messageFont();
    f.setBold(FALSE);
    dialog->setMessageFont(f);
    dialog->show();
  }

  if (alert.sound())
  {
    AlertConfig config;
    QString s;
    config.getData(AlertConfig::_SoundFile, s);
    Phonon::MediaObject *sound = Phonon::createPlayer(Phonon::MusicCategory,
                                                      Phonon::MediaSource(s));
    connect(sound, SIGNAL(finished()), sound, SLOT(deleteLater()));
    sound->play();
  }

  if (alert.mail())
  {
    AlertConfig config;
    QString addy, subject, body;
    config.getData(AlertConfig::_MailAddress, addy);
    config.getData(AlertConfig::_MailSubject, subject);
    config.getData(AlertConfig::_MailBody, body);

    QString command = "echo \"" + body + "\" | mail -s \"" + subject + "\" " + addy;
    int rc = QProcess::startDetached(command);
    if (! rc)
      qDebug() << "AlertDialog::done: process error";
  }
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
    newAlert2(alert);
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
  alert.setHitDate(QDateTime());

  db.transaction();
  db.setAlert(alert);
  db.commit();

  l.at(0)->setText(3, alert.statusToString(AlertItem::_Waiting));
  l.at(0)->setIcon(3, QIcon(que_xpm));
}

void AlertDialog::configureDialog ()
{
  AlertConfigureDialog *dialog = new AlertConfigureDialog;
  connect(dialog, SIGNAL(signalMessage(QString)), this, SIGNAL(signalMessage(QString)));
  connect(dialog, SIGNAL(signalInterval(int)), this, SLOT(intervalChanged(int)));
  connect(dialog, SIGNAL(signalEnable(int)), this, SLOT(enableChanged(int)));
  dialog->show();
}

void AlertDialog::intervalChanged (int d)
{
  _timer.setInterval(60000 * d);
}

void AlertDialog::enableChanged (int d)
{
  if (! d)
    _timer.stop();
  else
    _timer.start();
}

void AlertDialog::resizeColumns ()
{
  int loop = 0;
  for (; loop < _alerts->topLevelItemCount(); loop++)
    _alerts->resizeColumnToContents(loop);
}

