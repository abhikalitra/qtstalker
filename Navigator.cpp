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

#include "Navigator.h"
#include "ChartDb.h"
#include "Setting.h"
#include "dirclosed.xpm"
#include <qpixmap.h>
#include <qgroupbox.h>
#include <qlayout.h>

Navigator::Navigator (QWidget *w, QString bp) : QWidget(w)
{
  item = 0;
  basePath = bp;

  currentDir.setPath(bp);

  QHBoxLayout *hbox = new QHBoxLayout(this);
  hbox->setMargin(0);
  hbox->setSpacing(5);

  list = new QListView(this);
  list->addColumn(tr("Symbol"), 150);
  list->setSelectionMode(QListView::Single);
  connect(list, SIGNAL(clicked(QListViewItem *)), this, SLOT(fileSelection(QListViewItem *)));
  connect(list, SIGNAL(doubleClicked(QListViewItem *)), this, SLOT(slotDoubleClicked()));
  hbox->addWidget(list);

  QGroupBox *gbox = new QGroupBox(this);
  gbox->setColumnLayout(2, Horizontal);
  hbox->addWidget(gbox);
  gbox->setMinimumWidth(200);
  gbox->setMaximumWidth(200);

  QLabel *label = new QLabel(tr("Symbol"), gbox);
  symbol = new QLabel(0, gbox);

  label = new QLabel(tr("Title"), gbox);
  title = new QLabel(0, gbox);

  label = new QLabel(tr("Type"), gbox);
  type = new QLabel(0, gbox);

  label = new QLabel(tr("First Date"), gbox);
  firstDate = new QLabel(0, gbox);

  label = new QLabel(tr("Last Date"), gbox);
  lastDate = new QLabel(0, gbox);
}

Navigator::~Navigator ()
{
}

void Navigator::updateList ()
{
  list->clear();
  
  currentDir.setPath(currentDir.absPath());

  int loop;
  for (loop = 1; loop < (int) currentDir.count(); loop++)
  {
    item = new QListViewItem(list, currentDir[loop]);

    QString s = currentDir.absPath();
    s.append("/");
    s.append(currentDir[loop]);
    QFileInfo info(s);
    if (info.isDir())
      item->setPixmap(0, QPixmap(dirclosed));
  }

  item = 0;
}

void Navigator::upDirectory ()
{
  if (basePath.compare(currentDir.absPath()))
  {
    currentDir.cdUp();
    updateList();
  }
}

void Navigator::clearFileInfo ()
{
  symbol->setText("");
  title->setText("");
  type->setText("");
  firstDate->setText("");
  lastDate->setText("");
}

void Navigator::setFileInfo ()
{
  item = list->selectedItem();
  if (! item)
    return;

  QString s = currentDir.absPath();
  s.append("/");
  s.append(item->text(0));

  ChartDb *db = new ChartDb;
  if (db->openChart(s))
  {
    delete db;
    clearFileInfo();
    return;
  }

  Setting *set = db->getDetails();

  symbol->setText(set->getData("Symbol"));

  title->setText(set->getData("Title"));

  type->setText(set->getData("Chart Type"));

  s = set->getData("First Date");
  s.truncate(s.length() - 6);
  firstDate->setText(s);

  s = set->getData("Last Date");
  s.truncate(s.length() - 6);
  lastDate->setText(s);

  delete db;
}

void Navigator::fileSelection (QListViewItem *item)
{
  if (! item)
  {
    emit fileSelected(getFileSelection());
    return;
  }

  QString s = item->text(0);
  if (! s.compare(".."))
  {
    upDirectory();
    emit fileSelected(getFileSelection());
    return;
  }

  if (item->pixmap(0))
  {
    QString s = currentDir.absPath();
    s.append("/");
    s.append(item->text(0));
    currentDir.setPath(s);
    updateList();
    clearFileInfo();
    emit fileSelected(getFileSelection());
  }
  else
  {
    setFileInfo();
    emit fileSelected(getFileSelection());
  }
}

QString Navigator::getFileSelection ()
{
  QString s;

  item = list->selectedItem();
  if (! item)
    return s;

  if (! item->pixmap(0))
  {
    s = currentDir.absPath();
    s.append("/");
    s.append(item->text(0));

    QString s2 = basePath;
    s.remove(0, s2.length() + 1);
  }

  return s;
}

void Navigator::slotDoubleClicked ()
{
  emit doubleClick(getFileSelection());
}

void Navigator::setDirectory (QString d)
{
  if (d.length())
  {
    int i = d.findRev('/', -1, TRUE);
    d.truncate(i);
    currentDir.setPath(d);
  }
}

