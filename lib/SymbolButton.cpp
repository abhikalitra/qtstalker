/*
 *  Qtstalker stock charter
 *
 *  Copyright (C) 2001-2005 Stefan S. Stratigakos
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

#include "SymbolButton.h"
#include "SymbolDialog.h"

SymbolButton::SymbolButton (QWidget *w, QString &d, QString &s) : QPushButton (w)
{
  QObject::connect(this, SIGNAL(clicked()), this, SLOT(fileDialog()));
  setMaximumHeight(25);
  setToggleButton(FALSE);
  setSymbol(s);
  dirPath = d;
}

SymbolButton::~SymbolButton ()
{
}

QString SymbolButton::getSymbol ()
{
  return symbol;
}

QString SymbolButton::getPath ()
{
  return path;
}

void SymbolButton::fileDialog ()
{
  QString s("*");
  SymbolDialog *dialog = new SymbolDialog(this,
  					  dirPath,
					  s,
					  QFileDialog::ExistingFiles);
  dialog->setCaption(tr("Select Symbol"));

  int rc = dialog->exec();

  if (rc == QDialog::Accepted)
  {
    QStringList l = dialog->selectedFiles();
    if (l.count())
    {
      setSymbol(l[0]);
      emit symbolChanged();
    }
  }

  delete dialog;
}

void SymbolButton::setSymbol (QString &d)
{
  if (! d.length())
  {
    setText(d);
    path = d;
    return;
  }
  
  QStringList l = QStringList::split("/", d, FALSE);
  symbol = l[l.count() - 1];
  setText(symbol);
  path = d;
}



