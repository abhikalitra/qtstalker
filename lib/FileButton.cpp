/*
 *  Qtstalker stock charter
 *
 *  Copyright (C) 2001-2004 Stefan S. Stratigakos
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

#include "FileButton.h"
#include <qfiledialog.h>

FileButton::FileButton (QWidget *w) : QPushButton (w)
{
  QObject::connect(this, SIGNAL(pressed()), this, SLOT(fileDialog()));
  setMaximumHeight(25);
  setToggleButton(FALSE);
  setText(tr("0 Files"));
}

FileButton::~FileButton ()
{
}

QStringList FileButton::getFile ()
{
  return fileList;
}

void FileButton::fileDialog ()
{
  QString path = "*";

  fileList = QFileDialog::getOpenFileNames("*", path, this, "file dialog");
  
  QString s = QString::number(fileList.count());
  s.append(" ");
  s.append(tr("Files"));
  setText(s);
}

