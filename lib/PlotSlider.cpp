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

#include "PlotSlider.h"

#include <QDebug>
#include <QLayout>
#include <QToolButton>

#include "../pics/start.xpm"
#include "../pics/end.xpm"
#include "../pics/prev.xpm"
#include "../pics/next.xpm"
#include "../pics/ppage.xpm"
#include "../pics/npage.xpm"

PlotSlider::PlotSlider ()
{
  QVBoxLayout *vbox = new QVBoxLayout;
  vbox->setSpacing(0);
  vbox->setMargin(0);
  setLayout(vbox);
  
  QHBoxLayout *hbox = new QHBoxLayout;
  hbox->setSpacing(0);
  hbox->setMargin(0);
  vbox->addLayout(hbox);

  QToolButton *b = new QToolButton;
//  b->setMaximumHeight(h);
  b->setIcon(QIcon(start_xpm));
  b->setToolTip(QString(tr("Start")));
  connect(b, SIGNAL(clicked()), this, SLOT(startButtonClicked()));
  hbox->addWidget(b);
  
  b = new QToolButton;
//  b->setMaximumHeight(h);
  b->setIcon(QIcon(ppage_xpm));
  b->setToolTip(QString(tr("Previous Page")));
  connect(b, SIGNAL(clicked()), this, SLOT(pPageButtonClicked()));
  hbox->addWidget(b);
  
  b = new QToolButton;
//  b->setMaximumHeight(h);
  b->setIcon(QIcon(prev_xpm));
  b->setToolTip(QString(tr("Previous Bar")));
  connect(b, SIGNAL(clicked()), this, SLOT(pBarButtonClicked()));
  hbox->addWidget(b);
  
  b = new QToolButton;
//  b->setMaximumHeight(h);
  b->setIcon(QIcon(next_xpm));
  b->setToolTip(QString(tr("Next Bar")));
  connect(b, SIGNAL(clicked()), this, SLOT(nBarButtonClicked()));
  hbox->addWidget(b);
  
  b = new QToolButton;
//  b->setMaximumHeight(h);
  b->setIcon(QIcon(npage_xpm));
  b->setToolTip(QString(tr("Next Page")));
  connect(b, SIGNAL(clicked()), this, SLOT(nPageButtonClicked()));
  hbox->addWidget(b);
  
  b = new QToolButton;
//  b->setMaximumHeight(h);
  b->setIcon(QIcon(end_xpm));
  b->setToolTip(QString(tr("End")));
  connect(b, SIGNAL(clicked()), this, SLOT(endButtonClicked()));
  hbox->addWidget(b);
  
  hbox->addStretch(1);
  
  slider = new QSlider;
  slider->setRange(0, 0);
  slider->setValue(0);
  slider->setOrientation(Qt::Horizontal);
  slider->setToolTip(tr("Scroll Chart"));
  slider->setSizePolicy(QSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding));
  connect(slider, SIGNAL(valueChanged(int)), this, SIGNAL(signalValueChanged(int)));
  vbox->addWidget(slider);
}

void PlotSlider::startButtonClicked ()
{
  slider->setValue(slider->minimum());
}

void PlotSlider::pPageButtonClicked ()
{
  slider->setValue(slider->value() - slider->pageStep());
}

void PlotSlider::pBarButtonClicked ()
{
  slider->setValue(slider->value() - 1);
}

void PlotSlider::nBarButtonClicked ()
{
  slider->setValue(slider->value() + 1);
}

void PlotSlider::nPageButtonClicked ()
{
  slider->setValue(slider->value() + slider->pageStep());
}

void PlotSlider::endButtonClicked ()
{
  slider->setValue(slider->maximum());
}

void PlotSlider::setStart (int count, int width, QList<Setting> &zoomList)
{
  Setting set = zoomList[0];
  
  int page = width / set.getInt(1);
  int max = count - page;
  if (max < 0)
    max = 0;
  
  slider->blockSignals(TRUE);
  slider->setRange(0, count - 1);

  if (count < page)
  {
    slider->setValue(0);
    set.setData(0, 0);
  }
  else
  {
    slider->setValue(max + 1);
    set.setData(0, max + 1);
  }
  
  slider->blockSignals(FALSE);

  zoomList[0] = set;
}

int PlotSlider::getValue ()
{
  return slider->value();
}

void PlotSlider::setValue (int d)
{
  slider->setValue(d);
}

