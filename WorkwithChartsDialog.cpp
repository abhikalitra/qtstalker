/*
 *  Qtstalker stock charter
 *
 *  Copyright (C) 2001,2002 Stefan S. Stratigakos
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

#include "WorkwithChartsDialog.h"
#include "ChartDb.h"
#include "Setting.h"
#include "Plugin.h"
#include "open.xpm"
#include "edit.xpm"
#include "delete.xpm"
#include "export.xpm"
#include "newchart.xpm"
#include <qinputdialog.h>
#include <qtooltip.h>
#include <qlayout.h>
#include <qdir.h>
#include <qmessagebox.h>
#include <qgroupbox.h>
#include <qlibrary.h>

WorkwithChartsDialog::WorkwithChartsDialog (Config *c) : EditDialog (c)
{
  toolbar->expand(1, 9);
  okButton->hide();

  openButton = new QToolButton(this);
  QToolTip::add(openButton, tr("Open Chart"));
  openButton->setPixmap(QPixmap(open));
  connect(openButton, SIGNAL(clicked()), this, SLOT(openSymbol()));
  openButton->setMaximumWidth(30);
  openButton->setAutoRaise(TRUE);
  toolbar->addWidget(openButton, 0, 0);

  newButton = new QToolButton(this);
  QToolTip::add(newButton, tr("New Chart"));
  newButton->setPixmap(QPixmap(newchart));
  connect(newButton, SIGNAL(clicked()), this, SLOT(newChart()));
  newButton->setMaximumWidth(30);
  newButton->setAutoRaise(TRUE);
  toolbar->addWidget(newButton, 0, 3);

  editButton = new QToolButton(this);
  QToolTip::add(editButton, tr("Edit Chart"));
  editButton->setPixmap(QPixmap(edit));
  connect(editButton, SIGNAL(clicked()), this, SLOT(editChart()));
  editButton->setMaximumWidth(30);
  editButton->setAutoRaise(TRUE);
  toolbar->addWidget(editButton, 0, 4);

  deleteButton = new QToolButton(this);
  QToolTip::add(deleteButton, tr("Delete Chart"));
  deleteButton->setPixmap(QPixmap(deletefile));
  connect(deleteButton, SIGNAL(clicked()), this, SLOT(deleteChart()));
  deleteButton->setMaximumWidth(30);
  deleteButton->setAutoRaise(TRUE);
  toolbar->addWidget(deleteButton, 0, 5);

  exportButton = new QToolButton(this);
  QToolTip::add(exportButton, tr("Export Chart"));
  exportButton->setPixmap(QPixmap(exportfile));
  connect(exportButton, SIGNAL(clicked()), this, SLOT(exportSymbol()));
  exportButton->setMaximumWidth(30);
  exportButton->setAutoRaise(TRUE);
  toolbar->addWidget(exportButton, 0, 6);

  exportAllButton = new QToolButton(this);
  QToolTip::add(exportAllButton, tr("Export All Charts"));
  exportAllButton->setPixmap(QPixmap(exportfile));
  connect(exportAllButton, SIGNAL(clicked()), this, SLOT(exportAll()));
  exportAllButton->setMaximumWidth(30);
  exportAllButton->setAutoRaise(TRUE);
  toolbar->addWidget(exportAllButton, 0, 7);

  connect(fileList, SIGNAL(doubleClicked(QListViewItem *)), this, SLOT(openSymbol()));

  fileList->show();
  gbox->show();
  list->hide();
  upButton->show();
}

WorkwithChartsDialog::~WorkwithChartsDialog ()
{
}

void WorkwithChartsDialog::openSymbol ()
{
  item = fileList->selectedItem();
  if (! item)
    return;
    
  if (item->pixmap(0))
    return;

  QString s = currentDir.absPath();
  s.append("/");
  s.append(item->text(0));
  emit chartOpened (s);

  reject();
}

void WorkwithChartsDialog::deleteChart ()
{
  item = fileList->selectedItem();
  if (! item)
    return;

  QString s = currentDir.absPath();
  s.append("/");
  s.append(item->text(0));

  currentDir.remove(s, TRUE);

  delete item;
}

void WorkwithChartsDialog::editChart ()
{
  item = fileList->selectedItem();
  if (! item)
    return;

  QString selection = currentDir.absPath();
  selection.append("/");
  selection.append(item->text(0));

  ChartDb *db = new ChartDb();
  if (db->openChart(selection))
  {
    QMessageBox::information(this, tr("Qtstalker: Error"), tr("Can't open chart."));
    delete db;
    return;
  }

  Setting *set = db->getDetails();

  QString chartType = set->getData("Chart Type");

  EditDialog *dialog = new EditDialog(config);

  QString s = tr("Qtstalker: Edit ");
  s.append(chartType);
  dialog->setCaption(s);

  dialog->setItems(set);

  int rc = dialog->exec();

  if (rc == QDialog::Accepted)
    db->saveDetails();

  delete db;
}

void WorkwithChartsDialog::newChart()
{
  QStringList l = config->getQuotePlugins();
  QStringList l2;
  int loop;
  for (loop = 0; loop < (int) l.count(); loop++)
  {
    QString s = config->getData(Config::QuotePluginPath);
    s.append("/");
    s.append(l[loop]);
    s.append(".so");

    QLibrary *lib = new QLibrary(s);
    Plugin *(*so)() = 0;
    so = (Plugin *(*)()) lib->resolve("create");
    if (so)
    {
      Plugin *plug = (*so)();

      if (plug->getCreateFlag())
        l2.append(plug->getPluginName());

      delete plug;
    }

    delete lib;
  }

  bool ok = FALSE;
  QString type = QInputDialog::getItem(tr("New Chart Type"), tr("Choose chart type to create."),
 	                               l2, 0, FALSE, &ok, this);
  if (! ok || type.isNull())
    return;

  QString s = config->getData(Config::QuotePluginPath);
  s.append("/");
  s.append(type);
  s.append(".so");

  QLibrary *lib = new QLibrary(s);
  Plugin *(*so)() = 0;
  so = (Plugin *(*)()) lib->resolve("create");
  if (! so)
  {
    delete lib;
    return;
  }
  Plugin *plug = (*so)();

  plug->setDataPath(config->getData(Config::DataPath));

  Setting *details = plug->getCreateDetails();

  EditDialog *dialog = new EditDialog(config);

  s = tr("Qtstalker: New ");
  s.append(type);
  dialog->setCaption(s);

  dialog->setItems(details);

  int rc = dialog->exec();

  if (rc == QDialog::Accepted)
  {
    plug->createChart(details);
    currentDir.setPath(currentDir.path());
    updateFileList();
  }

  delete details;
  delete dialog;
  delete plug;
  delete lib;
}

void WorkwithChartsDialog::exportSymbol ()
{
  item = fileList->selectedItem();
  if (! item)
    return;
    
  if (item->pixmap(0))
    return;

  QString s = config->getData(Config::Home);
  s.append("/export");
  if (! currentDir.exists(s, TRUE))
  {
    if (! currentDir.mkdir(s, TRUE))
    {
      qDebug("Unable to create export directory.");
      return;
    }
  }

  s = currentDir.path();
  s.append("/");
  s.append(item->text(0));
  exportChart(s);
}

void WorkwithChartsDialog::exportAll ()
{
  QString s = config->getData(Config::Home);
  s.append("/export");
  if (! currentDir.exists(s, TRUE))
  {
    if (! currentDir.mkdir(s, TRUE))
    {
      qDebug("Unable to create export directory.");
      return;
    }
  }

  QDir dir(config->getData(Config::DataPath));

  int loop;
  for (loop = 2; loop < (int) dir.count(); loop++)
  {
    s = dir.path();
    s.append("/");
    s.append(dir[loop]);
    QDir dir2(s);

    int loop2;
    for (loop2 = 2; loop2 < (int) dir2.count(); loop2++)
    {
      s = dir2.path();
      s.append("/");
      s.append(dir2[loop2]);
      exportChart(s);
    }
  }
}

void WorkwithChartsDialog::exportChart (QString path)
{
  ChartDb *db = new ChartDb();
  if(db->openChart(path))
  {
    delete db;
    return;
  }

  Setting *details = db->getDetails();

  QString s = config->getData(Config::Home);
  s.append("/export/");
  s.append(details->getData("Symbol"));

  db->dump(s);

  delete db;
}

void WorkwithChartsDialog::setStartDir (QString d)
{
  if (d.length())
  {
    int i = d.findRev('/', -1, TRUE);
    d.truncate(i);
    currentDir.setPath(d);
  }
}

