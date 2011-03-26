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

#include "ADDialog.h"
#include "Globals.h"
#include "AD.h"

#include <QtDebug>
#include <QStringList>

ADDialog::ADDialog (QWidget *p, Setting *set) : Dialog (p)
{
  _settings = set;
  _keySize = "ADDialog_window_size";
  _keyPos = "ADDialog_window_position";

  QStringList l;
  l << "QtStalker" + g_session + ":" << "AD" << tr("Indicator") << _settings->data("NAME");
  setWindowTitle(l.join(" "));

  createGeneralPage();

  loadSettings();
}

void ADDialog::createGeneralPage ()
{
  // color
  _color = new ColorButton(this, QColor(_settings->data(AD::_COLOR)));
  _color->setColorButton();
  _form->addRow(tr("Color"), _color);

  // style
  Curve c;
  QStringList l;
  c.list(l, 1);

  _style = new QComboBox;
  _style->addItems(l);
  _style->setCurrentIndex(_style->findText(_settings->data(AD::_STYLE), Qt::MatchExactly));
  _form->addRow(tr("Style"), _style);

  // make room unused
  _message->hide();
}

void ADDialog::done ()
{
  _settings->setData(AD::_COLOR, _color->color().name());
  _settings->setData(AD::_STYLE, _style->currentText());

  saveSettings();
  
  accept();
}
