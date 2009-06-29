/*
 *  Qtstalker stock charter
 *
 *  Copyright (C) 2001-2007 Stefan S. Stratigakos
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

#include "Text.h"
#include "PrefDialog.h"
#include "Config.h"
#include "DataBase.h"
#include <QPainter>



Text::Text ()
{
  color.setNamedColor("white");
  helpFile = "text.html";
  label = tr("Text");
  type = (int) COText;
  
  loadDefaults();
}

Text::~Text ()
{
}

void Text::draw (QPixmap &buffer, Scaler &scaler, int startIndex, int pixelspace, int startX)
{
  QPainter painter;
  painter.begin(&buffer);
  
  int x2 = data->getX(date);
  if (x2 == -1)
    return;

  int x = startX + (x2 * pixelspace) - (startIndex * pixelspace);
  if (x == -1)
    return;

  painter.setFont(font);
  painter.setPen(color);
    
  int y = scaler.convertToY(value);

  painter.drawText(x, y, label);
  
  QFontMetrics fm = painter.fontMetrics();
  clearSelectionArea();
  setSelectionArea(new QRegion(x,
                   y - fm.height(),
		   fm.width(label, -1),
		   fm.height(),
		   QRegion::Rectangle));
    
  if (getStatus() == COBase::Selected)
  {
    clearGrabHandles();
    
    setGrabHandle(new QRegion(x - HANDLE_WIDTH - 1,
             	  y - (fm.height() / 2),
		  HANDLE_WIDTH,
		  HANDLE_WIDTH,
		  QRegion::Rectangle));
				    
    painter.fillRect(x - HANDLE_WIDTH - 1,
                     y - (fm.height() / 2),
		     HANDLE_WIDTH,
		     HANDLE_WIDTH,
		     color);
  }

  painter.end();
}

void Text::prefDialog ()
{
  QString cl = tr("Color");
  QString sd = tr("Set Default");
  QString fl = tr("Font");
  QString ll = tr("Label");
  QString vl = tr("Value");

  PrefDialog *dialog = new PrefDialog;
  dialog->setWindowTitle(tr("Edit Text"));
  dialog->addColorPrefItem(cl, color);
  dialog->addFontItem(fl, font);
  dialog->addTextItem(ll, label);
  dialog->addDoubleItem(vl, value);
  dialog->addCheckItem(sd, FALSE);
  
  int rc = dialog->exec();
  
  if (rc == QDialog::Accepted)
  {
    dialog->getColor(cl, color);
    dialog->getText(ll, label);
    dialog->getFont(fl, font);
    value = dialog->getDouble(vl);
    
    setSaveFlag(TRUE);
    
    bool f = dialog->getCheck(sd);
    if (f)
      saveDefaults();
    
    emit signalDraw();
  }
  
  delete dialog;
}

void Text::newObject (QString &ind, QString &n)
{
  indicator = ind;
  id = n;
  status = ClickWait;
  emit message(tr("Select point to place Text..."));
}

COBase::Status Text::pointerClick (QPoint &point, QDateTime &x, double y)
{
  switch (status)
  {
    case None:
      if (isSelected(point))
      {
        status = Selected;
        emit signalDraw();
      }
      break;
    case Selected:
      if (isGrabSelected(point))
        status = Moving;
      else
      {
        if (! isSelected(point))
        {
          status = None;
          emit signalDraw();
        }
      }
      break;
    case Moving:
      status = Selected;
      break;
    case ClickWait:
      date = x;
      value = y;
      setSaveFlag(TRUE);
      emit signalDraw();
      status = None;
      emit message("");
      break;
    default:
      break;
  }
    
  return status;    
}

void Text::pointerMoving (QPixmap &, QPoint &, QDateTime &x, double y)
{
  if (status != Moving)
    return;
    
  date = x;
  value = y;
  setSaveFlag(TRUE);
  emit signalDraw();
  QString s = x.toString(dateFormat) + " " + QString::number(y);
  emit message(s);
}

void Text::loadDefaults ()
{
  Config config;
  config.getData(Config::DefaultTextColor, color);
  config.getData(Config::DefaultTextFont, font);
}

void Text::saveDefaults ()
{
  Config config;
  config.setData(Config::DefaultTextColor, color);
  config.setData(Config::DefaultTextFont, font);
}

void Text::saveSettings ()
{
  COSettings co(id, symbol, indicator, QString::number(type));
  co.setDate(date);
  co.setValue(value);
  co.setColor(color);
  co.setText(label);

  DataBase db;
  db.setChartObject(co);
}

void Text::loadSettings (COSettings &co)
{
  co.getSymbol(symbol);
  co.getID(id);
  co.getIndicator(indicator);
  co.getDate(date);
  value = co.getValue();
  co.getColor(color);
  co.getText(label);
}

void Text::adjustForSplit (QDateTime &dt, double d)
{
  if (date < dt)
    value = value * d;
}

