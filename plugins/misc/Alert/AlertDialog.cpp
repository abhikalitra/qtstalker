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

#include <QLayout>
#include <QDialogButtonBox>
#include <QToolButton>
#include <QInputDialog>
//#include <QSound>
#include <QProcess>
#include <phonon/mediaobject.h>

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
  l << tr("ID") << tr("Indicator") << tr("Status");
  
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
  _resetButton->setToolTip(tr("Reset alert back to waiting"));
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
  
  item->setText(1, alert.indicator());
  
  item->setText(2, alert.statusToString(alert.status()));

  resizeColumns();
}

void AlertDialog::editAlert ()
{
  QList<QTreeWidgetItem *> l = _alerts->selectedItems();
  
  AlertItem alert;
  alert.setId(l.at(0)->text(0).toInt());

  AlertDataBase db;
  db.getAlert(alert);
  
  AlertEditDialog *dialog = new AlertEditDialog(alert);
  dialog->setWindowTitle("QtStalker" + g_session + ": " + tr("Edit Alert"));
  connect(dialog, SIGNAL(signalMessage(QString)), this, SIGNAL(signalMessage(QString)));
  connect(dialog, SIGNAL(signalEdit(AlertItem)), this, SLOT(editAlert2(AlertItem)));
  dialog->show();
}

void AlertDialog::editAlert2 (AlertItem alert)
{
  QTreeWidgetItem *item = _treeItems.value(alert.id());
  if (! item)
    return;
  
  item->setText(1, alert.indicator());

  resizeColumns();
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

  resizeColumns();
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

  alert.setStatus(AlertItem::_Hit);

  item->setText(2, alert.statusToString(alert.status()));

  AlertDataBase db;
  db.transaction();
  db.setAlert(alert);
  db.commit();

  if (alert.popup())
  {
    Dialog *dialog = new Dialog;
    dialog->setWindowTitle("Qtstalker" + g_session + ": " + tr("Alert Triggered"));
    QStringList l;
    l << tr("ID")  + ": " + QString::number(alert.id());

    QStringList l2 = alert.symbolHits();
    int loop = 0;
    for (; loop < l2.count(); loop++)
      l << tr("Symbol") + ": " + l2.at(loop);
    
    l << tr("Indicator")  + ": " + alert.indicator();
    l << tr("Time")  + ": " + QDateTime::currentDateTime().toString(Qt::ISODate);
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

  db.transaction();
  db.setAlert(alert);
  db.commit();

  l.at(0)->setText(2, alert.statusToString(AlertItem::_Waiting));
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

