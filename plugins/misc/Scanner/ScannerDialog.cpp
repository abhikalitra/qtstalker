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

#include "ScannerDialog.h"
#include "ScannerDataBase.h"
#include "ScannerConfig.h"
#include "Globals.h"
#include "ScannerEditDialog.h"
#include "ScannerThread.h"
#include "Config.h"
#include "ScannerNewDialog.h"
#include "ScannerSelectEditDialog.h"
#include "ScannerDeleteDialog.h"
#include "ScannerRunDialog.h"

#include "../pics/edit.xpm"
#include "../pics/newchart.xpm"
#include "../pics/delete.xpm"
#include "../pics/disable.xpm"

#include <QLayout>
#include <QDialogButtonBox>

ScannerDialog::ScannerDialog ()
{
  setWindowTitle("QtStalker" + g_session + ": Scanners ");

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

  createMainPage();

  loadSettings();

  selectionChanged();
}

void ScannerDialog::createMainPage ()
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

  _scanners = new QListWidget;
  _scanners->setSortingEnabled(TRUE);
  _scanners->setSelectionMode(QAbstractItemView::SingleSelection);
  connect(_scanners, SIGNAL(itemSelectionChanged()), this, SLOT(selectionChanged()));
  hbox->addWidget(_scanners);

  // create button box
  QVBoxLayout *bbox = new QVBoxLayout;
  bbox->setMargin(0);
  bbox->setSpacing(2);
  hbox->addLayout(bbox);

  QPushButton *b = new QPushButton;
  b->setText(tr("&Run"));
  b->setToolTip(tr("Run scanner"));
//  _runButton->setIcon(QPixmap(refresh_xpm));
  connect(b, SIGNAL(clicked()), this, SLOT(runScanner()));
  bbox->addWidget(b);

  b = new QPushButton;
  b->setText(tr("&New..."));
  b->setIcon(QPixmap(newchart_xpm));
  b->setToolTip(tr("Create a new scanner..."));
  connect(b, SIGNAL(clicked()), this, SLOT(newScanner()));
  bbox->addWidget(b);

  b = new QPushButton;
  b->setText(tr("&Edit..."));
  b->setIcon(QPixmap(edit_xpm));
  b->setToolTip(tr("Edit scanner..."));
  connect(b, SIGNAL(clicked()), this, SLOT(editScanner()));
  bbox->addWidget(b);

  b = new QPushButton;
  b->setText(tr("&Delete..."));
  b->setIcon(QPixmap(delete_xpm));
  b->setToolTip(tr("Delete scanner..."));
  connect(b, SIGNAL(clicked()), this, SLOT(deleteScanner()));
  bbox->addWidget(b);

  _stopButton = new QPushButton;
  _stopButton->setText(tr("&Stop"));
  _stopButton->setToolTip(tr("Stop scanner"));
  _stopButton->setIcon(QPixmap(disable_xpm));
  connect(_stopButton, SIGNAL(clicked()), this, SLOT(stopScanner()));
  bbox->addWidget(_stopButton);

  bbox->addStretch();

  _tabs->addTab(w, tr("Scanners"));
}

void ScannerDialog::loadSettings ()
{
  ScannerConfig config;

  // restore the size of the window
  QSize sz;
  config.getData(ScannerConfig::_Size, sz);
  if (! sz.isNull())
    resize(sz);

  // restore the position of the window
  QPoint p;
  config.getData(ScannerConfig::_Pos, p);
  if (! p.isNull())
    move(p);
}

void ScannerDialog::saveSettings ()
{
  ScannerConfig config;
  config.transaction();
  
  // save app size and position
  QSize sz = size();
  config.setData(ScannerConfig::_Size, sz);

  QPoint pt = pos();
  config.setData(ScannerConfig::_Pos, pt);

  config.commit();
}

void ScannerDialog::newScanner ()
{
  ScannerNewDialog *dialog = new ScannerNewDialog;
  connect(dialog, SIGNAL(signalMessage(QString)), this, SIGNAL(signalMessage(QString)));
  connect(dialog, SIGNAL(signalNew(QString)), this, SLOT(newScanner2(QString)));
  dialog->show();
}

void ScannerDialog::newScanner2 (QString name)
{
  if (! name.length())
    return;
  
  ScannerItem scanner;
  scanner.setName(name);

  ScannerDataBase db;
  if (db.getScanner(scanner))
  {
    qDebug() << "ScannerDialog::newScanner2: no scanner found";
    return;
  }
  
  ScannerEditDialog *dialog = new ScannerEditDialog(scanner);
  connect(dialog, SIGNAL(signalMessage(QString)), this, SIGNAL(signalMessage(QString)));
  dialog->show();
}

void ScannerDialog::editScanner ()
{
  ScannerSelectEditDialog *dialog = new ScannerSelectEditDialog;
  connect(dialog, SIGNAL(signalMessage(QString)), this, SIGNAL(signalMessage(QString)));
  connect(dialog, SIGNAL(signalSelect(QStringList)), this, SLOT(editScanner2(QStringList)));
  dialog->show();
}

void ScannerDialog::editScanner2 (QStringList l)
{
  ScannerItem scanner;
  scanner.setName(l.at(0));

  ScannerDataBase db;
  db.getScanner(scanner);

  ScannerEditDialog *dialog = new ScannerEditDialog(scanner);
  connect(dialog, SIGNAL(signalMessage(QString)), this, SIGNAL(signalMessage(QString)));
  dialog->show();
}

void ScannerDialog::deleteScanner ()
{
  ScannerDeleteDialog *dialog = new ScannerDeleteDialog;
  connect(dialog, SIGNAL(signalMessage(QString)), this, SIGNAL(signalMessage(QString)));
  dialog->show();
}

void ScannerDialog::runScanner ()
{
  ScannerRunDialog *dialog = new ScannerRunDialog;
  connect(dialog, SIGNAL(signalMessage(QString)), this, SIGNAL(signalMessage(QString)));
  connect(dialog, SIGNAL(signalSelect(QStringList)), this, SLOT(runScanner2(QStringList)));
  dialog->show();
}

void ScannerDialog::runScanner2 (QStringList l)
{
  ScannerDataBase db;
  
  int loop = 0;
  for (; loop < l.count(); loop++)
  {
    if (_runningList.contains(l.at(loop)))
      continue;

    ScannerItem scanner;
    scanner.setName(l.at(loop));
    db.getScanner(scanner);
    
    qRegisterMetaType<ScannerItem>("ScannerItem");
    ScannerThread *thread = new ScannerThread(this, scanner);
    connect(thread, SIGNAL(signalDone(QString)), this, SLOT(done(QString)));
    connect(thread, SIGNAL(signalStopped(QString)), this, SLOT(scannerStopped(QString)));
    thread->start();
    
    _threadList.insert(l.at(loop), thread);

    QListWidgetItem *item = new QListWidgetItem(_scanners);
    item->setText( scanner.name());
    _runningList.insert(l.at(loop), item);
  }
}

void ScannerDialog::done (QString name)
{
  QListWidgetItem *item = _runningList.value(name);
  if (! item)
    return;

  _runningList.remove(name);
  _threadList.remove(name);
  
  delete item;

  emit signalGroupRefresh();
}

void ScannerDialog::closeDialog ()
{
  saveSettings();
  hide();
}

void ScannerDialog::selectionChanged ()
{
  int status = 0;
  QList<QListWidgetItem *> l = _scanners->selectedItems();
  if (l.count())
    status = 1;

  _stopButton->setEnabled(status);
}

void ScannerDialog::stopScanner ()
{
  QList<QListWidgetItem *> l = _scanners->selectedItems();
  QListWidgetItem *item = l.at(0);
  
  ScannerThread *thread = _threadList.value(item->text());
  if (! thread)
    return;

  thread->stop();
}

void ScannerDialog::scannerStopped (QString name)
{
  QListWidgetItem *item = _runningList.value(name);
  if (! item)
    return;

  _threadList.remove(name);
  _runningList.remove(name);
  
  delete item;
}

