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
#include "up.xpm"
#include <qtooltip.h>

#define BUTTON_SIZE 24

Navigator::Navigator (QWidget *w, QString bp) : QWidget(w)
{
  buttonList.setAutoDelete(TRUE);
  basePath = bp;

  currentDir.setPath(bp);

  QVBoxLayout *vbox = new QVBoxLayout(this);
  vbox->setMargin(0);
  vbox->setSpacing(5);

  toolbar = new QGridLayout(vbox, 1, 2);
  toolbar->setSpacing(1);

  upButton = new QToolButton(this);
  QToolTip::add(upButton, tr("Up Directory"));
  upButton->setPixmap(QPixmap(up));
  upButton->setMaximumWidth(BUTTON_SIZE);
  upButton->setMaximumHeight(BUTTON_SIZE);
  upButton->setAutoRaise(TRUE);
  toolbar->addWidget(upButton, 0, 0);
  upButton->setEnabled(FALSE);
  connect(upButton, SIGNAL(clicked()), this, SLOT(upDirectory()));

  QHBoxLayout *hbox = new QHBoxLayout(vbox);
  hbox->setMargin(0);
  hbox->setSpacing(5);

  list = new QListBox(this);
  list->setSelectionMode(QListBox::Single);
  connect(list, SIGNAL(selectionChanged()), this, SLOT(fileSelection()));
  hbox->addWidget(list);
}

Navigator::~Navigator ()
{
}

void Navigator::updateList ()
{
  list->clear();

  currentDir.setPath(currentDir.absPath());

  int loop;
  for (loop = 2; loop < (int) currentDir.count(); loop++)
  {
    QString s = currentDir.absPath();
    s.append("/");
    s.append(currentDir[loop]);
    QFileInfo info(s);
    if (info.isDir())
      list->insertItem(QPixmap(dirclosed), currentDir[loop], -1);
    else
      list->insertItem(currentDir[loop], -1);
  }
}

void Navigator::upDirectory ()
{
  currentDir.cdUp();
  updateList();
  emit noSelection();

  if (! basePath.compare(currentDir.absPath()))
    upButton->setEnabled(FALSE);
  else
    upButton->setEnabled(TRUE);
}

void Navigator::fileSelection ()
{
  if (list->pixmap(list->currentItem()))
  {
    QString s = currentDir.absPath();
    s.append("/");
    s.append(list->currentText());
    currentDir.setPath(s);
    updateList();
    emit noSelection();
    upButton->setEnabled(TRUE);
  }
  else
    emit fileSelected(getFileSelection());
}

QString Navigator::getFileSelection ()
{
  QString s = currentDir.absPath();
  s.append("/");
  s.append(list->currentText());
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

void Navigator::setButton (QPixmap pix, QString tt, int pos)
{
  QToolButton *button = new QToolButton(this);
  QToolTip::add(button, tt);
  button->setPixmap(pix);
  button->setMaximumWidth(BUTTON_SIZE);
  button->setMaximumHeight(BUTTON_SIZE);
  button->setAutoRaise(TRUE);
  toolbar->addWidget(button, 0, pos + 1);
  buttonList.append(button);
  toolbar->expand(1, toolbar->numCols() + 1);
}

QToolButton * Navigator::getButton (int pos)
{
  return buttonList.at(pos);
}

void Navigator::setButtonStatus (int pos, bool status)
{
  QToolButton *button = buttonList.at(pos);
  button->setEnabled(status);
}


