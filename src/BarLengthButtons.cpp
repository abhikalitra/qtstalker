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

#include "BarLengthButtons.h"

#include <QString>
#include <QDebug>
#include <QToolButton>

#include "Config.h"

BarLengthButtons::BarLengthButtons (QToolBar *tb) : QObject (tb)
{
  createButtons(tb);
}

void BarLengthButtons::createButtons (QToolBar *tb)
{
  // button group for the bars group
  group = new QButtonGroup(this);
  connect(group, SIGNAL(buttonClicked(int)), this, SLOT(barLengthChanged(int)));
  
  QToolButton *b = new QToolButton;
  b->setToolTip(QString(tr("Monthly Bars")));
  b->setStatusTip(QString(tr("Monthly Bars")));
  b->setText(QString("M"));
  b->setCheckable(TRUE);
  tb->addWidget(b);
  group->addButton(b, 8);

  b = new QToolButton;
  b->setToolTip(QString(tr("Weekly Bars")));
  b->setStatusTip(QString(tr("Weekly Bars")));
  b->setText(QString("W"));
  b->setCheckable(TRUE);
  tb->addWidget(b);
  group->addButton(b, 7);

  b = new QToolButton;
  b->setToolTip(QString(tr("Daily Bars")));
  b->setStatusTip(QString(tr("Daily Bars")));
  b->setText(QString("D"));
  b->setCheckable(TRUE);
  tb->addWidget(b);
  group->addButton(b, 6);

  b = new QToolButton;
  b->setToolTip(QString(tr("60 Minute Bars")));
  b->setStatusTip(QString(tr("60 Minute Bars")));
  b->setText(QString("60"));
  b->setCheckable(TRUE);
  tb->addWidget(b);
  group->addButton(b, 5);

  b = new QToolButton;
  b->setToolTip(QString(tr("30 Minute Bars")));
  b->setStatusTip(QString(tr("30 Minute Bars")));
  b->setText(QString("30"));
  b->setCheckable(TRUE);
  tb->addWidget(b);
  group->addButton(b, 4);

  b = new QToolButton;
  b->setToolTip(QString(tr("15 Minute Bars")));
  b->setStatusTip(QString(tr("15 Minute Bars")));
  b->setText(QString("15"));
  b->setCheckable(TRUE);
  tb->addWidget(b);
  group->addButton(b, 3);

  b = new QToolButton;
  b->setToolTip(QString(tr("10 Minute Bars")));
  b->setStatusTip(QString(tr("10 Minute Bars")));
  b->setText(QString("10"));
  b->setCheckable(TRUE);
  tb->addWidget(b);
  group->addButton(b, 2);

  b = new QToolButton;
  b->setToolTip(QString(tr("5 Minute Bars")));
  b->setStatusTip(QString(tr("5 Minute Bars")));
  b->setText(QString("5"));
  b->setCheckable(TRUE);
  tb->addWidget(b);
  group->addButton(b, 1);

  b = new QToolButton;
  b->setToolTip(QString(tr("1 Minute Bars")));
  b->setStatusTip(QString(tr("1 Minute Bars")));
  b->setText(QString("1"));
  b->setCheckable(TRUE);
  tb->addWidget(b);
  group->addButton(b, 0);

  // set the button to last used position
  Config config;
  b = (QToolButton *) group->button(config.getInt((int) Config::BarLength));
  b->setChecked(TRUE);
}

void BarLengthButtons::barLengthChanged (int d)
{
  emit signalBarLengthChanged(d);

  Config config;
  config.setData(Config::BarLength, d);
}

int BarLengthButtons::getCurrentButton ()
{
  return group->checkedId();
}

