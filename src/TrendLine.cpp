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

#include "TrendLine.h"
#include "PrefDialog.h"
#include <qpainter.h>
#include <qcolor.h>

TrendLine::TrendLine (Scaler *s, QPixmap *p, BarData *d, QString indicator, QString name,
                      QString date, QString value, QString date2, QString value2)
{
  data = d;
  scaler = s;
  buffer = p;
  move2Flag = FALSE;
  
  settings.set("Type", "Trend Line", Setting::None);
  settings.set(tr("Color"), "white", Setting::Color);
  settings.set("Plot", indicator, Setting::None);
  settings.set("Name", name, Setting::None);
  settings.set(tr("Bar Field"), tr("Close"), Setting::InputField);
  settings.set(tr("Use Bar"), tr("False"), Setting::Bool);
  settings.set("Start Date", date, Setting::None);
  settings.set("Start Value", value, Setting::None);
  settings.set("End Date", date2, Setting::None);
  settings.set("End Value", value2, Setting::None);
  settings.set("ObjectType", QString::number(ChartObject::TrendLine), Setting::None);
}

TrendLine::~TrendLine ()
{
}

void TrendLine::draw (int x, int x2)
{
  QPainter painter;
  painter.begin(buffer);

  int y;
  int y2;
  if (! settings.getData(tr("Use Bar")).compare(tr("True")))
  {
    QString s = settings.getData(tr("Bar Field"));
    QDateTime dt = QDateTime::fromString(settings.getDateTime("Start Date"), Qt::ISODate);
    int i = data->getX(dt);
    dt = QDateTime::fromString(settings.getDateTime("End Date"), Qt::ISODate);
    int i2 = data->getX(dt);

    while (1)
    {
      if (! s.compare(tr("Open")))
      {
        y = scaler->convertToY(data->getOpen(i));
        y2 = scaler->convertToY(data->getOpen(i2));
	break;
      }

      if (! s.compare(tr("High")))
      {
        y = scaler->convertToY(data->getHigh(i));
        y2 = scaler->convertToY(data->getHigh(i2));
	break;
      }

      if (! s.compare(tr("Low")))
      {
        y = scaler->convertToY(data->getLow(i));
        y2 = scaler->convertToY(data->getLow(i2));
	break;
      }

      if (! s.compare(tr("Close")))
      {
        y = scaler->convertToY(data->getClose(i));
        y2 = scaler->convertToY(data->getClose(i2));
	break;
      }

      return;
    }
  }
  else
  {
    y = scaler->convertToY(settings.getFloat("Start Value"));
    y2 = scaler->convertToY(settings.getFloat("End Value"));
  }

  QColor color(settings.getData(tr("Color")));
  painter.setPen(color);

  painter.drawLine (x, y, x2, y2);
  
  QRegion r(x - 4, y - 4, 8, 8, QRegion::Rectangle);
  area = r;
  QRegion r2(x2 - 4, y2 - 4, 8, 8, QRegion::Rectangle);
  area2 = r2;

  // save old values;
  int tx2 = x2;
  int ty2 = y2;
  int tx = x;
  int ty = y;
  
  int ydiff = y - y2;
  int xdiff = x2 - x;
  while (x2 < buffer->width())
  {
    x = x2;
    y = y2;
    x2 = x2 + xdiff;
    y2 = y2 - ydiff;
    painter.drawLine (x, y, x2, y2);
  }

  if (status)
  {
    painter.fillRect(tx - 3, ty - 3, 6, 6, color);
    painter.fillRect(tx2 - 3, ty2 - 3, 6, 6, color);
  }
  
  painter.end();
}

QString TrendLine::getDate ()
{
  return settings.getDateTime("Start Date");
}

QString TrendLine::getDate2 ()
{
  return settings.getDateTime("End Date");
}

void TrendLine::prefDialog ()
{
  QStringList l;
  l.append(tr("Open"));
  l.append(tr("High"));
  l.append(tr("Low"));
  l.append(tr("Close"));

  PrefDialog *dialog = new PrefDialog();
  dialog->setCaption(tr("Edit Trend Line"));
  dialog->createPage (tr("Details"));
  dialog->addColorItem(tr("Color"), 1, QColor(settings.getData(tr("Color"))));
  dialog->addComboItem(tr("Bar Field"), 1, l, settings.getData(tr("Bar Field")));
  dialog->addCheckItem(tr("Use Bar"), 1, settings.getData(tr("Use Bar")));
  
  int rc = dialog->exec();
  
  if (rc == QDialog::Accepted)
  {
    QColor color = dialog->getColor(tr("Color"));
    settings.setData(tr("Color"), color.name());
    
    settings.setData(tr("Use Bar"), dialog->getCheckString(tr("Use Bar")));
    settings.setData(tr("Bar Field"), dialog->getCombo(tr("Bar Field")));
    
    saveFlag = TRUE;
    emit signalDraw();
  }
  
  delete dialog;
}

void TrendLine::move (QString d, QString v)
{
  if (! move2Flag)
  {
    if (d.toFloat() >= settings.getFloat("End Date"))
      return;
    
    settings.setData("Start Date", d);
    settings.setData("Start Value", v);
    saveFlag = TRUE;
    emit signalDraw();
  }
  else
  {
    if (d.toFloat() <= settings.getFloat("Start Date"))
      return;
    
    settings.setData("End Date", d);
    settings.setData("End Value", v);
    saveFlag = TRUE;
    emit signalDraw();
  }
}

bool TrendLine::isClicked (int x, int y)
{
  bool flag = FALSE;
  move2Flag = FALSE;
  QPoint p(x,y);
  
  if (area.contains(p))
    flag = TRUE;
  else
  {
    if (area2.contains(p))
      flag = TRUE;
    move2Flag = TRUE;
  }
  
  return flag;
}


