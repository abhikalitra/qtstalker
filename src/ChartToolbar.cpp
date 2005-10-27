/*
 *  Qtstalker stock charter
 * 
 *  Copyright (C) 2001-2005 Stefan S. Stratigakos
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
#include <qaccel.h>
#include <qvalidator.h>

ChartToolbar::ChartToolbar (QMainWindow *mw) : QToolBar (mw, "chartToolbar")
{
  keyFlag = FALSE;
  macroFlag = FALSE;
  macro = 0;
  Config config;
  
  BarData *bd = new BarData;
  compressionCombo = new MyComboBox(this, Macro::ChartToolbar);
  compressionCombo->show();
  QStringList l;
  bd->getBarCompressionList(l);
  compressionCombo->insertStringList(l, -1);
  compressionCombo->setCurrentItem((BarData::BarCompression) config.getData(Config::Compression).toInt());
  QToolTip::add(compressionCombo, tr("Chart Compression"));
  connect(compressionCombo, SIGNAL(activated(int)), this, SIGNAL(signalCompressionChanged(int)));
  connect(compressionCombo, SIGNAL(signalKeyPressed(int, int, int, int, QString)),
          this, SIGNAL(signalKeyPressed(int, int, int, int, QString)));
  delete bd;

  pixelspace = new MySpinBox(this, Macro::ChartToolbar);
  pixelspace->setRange(4, 99);
  connect (pixelspace, SIGNAL(valueChanged(int)), this, SIGNAL(signalPixelspaceChanged(int)));
  QToolTip::add(pixelspace, tr("Bar Spacing"));
  connect(pixelspace, SIGNAL(signalKeyPressed(int, int, int, int, QString)),
          this, SIGNAL(signalKeyPressed(int, int, int, int, QString)));

  QIntValidator *iv = new QIntValidator(1, 99999, this, 0);

  barCount = new MyLineEdit(this, Macro::ChartToolbar);
  barCount->setValidator(iv);
  barCount->setText(config.getData(Config::Bars));
  barCount->setMaximumWidth(50);
  QToolTip::add(barCount, tr("Total bars to load"));
  connect(barCount, SIGNAL(signalKeyPressed(int, int, int, int, QString)),
          this, SIGNAL(signalKeyPressed(int, int, int, int, QString)));
  connect(barCount, SIGNAL(returnPressed()), this, SLOT(barsChanged()));

  addSeparator();

  slider = new MySlider(this, Macro::ChartToolbar);
  slider->setOrientation(Qt::Horizontal);
  connect (slider, SIGNAL(valueChanged(int)), this, SIGNAL(signalSliderChanged(int)));
  slider->setEnabled(FALSE);
  QToolTip::add(slider, tr("Pan Chart"));
  connect(slider, SIGNAL(signalKeyPressed(int, int, int, int, QString)),
          this, SIGNAL(signalKeyPressed(int, int, int, int, QString)));

  setStretchableWidget(slider);
  
  QAccel *a = new QAccel(mw);
  connect(a, SIGNAL(activated(int)), this, SLOT(slotAccel(int)));
  a->insertItem(CTRL+Key_End, ChartPannerFocus);
  a->insertItem(CTRL+Key_Plus, BarsLoadedFocus);
  a->insertItem(CTRL+Key_Minus, BarSpacingFocus);
  a->insertItem(CTRL+Key_Prior, CompressionFocus);
  a->insertItem(CTRL+Key_B, ToolbarFocus);
  
  focusFlag = CompressionFocus;
}

ChartToolbar::~ChartToolbar ()
{
}

int ChartToolbar::getBars ()
{
  bool ok;
  int t = barCount->text().toInt(&ok);
  if (ok)
    return t;
  else
    return 0;
}

void ChartToolbar::enableSlider (bool d)
{
  slider->setEnabled(d);
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

int ChartToolbar::getSlider ()
{
  return slider->value();
}

int ChartToolbar::setSliderStart (int width, int records)
{
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
    slider->setValue(max + 1);
    rc = max + 1;
  }
  
  slider->blockSignals(FALSE);
  
  return rc;
}

void ChartToolbar::saveSettings ()
{
  Config config;
  QString s = QString::number(getBars());
  config.setData(Config::Bars, s);
  
  s = QString::number(getCompressionInt());
  config.setData(Config::Compression, s);
}

void ChartToolbar::setFocus ()
{
  compressionCombo->setFocus();
  focusFlag = CompressionFocus;
}

void ChartToolbar::setKeyFlag (bool d)
{
  keyFlag = d;
  compressionCombo->setKeyFlag(d);
  pixelspace->setKeyFlag(d);
  barCount->setKeyFlag(d);
  slider->setKeyFlag(d);
}

void ChartToolbar::slotAccel (int id)
{
  switch (id)
  {
    case CompressionFocus:
      compressionCombo->setFocus();
      focusFlag = CompressionFocus;
      if (keyFlag)
        emit signalKeyPressed (Macro::ChartToolbar, ControlButton, Key_Prior, 0, QString());
      break;  
    case ChartPannerFocus:
      slider->setFocus();
      focusFlag = ChartPannerFocus;
      if (keyFlag)
        emit signalKeyPressed (Macro::ChartToolbar, ControlButton, Key_End, 0, QString());
      break;  
    case BarsLoadedFocus:
      barCount->setFocus();
      focusFlag = BarsLoadedFocus;
      if (keyFlag)
        emit signalKeyPressed (Macro::ChartToolbar, ControlButton, Key_Plus, 0, QString());
      break;  
    case BarSpacingFocus:
      pixelspace->setFocus();
      focusFlag = BarSpacingFocus;
      if (keyFlag)
        emit signalKeyPressed (Macro::ChartToolbar, ControlButton, Key_Minus, 0, QString());
      break;  
    case ToolbarFocus:
      if (keyFlag)
        emit signalKeyPressed (Macro::ChartToolbar, ControlButton, Key_B, 0, QString());
      setFocus();
      break;
    default:
      break;
  }
}

void ChartToolbar::doKeyPress (QKeyEvent *key)
{
  key->accept();
  
  if (! key->state())
  {
    switch(focusFlag)
    {
      case CompressionFocus:
        compressionCombo->doKeyPress(key);
	break;
      case BarSpacingFocus:
        pixelspace->doKeyPress(key);
	break;
      case BarsLoadedFocus:
        barCount->doKeyPress(key);
	break;
      case ChartPannerFocus:
        slider->doKeyPress(key);
	break;
      default:
        break;
    }
  }
  else
  {
    if (key->state() == Qt::ControlButton)
    {
      switch (key->key())
      {
        case Qt::Key_Prior:
	  slotAccel(CompressionFocus);
          break;
        case Qt::Key_Plus:
	  slotAccel(BarsLoadedFocus);
          break;
        case Qt::Key_End:
	  slotAccel(ChartPannerFocus);
          break;
        case Qt::Key_Minus:
	  slotAccel(BarSpacingFocus);
          break;
        case Qt::Key_B:
	  slotAccel(ToolbarFocus);
          break;
        default:
          break;
      }
    }
  }
}

void ChartToolbar::runMacro (Macro *d)
{
  macro = d;
  macroFlag = TRUE;
  
  while (macro->getZone(macro->getIndex()) == Macro::ChartToolbar)
  {
    doKeyPress(macro->getKey(macro->getIndex()));
    
    macro->incIndex();
    if (macro->getIndex() >= macro->getCount())
      break;
  }
  
  macroFlag = FALSE;
}

void ChartToolbar::barsChanged ()
{
  bool ok;
  int t = barCount->text().toInt(&ok);
  if (ok)
    emit signalBarsChanged(t);
}

