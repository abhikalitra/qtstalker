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
#include "Setting.h"

#include <QDebug>
#include <QLayout>

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

  _startButton = new QToolButton;
  _startButton->setIcon(QIcon(start_xpm));
  _startButton->setToolTip(QString(tr("Start")));
  connect(_startButton, SIGNAL(clicked()), this, SLOT(startButtonClicked()));
  hbox->addWidget(_startButton);
  
  _pPageButton = new QToolButton;
  _pPageButton->setIcon(QIcon(ppage_xpm));
  _pPageButton->setToolTip(QString(tr("Previous Page")));
  connect(_pPageButton, SIGNAL(clicked()), this, SLOT(pPageButtonClicked()));
  hbox->addWidget(_pPageButton);
  
  _pBarButton = new QToolButton;
  _pBarButton->setIcon(QIcon(prev_xpm));
  _pBarButton->setToolTip(QString(tr("Previous Bar")));
  connect(_pBarButton, SIGNAL(clicked()), this, SLOT(pBarButtonClicked()));
  hbox->addWidget(_pBarButton);
  
  _nBarButton = new QToolButton;
  _nBarButton->setIcon(QIcon(next_xpm));
  _nBarButton->setToolTip(QString(tr("Next Bar")));
  connect(_nBarButton, SIGNAL(clicked()), this, SLOT(nBarButtonClicked()));
  hbox->addWidget(_nBarButton);
  
  _nPageButton = new QToolButton;
  _nPageButton->setIcon(QIcon(npage_xpm));
  _nPageButton->setToolTip(QString(tr("Next Page")));
  connect(_nPageButton, SIGNAL(clicked()), this, SLOT(nPageButtonClicked()));
  hbox->addWidget(_nPageButton);
  
  _endButton = new QToolButton;
  _endButton->setIcon(QIcon(end_xpm));
  _endButton->setToolTip(QString(tr("End")));
  connect(_endButton, SIGNAL(clicked()), this, SLOT(endButtonClicked()));
  hbox->addWidget(_endButton);
  
  hbox->addStretch(1);
  
  _slider = new QSlider;
  _slider->setRange(0, 0);
  _slider->setValue(0);
  _slider->setOrientation(Qt::Horizontal);
  _slider->setToolTip(tr("Scroll Chart"));
  _slider->setSizePolicy(QSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding));
//  connect(_slider, SIGNAL(valueChanged(int)), this, SIGNAL(signalValueChanged(int)));
  connect(_slider, SIGNAL(valueChanged(int)), this, SLOT(sliderChanged(int)));
  connect(_slider, SIGNAL(sliderReleased()), this, SLOT(buttonStatus()));
  vbox->addWidget(_slider);

  buttonStatus();
}

void PlotSlider::startButtonClicked ()
{
  _slider->setValue(_slider->minimum());
  buttonStatus();
}

void PlotSlider::pPageButtonClicked ()
{
  _slider->setValue(_slider->value() - _slider->pageStep());
  buttonStatus();
}

void PlotSlider::pBarButtonClicked ()
{
  _slider->setValue(_slider->value() - 1);
  buttonStatus();
}

void PlotSlider::nBarButtonClicked ()
{
  _slider->setValue(_slider->value() + 1);
  buttonStatus();
}

void PlotSlider::nPageButtonClicked ()
{
  _slider->setValue(_slider->value() + _slider->pageStep());
  buttonStatus();
}

void PlotSlider::endButtonClicked ()
{
  _slider->setValue(_slider->maximum());
  buttonStatus();
}

void PlotSlider::setStart (int count, int width, int pixelSpace)
{
  int page = width / pixelSpace;
  int max = count - page;
  if (max < 0)
    max = 0;
  
  _slider->blockSignals(TRUE);
  _slider->setRange(0, count - 1);

  if (count < page)
    _slider->setValue(0);
  else
    _slider->setValue(max + 1);
  
  _slider->blockSignals(FALSE);

  buttonStatus();
}

int PlotSlider::getValue ()
{
  return _slider->value();
}

void PlotSlider::setValue (int d)
{
  _slider->setValue(d);
  buttonStatus();
}

void PlotSlider::sliderChanged (int d)
{
  emit signalValueChanged (d);
  buttonStatus();
}

void PlotSlider::buttonStatus ()
{
  bool left = FALSE;
  bool right = FALSE;
  
  if (_slider->value() > 0 && _slider->value() < _slider->maximum())
  {
    left = TRUE;
    right = TRUE;
  }
  else if (_slider->value() == 0 && _slider->maximum() > 1)
    right = TRUE;
  else if (_slider->value() == _slider->maximum() && _slider->value() > 1)
    left = TRUE;
  
  _startButton->setEnabled(left);
  _pPageButton->setEnabled(left);
  _pBarButton->setEnabled(left);
  _endButton->setEnabled(right);
  _nPageButton->setEnabled(right);
  _nBarButton->setEnabled(right);
}

