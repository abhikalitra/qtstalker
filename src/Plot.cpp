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

#include "Plot.h"
#include "BuyArrow.h"
#include "SellArrow.h"
#include "TrendLine.h"
#include "HorizontalLine.h"
#include "VerticalLine.h"
#include "FiboLine.h"
#include "Text.h"
#include <qpainter.h>
#include <qpen.h>
#include <qpoint.h>
#include <qpointarray.h>
#include <qcursor.h>
#include <math.h>
#include <qprinter.h>
#include <qpaintdevicemetrics.h>
#include <qimage.h>
#include <qmessagebox.h>
#include <qinputdialog.h>

#include "indicator.xpm"
#include "edit.xpm"
#include "delete.xpm"
#include "co.xpm"
#include "text.xpm"
#include "buyarrow.xpm"
#include "sellarrow.xpm"
#include "fib.xpm"
#include "horizontal.xpm"
#include "vertical.xpm"
#include "trend.xpm"
#include "print.xpm"

#define SCALE_WIDTH 60

Plot::Plot (QWidget *w) : QWidget(w)
{
  buffer = new QPixmap;
  scaler = new Scaler;
  chartPlugin = 0;
  config = 0;
  setBackgroundMode(NoBackground);
  scaleWidth = SCALE_WIDTH;
  dateHeight = DATE_HEIGHT;
  _height = 0;
  _width = 0;
  startX = 2;
  backgroundColor.setNamedColor("black");
  borderColor.setNamedColor("white");
  gridColor.setNamedColor("#626262");
  pixelspace = 0;
  minPixelspace = 0;
  dateFlag = FALSE;
  gridFlag = TRUE;
  interval = ChartDb::Daily;
  mainFlag = FALSE;
  scaleToScreen = FALSE;
  logScale = FALSE;
  startIndex = 0;
  mainHigh = -99999999;
  mainLow = 99999999;
  chartType = "None";
  mouseFlag = None;
  hideMainPlot = FALSE;
  tabFlag = TRUE;
  crossHairFlag = FALSE;
  chartMenu = 0;
  drawMode = FALSE;

  plotFont.setFamily("Helvetica");
  plotFont.setPointSize(12);
  plotFont.setWeight(50);

  indicators.setAutoDelete(TRUE);
//  paintBars.setAutoDelete(TRUE);
  chartObjects.setAutoDelete(TRUE);
  data = 0;

  chartMenu = new QPopupMenu();
  
  setMouseTracking(TRUE);

  setFocusPolicy(QWidget::ClickFocus);
}

Plot::~Plot ()
{
  delete buffer;
  delete scaler;
  delete chartMenu;
}

void Plot::clear ()
{
  mainHigh = -99999999;
  mainLow = 99999999;
  indicators.clear();
//  paintBars.clear();
  
  slotSaveChartObjects();
  chartObjects.clear();
  
  data = 0;
  mouseFlag = None;
}

void Plot::setData (BarData *l)
{
  if (! l->count())
    return;

  data = l;
  
  if (mainFlag)
  {
    mainHigh = data->getMax();
    mainLow = data->getMin();

//    FIXME
//    int loop;
//    for (loop = 0; loop < (int) data->count(); loop++)
//      paintBars.append(new QColor(neutralColor.red(), neutralColor.green(), neutralColor.blue()));
  }

  createXGrid();
}

int Plot::setChartType (QString d)
{
  if (chartType.length())
    config->closePlugin(chartType);
  
  chartPlugin = config->getPlugin(Config::ChartPluginPath, d);
  if (! chartPlugin)
  {
    qDebug("Plot::setChartType:unable to open %s chart plugin", d.latin1());
    return TRUE;
  }
  
  if (chartType.length())
    setChartInput();
      
  chartType = d;
  
  minPixelspace = chartPlugin->getMinPixelspace();
  pixelspace = minPixelspace;
  startX = chartPlugin->getStartX();
  dateFlag = TRUE;  
  
  QObject::connect(chartPlugin, SIGNAL(draw()), this, SLOT(draw()));
  
  return FALSE;
}

void Plot::setChartInput ()
{
  chartPlugin->setChartInput(data, scaler, buffer);
}

void Plot::setConfig (Config *c)
{
  config = c;
}

void Plot::setMainFlag (bool d)
{
  mainFlag = d;
}

void Plot::setScaleToScreen (bool d)
{
  scaleToScreen = d;
}

void Plot::setLogScale (bool d)
{
  logScale = d;
}

void Plot::setHideMainPlot (bool d)
{
  hideMainPlot = d;
}

bool Plot::getHideMainPlot ()
{
  return hideMainPlot;
}

void Plot::setChartPath (QString d)
{
  chartPath = d;
}

void Plot::setDrawMode (bool d)
{
  drawMode = d;
}

void Plot::draw ()
{
  crossHairFlag = FALSE;

  buffer->fill(backgroundColor);

  if (data)
  {
    if (! mainFlag)
    {
      if (! indicators.count())
      {
        paintEvent(0);
        return;
      }
    }

    if (mainFlag)
    {
      if (chartPlugin->getMinPixelspace() != minPixelspace)
      {
        minPixelspace = chartPlugin->getMinPixelspace();
        if (minPixelspace > pixelspace)
          pixelspace = minPixelspace;
        emit signalMinPixelspace(minPixelspace);
      }
    }
    
    setHeight();

    setWidth();

    setScale();

    drawXGrid();

    drawYGrid();

    if (mainFlag)
    {
      if (hideMainPlot == TRUE)
      {
        drawLines();
        drawObjects();
      }
      else
      {
        chartPlugin->drawChart(startX, startIndex, pixelspace);
	
        if (! chartPlugin->getIndicatorFlag())
        {
          drawLines();
          drawObjects();
        }
      }
    }
    else
    {
      drawLines();
      drawObjects();
    }

    drawScale();

    drawInfo();

    if (dateFlag)
      drawDate();
  }

  paintEvent(0);
}

void Plot::drawLines ()
{
  if (mainFlag)
  {
    QDictIterator<Indicator> it(indicators);
    for (; it.current(); ++it)
    {
      Indicator *i = it.current();

      if (! i->getEnable())
        continue;

      int loop;
      for (loop = 0; loop < i->getLines(); loop++)
      {
        currentLine = i->getLine(loop);
        currentLine->draw(data->count(), startX, startIndex, pixelspace);
      }
    }
  }
  else
  {
    QDictIterator<Indicator> it(indicators);
    it.toFirst();
    Indicator *i = it.current();

    int loop;
    for (loop = 0; loop < i->getLines(); loop++)
    {
      currentLine = i->getLine(loop);
      currentLine->draw(data->count(), startX, startIndex, pixelspace);
    }
  }
}

void Plot::paintEvent (QPaintEvent *)
{
  bitBlt(this, 0, 0, buffer);

  // redraw the crosshair
  if (crossHairFlag)
  {
    QPainter painter;
    painter.begin(this);
    painter.setPen(QPen(borderColor, 1, QPen::DotLine));
    painter.drawLine (0, crossHairY, buffer->width() - SCALE_WIDTH, crossHairY);
    painter.drawLine (crossHairX, 0, crossHairX, buffer->height());
    painter.end();
  }
}

void Plot::resizeEvent (QResizeEvent *event)
{
  buffer->resize(event->size());

  setHeight();

  setWidth();

  draw();
}

void Plot::mousePressEvent (QMouseEvent *event)
{
  if (mainFlag)
  {
    if (! data)
      return;
  }
  else
  {
    if (! indicators.count())
      return;
  }

  if (event->x() > buffer->width() - SCALE_WIDTH)
    return;

  if (! drawMode)
  {
    if (event->button() == LeftButton)
    {
      crossHair(event->x(), event->y());
      updateStatusBar(event->x(), event->y());
      emit leftMouseButton(event->x(), event->y(), mainFlag);
      return;
    }
    else
    {
      if (event->button() == RightButton)
      {
        showPopupMenu();
        return;
      }
    }
  }
    
  if (event->button() == LeftButton)
  {
    switch (mouseFlag)
    {
      case ClickWait2:
        getXY(event->x(), event->y(), 1);
        newChartObject();
        break;
      case ClickWait:
	getXY(event->x(), event->y(), 0);
        if (objectFlag == ChartObject::TrendLine || objectFlag == ChartObject::FibonacciLine)
	{
          mouseFlag = ClickWait2;
          if (objectFlag == ChartObject::TrendLine)
            emit statusMessage(tr("Select end point of trend line..."));
	  else
            emit statusMessage(tr("Select low point of fibonacci line..."));
	}
	else
	  newChartObject();
	break;
      case COSelected:
        if (! tco->isClicked(event->x(), event->y()))
	{
	  tco->unselect();
	  mouseFlag = None;
	}
        break;
      default:
        emit signalMouseLeftClick(event->x(), event->y());
	break;
    }
  }
  else
  {
    if (event->button() == RightButton)
    {
      if (mouseFlag == COSelected)
      {
        if (tco->isClicked(event->x(), event->y()))
          tco->showMenu();
	else
          showPopupMenu();
      }
      else
        showPopupMenu();
    }
  }
}

void Plot::mouseMoveEvent (QMouseEvent *event)
{
  if (mainFlag)
  {
    if (! data)
      return;
  }
  else
  {
    if (! indicators.count())
      return;
  }

  if (event->x() > buffer->width() - SCALE_WIDTH)
    return;
   
  // are we trying to drag a chart object?
  if ((mouseFlag == COSelected) && (event->state() == LeftButton) && drawMode)
  {
    getXY(event->x(), event->y(), 0);
    tco->move(x1, y1);
    return;
  }

  int i = (event->x() / pixelspace) + startIndex;
  if (i >= (int) data->count())
    i = data->count() - 1;
  if (i < startIndex)
    i = startIndex;

  Setting *r = new Setting;
  r->setData("D", data->getDate(i).getDateString(TRUE));
  r->setData("T", data->getDate(i).getTimeString(TRUE));
  
  if (mainFlag)
  {
    r->setData("O", strip(data->getOpen(i)));
    r->setData("H", strip(data->getHigh(i)));
    r->setData("L", strip(data->getLow(i)));
    r->setData("C", strip(data->getClose(i)));
  }

  QDictIterator<Indicator> it(indicators);
  for (; it.current(); ++it)
  {
    Indicator *ind = it.current();

    if (! ind->getEnable())
      continue;

    int loop;
    for (loop = 0; loop < (int) ind->getLines(); loop++)
    {
      PlotLine *line = ind->getLine(loop);
      int li = line->getSize() - data->count() + i;
      if (li > -1 && li <= line->getSize())
        r->setData(line->getLabel(), strip(line->getData(li)));
    }
  }

  if (r->count())
    emit infoMessage(r);
  else
    delete r;
}

void Plot::keyPressEvent (QKeyEvent *key)
{
  switch (key->key())
  {
    case Qt::Key_Left:
    case Qt::Key_Right:
    case Qt::Key_Home:
    case Qt::Key_End:
    case Qt::Key_Plus:
    case Qt::Key_Minus:
    case Qt::Key_Prior:
    case Qt::Key_Next:
    case Qt::Key_Up:
    case Qt::Key_Down:
      emit keyPressed(key);
      break;
    default:
      key->ignore();
      break;
  }
}

void Plot::setScaleWidth (int d)
{
  if (d > 999 || d < SCALE_WIDTH)
    return;
  else
    scaleWidth = d;
}

void Plot::setBackgroundColor (QColor d)
{
  backgroundColor = d;
}

void Plot::setBorderColor (QColor d)
{
  borderColor = d;
}

void Plot::setGridColor (QColor d)
{
  gridColor = d;
}

void Plot::setPlotFont (QFont d)
{
  plotFont = d;
}

void Plot::setGridFlag (bool d)
{
  gridFlag = d;
}

void Plot::setPixelspace (int d)
{
  pixelspace = d;
}

void Plot::setIndex (int d)
{
  startIndex = d;
}

void Plot::setTabFlag (bool d)
{
  tabFlag = d;
}

bool Plot::getTabFlag ()
{
  return tabFlag;
}

bool Plot::getMainFlag ()
{
  return mainFlag;
}

//***********************************************************************
//******************** DATE FUNCTIONS ***********************************
//***********************************************************************

void Plot::setInterval (ChartDb::BarCompression d)
{
  interval = d;
}

void Plot::setDateHeight (int d)
{
  if (d > 999 || d < DATE_HEIGHT)
    return ;
  else
    dateHeight = d;
}

void Plot::setDateFlag (bool d)
{
  dateFlag = d;
}

void Plot::drawDate ()
{
  QPainter painter;
  painter.begin(buffer);
  painter.setPen(borderColor);
  
  // clear date area
  painter.fillRect(0, buffer->height() - dateHeight, buffer->width() - scaleWidth, dateHeight, backgroundColor);

  // draw the seperator line
  painter.drawLine (0, buffer->height() - dateHeight, buffer->width() - scaleWidth, buffer->height() - dateHeight);

  painter.end();
    
  switch (interval)
  {
    case ChartDb::Daily:
      drawDailyDate();
      break;
    case ChartDb::Weekly:
      drawWeeklyDate();
      break;
    case ChartDb::Monthly:
      drawMonthlyDate();
      break;
    default:
      break;
  }
}

void Plot::draw5Date ()
{
}

void Plot::draw15Date ()
{
}

void Plot::draw30Date ()
{
}

void Plot::drawHourlyDate ()
{
}

void Plot::drawDailyDate ()
{
  QPainter painter;
  painter.begin(buffer);
  painter.setPen(borderColor);
  painter.setFont(plotFont);

  QFontMetrics fm = painter.fontMetrics();
  
  int x = startX;
  int loop = startIndex;

  QDate oldDate = data->getDate(loop).getDate();
  QDate oldWeek = oldDate;
  oldWeek = oldWeek.addDays(7 - oldWeek.dayOfWeek());

  while(x <= _width && loop < (int) data->count())
  {
    QDate date = data->getDate(loop).getDate();

    if (date.month() != oldDate.month())
    {
      oldDate = date;

      painter.drawLine (x, _height + 1, x, _height + dateHeight - fm.height() - 1);

      QString s = date.toString("MMM'yy");
      painter.drawText (x - (fm.width(s, -1) / 2), buffer->height() - 2, s, -1);

      oldWeek = date;
      oldWeek = oldWeek.addDays(7 - oldWeek.dayOfWeek());
    }
    else
    {
      // if start of new week make a tick
      if (date > oldWeek)
      {
        painter.drawLine (x, _height + 1, x, _height + 4);

        QString s = date.toString("d");
        painter.drawText (x - (fm.width(s, -1) / 2),
			  buffer->height() - dateHeight + fm.height() + 1,
			  s,
			  -1);

  	oldWeek = date;
        oldWeek = oldWeek.addDays(7 - oldWeek.dayOfWeek());
      }
    }

    x = x + pixelspace;
    loop++;
  }

  painter.end();
}

void Plot::drawWeeklyDate ()
{
  QPainter painter;
  painter.begin(buffer);
  painter.setPen(borderColor);
  painter.setFont(plotFont);

  QFontMetrics fm = painter.fontMetrics();
  
  int x = startX;
  int loop = startIndex;

  QDate oldMonth = data->getDate(loop).getDate();

  while(x <= _width && loop < (int) data->count())
  {
    QDate date = data->getDate(loop).getDate();

    if (date.month() != oldMonth.month())
    {
      oldMonth = date;

      painter.drawLine (x, _height + 1, x, _height + (dateHeight / 3));

      QString s;
      s = date.toString("MMM'yy");

      if (fm.width(s, -1) > 4 * pixelspace)
      {
        if (date.month() == 1)
          s = date.toString("yy");
	else
	{
          s = date.toString("MMM");
          s.truncate(1);
	}
      }

      painter.drawText (x - (fm.width(s, -1) / 2),
  		        _height + (dateHeight / 3) + fm.height() + 2,
			s,
			-1);
    }

    x = x + pixelspace;
    loop++;
  }
  
  painter.end();
}

void Plot::drawMonthlyDate ()
{
  QPainter painter;
  painter.begin(buffer);
  painter.setPen(borderColor);
  painter.setFont(plotFont);

  QFontMetrics fm = painter.fontMetrics();
  
  int x = startX;
  int loop = startIndex;

  QDate oldYear = data->getDate(loop).getDate();

  while(x <= _width && loop < (int) data->count())
  {
    QDate date = data->getDate(loop).getDate();

    if (date.year() != oldYear.year())
    {
      oldYear = date;
      painter.drawLine (x, _height + 1, x, _height + (dateHeight / 3));
      QString s = date.toString("yyyy");
      painter.drawText (x - (fm.width(s, -1) / 2),
	  		_height + (dateHeight / 3) + fm.height() + 2,
			s,
			-1);
    }

    x = x + pixelspace;
    loop++;
  }

  painter.end();
}

void Plot::drawXGrid ()
{
  if (gridFlag == FALSE)
    return;

  QPainter painter;
  painter.begin(buffer);
  painter.setPen(QPen(gridColor, 1, QPen::DotLine));

  int loop;
  for (loop = 0; loop < (int) xGrid.size(); loop++)
  {
    if (xGrid[loop] >= startIndex)
    {
      int x = startX + (xGrid[loop] * pixelspace) - (startIndex * pixelspace);
      painter.drawLine (x, 0, x, _height);
    }
  }

  painter.end();
}

void Plot::createXGrid ()
{
  xGrid.resize(0);

  int loop = 0;

  QDate oldDate = data->getDate(loop).getDate();

  for (loop = 0; loop < (int) data->count(); loop++)
  {
    QDate date = data->getDate(loop).getDate();

    if (interval == ChartDb::Daily)
    {
      if (date.month() != oldDate.month())
      {
        oldDate = date;
	xGrid.resize(xGrid.size() + 1);
	xGrid[xGrid.size() - 1] = loop;
      }
    }
    else
    {
      if (date.year() != oldDate.year())
      {
        oldDate = date;
	xGrid.resize(xGrid.size() + 1);
	xGrid[xGrid.size() - 1] = loop;
      }
    }
  }
}

void Plot::addIndicator (QString d, Indicator *i)
{
  indicators.replace(d, i);
  
  // dont forget to update plotlines with this plots drawing pointers
  int loop;
  for (loop = 0; loop < i->getLines(); loop++)
  {
    PlotLine *pl = i->getLine(loop);
    pl->setPointers(scaler, buffer);
  }
}

Indicator * Plot::getIndicator (QString d)
{
  return indicators[d];
}

void Plot::deleteIndicator (QString d)
{
  indicators.remove(d);
}

void Plot::setHeight ()
{
  _height = buffer->height();
  if (dateFlag)
    _height = _height - dateHeight - 1;
}

void Plot::setWidth ()
{
  _width = buffer->width() - scaleWidth - startX;
}

void Plot::drawScale ()
{
  QPainter painter;
  painter.begin(buffer);
  painter.setFont(plotFont);

  QFontMetrics fm = painter.fontMetrics();

  painter.fillRect(buffer->width() - scaleWidth, 0, scaleWidth, buffer->height(), backgroundColor);

  int x = buffer->width() - scaleWidth;

  int loop;
  for (loop = 0; loop < (int) scaleArray.size(); loop++)
  {
    int y = scaler->convertToY(scaleArray[loop]);
    painter.setPen(QPen(borderColor, 1, QPen::SolidLine));
    painter.drawLine (x, y, x + 4, y);

    // draw the text
    QString s = strip(scaleArray[loop]);
    
    // abbreviate too many (>=3) trailing zeroes in large numbers on y-axes
    if (! mainFlag)
    {
      bool flag = FALSE;
      
      if (s.toDouble() < 0)
      {
        flag = TRUE;
	s.remove(0, 1);  
      }
      
      if (s.toDouble() >= 1000000000)
      {
        s = strip(s.toDouble() / 1000000000);
	s.append("b");
      }
      else
      {
        if (s.toDouble() >= 1000000)
        {
          s = strip(s.toDouble() / 1000000);
	  s.append("m");
        }
        else
        {
          if (s.toDouble() >= 1000)
          {
            s = strip(s.toDouble() / 1000);
	    s.append("k");
	  }
	}
      }
      
      if (flag)
        s.prepend("-");
    }
    
    painter.drawText(x + 7, y + (fm.height() / 2), s);
  }

  painter.setPen(QPen(borderColor, 1, QPen::SolidLine));
  painter.drawLine (x, 0, x, _height);
  
  // draw the last value pointer on the scale of main plot
  if (mainFlag && ! hideMainPlot)
  {
    int y = scaler->convertToY(data->getClose(data->count() - 1));
    
    QPointArray array;
    array.setPoints(3, x + 2, y,
                    x + 8, y - 4,
	            x + 8, y + 4);
    painter.setBrush(borderColor);
    painter.drawPolygon(array, TRUE, 0, -1);
  }

  painter.end();
}

void Plot::drawYGrid ()
{
  if (! gridFlag)
    return;

  QPainter painter;
  painter.begin(buffer);
  painter.setPen(QPen(gridColor, 1, QPen::DotLine));

  int loop;
  for (loop = 0; loop < (int) scaleArray.size(); loop++)
  {
    int y = scaler->convertToY(scaleArray[loop]);
    painter.drawLine (startX, y, _width, y);
  }

  painter.end();
}

void Plot::drawInfo ()
{
  QPainter painter;
  painter.begin(buffer);
  painter.setPen(borderColor);
  painter.setFont(plotFont);

  QFontMetrics fm = painter.fontMetrics();
  int pos = startX;

  QString s = data->getDate(data->count() - 1).getDateString(TRUE);
  s.append(" ");
  painter.drawText(pos, 10, s, -1);
  pos = pos + fm.width(s);

  if (data->count() && mainFlag)
  {
    s = "O=";
    s.append(strip(data->getOpen(data->count() - 1)));
    s.append(" H=");
    s.append(strip(data->getHigh(data->count() - 1)));
    s.append(" L=");
    s.append(strip(data->getLow(data->count() - 1)));
    s.append(" C=");
    s.append(strip(data->getClose(data->count() - 1)));
    s.append(" ");
    painter.drawText(pos, 10, s, -1);
    pos = pos + fm.width(s);

    double ch = 0;
    if (data->count() > 1)
      ch = data->getClose(data->count() - 1) - data->getClose(data->count() - 2);
    s = "CH=";
    s.append(strip(ch));
    s.append(" ");
    if (ch < 0)
      painter.setPen(QColor("red"));
    else
    {
      if (ch > 0)
        painter.setPen(QColor("green"));
      else
        painter.setPen(QColor("blue"));
    }

    painter.drawText(pos, 10, s, -1);
    pos = pos + fm.width(s);
  }

  if (mainFlag)
  {
    QDictIterator<Indicator> it(indicators);
    for (; it.current(); ++it)
    {
      Indicator *i = it.current();

      if (! i->getEnable())
        continue;

      int loop;
      for (loop = 0; loop < (int) i->getLines(); loop++)
      {
        PlotLine *line = i->getLine(loop);
        if (line->getSize() > 1)
        {
          s = line->getLabel();
          s.append("=");
          s.append(strip(line->getData(line->getSize() - 1)));
          s.append(" ");

          painter.setPen(line->getColor());
          painter.drawText(pos, 10, s, -1);
          pos = pos + fm.width(s);
        }
      }
    }
  }
  else
  {
    QDictIterator<Indicator> it(indicators);
    it.toFirst();
    Indicator *i = it.current();

    int loop;
    for (loop = 0; loop < (int) i->getLines(); loop++)
    {
      PlotLine *line = i->getLine(loop);
      if (line->getSize() > 1)
      {
        s = line->getLabel();
        s.append("=");
        s.append(strip(line->getData(line->getSize() - 1)));
        s.append(" ");

        painter.setPen(line->getColor());
        painter.drawText(pos, 10, s, -1);
        pos = pos + fm.width(s);
      }
    }
  }

  painter.end();
}

void Plot::crossHair (int x, int y)
{
  crossHairFlag = TRUE;
  crossHairX = x;
  crossHairY = y;

  paintEvent(0);

  QPainter painter;
  painter.begin(this);
  painter.setPen(QPen(borderColor, 1, QPen::DotLine));
  painter.drawLine (0, y, buffer->width() - SCALE_WIDTH, y);
  painter.drawLine (x, 0, x, buffer->height());
  painter.end();
}

void Plot::updateStatusBar (int x, int y)
{
  int i = (x / pixelspace) + startIndex;
  if (i >= (int) data->count())
    i = data->count() - 1;
  if (i < startIndex)
    i = startIndex;

  QString s = data->getDate(i).getDateTimeString(TRUE);
  s.append(" ");
  s.append(strip(scaler->convertToVal(y)));
  emit statusMessage(s);
}

void Plot::getXY (int x, int y, int f)
{
  int i = (x / pixelspace) + startIndex;
  if (i >= (int) data->count())
    i = data->count() - 1;
  if (i < startIndex)
    i = startIndex;

  if (f == 0)
  {
    x1 = data->getDate(i);
    y1 = scaler->convertToVal(y);
  }
  else
  {
    x2 = data->getDate(i);
    y2 = scaler->convertToVal(y);
  }
}

void Plot::setScale ()
{
  double scaleHigh = -99999999;
  double scaleLow = 99999999;

  if (mainFlag)
  {
    if (! scaleToScreen)
    {
      scaleHigh = mainHigh;
      scaleLow = mainLow;
    }
    else
    {
      int x = startX;
      int loop = startIndex;
      while ((x < _width) && (loop < (int) data->count()))
      {
	double t = data->getHigh(loop);
        if (t > scaleHigh)
	  scaleHigh = t;

	t = data->getLow(loop);
        if (t < scaleLow)
	  scaleLow = t;

        x = x + pixelspace;
        loop++;
      }
    }
  }

  if (! scaleToScreen)
  {
    QDictIterator<Indicator> it(indicators);
    if (! mainFlag)
      it.toFirst();
    for (; it.current(); ++it)
    {
      Indicator *i = it.current();

      if (! i->getEnable())
        continue;

      int loop;
        for (loop = 0; loop < i->getLines(); loop++)
      {
	PlotLine *line = i->getLine(loop);
	if (line->getType() == PlotLine::Invisible)
	  continue;

        if (line->getHigh() > scaleHigh)
          scaleHigh = line->getHigh();

        if (line->getLow() < scaleLow)
          scaleLow = line->getLow();
      }
    }
  }
  else
  {
    QDictIterator<Indicator> it(indicators);
    if (! mainFlag)
      it.toFirst();
    for (; it.current(); ++it)
    {
      Indicator *i = it.current();

      if (! i->getEnable())
        continue;

      int loop;
        for (loop = 0; loop < i->getLines(); loop++)
      {
	PlotLine *line = i->getLine(loop);
	if (line->getType() == PlotLine::Invisible)
	  continue;

        int x = startX;
        int loop2 = line->getSize() - data->count() + startIndex;
        while ((x < _width) && (loop2 < (int) line->getSize()))
        {
          if (loop2 > -1)
          {
            if (line->getData(loop2) > scaleHigh)
              scaleHigh = line->getData(loop2);

            if (line->getData(loop2) < scaleLow)
              scaleLow = line->getData(loop2);
          }

          x = x + pixelspace;
          loop2++;
        }
      }
    }
  }

  QDictIterator<ChartObject> it(chartObjects);
  for (; it.current(); ++it)
  {
    ChartObject *co = it.current();

    ChartObject::ObjectType type = co->getType();
    if (type == ChartObject::VerticalLine)
      continue;

    if ((type == ChartObject::TrendLine) || (type == ChartObject::FibonacciLine))
    {
      double v = co->getValue();
      double v2 = co->getValue2();
      if (v > scaleHigh)
        scaleHigh = v;
      if (v2 > scaleHigh)
        scaleHigh = v2;

      if (v < scaleLow)
        scaleLow = v;
      if (v2 < scaleLow)
        scaleLow = v2;

      continue;
    }

    double v = co->getValue();
    if (v > scaleHigh)
      scaleHigh = v;
    if (v < scaleLow)
      scaleLow = v;
  }

  scaleHigh = scaleHigh * 1.01;
  if (scaleLow < 0)
    scaleLow = scaleLow * 1.1;
  else
    scaleLow = scaleLow * 0.99;

  double logScaleHigh = 1;
  double logRange = 0;
  if (mainFlag && logScale)
  {
    logScaleHigh = scaleHigh > 0.0 ? log(scaleHigh) : 1;
    double logScaleLow = scaleLow > 0.0 ? log(scaleLow) : 0;
    logRange = logScaleHigh - logScaleLow;
  }

  scaler->set(_height,
  	     scaleHigh,
	     scaleLow,
	     logScaleHigh,
	     logRange,
	     dateFlag,
	     logScale);

  scaleArray = scaler->getScaleArray();
}

int Plot::getWidth ()
{
  return _width;
}

int Plot::getPixelspace ()
{
  return pixelspace;
}

int Plot::getMinPixelspace ()
{
  return minPixelspace;
}

int Plot::getXFromDate (BarDate d)
{
  int x2 = data->getX(d);
  if (x2 == -1)
    return -1;

  int x = startX + (x2 * pixelspace) - (startIndex * pixelspace);

  return x;
}

QString Plot::strip (double d)
{
  QString s = QString::number(d, 'f', 4);

  while (1)
  {
    if (s.find('.', -1, TRUE) != -1)
    {
      s.truncate(s.length() - 1);
      break;
    }
    else
    {
      if (s.find('0', -1, TRUE) != -1)
        s.truncate(s.length() - 1);
      else
        break;
    }
  }

  return s;
}

// FIXME
/*
void Plot::setPaintBars (QList<QColor> d)
{
  paintBars.clear();
  int loop;
  for (loop = 0; loop < (int) d.count(); loop++)
  {
    QColor *color = d.at(loop);
    paintBars.append(new QColor(color->red(), color->green(), color->blue()));
  }
}
*/

QStringList Plot::getIndicators ()
{
  QStringList l;

  QDictIterator<Indicator> it(indicators);
  for (; it.current(); ++it)
    l.append(it.currentKey());

  return l;
}

void Plot::printChart ()
{
  QPrinter printer;
  printer.setPageSize(QPrinter::Letter);

  if (printer.setup())
  {
    emit statusMessage(tr("Creating chart snapshot..."));

    printer.setFullPage(true);
    QSize margins = printer.margins();
    int leftMargin = margins.width();
    int topMargin  = margins.height();

    QPaintDeviceMetrics prm(&printer);
    
    int prmw = prm.width() - leftMargin;
    int prmh = prm.height() - topMargin;

    QPixmap &pix = *buffer;

    if ((pix.width() > prmw) || (pix.height() > prmh))
    {
      QImage image = pix.convertToImage();
      image = image.smoothScale(prmw, prmh, QImage::ScaleMin);
      pix.convertFromImage(image);
    }

    emit statusMessage(tr("Printing..."));

    QPainter painter;
    painter.begin(&printer);
    painter.drawPixmap(leftMargin/2, topMargin/2, pix);
    painter.end();
    emit statusMessage(tr("Printing complete."));
  }
}

void Plot::showPopupMenu ()
{
  if (! data->count())
    return;

  chartMenu->clear();
    
  if (mainFlag)
  {
    chartMenu->insertItem(tr("Chart Prefs"), this, SLOT(slotEditChartPrefs()));
    chartMenu->insertSeparator ();
  }
  
  chartMenu->insertItem(QPixmap(indicator), tr("New Indicator"), this, SLOT(slotNewIndicator()));
  chartDeleteMenu = new QPopupMenu();
  chartEditMenu = new QPopupMenu();
  chartMenu->insertItem(QPixmap(edit), tr("Edit Indicator"), chartEditMenu);
  chartMenu->insertItem (QPixmap(deleteitem), tr("Delete Indicator"), chartDeleteMenu);
  chartMenu->insertSeparator ();

  if (drawMode)
  {
    chartObjectMenu = new QPopupMenu();
    QStringList l = getChartObjectList();
    int id = chartObjectMenu->insertItem(QPixmap(buyarrow), l[0], this, SLOT(slotNewChartObject(int)));
    chartObjectMenu->setItemParameter(id, id);
    id = chartObjectMenu->insertItem(QPixmap(sellarrow), l[1], this, SLOT(slotNewChartObject(int)));
    chartObjectMenu->setItemParameter(id, id);
    id = chartObjectMenu->insertItem(QPixmap(fib), l[2], this, SLOT(slotNewChartObject(int)));
    chartObjectMenu->setItemParameter(id, id);
    id = chartObjectMenu->insertItem(QPixmap(horizontal), l[3], this, SLOT(slotNewChartObject(int)));
    chartObjectMenu->setItemParameter(id, id);
    id = chartObjectMenu->insertItem(QPixmap(vertical), l[4], this, SLOT(slotNewChartObject(int)));
    chartObjectMenu->setItemParameter(id, id);
    id = chartObjectMenu->insertItem(QPixmap(trend), l[5], this, SLOT(slotNewChartObject(int)));
    chartObjectMenu->setItemParameter(id, id);
    id = chartObjectMenu->insertItem(QPixmap(text), l[6], this, SLOT(slotNewChartObject(int)));
    chartObjectMenu->setItemParameter(id, id);

    chartMenu->insertItem (QPixmap(co), tr("New Chart Object"), chartObjectMenu);
  }

  chartMenu->insertItem(QPixmap(deleteitem), tr("Delete All Chart Objects"), this, SLOT(slotDeleteAllChartObjects()));

  chartMenu->insertSeparator ();
  chartMenu->insertItem(QPixmap(print), tr("Print Chart"), this, SLOT(printChart()));
    
  QDictIterator<Indicator> it(indicators);
  for(; it.current(); ++it)
  {
    if (it.currentKey().compare("Main Plot"))
    {
      int id = chartDeleteMenu->insertItem(QPixmap(indicator), it.currentKey(), this, SLOT(slotDeleteIndicator(int)));
      chartDeleteMenu->setItemParameter(id, id);

      id = chartEditMenu->insertItem(QPixmap(indicator), it.currentKey(), this, SLOT(slotEditIndicator(int)));
      chartEditMenu->setItemParameter(id, id);
    }
  }

  chartMenu->exec(QCursor::pos());
}

void Plot::slotEditIndicator (int id)
{
  QString s = chartEditMenu->text(id);
  emit signalEditIndicator(s, this);
}

void Plot::slotDeleteIndicator (int id)
{
  QString s = chartDeleteMenu->text(id);
  emit signalDeleteIndicator(s, this);
}

void Plot::slotNewIndicator ()
{
  emit signalNewIndicator();
}

void Plot::slotEditChartPrefs ()
{
  chartPlugin->prefDialog();
}

void Plot::slotMessage (QString d)
{
  emit statusMessage(d);
}

//*************************************************************************
//******************** chart object functions *****************************
//*************************************************************************

void Plot::newChartObject ()
{
  bool cancelFlag = FALSE;
  ChartObject *co = 0;
  QString name = objectName;
  QString pl = "Main Plot";
  if (! mainFlag)
  {
    QDictIterator<Indicator> it(indicators);
    it.toFirst();
    pl = it.currentKey();
  }

  while (1)
  {
    if (objectFlag == ChartObject::BuyArrow)
    {
      co = new BuyArrow(scaler, buffer, pl, name, x1, y1);
      break;
    }
    
    if (objectFlag == ChartObject::SellArrow)
    {
      co = new SellArrow(scaler, buffer, pl, name, x1, y1);
      break;
    }
    
    if (objectFlag == ChartObject::FibonacciLine)
    {
      if (y1 <= y2)
      {
	cancelFlag = TRUE;
        QMessageBox::information(this,
	                         tr("Qtstalker: Error"),
    			         tr("Fibonacci high point <= low point."));
        break;
      }
      
      if (y2 >= y1)
      {
	cancelFlag = TRUE;
        QMessageBox::information(this,
	                         tr("Qtstalker: Error"),
    			         tr("Fibonacci low point >= high point."));
        break;
      }
      
      co = new FiboLine(scaler, buffer, pl, name, x1, y1, x2, y2);
      break;
    }
    
    if (objectFlag == ChartObject::HorizontalLine)
    {
      co = new HorizontalLine(scaler, buffer, pl, name, y1);
      break;
    }
    
    if (objectFlag == ChartObject::VerticalLine)
    {
      co = new VerticalLine(buffer, pl, name, x1);
      break;
    }
    
    if (objectFlag == ChartObject::TrendLine)
    {
      if (x2.getDateValue() <= x1.getDateValue())
      {
	cancelFlag = TRUE;
        QMessageBox::information(this, tr("Qtstalker: Error"),
    			         tr("Trendline end point <= start point."));
        break;
      }
      co = new TrendLine(scaler, buffer, data, pl, name, x1, y1, x2, y2);
      break;
    }
    
    if (objectFlag == ChartObject::Text)
    {
      co = new Text(scaler, buffer, pl, name, x1, y1);
      break;
    }
    
    break;
  }

  if (! cancelFlag)
  {
    QObject::connect(co, SIGNAL(signalDraw()), this, SLOT(draw()));
    QObject::connect(this, SIGNAL(signalMouseLeftClick(int, int)), co, SLOT(selected(int, int)));
    QObject::connect(co, SIGNAL(signalChartObjectSelected(ChartObject *)), this, SLOT(slotChartObjectSelected(ChartObject *)));
    QObject::connect(co, SIGNAL(signalDeleteChartObject(QString)), this, SLOT(slotDeleteChartObject(QString)));
    QObject::connect(co, SIGNAL(message(QString)), this, SLOT(slotMessage(QString)));

    addChartObject(co);
    co->setSaveFlag(TRUE);
    draw();
  }

  mouseFlag = None;
  emit statusMessage("");
  setCursor(QCursor(Qt::ArrowCursor));
}

QStringList Plot::getChartObjectList ()
{
  QStringList l;
  l.append(QObject::tr("Buy Arrow"));
  l.append(QObject::tr("Sell Arrow"));
  l.append(QObject::tr("Fibonacci Line"));
  l.append(QObject::tr("Horizontal Line"));
  l.append(QObject::tr("Vertical Line"));
  l.append(QObject::tr("Trend Line"));
  l.append(QObject::tr("Text"));
  return l;
}

void Plot::createChartObject (QString d, QString n)
{
  setCursor(QCursor(Qt::PointingHandCursor));

  objectName = n;

  while (1)
  {
    if (! d.compare(tr("Vertical Line")))
    {
      mouseFlag = ClickWait;
      objectFlag = ChartObject::VerticalLine;
      emit statusMessage(tr("Select point to place vertical line..."));
      break;
    }

    if (! d.compare(tr("Horizontal Line")))
    {
      mouseFlag = ClickWait;
      objectFlag = ChartObject::HorizontalLine;
      emit statusMessage(tr("Select point to place horizontal line..."));
      break;
    }

    if (! d.compare(tr("Text")))
    {
      mouseFlag = ClickWait;
      objectFlag = ChartObject::Text;
      emit statusMessage(tr("Select point to place text..."));
      break;
    }

    if (! d.compare(tr("Trend Line")))
    {
      mouseFlag = ClickWait;
      objectFlag = ChartObject::TrendLine;
      emit statusMessage(tr("Select start point of trend line..."));
      break;
    }

    if (! d.compare(tr("Buy Arrow")))
    {
      mouseFlag = ClickWait;
      objectFlag = ChartObject::BuyArrow;
      emit statusMessage(tr("Select point to place buy arrow..."));
      break;
    }

    if (! d.compare(tr("Sell Arrow")))
    {
      mouseFlag = ClickWait;
      objectFlag = ChartObject::SellArrow;
      emit statusMessage(tr("Select point to place sell arrow..."));
      break;
    }

    if (! d.compare(tr("Fibonacci Line")))
    {
      mouseFlag = ClickWait;
      objectFlag = ChartObject::FibonacciLine;
      emit statusMessage(tr("Select high point of fibonacci line..."));
      break;
    }

    break;
  }
}

void Plot::slotDeleteChartObject (QString s)
{
  ChartDb *db = new ChartDb();
  db->openChart(chartPath);
  db->deleteChartObject(s);
  delete db;
  
  chartObjects.remove(s);
  
  mouseFlag = None;
  
  draw();
}

void Plot::slotNewChartObject (int id)
{
  QString selection = chartObjectMenu->text(id);
  int loop = 0;
  QString name;
  
  ChartDb *db = new ChartDb();
  db->openChart(chartPath);
  QStringList l = db->getChartObjectsList();
  delete db;
  
  while (1)
  {
    name = QString::number(loop);
    if (l.findIndex(name) != -1)
      loop++;
    else
      break;
  }

  createChartObject(selection, name);
}

void Plot::addChartObject (ChartObject *co)
{
  chartObjects.replace(co->getName(), co);
}

void Plot::addChartObject (Setting *set)
{
  ChartObject *co = 0;
  ChartObject::ObjectType ot = (ChartObject::ObjectType) set->getInt("ObjectType");
  BarDate dt;
  
  while (1)
  {
    if (ot == ChartObject::BuyArrow)
    {
      co = new BuyArrow(scaler, buffer, QString(), QString(), dt, 0);
      break;
    }
      
    if (ot == ChartObject::SellArrow)
    {
      co = new SellArrow(scaler, buffer, QString(), QString(), dt, 0);
      break;
    }
    
    if (ot == ChartObject::FibonacciLine)
    {
      co = new FiboLine(scaler, buffer, QString(), QString(), dt, 0, dt, 0);
      break;
    }
    
    if (ot == ChartObject::HorizontalLine)
    {
      co = new HorizontalLine(scaler, buffer, QString(), QString(), 0);
      break;
    }
    
    if (ot == ChartObject::VerticalLine)
    {
      co = new VerticalLine(buffer, QString(), QString(), dt);
      break;
    }
    
    if (ot == ChartObject::TrendLine)
    {
      co = new TrendLine(scaler, buffer, data, QString(), QString(), dt, 0, dt , 0);
      break;
    }
    
    if (ot == ChartObject::Text)
    {
      co = new Text(scaler, buffer, QString(), QString(), dt, 0);
      break;
    }
    
    break;
  }

  co->setSettings(set);
  
  QObject::connect(co, SIGNAL(signalDraw()), this, SLOT(draw()));
  QObject::connect(this, SIGNAL(signalMouseLeftClick(int, int)), co, SLOT(selected(int, int)));
  QObject::connect(co, SIGNAL(signalChartObjectSelected(ChartObject *)), this, SLOT(slotChartObjectSelected(ChartObject *)));
  QObject::connect(co, SIGNAL(signalDeleteChartObject(QString)), this, SLOT(slotDeleteChartObject(QString)));
  QObject::connect(co, SIGNAL(message(QString)), this, SLOT(slotMessage(QString)));
  
  chartObjects.replace(co->getName(), co);
}

QStringList Plot::getChartObjects ()
{
  QStringList l;
  QDictIterator<ChartObject> it(chartObjects);
  for (; it.current(); ++it)
    l.append(it.currentKey());

  return l;
}

void Plot::drawObjects ()
{
  QDictIterator<ChartObject> it(chartObjects);
  for (; it.current(); ++it)
  {
    int x = -1;
    int x2 = -1;
    bool flag = FALSE;
    bool flag2 = FALSE;
    
    ChartObject *co = it.current();
    
    switch(co->getType())
    {
      case ChartObject::BuyArrow:
      case ChartObject::SellArrow:
      case ChartObject::Text:
      case ChartObject::VerticalLine:
        flag = TRUE;
	break;
      case ChartObject::TrendLine:
      case ChartObject::FibonacciLine:
        flag = TRUE;
	flag = TRUE;
        flag2 = TRUE;
	flag2 = TRUE;
      default:
        break;
    }
    
    if (flag)
    {
      BarDate dt = co->getDate();
      if (dt.getDate().isValid())
      {
        x = getXFromDate(dt);
        if (x == -1)
          continue;
      }
      else
        continue;
    }

    if (flag2)
    {
      BarDate dt = co->getDate2();
      if (dt.getDate().isValid())
      {
        x2 = getXFromDate(dt);
        if (x2 == -1)
          continue;
      }
      else
        continue;
    }
    
    co->draw(x, x2);
  }
}

void Plot::slotSaveChartObjects ()
{
  if (! chartPath.length())
    return;
    
  ChartDb *db = new ChartDb();
  db->openChart(chartPath);

  QDictIterator<ChartObject> it(chartObjects);
  for (; it.current(); ++it)
  {
    ChartObject *co = it.current();
    
    if (co->getSaveFlag())
    {
      Setting *set = co->getSettings();
      db->setChartObject(co->getName(), set);
    }
  }
  
  delete db;
}

void Plot::slotChartObjectSelected (ChartObject *co)
{
  tco = co;
  mouseFlag = COSelected;
}

void Plot::slotDeleteAllChartObjects ()
{
  int  rc = QMessageBox::warning(this,
  			         tr("Qtstalker: Warning"),
				 tr("Are you sure you want to delete all chart objects?"),
				 QMessageBox::Yes,
				 QMessageBox::No,
				 QMessageBox::NoButton);

  if (rc == QMessageBox::No)
    return;
   
  ChartDb *db = new ChartDb();
  db->openChart(chartPath);
  QStringList l = db->getChartObjectsList();

  int loop;  
  for (loop = 0; loop < (int) l.count(); loop++)
  {
    db->deleteChartObject(l[loop]);
    chartObjects.remove(l[loop]);
  }
  
  delete db;
  mouseFlag = None;
  draw();
}
