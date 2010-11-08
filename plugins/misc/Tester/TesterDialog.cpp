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

#include "TesterDialog.h"
#include "TesterConfig.h"
#include "Globals.h"
#include "TesterNewDialog.h"
#include "TesterOpenDialog.h"
#include "TesterDeleteDialog.h"
#include "TesterDataBase.h"
#include "TesterRunDialog.h"
#include "TesterReportDataBase.h"
#include "TesterReportDialog.h"

#include "../../../pics/edit.xpm"
#include "../../../pics/newchart.xpm"
#include "../../../pics/delete.xpm"
#include "../../../pics/disable.xpm"

#include <QLayout>

TesterDialog::TesterDialog ()
{
//  _helpFile = "Tester.html";
  
  setWindowTitle("QtStalker" + g_session + ": Tester ");

  // buttons
  _buttonBox->removeButton(_okButton);
  _buttonBox->removeButton(_cancelButton);

  QPushButton *b = new QPushButton;
  b->setText(tr("Close"));
  b->setIcon(QIcon(disable_xpm));
  connect(b, SIGNAL(clicked()), this, SLOT(closeDialog()));
  _buttonBox->addButton(b, QDialogButtonBox::ActionRole);

  createTesterPage();
  
  createReportPage();

  loadSettings();

  updateReports();

  testSelectionChanged();
  
  reportSelectionChanged();
}

void TesterDialog::createTesterPage ()
{
  QWidget *w = new QWidget;
  
  QHBoxLayout *hbox = new QHBoxLayout;
  hbox->setSpacing(2);
  hbox->setMargin(5);
  w->setLayout(hbox);

  _testers = new QListWidget;
  _testers->setSortingEnabled(TRUE);
  _testers->setSelectionMode(QAbstractItemView::SingleSelection);
  connect(_testers, SIGNAL(itemSelectionChanged()), this, SLOT(testSelectionChanged()));
  hbox->addWidget(_testers);

  // create button box
  QVBoxLayout *bbox = new QVBoxLayout;
  bbox->setMargin(0);
  bbox->setSpacing(2);
  hbox->addLayout(bbox);

  QPushButton *b = new QPushButton;
  b->setText(tr("&Run"));
  b->setToolTip(tr("Run back test"));
  connect(b, SIGNAL(clicked()), this, SLOT(runTest()));
  bbox->addWidget(b);

  b = new QPushButton;
  b->setText(tr("&New"));
  b->setIcon(QPixmap(newchart_xpm));
  b->setToolTip(tr("Create a new back tester"));
  connect(b, SIGNAL(clicked()), this, SLOT(newTest()));
  bbox->addWidget(b);

  b = new QPushButton;
  b->setText(tr("&Edit"));
  b->setToolTip(tr("Edit back tester"));
  b->setIcon(QPixmap(edit_xpm));
  connect(b, SIGNAL(clicked()), this, SLOT(editTest()));
  bbox->addWidget(b);

  b = new QPushButton;
  b->setText(tr("&Delete"));
  b->setToolTip(tr("Delete back tester"));
  b->setIcon(QPixmap(delete_xpm));
  connect(b, SIGNAL(clicked()), this, SLOT(deleteTest()));
  bbox->addWidget(b);

  _stopButton = new QPushButton;
  _stopButton->setText(tr("&Stop"));
  _stopButton->setToolTip(tr("Stop back test"));
  _stopButton->setIcon(QPixmap(disable_xpm));
  connect(_stopButton, SIGNAL(clicked()), this, SLOT(stopTest()));
  bbox->addWidget(_stopButton);

  bbox->addStretch();

  _tabs->addTab(w, tr("Tester"));
}

void TesterDialog::createReportPage ()
{
  QWidget *w = new QWidget;

  QHBoxLayout *hbox = new QHBoxLayout;
  hbox->setSpacing(2);
  hbox->setMargin(5);
  w->setLayout(hbox);

  QStringList l;
  l << tr("Name") << tr("Net Profit %") << tr("Profit Factor") << tr("Payoff Ratio");

  _reports = new QTreeWidget;
  _reports->setSortingEnabled(TRUE);
  _reports->setRootIsDecorated(FALSE);
  _reports->setHeaderLabels(l);
  _reports->setSelectionMode(QAbstractItemView::SingleSelection);
  connect(_reports, SIGNAL(itemSelectionChanged()), this, SLOT(reportSelectionChanged()));
  connect(_reports, SIGNAL(itemDoubleClicked(QTreeWidgetItem *, int)), this, SLOT(reportView()));
  hbox->addWidget(_reports);

  // create button box
  QVBoxLayout *bbox = new QVBoxLayout;
  bbox->setMargin(0);
  bbox->setSpacing(2);
  hbox->addLayout(bbox);

  _viewButton = new QPushButton;
  _viewButton->setText(tr("&View"));
  _viewButton->setToolTip(tr("View back test report"));
  connect(_viewButton, SIGNAL(clicked()), this, SLOT(reportView()));
  bbox->addWidget(_viewButton);

  bbox->addStretch();

  _tabs->addTab(w, tr("Report"));
}

void TesterDialog::loadSettings ()
{
  TesterConfig config;

  // restore the size of the window
  QSize sz;
  config.getData(TesterConfig::Size, sz);
  if (! sz.isNull())
    resize(sz);

  // restore the position of the window
  QPoint p;
  config.getData(TesterConfig::Pos, p);
  if (! p.isNull())
    move(p);
}

void TesterDialog::saveSettings ()
{
  TesterConfig config;
  config.transaction();
  
  // save app size and position
  QSize sz = size();
  config.setData(TesterConfig::Size, sz);

  QPoint pt = pos();
  config.setData(TesterConfig::Pos, pt);

  config.commit();
}

void TesterDialog::newTest ()
{
  TesterNewDialog *dialog = new TesterNewDialog;
  dialog->show();
}

void TesterDialog::editTest ()
{
  TesterOpenDialog *dialog = new TesterOpenDialog;
  dialog->show();
}

void TesterDialog::deleteTest ()
{
  TesterDeleteDialog *dialog = new TesterDeleteDialog;
  dialog->show();
}

void TesterDialog::runTest ()
{
  TesterRunDialog *dialog = new TesterRunDialog;
  connect(dialog, SIGNAL(signalMessage(QString)), this, SIGNAL(signalMessage(QString)));
  connect(dialog, SIGNAL(signalSelect(QStringList)), this, SLOT(runTest2(QStringList)));
  dialog->show();
}

void TesterDialog::runTest2 (QStringList l)
{
  TesterDataBase db;

  int loop = 0;
  for (; loop < l.count(); loop++)
  {
    if (_runningList.contains(l.at(loop)))
      continue;

    TesterSettings settings;
    settings.setName(l.at(loop));
    db.getRule(settings);
    
    TesterThread *thread = new TesterThread(this, settings);
    connect(thread, SIGNAL(signalDone(QString)), this, SLOT(done(QString)));
    connect(thread, SIGNAL(signalStopped(QString)), this, SLOT(testStopped(QString)));
    thread->start();

    _threadList.insert(l.at(loop), thread);

    QListWidgetItem *item = new QListWidgetItem(_testers);
    item->setText(settings.name());
    _runningList.insert(l.at(loop), item);
  }
}

void TesterDialog::done (QString name)
{
  QListWidgetItem *item = _runningList.value(name);
  if (! item)
    return;

  _runningList.remove(name);
  _threadList.remove(name);

  delete item;

  updateReports();
}

void TesterDialog::testSelectionChanged ()
{
  int status = 0;
  QList<QListWidgetItem *> l = _testers->selectedItems();
  if (l.count())
    status = 1;

  _stopButton->setEnabled(status);
}

void TesterDialog::reportSelectionChanged ()
{
  int status = 0;
  QList<QTreeWidgetItem *> l = _reports->selectedItems();
  if (l.count())
    status = 1;

  _viewButton->setEnabled(status);
}

void TesterDialog::stopTest ()
{
  QList<QListWidgetItem *> l = _testers->selectedItems();
  if (! l.count())
    return;
  
  QListWidgetItem *item = l.at(0);

  TesterThread *thread = _threadList.value(item->text());
  if (! thread)
    return;

  thread->stop();
}

void TesterDialog::testStopped (QString name)
{
  QListWidgetItem *item = _runningList.value(name);
  if (! item)
    return;

  _threadList.remove(name);
  _runningList.remove(name);

  delete item;
}

void TesterDialog::updateReports ()
{
  TesterReportDataBase db;
  QList<TesterReport> rl;
  db.reportRatings(rl);

  _reports->clear();

  int loop = 0;
  for (; loop < rl.count(); loop++)
  {
    TesterReport report = rl.at(loop);

    QStringList l;
    l << report.name();
    l << QString::number(report.equityGain());
    l << QString::number(report.profitFactor());
    l << QString::number(report.payoffRatio());

    new QTreeWidgetItem(_reports, l);
  }

  for (loop = 0; loop < _reports->columnCount(); loop++)
    _reports->resizeColumnToContents(loop);
}

void TesterDialog::reportView ()
{
  QList<QTreeWidgetItem *> l = _reports->selectedItems();
  if (! l.count())
    return;
  
  QTreeWidgetItem *item = l.at(0);

  TesterReportDialog *dialog = new TesterReportDialog(item->text(0));
  dialog->show();
}

void TesterDialog::closeDialog ()
{
  saveSettings();
  hide();
}

