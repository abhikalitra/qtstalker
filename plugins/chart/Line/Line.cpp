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

#include "Line.h"
#include "PrefDialog.h"
#include <qpainter.h>
#include <qsettings.h>

Line::Line ()
{
  pluginName = "Line";
  startX = 0;
  indicatorFlag = FALSE;
  type = Close;

  loadSettings();  
}

Line::~Line ()
{
}

void Line::drawChart (int startX, int startIndex, int pixelspace)
{
  QPainter painter;
  painter.begin(buffer);

  int x = -1;
  int x2 = startX;
  int y = -1;
  int y2 = -1;
  int loop = startIndex;

  painter.setPen(color);

  while ((x < buffer->width()) && (loop < (int) data->count()))
  {
    switch(type)
    {
      case Open:
        y2 = scaler->convertToY(data->getOpen(loop));
        break;
      case High:
        y2 = scaler->convertToY(data->getHigh(loop));
        break;
      case Low:
        y2 = scaler->convertToY(data->getLow(loop));
        break;
      case Tp:
        y2 = scaler->convertToY(((data->getHigh(loop) + data->getLow(loop) + data->getClose(loop)) / 3));
        break;
      case Ap:
        y2 = scaler->convertToY(((data->getOpen(loop) + data->getHigh(loop) + data->getLow(loop) + data->getClose(loop)) / 4));
        break;
      case Wp:
        y2 = scaler->convertToY(((data->getHigh(loop) + data->getLow(loop) + data->getClose(loop) + data->getClose(loop)) / 4));
        break;
      case Hl:
        y2 = scaler->convertToY(((data->getHigh(loop) + data->getLow(loop)) / 2));
        break;
      case Oc:
        y2 = scaler->convertToY(((data->getOpen(loop) + data->getClose(loop)) / 2));
        break;
      default:
        y2 = scaler->convertToY(data->getClose(loop));
        break;
    }  
  
    if (y != -1)
      painter.drawLine (x, y, x2, y2);
    x = x2;
    y = y2;

    x2 = x2 + pixelspace;
    loop++;
  }

  painter.end();
}

void Line::prefDialog ()
{
  QStringList l;
  l.append(tr("Open"));
  l.append(tr("High"));
  l.append(tr("Low"));
  l.append(tr("Close"));
  l.append(tr("Typical Price"));
  l.append(tr("Average Price"));
  l.append(tr("Weighted Price"));
  l.append(tr("(H+L) /2"));
  l.append(tr("(O+C) /2"));

  PrefDialog *dialog = new PrefDialog();
  dialog->setCaption(tr("Line Chart Prefs"));
  dialog->createPage (tr("Colors"));
  dialog->addColorItem(tr("Line Color"), 1, color);
  dialog->addComboItem(tr("Line Type"), 1, l, l[(int) type]);
  dialog->addIntItem(tr("Min Bar Spacing"), 1, minPixelspace, 1, 99);
  int rc = dialog->exec();
  
  if (rc == QDialog::Accepted)
  {
    color = dialog->getColor(tr("Line Color"));
    type = (LineType) l.findIndex(dialog->getCombo(tr("Line Type")));
    minPixelspace = dialog->getInt(tr("Min Bar Spacing"));
    
    saveFlag = TRUE;
    emit draw();
  }
  
  delete dialog;
}

void Line::loadSettings ()
{
  QSettings settings;
  settings.beginGroup("/Qtstalker/Line plugin");

  QString s = settings.readEntry("/Color", "green");
  color.setNamedColor(s);
  
  s = settings.readEntry("/Type", QString::number(type));
  type = (LineType) s.toInt();
  
  minPixelspace = settings.readNumEntry("/minPixelspace", 3);
  
  settings.endGroup();
}

void Line::saveSettings ()
{
  if (! saveFlag)
    return;

  QSettings settings;
  settings.beginGroup("/Qtstalker/Line plugin");
  
  settings.writeEntry("/Color", color.name());
  settings.writeEntry("/Type", QString::number(type));
  settings.writeEntry("/minPixelspace", minPixelspace);
  
  settings.endGroup();
}

//*************************************************
//*************************************************

Plugin * create ()
{
  Line *o = new Line;
  return ((Plugin *) o);
}

