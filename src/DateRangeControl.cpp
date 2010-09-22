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

#include "DateRangeControl.h"
#include "Config.h"
#include "DateRange.h"
#include "../pics/prev.xpm"
#include "../pics/next.xpm"

#include <QDebug>
#include <QString>
#include <QStringList>
#include <QIcon>

DateRangeControl::DateRangeControl (QToolBar *tb) : QObject (tb)
{
  createButtons(tb);
}

void DateRangeControl::createButtons (QToolBar *tb)
{
  Config config;

  // previous button
  _prevButton = new QToolButton;
  _prevButton->setIcon(QIcon(prev_xpm));
  _prevButton->setToolTip(QString(tr("Previous Date Range")));
  _prevButton->setStatusTip(QString(tr("Previous Date Range")));
  _prevButton->setCheckable(FALSE);
  connect(_prevButton, SIGNAL(clicked()), this, SLOT(prevRange()));
  tb->addWidget(_prevButton);

  QStringList l;
  DateRange dr;
  dr.list(l);
  _ranges = new QComboBox;
  _ranges->setToolTip(QString(tr("Date Range")));
  _ranges->setStatusTip(QString(tr("Date Range")));
  _ranges->addItems(l);
  _ranges->setCurrentIndex(5); // 1 year default
  
  QString s;
  config.getData(Config::LastDateRange, s);
  if (s.isEmpty())
  {
    config.transaction();
    config.setData(Config::LastDateRange, _ranges->currentIndex());
    config.commit();
  }
  else
    _ranges->setCurrentIndex(s.toInt());
  
  _ranges->setMaxVisibleItems(l.count());
  connect(_ranges, SIGNAL(currentIndexChanged(int)), this, SLOT(rangeChanged(int)));
  tb->addWidget(_ranges);
  
  // next button
  _nextButton = new QToolButton;
  _nextButton->setIcon(QIcon(next_xpm));
  _nextButton->setToolTip(QString(tr("Next Date Range")));
  _nextButton->setStatusTip(QString(tr("Next Date Range")));
  _nextButton->setCheckable(FALSE);
  connect(_nextButton, SIGNAL(clicked()), this, SLOT(nextRange()));
  tb->addWidget(_nextButton);
}

void DateRangeControl::prevRange ()
{
  int t = _ranges->currentIndex();
  t--;
  if (t == 0)
    _prevButton->setEnabled(FALSE);
  else
    _prevButton->setEnabled(TRUE);

  _ranges->setCurrentIndex(t);

  buttonStatus();
}

void DateRangeControl::nextRange ()
{
  int t = _ranges->currentIndex();
  t++;
  if (t == _ranges->count() - 1)
    _nextButton->setEnabled(FALSE);
  else
    _nextButton->setEnabled(TRUE);

  _ranges->setCurrentIndex(t);

  buttonStatus();
}

void DateRangeControl::rangeChanged (int d)
{
  Config config;
  config.transaction();
  config.setData(Config::LastDateRange, d);
  config.commit();
  
  buttonStatus();

  emit signalDateRangeChanged();
}

int DateRangeControl::dateRange ()
{
  return _ranges->currentIndex();
}

void DateRangeControl::buttonStatus ()
{
  bool left = TRUE;
  bool right = TRUE;

  if (_ranges->currentIndex() == 0)
    left = FALSE;
  else if (_ranges->currentIndex() == _ranges->count() - 1)
    right = FALSE;

  _prevButton->setEnabled(left);
  _nextButton->setEnabled(right);
}

