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

#include "ChartPage.h"
#include "EditChartDialog.h"
#include "ChartDb.h"
#include "Setting.h"
#include "edit.xpm"
#include "delete.xpm"
#include "export.xpm"
#include "up.xpm"
#include <qmessagebox.h>

ChartPage::ChartPage (QWidget *w, Config *c) : BaseDialog(w)
{
  config = c;

  nav = new Navigator(this, config->getData(Config::DataPath));
  connect(nav, SIGNAL(fileSelected(QString)), this, SLOT(chartSelected(QString)));
  connect(nav, SIGNAL(noSelection()), this, SLOT(chartNoSelection()));
  connect(nav, SIGNAL(directoryStatus(bool)), this, SLOT(directoryStatus(bool)));
  nav->updateList();
  basebox->addWidget(nav);

  setButton(QPixmap(up), tr("Up Directory"), 0);
  connect(getButton(0), SIGNAL(clicked()), nav, SLOT(upDirectory()));
  setButtonStatus(0, FALSE);

  setButton(QPixmap(edit), tr("Edit Chart"), 1);
  connect(getButton(1), SIGNAL(clicked()), this, SLOT(editChart()));
  setButtonStatus(1, FALSE);

  setButton(QPixmap(deletefile), tr("Delete Chart"), 2);
  connect(getButton(2), SIGNAL(clicked()), this, SLOT(deleteChart()));
  setButtonStatus(2, FALSE);

  setButton(QPixmap(exportfile), tr("Export Chart"), 3);
  connect(getButton(3), SIGNAL(clicked()), this, SLOT(exportSymbol()));
  setButtonStatus(3, FALSE);

  setButton(QPixmap(exportfile), tr("Export All Charts"), 4);
  connect(getButton(4), SIGNAL(clicked()), this, SLOT(exportAll()));
  setButtonStatus(4, TRUE);
}

ChartPage::~ChartPage ()
{
}

void ChartPage::deleteChart ()
{
  QString symbol = nav->getFileSelection();
  if (! symbol.length())
    return;

  int rc = QMessageBox::warning(this,
  					    tr("Qtstalker: Warning"),
					    tr("Are you sure you want to delete this chart?"),
					    QMessageBox::Yes,
					    QMessageBox::No,
					    QMessageBox::NoButton);

  if (rc == QMessageBox::No)
    return;

  QDir dir(symbol);
  dir.remove(symbol, TRUE);

  nav->updateList();
  
  chartNoSelection();
}

void ChartPage::editChart ()
{
  QString symbol = nav->getFileSelection();
  if (! symbol.length())
    return;

  EditChartDialog *dialog = new EditChartDialog(config, symbol);

  dialog->exec();

  delete dialog;
}

void ChartPage::exportSymbol ()
{
  QString symbol = nav->getFileSelection();
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

  exportChart(symbol);
}

void ChartPage::exportAll ()
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

void ChartPage::traverse(QString dirname)
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

void ChartPage::exportChart (QString path)
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

void ChartPage::chartSelected (QString d)
{
  setButtonStatus(1, TRUE);
  setButtonStatus(2, TRUE);
  setButtonStatus(3, TRUE);
  emit fileSelected(d);
}

void ChartPage::chartNoSelection ()
{
  setButtonStatus(1, FALSE);
  setButtonStatus(2, FALSE);
  setButtonStatus(3, FALSE);
}

void ChartPage::directoryStatus (bool d)
{
  setButtonStatus(0, d);
}

