/*
 *  Qtstalker stock charter
 *
 *  Copyright (C) 2001-2006 Stefan S. Stratigakos
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
#include "../pics/dirclosed.xpm"
#include <qpixmap.h>

#define BUTTON_SIZE 24

Navigator::Navigator (QWidget *w, QString &bp) : QListBox(w)
{
  basePath = bp;
  id = 0;
  keyFlag = FALSE;

  currentDir.setPath(bp);
  currentDir.setMatchAllDirs(TRUE);

  setSelectionMode(QListBox::Single);
  connect(this, SIGNAL(currentChanged(QListBoxItem *)), this, SLOT(fileSelection(QListBoxItem *)));
  connect(this, SIGNAL(selected(QListBoxItem *)), this, SLOT(checkDirectory(QListBoxItem *)));
}

Navigator::~Navigator ()
{
}

void Navigator::updateList ()
{
  int ci = currentItem();

  clear();

  currentDir.setPath(currentDir.absPath());

  int loop;
  if (! basePath.compare(currentDir.absPath()))
  {
    loop = 2;
    currentDir.setFilter(QDir::Dirs);
  }
  else
  {
    loop = 1;
    currentDir.setFilter(QDir::All);
  }

  for (; loop < (int) currentDir.count(); loop++)
  {
    QString s = currentDir.absPath();
    s.append("/");
    s.append(currentDir[loop]);
    QFileInfo info(s);
    
    if (info.isDir())
    {
      if (currentDir[loop].compare("."))
        insertItem(QPixmap(dirclosed), currentDir[loop], -1);
    }
    else
    {
      // check if this is a broken group link
      if (! info.exists())
      {
        QDir dir;
	dir.remove(s);
	continue;
      }
    
      insertItem(currentDir[loop], -1);
    }
  }

  clearSelection();

  if (ci != -1)
    setCurrentItem(ci);
}

void Navigator::upDirectory ()
{
  QString s = currentDir.dirName();
  currentDir.cdUp();
  updateList();
  setCurrentItem(findItem(s, Qt::ExactMatch));
  ensureCurrentVisible();
  emit noSelection();
}

void Navigator::fileSelection (QListBoxItem *item)
{
  if (! item)
  {
    emit noSelection();
    return;
  }

  if (item->pixmap())
  {
    emit noSelection();
    return;
  }

  QString s;
  getFileSelection(s);
  emit fileSelected(s);
}

void Navigator::getFileSelection (QString &s)
{
  s = currentDir.absPath();
  s.append("/");
  s.append(currentText());
}

void Navigator::setDirectory (QString &d)
{
  if (d.length())
  {
    QFileInfo fi(d);
    currentDir.setPath(fi.dirPath(TRUE));
  }
}

void Navigator::getCurrentPath (QString &d)
{
  d = currentDir.absPath();
}

void Navigator::checkDirectory (QListBoxItem *item)
{
  if (! item)
  {
    emit noSelection();
    return;
  }

  if (! item->text().compare(".."))
  {
    upDirectory();
    return;
  }

  QString s;
  if (item->pixmap())
  {
    s = currentDir.absPath() + "/" + item->text();
    currentDir.setPath(s);
    updateList();
    emit noSelection();
    return;
  }
  
  getFileSelection(s);
  emit fileOpened(s);
}

void Navigator::setFilter (QString &d)
{
  currentDir.setNameFilter(d);
  updateList();
  emit noSelection();
}

void Navigator::setId (int d)
{
  id = d;
}

void Navigator::setKeyFlag (bool d)
{
  keyFlag = d;
}

void Navigator::setHome ()
{
  QString s = basePath;
  currentDir.setPath(s);
  updateList();
  emit noSelection();
}

void Navigator::keyPressEvent (QKeyEvent *key)
{
  if (keyFlag)
    emit signalKeyPressed (id, key->state(), key->key(), key->ascii(), key->text());
    
  doKeyPress(key);  
}

void Navigator::doKeyPress (QKeyEvent *key)
{
  switch (key->key())
  {
    case Qt::Key_Delete:
      key->accept();
      emit keyPress(key->state(), key->key());
      break;
    case Qt::Key_Left: // segfaults if we dont trap this
    case Qt::Key_Right: // segfaults if we dont trap this
      key->accept();
      break;      
    case Qt::Key_Enter:
    case Qt::Key_Return:
      key->accept();
      checkDirectory(item(currentItem()));
      break;
    case Qt::Key_Home:
      key->accept();
      setHome();
      QListBox::keyPressEvent(key);
      break;
    default:
      key->accept();
      QListBox::keyPressEvent(key);
      break;
  }
}

