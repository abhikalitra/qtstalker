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

FiboLine::FiboLine (Scaler *s, QPixmap *p, QString indicator, QString n, QDateTime d, double v,
                    QDateTime d2, double v2)
{
  scaler = s;
  buffer = p;
  move2Flag = FALSE;
  type = ChartObject::FibonacciLine;
  plot = indicator;
  name = n;
  date = d;
  date2 = d2;
  value = v;
  value2 = v2;
  color.setNamedColor("white");
  line1 = 0.5;
  line2 = 0;
  line3 = 0;
  line4 = 0;
  line5 = 0;
  line6 = 0;

  menu->insertItem(tr("Edit Fibonacci Line"), this, SLOT(prefDialog()));
  menu->insertItem(tr("Delete Fibonacci Line"), this, SLOT(remove()));
}

FiboLine::~FiboLine ()
{
}

void FiboLine::draw (int x, int x2)
{
  QPainter painter;
  painter.begin(buffer);
  painter.setPen(color);

  if (line1 != 0)
  {
    double r = getY(line1, value, value2);
    int y = scaler->convertToY(r);
    painter.drawLine (x, y, x2, y);
  }

  if (line2 != 0)
  {
    double r = getY(line2, value, value2);
    int y = scaler->convertToY(r);
    painter.drawLine (x, y, x2, y);
  }

  if (line3 != 0)
  {
    double r = getY(line3, value, value2);
    int y = scaler->convertToY(r);
    painter.drawLine (x, y, x2, y);
  }

  if (line4 != 0)
  {
    double r = getY(line4, value, value2);
    int y = scaler->convertToY(r);
    painter.drawLine (x, y, x2, y);
  }

  if (line5 != 0)
  {
    double r = getY(line5, value, value2);
    int y = scaler->convertToY(r);
    painter.drawLine (x, y, x2, y);
  }

  if (line6 != 0)
  {
    double r = getY(line6, value, value2);
    int y = scaler->convertToY(r);
    painter.drawLine (x, y, x2, y);
  }

  //bottom left corner
  int y = scaler->convertToY(value2);
  QRegion r(x - 4, y - 4, 8, 8, QRegion::Rectangle);
  area = r;
  painter.drawLine (x, y, x2, y);
  
  //top right corner
  int y2 = scaler->convertToY(value);
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

double FiboLine::getY (double v, double high, double low)
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
      if (v < 0)
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
  dialog->addColorItem(tr("Color"), 1, color);

  dialog->createPage (tr("Levels"));
  dialog->addFloatItem(tr("Line 1"), 2, line1);
  dialog->addFloatItem(tr("Line 2"), 2, line2);
  dialog->addFloatItem(tr("Line 3"), 2, line3);
  dialog->addFloatItem(tr("Line 4"), 2, line4);
  dialog->addFloatItem(tr("Line 5"), 2, line5);
  dialog->addFloatItem(tr("Line 6"), 2, line6);
        
  int rc = dialog->exec();
  
  if (rc == QDialog::Accepted)
  {
    color = dialog->getColor(tr("Color"));
    line1 = dialog->getFloat(tr("Line 1"));
    line2 = dialog->getFloat(tr("Line 2"));
    line3 = dialog->getFloat(tr("Line 3"));
    line4 = dialog->getFloat(tr("Line 4"));
    line5 = dialog->getFloat(tr("Line 5"));
    line6 = dialog->getFloat(tr("Line 6"));

    saveFlag = TRUE;
    emit signalDraw();
  }
  
  delete dialog;
}

void FiboLine::move (QDateTime d, double v)
{
  if (! move2Flag)
  {
    // bottom left corner
    if (d >= date2)
      return;
    
    if (v >= value)
      return;
    
    date = d;
    value2 = v;
    saveFlag = TRUE;
    emit signalDraw();
    
    QString s = d.toString("yyyyMMdd ");
    s.append(QString::number(v));
    emit message(s);
  }
  else
  {
    //top right corner
    if (d <= date)
      return;

    if (v <= value2)
      return;
          
    date2 = d;
    value = v;
    saveFlag = TRUE;
    emit signalDraw();
    
    QString s = d.toString("yyyyMMdd ");
    s.append(QString::number(v));
    emit message(s);
  }
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

Setting * FiboLine::getSettings ()
{
  Setting *set = new Setting;
  set->set("Color", color.name(), Setting::Color);
  set->set("Plot", plot, Setting::None);
  set->set("Name", name, Setting::None);
  set->set("ObjectType", QString::number(type), Setting::None);
  set->set("High", QString::number(value), Setting::None);
  set->set("Low", QString::number(value2), Setting::None);
  set->set("Start Date", date.toString("yyyy-MM-dd00:00:00"), Setting::None);
  set->set("End Date", date2.toString("yyyy-MM-dd00:00:00"), Setting::None);
  set->set("Line 1", QString::number(line1), Setting::None);
  set->set("Line 2", QString::number(line2), Setting::None);
  set->set("Line 3", QString::number(line3), Setting::None);
  set->set("Line 4", QString::number(line4), Setting::None);
  set->set("Line 5", QString::number(line5), Setting::None);
  set->set("Line 6", QString::number(line6), Setting::None);
  return set;
}

void FiboLine::setSettings (Setting *set)
{
  color.setNamedColor(set->getData("Color"));
  plot = set->getData("Plot");
  name = set->getData("Name");
  type = (ChartObject::ObjectType) set->getInt("ObjectType");
  value = set->getFloat("High");
  value2 = set->getFloat("Low");
  date = QDateTime::fromString(set->getData("Start Date"), Qt::ISODate);
  date2 = QDateTime::fromString(set->getData("End Date"), Qt::ISODate);
  line1 = set->getFloat("Line 1");
  line2 = set->getFloat("Line 2");
  line3 = set->getFloat("Line 3");
  line4 = set->getFloat("Line 4");
  line5 = set->getFloat("Line 5");
  line6 = set->getFloat("Line 6");
}

