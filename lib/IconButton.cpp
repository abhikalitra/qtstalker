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

#include "IconButton.h"
#include "WindowTitle.h"

#include <QFileDialog>

IconButton::IconButton (QWidget *w, QString p) : QPushButton (w)
{
  QObject::connect(this, SIGNAL(clicked()), this, SLOT(fileDialog()));
  
  _file = QDir::homePath();
  if (p.length())
    _file = p;
  
  updateButton();
}

QString IconButton::file ()
{
  return _file;
}

void IconButton::setFile (QString d)
{
  _file = d;
  updateButton();
}

void IconButton::fileDialog ()
{
  WindowTitle wt;
  QFileDialog *dialog = new QFileDialog(this);
  dialog->setDirectory(_file);
  dialog->setWindowTitle(wt.title(tr("Select Icon"), QString()));
  connect(dialog, SIGNAL(finished(int)), dialog, SLOT(deleteLater()));
  connect(dialog, SIGNAL(fileSelected(const QString &)), this, SLOT(setFile(QString)));
  dialog->show();
}

void IconButton::updateButton ()
{
  setIcon(QIcon(_file));
  emit signalSelectionChanged();
}
