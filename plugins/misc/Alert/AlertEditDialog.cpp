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

#include "AlertEditDialog.h"
#include "Globals.h"
#include "AlertDataBase.h"
#include "AlertConfig.h"
#include "DateRange.h"

#include <QtDebug>
#include <QComboBox>
#include <QDoubleSpinBox>
#include <QToolButton>
#include <QIcon>
#include <QFormLayout>
#include <QGroupBox>

AlertEditDialog::AlertEditDialog (AlertItem item)
{
  _item = item;
  
  createMainPage();

  setSettings();
}

void AlertEditDialog::createMainPage ()
{
  QWidget *w = new QWidget;

  QVBoxLayout *vbox = new QVBoxLayout;
  vbox->setMargin(5);
  vbox->setSpacing(10);
  w->setLayout(vbox);

  QHBoxLayout *hbox = new QHBoxLayout;
  hbox->setMargin(0);
  hbox->setSpacing(10);
  vbox->addLayout(hbox);

  QFormLayout *form = new QFormLayout;
  form->setMargin(0);
  form->setSpacing(2);
  hbox->addLayout(form);

  // symbol
  _symbols = new SymbolButton(w);
  form->addRow(tr("Symbols"), _symbols);

  // bar length
  BarData bd;
  QStringList l;
  bd.getBarLengthList(l);
  
  _barLength = new QComboBox;
  _barLength->addItems(l);
  form->addRow(tr("Bar Length"), _barLength);

  // bar range
  DateRange dr;
  l.clear();
  dr.list(l);
  
  _bars = new QComboBox;
  _bars->addItems(l);
  _bars->setToolTip(tr("The amount of bars to use for the indicator"));
  form->addRow(tr("Bars"), _bars);

  form = new QFormLayout;
  form->setMargin(0);
  form->setSpacing(2);
  hbox->addLayout(form);

  // mail notify
  _mailNotify = new QCheckBox;
  form->addRow(tr("Mail Notify"), _mailNotify);
  
  // sound notify
  _soundNotify = new QCheckBox;
  form->addRow(tr("Sound Notify"), _soundNotify);

  // popup notify
  _popupNotify = new QCheckBox;
  form->addRow(tr("Popup Notify"), _popupNotify);

  // alert list

  QGroupBox *box = new QGroupBox;
  box->setTitle(tr("Rule"));
  vbox->addWidget(box);

  QVBoxLayout *tvbox = new QVBoxLayout;
  tvbox->setMargin(0);
  tvbox->setSpacing(0);
  box->setLayout(tvbox);
  
  _alertList = new IndicatorPlotList;
  tvbox->addWidget(_alertList);
  
  _tabs->addTab(w, tr("Settings"));
}

void AlertEditDialog::setSettings ()
{
  QStringList l;
  QString s = _item.symbol();
  if (! s.isEmpty())
    l << s;
  _symbols->setSymbolList(l);
  
  _barLength->setCurrentIndex(_item.barLength());
  
  _bars->setCurrentIndex(_item.bars());

  _mailNotify->setChecked(_item.mail());
  
  _soundNotify->setChecked(_item.sound());
  
  _popupNotify->setChecked(_item.popup());

  _alertList->setIndicator(_item.indicator());
  _alertList->setList(_item.plots());
  _alertList->setSettings(_item.settings());
}

void AlertEditDialog::done ()
{
  if (_item.id() == -1)
  {
    AlertConfig db;
    int id = db.getInt(AlertConfig::_LastId);
    _item.setId(id);

    id++;
    db.transaction();
    db.setData(AlertConfig::_LastId, id);
    db.commit();
  }

  QStringList l;
  _symbols->symbolList(l);
  if (l.count())
    _item.setSymbol(l.at(0));
  else
    _item.setSymbol(QString());

  _item.setBarLength(_barLength->currentIndex());

  _item.setBars(_bars->currentIndex());

  _item.setMail(_mailNotify->isChecked());

  _item.setSound(_soundNotify->isChecked());

  _item.setPopup(_popupNotify->isChecked());

  _item.setIndicator(_alertList->indicator());
  _alertList->list(_item.plots());
  _item.setSettings(_alertList->settings());

  AlertDataBase db;
  db.transaction();
  db.setAlert(_item);
  db.commit();

  emit signalEdit(_item);
  
  accept();
}

