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

#include "FiboLine.h"
#include <qpainter.h>
#include <qcolor.h>

FiboLine::FiboLine (QString indicator, QString name, QString value, QString value2)
{
  settings.set("Type", "Fibonacci Line", Setting::None);
  settings.set(tr("Color"), "white", Setting::Color);
  settings.set("Plot", indicator, Setting::None);
  settings.set("Name", name, Setting::None);
  settings.set(tr("Start Bar"), tr("Close"), Setting::InputField);
  settings.set(tr("End Bar"), tr("Close"), Setting::InputField);
  settings.set(tr("Use Bar"), tr("False"), Setting::Bool);
  settings.set(tr("High"), value, Setting::Float);
  settings.set(tr("Low"), value2, Setting::Float);
  settings.set(tr("Support"), tr("False"), Setting::Bool);
  settings.set("0", tr("True"), Setting::Bool);
  settings.set("0.238", tr("True"), Setting::Bool);
  settings.set("0.383", tr("True"), Setting::Bool);
  settings.set("0.5", tr("True"), Setting::Bool);
  settings.set("0.618", tr("True"), Setting::Bool);
  settings.set("1", tr("False"), Setting::Bool);
  settings.set("1.618", tr("False"), Setting::Bool);
  settings.set("2.618", tr("False"), Setting::Bool);
  settings.set("4.236", tr("False"), Setting::Bool);
}

FiboLine::~FiboLine ()
{
}

void FiboLine::draw (Scaler &scaler, QPixmap &buffer, int, int)
{
  QColor color(settings.getData(tr("Color")));
  double high = settings.getFloat(tr("High"));
  double low = settings.getFloat(tr("Low"));

  bool support = FALSE;
  QString s = settings.getData(tr("Support"));
  if (! s.compare(tr("True")))
    support = TRUE;
    
  QPainter painter;
  painter.begin(&buffer);
//  painter.setFont(plotFont);
  painter.setPen(color);

  QString label;
  QString v = "0";
  s = settings.getData(v);
  if (! s.compare(tr("True")))
  {
    label = "0%";
    if (! support)
    {
      v.prepend("-");
      label.prepend("-");
    }
    double r = getY(label, high, low, v.toFloat());
    int y = scaler.convertToY(r);
    painter.drawLine (0, y, buffer.width(), y);
    QString s = label;
    s.append(" - ");
    s.append(QString::number(r));
    painter.drawText(0, y - 1, s, -1);
  }

  v = "0.238";
  s = settings.getData(v);
  if (! s.compare(tr("True")))
  {
    label = "23.8%";
    if (! support)
    {
      v.prepend("-");
      label.prepend("-");
    }
    double r = getY(label, high, low, v.toFloat());
    int y = scaler.convertToY(r);
    painter.drawLine (0, y, buffer.width(), y);
    QString s = label;
    s.append(" - ");
    s.append(QString::number(r));
    painter.drawText(0, y - 1, s, -1);
  }

  v = "0.383";
  s = settings.getData(v);
  if (! s.compare(tr("True")))
  {
    label = "38.3%";
    if (! support)
    {
      v.prepend("-");
      label.prepend("-");
    }
    double r = getY(label, high, low, v.toFloat());
    int y = scaler.convertToY(r);
    painter.drawLine (0, y, buffer.width(), y);
    QString s = label;
    s.append(" - ");
    s.append(QString::number(r));
    painter.drawText(0, y - 1, s, -1);
  }

  v = "0.5";
  s = settings.getData(v);
  if (! s.compare(tr("True")))
  {
    label = "50%";
    if (! support)
    {
      v.prepend("-");
      label.prepend("-");
    }
    double r = getY(label, high, low, v.toFloat());
    int y = scaler.convertToY(r);
    painter.drawLine (0, y, buffer.width(), y);
    QString s = label;
    s.append(" - ");
    s.append(QString::number(r));
    painter.drawText(0, y - 1, s, -1);
  }

  v = "0.618";
  s = settings.getData(v);
  if (! s.compare(tr("True")))
  {
    label = "61.8%";
    if (! support)
    {
      v.prepend("-");
      label.prepend("-");
    }
    double r = getY(label, high, low, v.toFloat());
    int y = scaler.convertToY(r);
    painter.drawLine (0, y, buffer.width(), y);
    QString s = label;
    s.append(" - ");
    s.append(QString::number(r));
    painter.drawText(0, y - 1, s, -1);
  }

  v = "1";
  s = settings.getData(v);
  if (! s.compare(tr("True")))
  {
    label = "100%";
    if (! support)
    {
      v.prepend("-");
      label.prepend("-");
    }
    double r = getY(label, high, low, v.toFloat());
    int y = scaler.convertToY(r);
    painter.drawLine (0, y, buffer.width(), y);
    QString s = label;
    s.append(" - ");
    s.append(QString::number(r));
    painter.drawText(0, y - 1, s, -1);
  }

  v = "1.618";
  s = settings.getData(v);
  if (! s.compare(tr("True")))
  {
    label = "161.8%";
    if (! support)
    {
      v.prepend("-");
      label.prepend("-");
    }
    double r = getY(label, high, low, v.toFloat());
    int y = scaler.convertToY(r);
    painter.drawLine (0, y, buffer.width(), y);
    QString s = label;
    s.append(" - ");
    s.append(QString::number(r));
    painter.drawText(0, y - 1, s, -1);
  }

  v = "2.618";
  s = settings.getData(v);
  if (! s.compare(tr("True")))
  {
    label = "261.8%";
    if (! support)
    {
      v.prepend("-");
      label.prepend("-");
    }
    double r = getY(label, high, low, v.toFloat());
    int y = scaler.convertToY(r);
    painter.drawLine (0, y, buffer.width(), y);
    QString s = label;
    s.append(" - ");
    s.append(QString::number(r));
    painter.drawText(0, y - 1, s, -1);
  }

  v = "4.236";
  s = settings.getData(v);
  if (! s.compare(tr("True")))
  {
    label = "423.6%";
    if (! support)
    {
      v.prepend("-");
      label.prepend("-");
    }
    double r = getY(label, high, low, v.toFloat());
    int y = scaler.convertToY(r);
    painter.drawLine (0, y, buffer.width(), y);
    QString s = label;
    s.append(" - ");
    s.append(QString::number(r));
    painter.drawText(0, y - 1, s, -1);
  }
  
  painter.end();
}

double FiboLine::getY (QString label, double high, double low, double v)
{
  double range = high - low;
  double r = 0;
  if (v < 0)
    r = high + (range * v);
  else
  {
    if (v > 0)
      r = low + (range * v);
    else
    {
      if (label.contains("-"))
        r = high;
      else
        r = low;
    }
  }

  return r;
}



