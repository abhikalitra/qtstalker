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
#include "PrefDialog.h"
#include <qpainter.h>
#include <qcolor.h>

FiboLine::FiboLine (Scaler *s, QPixmap *p, QString indicator, QString name, QString date, QString value,
                    QString date2, QString value2)
{
  scaler = s;
  buffer = p;
  move2Flag = FALSE;
  
  settings.set("Type", "Fibonacci Line", Setting::None);
  settings.set(tr("Color"), "white", Setting::Color);
  settings.set("Plot", indicator, Setting::None);
  settings.set("Name", name, Setting::None);
  settings.set("High", value, Setting::None);
  settings.set("Start Date", date, Setting::None);
  settings.set("Low", value2, Setting::None);
  settings.set("End Date", date2, Setting::None);
  settings.set("Line 1", "0", Setting::None);
  settings.set("Line 2", "0.238", Setting::None);
  settings.set("Line 3", "0.383", Setting::None);
  settings.set("Line 4", "0.5", Setting::None);
  settings.set("Line 5", "0.618", Setting::None);
  settings.set("Line 6", "1", Setting::None);
  settings.set("ObjectType", QString::number(ChartObject::FibonacciLine), Setting::None);
}

FiboLine::~FiboLine ()
{
}

void FiboLine::draw (int x, int x2)
{
  QColor color(settings.getData(tr("Color")));
  double high = settings.getFloat("High");
  double low = settings.getFloat("Low");

  QPainter painter;
  painter.begin(buffer);
//  painter.setFont(plotFont);
  painter.setPen(color);

  QString v = settings.getData(tr("Line 1"));
  if (v.length())
  {
    double r = getY(v, high, low, v.toFloat());
    int y = scaler->convertToY(r);
    painter.drawLine (x, y, x2, y);
  }

  v = settings.getData(tr("Line 2"));
  if (v.length())
  {
    double r = getY(v, high, low, v.toFloat());
    int y = scaler->convertToY(r);
    painter.drawLine (x, y, x2, y);
  }

  v = settings.getData(tr("Line 3"));
  if (v.length())
  {
    double r = getY(v, high, low, v.toFloat());
    int y = scaler->convertToY(r);
    painter.drawLine (x, y, x2, y);
  }

  v = settings.getData(tr("Line 4"));
  if (v.length())
  {
    double r = getY(v, high, low, v.toFloat());
    int y = scaler->convertToY(r);
    painter.drawLine (x, y, x2, y);
  }

  v = settings.getData(tr("Line 5"));
  if (v.length())
  {
    double r = getY(v, high, low, v.toFloat());
    int y = scaler->convertToY(r);
    painter.drawLine (x, y, x2, y);
  }

  v = settings.getData(tr("Line 6"));
  if (v.length())
  {
    double r = getY(v, high, low, v.toFloat());
    int y = scaler->convertToY(r);
    painter.drawLine (x, y, x2, y);
  }

  //bottom left corner
  int y = scaler->convertToY(low);
  QRegion r(x - 4, y - 4, 8, 8, QRegion::Rectangle);
  area = r;
  painter.drawLine (x, y, x2, y);
  
  //top right corner
  int y2 = scaler->convertToY(high);
  QRegion r2(x2 - 4, y2 - 4, 8, 8, QRegion::Rectangle);
  area2 = r2;
  painter.drawLine (x, y2, x2, y2);

  if (status)
  {
    //bottom left corner
    painter.fillRect(x - 3, y - 3, 6, 6, color);
    
    //top right corner
    painter.fillRect(x2 - 3, y2 - 3, 6, 6, color);
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

void FiboLine::prefDialog ()
{
  QStringList l;
  l.append(tr("Open"));
  l.append(tr("High"));
  l.append(tr("Low"));
  l.append(tr("Close"));

  PrefDialog *dialog = new PrefDialog();
  dialog->setCaption(tr("Edit Fibonacci Lines"));
  dialog->createPage (tr("Details"));
  dialog->addColorItem(tr("Color"), 1, QColor(settings.getData(tr("Color"))));

  dialog->createPage (tr("Levels"));
  dialog->addFloatItem(tr("Line 1"), 2, settings.getFloat(tr("Line 1")));
  dialog->addFloatItem(tr("Line 2"), 2, settings.getFloat(tr("Line 2")));
  dialog->addFloatItem(tr("Line 3"), 2, settings.getFloat(tr("Line 3")));
  dialog->addFloatItem(tr("Line 4"), 2, settings.getFloat(tr("Line 4")));
  dialog->addFloatItem(tr("Line 5"), 2, settings.getFloat(tr("Line 5")));
  dialog->addFloatItem(tr("Line 6"), 2, settings.getFloat(tr("Line 6")));
        
  int rc = dialog->exec();
  
  if (rc == QDialog::Accepted)
  {
    QColor color = dialog->getColor(tr("Color"));
    settings.setData(tr("Color"), color.name());
    
    settings.setData(tr("Line 1"), QString::number(dialog->getFloat(tr("Line 1"))));
    settings.setData(tr("Line 2"), QString::number(dialog->getFloat(tr("Line 2"))));
    settings.setData(tr("Line 3"), QString::number(dialog->getFloat(tr("Line 3"))));
    settings.setData(tr("Line 4"), QString::number(dialog->getFloat(tr("Line 4"))));
    settings.setData(tr("Line 5"), QString::number(dialog->getFloat(tr("Line 5"))));
    settings.setData(tr("Line 6"), QString::number(dialog->getFloat(tr("Line 6"))));

    saveFlag = TRUE;
    emit signalDraw();
  }
  
  delete dialog;
}

void FiboLine::move (QString d, QString v)
{
  if (! move2Flag)
  {
    // bottom left corner
    if (d.toFloat() >= settings.getFloat("End Date"))
      return;
    
    if (v.toFloat() >= settings.getFloat("High"))
      return;
    
    settings.setData("Start Date", d);
    settings.setData("Low", v);
    saveFlag = TRUE;
    emit signalDraw();
  }
  else
  {
    //top right corner
    if (d.toFloat() <= settings.getFloat("Start Date"))
      return;

    if (v.toFloat() <= settings.getFloat("Low"))
      return;
          
    settings.setData("End Date", d);
    settings.setData("High", v);
    saveFlag = TRUE;
    emit signalDraw();
  }
}

QString FiboLine::getDate ()
{
  return settings.getDateTime("Start Date");
}

QString FiboLine::getDate2 ()
{
  return settings.getDateTime("End Date");
}


bool FiboLine::isClicked (int x, int y)
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
