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
#include <qpointarray.h>

TrendLine::TrendLine (Scaler *s, QPixmap *p, BarData *d, QString indicator, QString n,
                      BarDate dt, double v, BarDate dt2, double v2)
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
  useBar = FALSE;
  barField = tr("Close");
  extend = TRUE;
  
  menu->insertItem(tr("&Edit Trend Line"), this, SLOT(prefDialog()), tr("Ctrl+E"));
  menu->insertItem(tr("&Move Trend Line"), this, SLOT(moveObject()), tr("Ctrl+M"));
  menu->insertItem(tr("&Delete Trend Line"), this, SLOT(remove()), tr("Ctrl+D"));
  
  loadDefaults("TrendLine");
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

  // save old values;
  int tx2 = x2;
  int ty2 = y2;
  int tx = x;
  int ty = y;

  if (extend)
  {  
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
  }

  // store the selectable area the line occupies
  selectionArea.clear();
  QPointArray array;
  array.putPoints(0, 4, tx, ty - 4, tx, ty + 4, x2, y2 + 4, x2, y2 - 4);
  selectionArea.append(new QRegion(array));
  
  if (status)
  {
    grabHandles.clear();

    grabHandles.append(new QRegion(tx,
             			   ty - (HANDLE_WIDTH / 2),
				   HANDLE_WIDTH,
				   HANDLE_WIDTH,
				   QRegion::Rectangle));
    painter.fillRect(tx, ty - (HANDLE_WIDTH / 2), HANDLE_WIDTH, HANDLE_WIDTH, color);
      
    grabHandles.append(new QRegion(tx2,
             			   ty2 - (HANDLE_WIDTH / 2),
				   HANDLE_WIDTH,
				   HANDLE_WIDTH,
				   QRegion::Rectangle));
    painter.fillRect(tx2, ty2 - (HANDLE_WIDTH / 2), HANDLE_WIDTH, HANDLE_WIDTH, color);
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
  dialog->addColorItem(tr("Color"), tr("Details"), color);
  dialog->addComboItem(tr("Bar Field"), tr("Details"), l, barField);
  dialog->addCheckItem(tr("Use Bar"), tr("Details"), useBar);
  dialog->addCheckItem(tr("Extend Line"), tr("Details"), extend);
  dialog->addCheckItem(tr("Set Default"), tr("Details"), FALSE);
  
  int rc = dialog->exec();
  
  if (rc == QDialog::Accepted)
  {
    color = dialog->getColor(tr("Color"));
    useBar = dialog->getCheck(tr("Use Bar"));
    barField = dialog->getCombo(tr("Bar Field"));
    extend = dialog->getCheck(tr("Extend Line"));
    
    bool f = dialog->getCheck(tr("Set Default"));
    if (f)
      saveDefaults("TrendLine");
    
    saveFlag = TRUE;
    emit signalDraw();
  }
  
  delete dialog;
}

void TrendLine::move (BarDate d, double v)
{
  if (! move2Flag)
  {
    if (d.getDateValue() >= date2.getDateValue())
      return;
    
    date = d;
    value = v;
    saveFlag = TRUE;
    emit signalDraw();
    
    QString s = d.getDateString(TRUE) + " " + QString::number(v);
    emit message(s);
  }
  else
  {
    if (d.getDateValue() <= date.getDateValue())
      return;
    
    date2 = d;
    value2 = v;
    saveFlag = TRUE;
    emit signalDraw();
    
    QString s = d.getDateString(TRUE) + " " + QString::number(v);
    emit message(s);
  }
}

bool TrendLine::handleClicked (int x, int y)
{
  bool flag = FALSE;
  move2Flag = FALSE;
  QPoint p(x,y);

  QRegion *r = grabHandles.at(0);  
  if (r->contains(p))
    flag = TRUE;
  else
  {
    r = grabHandles.at(1);  
    if (r->contains(p))
      flag = TRUE;
    move2Flag = TRUE;
  }

  if (flag)
    moveObject();
      
  return flag;
}

Setting * TrendLine::getSettings ()
{
  Setting *set = new Setting;
  set->setData("Start Date", date.getDateTimeString(FALSE));
  set->setData("End Date", date2.getDateTimeString(FALSE));
  set->setData("Start Value", QString::number(value));
  set->setData("End Value", QString::number(value2));
  set->setData("Bar Field", barField);
  set->setData("Use Bar", QString::number(useBar));
  set->setData("Extend Line", QString::number(extend));
  set->setData("Color", color.name());
  set->setData("Plot", plot);
  set->setData("Name", name);
  set->setData("ObjectType", QString::number(type));
  return set;
}

void TrendLine::setSettings (Setting *set)
{
  date.setDate(set->getData("Start Date"));
  date2.setDate(set->getData("End Date"));
  value = set->getFloat("Start Value");
  value2 = set->getFloat("End Value");
  barField = set->getData("Bar Field");
  useBar = set->getInt("Use Bar");
  extend = set->getInt("Extend Line");
  color.setNamedColor(set->getData("Color"));
  plot = set->getData("Plot");
  name = set->getData("Name");
  type = (ChartObject::ObjectType) set->getInt("ObjectType");
}

