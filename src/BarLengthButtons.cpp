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
#include "Config.h"
#include "BarData.h"

#include "../pics/prev.xpm"
#include "../pics/next.xpm"

#include <QDebug>
#include <QString>
#include <QStringList>
#include <QIcon>

BarLengthButtons::BarLengthButtons (QToolBar *tb) : QObject (tb)
{
  createButtons(tb);
}

void BarLengthButtons::createButtons (QToolBar *tb)
{
  // previous button
  _prevButton = new QToolButton;
  _prevButton->setIcon(QIcon(prev_xpm));
  _prevButton->setToolTip(QString(tr("Previous Bar Length")));
  _prevButton->setStatusTip(QString(tr("Previous Bar Length")));
  _prevButton->setCheckable(FALSE);
  connect(_prevButton, SIGNAL(clicked()), this, SLOT(prevLength()));
  tb->addWidget(_prevButton);

  QStringList l;
  BarData bd;
  bd.getBarLengthList(l);
  _lengths = new QComboBox;
  _lengths->setToolTip(QString(tr("Bar Length")));
  _lengths->setStatusTip(QString(tr("Bar Length")));
  _lengths->addItems(l);
  _lengths->setCurrentIndex(6); // daily default
  
  QString s;
  Config config;
  config.getData(Config::BarLength, s);
  if (s.isEmpty())
  {
    config.transaction();
    config.setData(Config::BarLength, _lengths->currentIndex());
    config.commit();
  }
  else
    _lengths->setCurrentIndex(s.toInt());
  
  _lengths->setMaxVisibleItems(l.count());
  connect(_lengths, SIGNAL(currentIndexChanged(int)), this, SLOT(lengthChanged(int)));
  tb->addWidget(_lengths);
  
  // next button
  _nextButton = new QToolButton;
  _nextButton->setIcon(QIcon(next_xpm));
  _nextButton->setToolTip(QString(tr("Next Bar Length")));
  _nextButton->setStatusTip(QString(tr("Next Bar Length")));
  _nextButton->setCheckable(FALSE);
  connect(_nextButton, SIGNAL(clicked()), this, SLOT(nextLength()));
  tb->addWidget(_nextButton);
}

void BarLengthButtons::prevLength ()
{
  int t = _lengths->currentIndex();
  t--;
  if (t == 0)
    _prevButton->setEnabled(FALSE);
  else
    _prevButton->setEnabled(TRUE);

  _lengths->setCurrentIndex(t);

  buttonStatus();
}

void BarLengthButtons::nextLength ()
{
  int t = _lengths->currentIndex();
  t++;
  if (t == _lengths->count() - 1)
    _nextButton->setEnabled(FALSE);
  else
    _nextButton->setEnabled(TRUE);

  _lengths->setCurrentIndex(t);

  buttonStatus();
}

void BarLengthButtons::lengthChanged (int d)
{
  Config config;
  config.transaction();
  config.setData(Config::BarLength, d);
  config.commit();
  
  buttonStatus();

  emit signalBarLengthChanged(d);
}

int BarLengthButtons::length ()
{
  return _lengths->currentIndex();
}

void BarLengthButtons::buttonStatus ()
{
  bool left = TRUE;
  bool right = TRUE;

  if (_lengths->currentIndex() == 0)
    left = FALSE;
  else if (_lengths->currentIndex() == _lengths->count() - 1)
    right = FALSE;

  _prevButton->setEnabled(left);
  _nextButton->setEnabled(right);
}

void BarLengthButtons::getCurrentText (QString &d)
{
  BarData bd;
  bd.barLengthText((BarData::BarLength) _lengths->currentIndex(), d);
}

