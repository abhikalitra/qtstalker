/*
 *  Qtstalker stock charter
 * 
 *  Copyright (C) 2001-2004 Stefan S. Stratigakos
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

#include "ChartToolbar.h"
#include "Config.h"
#include "BarData.h"
#include <qtooltip.h>

ChartToolbar::ChartToolbar (QMainWindow *mw) : QToolBar (mw, "chartToolbar")
{
  keyFlag = FALSE;
  Config config;
  
  compressionCombo = new QComboBox(this);
  BarData *bd = new BarData;
  compressionCombo->show();
  compressionCombo->insertStringList(bd->getBarCompressionList(), -1);
  compressionCombo->setCurrentItem((BarData::BarCompression) config.getData(Config::Compression).toInt());
  QToolTip::add(compressionCombo, tr("Chart Compression"));
  connect(compressionCombo, SIGNAL(activated(int)), this, SIGNAL(signalCompressionChanged(int)));
  delete bd;

  chartTypeCombo = new QComboBox(this);
  chartTypeCombo->show();
  chartTypeCombo->insertStringList(config.getPluginList(Config::ChartPluginPath), -1);
  QToolTip::add(chartTypeCombo, tr("Chart Type"));
  chartTypeCombo->setCurrentText(config.getData(Config::ChartStyle));
  connect(chartTypeCombo, SIGNAL(activated(int)), this, SIGNAL(signalChartTypeChanged(int)));

  pixelspace = new QSpinBox(this);
  connect (pixelspace, SIGNAL(valueChanged(int)), this, SIGNAL(signalPixelspaceChanged(int)));
  QToolTip::add(pixelspace, tr("Bar Spacing"));

  barCount = new QSpinBox(1, 99999999, 1, this);
  barCount->setValue(config.getData(Config::Bars).toInt());  
  QToolTip::add(barCount, tr("Total bars to load"));

  addSeparator();

  slider = new QSlider(this);
  slider->setOrientation(Qt::Horizontal);
  connect (slider, SIGNAL(valueChanged(int)), this, SIGNAL(signalSliderChanged(int)));
  slider->setEnabled(FALSE);
  QToolTip::add(slider, tr("Pan Chart"));

  setStretchableWidget(slider);
}

ChartToolbar::~ChartToolbar ()
{
}

int ChartToolbar::getBars ()
{
  return barCount->value();
}

void ChartToolbar::enableSlider (bool d)
{
  slider->setEnabled(d);
}

void ChartToolbar::setMinPixelspace (int d)
{
  pixelspace->blockSignals(TRUE);
  pixelspace->setRange(d, 99);
  pixelspace->blockSignals(FALSE);
}

void ChartToolbar::setPixelspace (int min, int d)
{
  pixelspace->blockSignals(TRUE);
  pixelspace->setRange(min, 99);
  pixelspace->setValue(d);
  pixelspace->blockSignals(FALSE);
}

int ChartToolbar::getPixelspace ()
{
  return pixelspace->value();
}

int ChartToolbar::getCompressionInt ()
{
  return compressionCombo->currentItem();
}

QString ChartToolbar::getCompression ()
{
  return compressionCombo->currentText();
}

QString ChartToolbar::getChartType ()
{
  return chartTypeCombo->currentText();
}

int ChartToolbar::getSlider ()
{
  return slider->value();
}

int ChartToolbar::setSliderStart (int ov, bool flag, int width, int records)
{
  int oldpage = width / ov;
  int sv = slider->value();
  int page = width / getPixelspace();
  int max = records - page;
  if (max < 0)
    max = 0;
  slider->blockSignals(TRUE);
  slider->setRange(0, records - 1);
  int rc = 0;
  
  if ((int) records < page)
    slider->setValue(0);
  else
  {
    if (flag)
    {
      slider->setValue(max + 1);
      rc = max + 1;
    }
    else
    {
      if (oldpage < page)
      {
        int x = ((page - oldpage) / 2);
	if ((sv - x) < 0)
	  x = 0;
        slider->setValue(sv - x);
	rc = sv - x;
      }
  
      if (oldpage > page)
      {
        int x = ((oldpage - page) / 2);
        slider->setValue(sv + x);
	rc = sv + x;
      }
    }
  }
  
  slider->blockSignals(FALSE);
  
  return rc;
}

void ChartToolbar::saveSettings ()
{
  Config config;
  
  config.setData(Config::Bars, QString::number(getBars()));
}

void ChartToolbar::setFocus ()
{
  compressionCombo->setFocus();
}

void ChartToolbar::setKeyFlag (bool d)
{
  keyFlag = d;
}


// old key stuff needs to be done
/*
  switch (key->key())
  {
    case Qt::Key_Left:
      slider->setValue(slider->value() - 1);
      break;
    case Qt::Key_Right:
      slider->setValue(slider->value() + 1);
      break;
    case Qt::Key_Home:
      slider->setValue(0);
      break;
    case Qt::Key_End:
      slider->setValue(slider->maxValue());
      break;
    case Qt::Key_Prior:
      slider->addStep();
      break;
    case Qt::Key_Next:
      slider->subtractStep();
      break;
    case Qt::Key_Minus:
      pixelspace->stepDown();
      break;
    case Qt::Key_Plus:
      pixelspace->stepUp();
      break;
    case Qt::Key_Up:
      if (compressionCombo->currentItem() != (compressionCombo->count() - 1))
      {
        compressionCombo->setCurrentItem(compressionCombo->currentItem() + 1);
        slotCompressionChanged(compressionCombo->currentItem());
      }
      break;
    case Qt::Key_Down:
      if (compressionCombo->currentItem() != 0)
      {
        compressionCombo->setCurrentItem(compressionCombo->currentItem() - 1);
        slotCompressionChanged(compressionCombo->currentItem());
      }
      break;
    default:
      break;
  }
*/

