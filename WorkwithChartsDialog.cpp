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
#include "EditDialog.h"
#include "open.xpm"
#include "edit.xpm"
#include "delete.xpm"
#include "export.xpm"
#include "stop.xpm"
#include <qtooltip.h>
#include <qdir.h>
#include <qmessagebox.h>
#include <qfileinfo.h>

WorkwithChartsDialog::WorkwithChartsDialog (Config *c) : QDialog (0, "WorkwithChartsDialog", TRUE)
{
  config = c;
  
  QVBoxLayout *vbox = new QVBoxLayout(this);
  vbox->setMargin(5);
  vbox->setSpacing(5);

  toolbar = new QGridLayout(vbox, 1, 7);
  toolbar->setSpacing(1);

  cancelButton = new QToolButton(this);
  QToolTip::add(cancelButton, tr("Cancel"));
  cancelButton->setPixmap(QPixmap(stop));
  connect(cancelButton, SIGNAL(clicked()), this, SLOT(reject()));
  cancelButton->setMaximumWidth(30);
  cancelButton->setAutoRaise(TRUE);
  toolbar->addWidget(cancelButton, 0, 0);

  openButton = new QToolButton(this);
  QToolTip::add(openButton, tr("Open Chart"));
  openButton->setPixmap(QPixmap(open));
  connect(openButton, SIGNAL(clicked()), this, SLOT(openSymbol()));
  openButton->setMaximumWidth(30);
  openButton->setAutoRaise(TRUE);
  toolbar->addWidget(openButton, 0, 1);
  openButton->setEnabled(FALSE);

  editButton = new QToolButton(this);
  QToolTip::add(editButton, tr("Edit Chart"));
  editButton->setPixmap(QPixmap(edit));
  connect(editButton, SIGNAL(clicked()), this, SLOT(editChart()));
  editButton->setMaximumWidth(30);
  editButton->setAutoRaise(TRUE);
  toolbar->addWidget(editButton, 0, 2);
  editButton->setEnabled(FALSE);

  deleteButton = new QToolButton(this);
  QToolTip::add(deleteButton, tr("Delete Chart"));
  deleteButton->setPixmap(QPixmap(deletefile));
  connect(deleteButton, SIGNAL(clicked()), this, SLOT(deleteChart()));
  deleteButton->setMaximumWidth(30);
  deleteButton->setAutoRaise(TRUE);
  toolbar->addWidget(deleteButton, 0, 3);
  deleteButton->setEnabled(FALSE);

  exportButton = new QToolButton(this);
  QToolTip::add(exportButton, tr("Export Chart"));
  exportButton->setPixmap(QPixmap(exportfile));
  connect(exportButton, SIGNAL(clicked()), this, SLOT(exportSymbol()));
  exportButton->setMaximumWidth(30);
  exportButton->setAutoRaise(TRUE);
  toolbar->addWidget(exportButton, 0, 4);
  exportButton->setEnabled(FALSE);

  exportAllButton = new QToolButton(this);
  QToolTip::add(exportAllButton, tr("Export All Charts"));
  exportAllButton->setPixmap(QPixmap(exportfile));
  connect(exportAllButton, SIGNAL(clicked()), this, SLOT(exportAll()));
  exportAllButton->setMaximumWidth(30);
  exportAllButton->setAutoRaise(TRUE);
  toolbar->addWidget(exportAllButton, 0, 5);

  navigator = new Navigator(this, config->getData(Config::DataPath));
  connect(navigator, SIGNAL(doubleClick(QString)), this, SLOT(openSymbol(QString)));
  connect(navigator, SIGNAL(fileSelected(QString)), this, SLOT(symbolSelected(QString)));
  vbox->addWidget(navigator);
}

WorkwithChartsDialog::~WorkwithChartsDialog ()
{
}

void WorkwithChartsDialog::openSymbol ()
{
  openSymbol(navigator->getFileSelection());
}

void WorkwithChartsDialog::openSymbol (QString symbol)
{
  if (! symbol.length())
    return;

  QString s = config->getData(Config::DataPath);
  s.append("/");
  s.append(symbol);

  emit chartOpened (s);

  reject();
}

void WorkwithChartsDialog::deleteChart ()
{
  QString symbol = navigator->getFileSelection();
  if (! symbol.length())
    return;

  QString s = config->getData(Config::DataPath);
  s.append("/");
  s.append(symbol);

  QDir dir(s);
  dir.remove(s, TRUE);

  navigator->updateList();
}

void WorkwithChartsDialog::editChart ()
{
  QString symbol = navigator->getFileSelection();
  if (! symbol.length())
    return;

  QString selection = config->getData(Config::DataPath);
  selection.append("/");
  selection.append(symbol);

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

void WorkwithChartsDialog::exportSymbol ()
{
  QString symbol = navigator->getFileSelection();
  if (! symbol.length())
    return;

  QString s = config->getData(Config::Home);
  s.append("/export");
  QDir dir(s);
  if (! dir.exists(s, TRUE))
  {
    if (! dir.mkdir(s, TRUE))
    {
      qDebug("Unable to create export directory.");
      return;
    }
  }

  s = config->getData(Config::DataPath);
  s.append("/");
  s.append(symbol);
  exportChart(s);
}

void WorkwithChartsDialog::exportAll ()
{
  QString s = config->getData(Config::Home);
  s.append("/export");
  QDir dir(s);
  if (! dir.exists(s, TRUE))
  {
    if (! dir.mkdir(s, TRUE))
    {
      qDebug("Unable to create export directory.");
      return;
    }
  }

  dir.setPath(config->getData(Config::DataPath));

  int loop;
  for (loop = 2; loop < (int) dir.count(); loop++)
  {
    s = dir.path();
    s.append("/");
    s.append(dir[loop]);
    traverse(s);
  }
}

void WorkwithChartsDialog::traverse(QString dirname)
{
  QDir dir(dirname);
  dir.setFilter(QDir::Dirs|QDir::Files);

  const QFileInfoList *fileinfolist = dir.entryInfoList();
  QFileInfoListIterator it(*fileinfolist);
  QFileInfo *fi;
  while((fi = it.current()))
  {
    if(fi->fileName() == "." || fi->fileName() == "..")
    {
      ++it;
      continue;
    }

    if(fi->isDir() && fi->isReadable())
      traverse(fi->absFilePath());
    else
      exportChart(fi->absFilePath());

    ++it;
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
  navigator->setDirectory(d);
  navigator->updateList();
}

void WorkwithChartsDialog::symbolSelected (QString d)
{
  if (! d.length())
  {
    openButton->setEnabled(FALSE);
    editButton->setEnabled(FALSE);
    deleteButton->setEnabled(FALSE);
    exportButton->setEnabled(FALSE);
  }
  else
  {
    openButton->setEnabled(TRUE);
    editButton->setEnabled(TRUE);
    deleteButton->setEnabled(TRUE);
    exportButton->setEnabled(TRUE);
  }
}


