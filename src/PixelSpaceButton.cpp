/*
 *  Qtstalker stock charter
 *
 *  Copyright (C) 2001-2010 Stefan S. Stratigakos
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

#include "PixelSpaceButton.h"
#include "Globals.h"

#include <QDebug>
#include <QString>
#include <QInputDialog>
#include <QSettings>

PixelSpaceButton::PixelSpaceButton (QString d)
{
  _key = d;

  QSettings settings(g_settingsFile);
  _pixelSpace = settings.value(_key, 6).toInt();

  setCheckable(FALSE);

  changeText();

  connect(this, SIGNAL(clicked()), this, SLOT(buttonClicked()));
}

void PixelSpaceButton::contextMenuEvent (QContextMenuEvent *)
{
  dialog();
}

void PixelSpaceButton::buttonClicked ()
{
  emit signalPixelSpaceChanged(_pixelSpace);
}

void PixelSpaceButton::setPixelSpace (int d)
{
  _pixelSpace = d;

  changeText();

  QSettings settings(g_settingsFile);
  settings.setValue(_key, _pixelSpace);
  settings.sync();
}

void PixelSpaceButton::dialog ()
{
  QInputDialog *dialog = new QInputDialog;
  dialog->setWindowTitle("Qtstalker" + g_session + ": " + tr("Set Bar Spacing Button"));
  dialog->setLabelText(tr("Enter bar spacing"));
  dialog->setInputMode(QInputDialog::IntInput);
  dialog->setIntRange(6, 99);
  dialog->setIntStep(1);
  connect(dialog, SIGNAL(intValueSelected(int)), this, SLOT(setPixelSpace(int)));
  connect(dialog, SIGNAL(finished(int)), dialog, SLOT(deleteLater()));
  dialog->show();
}

void PixelSpaceButton::changeText ()
{
  QString s = tr("Set Bar Spacing to ");
  QString s2 = QString::number(_pixelSpace);
  setToolTip(s + s2);
  setStatusTip(s + s2 + tr(" Right click mouse for options."));
  setText(s2);
}
