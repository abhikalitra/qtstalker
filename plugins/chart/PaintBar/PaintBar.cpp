/*
 *  Qtstalker stock charter
 *
 *  Copyright (C) 2001-2003 Stefan S. Stratigakos
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

#include "PaintBar.h"
#include "PrefDialog.h"
#include <qsettings.h>
#include <qpainter.h>

PaintBar::PaintBar ()
{
  pluginName = "PaintBar";
//  minPixelspace = 4;
  startX = 2;
  indicatorFlag = FALSE;

  loadSettings();  
}

PaintBar::~PaintBar ()
{
}

void PaintBar::drawChart (int startX, int startIndex, int pixelspace)
{
  QPainter painter;
  painter.begin(buffer);

  int x = startX;
  int loop = startIndex;
  
  while ((x < buffer->width()) && (loop < (int) data->count()))
  {
//    QColor *color = paintBars.at(loop);
//    painter.setPen(QColor(color->red(), color->green(), color->blue()));

    int y;
    if (data->getOpen(loop) != 0)
    {
      y = scaler->convertToY(data->getOpen(loop));
      painter.drawLine (x - 2, y, x, y);
    }

    y = scaler->convertToY(data->getClose(loop));
    painter.drawLine (x + 2, y, x, y);

    int h = scaler->convertToY(data->getHigh(loop));
    int l = scaler->convertToY(data->getLow(loop));
    painter.drawLine (x, h, x, l);

    x = x + pixelspace;
    loop++;
  }

  painter.end();

}

void PaintBar::prefDialog ()
{
//  set->set(tr("Paint Bar Indicator"), config->getData(Config::PaintBarIndicator), Setting::List);
//  set->setList(tr("Paint Bar Indicator"), config->getIndicators());

  PrefDialog *dialog = new PrefDialog();
  dialog->setCaption(tr("PaintBar Chart Prefs"));
  dialog->createPage (tr("Colors"));
  dialog->addColorItem(tr("Up Color"), 1, upColor);
  dialog->addColorItem(tr("Down Color"), 1, downColor);
  dialog->addIntItem(tr("Min Bar Spacing"), 1, minPixelspace, 4, 99);
  int rc = dialog->exec();
  
  if (rc == QDialog::Accepted)
  {
    upColor = dialog->getColor(tr("Up Color"));
    downColor = dialog->getColor(tr("Down Color"));
    minPixelspace = dialog->getInt(tr("Min Bar Spacing"));
    
    saveFlag = TRUE;
//    config->setData(Config::PaintBarIndicator, set->getData(tr("Paint Bar Indicator")));
    emit draw();
  }
  
  delete dialog;
}

void PaintBar::loadSettings ()
{
  QSettings settings;
  settings.beginGroup("/Qtstalker/PaintBar plugin");

  QString s = settings.readEntry("/UpColor", "green");
  upColor.setNamedColor(s);
  
  s = settings.readEntry("/DownColor", "red");
  downColor.setNamedColor(s);
  
  minPixelspace = settings.readNumEntry("/minPixelspace", 4);
  
  settings.endGroup();
}

void PaintBar::saveSettings ()
{
  if (! saveFlag)
    return;

  QSettings settings;
  settings.beginGroup("/Qtstalker/PaintBar plugin");
  
  settings.writeEntry("/UpColor", upColor.name());
  settings.writeEntry("/DownColor", downColor.name());
  settings.writeEntry("/minPixelspace", minPixelspace);
  
  settings.endGroup();
}

//*************************************************
//*************************************************

Plugin * create ()
{
  PaintBar *o = new PaintBar;
  return ((Plugin *) o);
}
