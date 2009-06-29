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

#include "SymbolDialog.h"
#include <QKeyEvent>

SymbolDialog::SymbolDialog (QWidget *w, QString &bp, QString &dir, QString &filter, QFileDialog::FileMode mode) :
                            QFileDialog (w, QString(), dir, filter)
{
  basePath = bp;
  setFileMode(mode);
  connect(this, SIGNAL(directoryEntered(const QString &)), this, SLOT(dirSelected(const QString &)));
  keyFlag = FALSE;
}

SymbolDialog::~SymbolDialog ()
{
}

void SymbolDialog::dirSelected (const QString &d)
{
  if (fileMode() == QFileDialog::DirectoryOnly)
  {
    blockSignals(TRUE);
    setDirectory(basePath);
    blockSignals(FALSE);
    return;
  }
  
  if (d.length() < basePath.length())
  {
    blockSignals(TRUE);
    setDirectory(basePath);
    blockSignals(FALSE);
  }
}

void SymbolDialog::setKeyFlag (bool d)
{
  keyFlag = d;
}

void SymbolDialog::keyPressEvent (QKeyEvent *key)
{
//  if (keyFlag)
//    emit signalKeyPressed (type, key->state(), key->key(), key->ascii(), key->text());
    
  doKeyPress(key);  
}

void SymbolDialog::doKeyPress (QKeyEvent *key)
{
  key->accept();
  QFileDialog::keyPressEvent(key);
}

void SymbolDialog::setType (int d)
{
  type = d;
}

