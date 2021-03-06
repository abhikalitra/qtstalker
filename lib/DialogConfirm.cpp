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

#include "DialogConfirm.h"
#include "WindowTitle.h"

#include <QtDebug>
#include <QMessageBox>

DialogConfirm::DialogConfirm (QWidget *p, QString id, Entity settings) : Dialog (p)
{
  _id = id;
  _settings = settings;
  _keySize = "confirm_dialog_window_size";
  _keyPos = "confirm_dialog_window_position";

  WindowTitle wt;
  setWindowTitle(wt.title(tr("Confirm"), QString()));

  createGUI();
  
  if (! _id.isEmpty())
  {
    Data text;
    _settings.toData(QString("TEXT"), text);
    _text->setText(text.toString());
  }

  loadSettings();
}

void DialogConfirm::createGUI ()
{
  QHBoxLayout *hbox = new QHBoxLayout;
  hbox->setSpacing(2);
  _vbox->insertLayout(0, hbox);

  QVBoxLayout *vbox = new QVBoxLayout;
  vbox->setSpacing(0);
  hbox->addLayout(vbox);

  QMessageBox mb;
  mb.setIcon(QMessageBox::Warning);

  QLabel *label = new QLabel;
  label->setPixmap(mb.iconPixmap());
  vbox->addWidget(label);
  vbox->addStretch(1);

  _text = new QTextEdit;
  _text->setReadOnly(TRUE);
  hbox->addWidget(_text);

  _message->hide();
}

void DialogConfirm::setMessage (QString d)
{
  _text->setText(d);
}

void DialogConfirm::done ()
{
  _saveFlag++;
  Dialog::done();
}
