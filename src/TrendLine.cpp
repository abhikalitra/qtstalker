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

TrendLine::TrendLine (Scaler *s, QPixmap *p, BarData *d, QString indicator, QString n,
                      QDateTime dt, double v, QDateTime dt2, double v2)
{
  data = d;
  scaler = s;
  buffer = p;
  move2Flag = FALSE;
  type = ChartObject::TrendLine;
  plot = indicator;
  name = n;
  date = dt;
  date2 = dt2;
  value = v;
  value2 = v2;
  color.setNamedColor("white");
  useBar = FALSE;
  barField = tr("Close");
  
  menu->insertItem(tr("Edit Trend Line"), this, SLOT(prefDialog()));
  menu->insertItem(tr("Delete Trend Line"), this, SLOT(remove()));
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
  if (useBar)
  {
    int i = data->getX(date);
    int i2 = data->getX(date2);

    while (1)
    {
      if (! barField.compare(tr("Open")))
      {
        y = scaler->convertToY(data->getOpen(i));
        y2 = scaler->convertToY(data->getOpen(i2));
	break;
      }

      if (! barField.compare(tr("High")))
      {
        y = scaler->convertToY(data->getHigh(i));
        y2 = scaler->convertToY(data->getHigh(i2));
	break;
      }

      if (! barField.compare(tr("Low")))
      {
        y = scaler->convertToY(data->getLow(i));
        y2 = scaler->convertToY(data->getLow(i2));
	break;
      }

      if (! barField.compare(tr("Close")))
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
    y = scaler->convertToY(value);
    y2 = scaler->convertToY(value2);
  }

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
  dialog->addColorItem(tr("Color"), 1, color);
  dialog->addComboItem(tr("Bar Field"), 1, l, barField);
  dialog->addCheckItem(tr("Use Bar"), 1, useBar);
  
  int rc = dialog->exec();
  
  if (rc == QDialog::Accepted)
  {
    color = dialog->getColor(tr("Color"));
    useBar = dialog->getCheckString(tr("Use Bar"));
    barField = dialog->getCombo(tr("Bar Field"));
    saveFlag = TRUE;
    emit signalDraw();
  }
  
  delete dialog;
}

void TrendLine::move (QDateTime d, double v)
{
  if (! move2Flag)
  {
    if (d >= date2)
      return;
    
    date = d;
    value = v;
    saveFlag = TRUE;
    emit signalDraw();
    
    QString s = d.toString("yyyyMMdd ");
    s.append(QString::number(v));
    emit message(s);
  }
  else
  {
    if (d <= date)
      return;
    
    date2 = d;
    value2 = v;
    saveFlag = TRUE;
    emit signalDraw();
    
    QString s = d.toString("yyyyMMdd ");
    s.append(QString::number(v));
    emit message(s);
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

Setting * TrendLine::getSettings ()
{
  Setting *set = new Setting;
  set->set("Start Date", date.toString("yyyy-MM-dd00:00:00"), Setting::None);
  set->set("End Date", date2.toString("yyyy-MM-dd00:00:00"), Setting::None);
  set->set("Start Value", QString::number(value), Setting::None);
  set->set("End Value", QString::number(value2), Setting::None);
  set->set("Bar Field", barField, Setting::InputField);
  set->set("Use Bar", QString::number(useBar), Setting::Bool);
  set->set("Color", color.name(), Setting::Color);
  set->set("Plot", plot, Setting::None);
  set->set("Name", name, Setting::None);
  set->set("ObjectType", QString::number(type), Setting::None);
  return set;
}

void TrendLine::setSettings (Setting *set)
{
  date = QDateTime::fromString(set->getData("Start Date"), Qt::ISODate);
  date2 = QDateTime::fromString(set->getData("End Date"), Qt::ISODate);
  value = set->getFloat("Start Value");
  value2 = set->getFloat("End Value");
  barField = set->getData("Bar Field");
  useBar = set->getInt("Use bar");
  color.setNamedColor(set->getData("Color"));
  plot = set->getData("Plot");
  name = set->getData("Name");
  type = (ChartObject::ObjectType) set->getInt("ObjectType");
}

