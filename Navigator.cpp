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
#include "dirclosed.xpm"
#include <qpixmap.h>

#define BUTTON_SIZE 24

Navigator::Navigator (QWidget *w, QString bp) : QListBox(w)
{
  basePath = bp;

  currentDir.setPath(bp);

  setSelectionMode(QListBox::Single);
  connect(this, SIGNAL(currentChanged(QListBoxItem *)), this, SLOT(fileSelection(QListBoxItem *)));
}

Navigator::~Navigator ()
{
}

void Navigator::updateList ()
{
  clear();
  
  currentDir.refresh();

  int loop;
  for (loop = 2; loop < (int) currentDir.count(); loop++)
  {
    QString s = currentDir.absPath();
    s.append("/");
    s.append(currentDir[loop]);
    QFileInfo info(s);
    if (info.isDir())
      insertItem(QPixmap(dirclosed), currentDir[loop], -1);
    else
      insertItem(currentDir[loop], -1);
  }
  
  clearSelection();
}

void Navigator::upDirectory ()
{
  currentDir.cdUp();
  updateList();
  emit noSelection();

  if (! basePath.compare(currentDir.absPath()))
    emit directoryStatus(FALSE);
  else
    emit directoryStatus(TRUE);
}

void Navigator::fileSelection (QListBoxItem *item)
{
  if (! item)
    return;

  if (item->pixmap())
  {
    QString s = currentDir.absPath();
    s.append("/");
    s.append(item->text());
    currentDir.setPath(s);
    updateList();
    emit noSelection();
    emit directoryStatus(TRUE);
  }
  else
    emit fileSelected(getFileSelection());
}

QString Navigator::getFileSelection ()
{
  QString s = currentDir.absPath();
  s.append("/");
  s.append(currentText());
  return s;
}

void Navigator::setDirectory (QString d)
{
  if (d.length())
  {
    QFileInfo fi(d);
    currentDir.setPath(fi.dirPath(TRUE));
  }
}

QString Navigator::getCurrentPath ()
{
  return currentDir.absPath();
}


