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
#include "ChartDb.h"
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
#include <qdir.h>

#include "indicator.xpm"
#include "edit.xpm"
#include "delete.xpm"
#include "co.xpm"
#include "print.xpm"
#include "crosshair.xpm"

#define SCALE_WIDTH 60

Plot::Plot (QWidget *w) : QWidget(w)
{
  buffer = new QPixmap;
  scaler = new Scaler;
  chartPlugin = 0;
  coPlugin = 0;
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
  interval = BarData::DailyBar;
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
  crosshairs = TRUE;
  infoFlag = TRUE;
  coPlugin = 0;

  plotFont.setFamily("Helvetica");
  plotFont.setPointSize(12);
  plotFont.setWeight(50);

  indicators.setAutoDelete(TRUE);
  data = 0;

  chartMenu = new QPopupMenu();
  
  setMouseTracking(TRUE);

  setFocusPolicy(QWidget::ClickFocus);
  
  coList = config.getPluginList(Config::COPluginPath);
  coPlugins.setAutoDelete(FALSE);
}

Plot::~Plot ()
{
  delete buffer;
  delete scaler;
  delete chartMenu;
  config.closePlugins();
}

void Plot::clear ()
{
  mainHigh = -99999999;
  mainLow = 99999999;
  indicators.clear();
  data = 0;
  mouseFlag = None;
  crossHairFlag = FALSE;

  QDictIterator<COPlugin> it(coPlugins);
  for (; it.current(); ++it)
  {
    COPlugin *plug = it.current();
    plug->saveObjects(chartPath);
    plug->clear();
  }
  
  if (chartPlugin)
    chartPlugin->savePixelspace();
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
  }

  createXGrid();
  
  QDictIterator<COPlugin> it(coPlugins);
  for (; it.current(); ++it)
  {
    COPlugin *plug = it.current();
    plug->setData(data);
  }
}

int Plot::setChartType (QString d)
{
  if (chartType.length())
  {
    if (chartPlugin)
      chartPlugin->savePixelspace();
    config.closePlugin(chartType);
  }
  
  chartPlugin = config.getChartPlugin(d);
  if (! chartPlugin)
  {
    qDebug("Plot::setChartType:unable to open %s chart plugin", d.latin1());
    return TRUE;
  }
  
  if (chartType.length())
    setChartInput();
      
  chartType = d;
  
  minPixelspace = chartPlugin->getMinPixelspace();
  pixelspace = chartPlugin->getPixelspace();
  startX = chartPlugin->getStartX();
  dateFlag = TRUE;  
  
  QObject::connect(chartPlugin, SIGNAL(draw()), this, SLOT(draw()));
  
  return FALSE;
}

void Plot::setChartInput ()
{
  chartPlugin->setChartInput(data, scaler, buffer);
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
  
  if (drawMode)
    setCursor(QCursor(ArrowCursor));
  else
    setCursor(QCursor(CrossCursor));
    
  if (! drawMode && coPlugin)
  {
    coPlugin->pointerClick(QPoint(-1, -1), x1, y1);
    mouseFlag = None;
  }
}

bool Plot::getCrosshairsStatus ()
{
  return crosshairs;
}

void Plot::setInfoFlag (bool d)
{
  infoFlag = d;
}

void Plot::draw ()
{
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
    
    drawCrossHair();

    drawScale();

    drawInfo();

    if (dateFlag)
      drawDate();
  }

  paintEvent(0);
}

void Plot::drawRefresh ()
{
  paintEvent(0);
}

void Plot::drawLines ()
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

void Plot::paintEvent (QPaintEvent *)
{
  bitBlt(this, 0, 0, buffer);
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
    if (! indicators.count() || ! data)
      return;
  }

  if (event->x() > buffer->width() - SCALE_WIDTH)
    return;

  if (! drawMode)
  {
    if (event->button() == LeftButton)
    {
      if (crosshairs)
      {
        crossHair(event->x(), event->y(), TRUE);
        updateStatusBar(event->x(), event->y());
        emit leftMouseButton(event->x(), event->y(), mainFlag);
      }
      else
        updateStatusBar(event->x(), event->y());
      return;
    }
  }
    
  if (event->button() == LeftButton)
  {
    getXY(event->x(), event->y());
    
    if (mouseFlag == None)
    {
      QDictIterator<COPlugin> it(coPlugins);
      for (; it.current(); ++it)
      {
        coPlugin = it.current();
        COPlugin::Status rc = coPlugin->pointerClick(QPoint(event->x(), event->y()), x1, y1);
        if (rc != COPlugin::None)
        {
          mouseFlag = COSelected;
	  return;
        }
      }
    }
    
    if (mouseFlag == COSelected)
    {
      COPlugin::Status rc = coPlugin->pointerClick(QPoint(event->x(), event->y()), x1, y1);
      if (rc == COPlugin::Moving)
      {
        mouseFlag = Moving;
	return;
      }
      
      if (rc == COPlugin::None)
      {
        mouseFlag = None;
	return;
      }
    }
    
    if (mouseFlag == Moving)
    {
      coPlugin->pointerClick(QPoint(event->x(), event->y()), x1, y1);
      mouseFlag = COSelected;
      return;
    }
    
    if (mouseFlag == ClickWait)
    {
      COPlugin::Status rc = coPlugin->pointerClick(QPoint(event->x(), event->y()), x1, y1);
      if (rc == COPlugin::None)
      {
        mouseFlag = None;
        emit statusMessage("");
        setCursor(QCursor(Qt::ArrowCursor));
	coPlugin->saveObjects(chartPath);
	return;
      }
    }
  }
}

void Plot::contextMenuEvent (QContextMenuEvent *)
{
  if (drawMode && mouseFlag == COSelected && coPlugin)
    coPlugin->showMenu();
  else
    showPopupMenu();
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
    if (! indicators.count() || ! data)
      return;
  }

  if (event->x() > buffer->width() - SCALE_WIDTH)
    return;
   
  // ignore moves above the top of the chart - we get draw errors if we don't
  if (event->y() <= 0)
    return;
    
  // are we trying to drag a chart object?
  if (drawMode)
  {
    if (mouseFlag == Moving || mouseFlag == ClickWait)
    {
      getXY(event->x(), event->y());
      coPlugin->pointerMoving(QPoint(event->x(), event->y()), x1, y1);
      return;
    }
  }
  
  if (! infoFlag)
    return;
    
  int i = convertXToDataIndex(event->x());

  Setting *r = new Setting;
  r->setData("D", data->getDate(i).getDateString(TRUE));
  r->setData("T", data->getDate(i).getTimeString(TRUE));
  
  if (mainFlag)
  {
    r->setData("O", strip(data->getOpen(i), 4));
    r->setData("H", strip(data->getHigh(i), 4));
    r->setData("L", strip(data->getLow(i), 4));
    r->setData("C", strip(data->getClose(i), 4));
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
        r->setData(line->getLabel(), strip(line->getData(li), 4));
    }
  }

  if (r->count())
    emit infoMessage(r);
  else
    delete r;
}

void Plot::mouseDoubleClickEvent (QMouseEvent *event)
{
  if (mainFlag)
  {
    if (! data)
      return;
  }
  else
  {
    if (! indicators.count() || ! data)
      return;
  }

  if (event->x() > buffer->width() - SCALE_WIDTH)
    return;

  if (mouseFlag != COSelected)
    return;
    
  coPlugin->prefDialog();
}

void Plot::keyPressEvent (QKeyEvent *key)
{
  // if chart object selected then pass keyevent to it
  if (mouseFlag == COSelected)
  {
    coPlugin->keyEvent(key);
    return;
  }
  
  // process for plot keyevents
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

  if (chartPlugin)
    chartPlugin->setPixelspace(d);
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

void Plot::setInterval (BarData::BarCompression d)
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
  
  if (data->getBarType() == BarData::Tick)
  { 
    switch (interval)
    {
      case BarData::Minute5:
      case BarData::Minute15:
        draw15Date();
        break;
      case BarData::Minute30:
      case BarData::Minute60:
        drawHourlyDate();
        break;
      case BarData::WeeklyBar:
        drawWeeklyDate();
        break;
      case BarData::MonthlyBar:
        drawMonthlyDate();
        break;
      default:
        drawDailyDate();
        break;
    }
  }
  else
  {
    switch (interval)
    {
      case BarData::WeeklyBar:
        drawWeeklyDate();
        break;
      case BarData::MonthlyBar:
        drawMonthlyDate();
        break;
      default:
        drawDailyDate();
        break;
    }
  }
}

void Plot::draw15Date ()
{
  QPainter painter;
  painter.begin(buffer);
  painter.setPen(borderColor);
  painter.setFont(plotFont);

  QFontMetrics fm = painter.fontMetrics();
  
  int x = startX;
  int loop = startIndex;

  BarDate nextHour = data->getDate(loop);
  BarDate oldDay = data->getDate(loop);
  nextHour.setTime(nextHour.getHour(), 0, 0);
  if ((nextHour.getHour() % 2) == 0)
    nextHour.addSecs(7200);
  else
    nextHour.addSecs(3600);

  while(x <= _width && loop < (int) data->count())
  {
    BarDate date = data->getDate(loop);
    
    if (date.getDate().day() != oldDay.getDate().day())
    {
      painter.drawLine (x, _height + 1, x, _height + dateHeight - fm.height() - 1);
      QString s = date.getDate().toString("d");
      painter.drawText (x - (fm.width(s, -1) / 2), buffer->height() - 2, s, -1);
      oldDay = date;
    }
    else
    {
      // every 2 hours make a small tick
      if (date.getDateValue() >= nextHour.getDateValue())
      {
        painter.drawLine (x, _height + 1, x, _height + 4);

        QString s;
        if (date.getHour() >= 12)
	{
	  if (date.getHour() == 12)
            s = QString::number(date.getHour());
	  else
            s = QString::number(date.getHour() - 12);	
	  s.append("p");
	}
	else
	{
          s = QString::number(date.getHour());	
	  s.append("a");
	}
        painter.drawText (x - (fm.width(s, -1) / 2),
		          buffer->height() - dateHeight + fm.height() + 1,
			  s,
			  -1);
      }
    }
    
    if (date.getDateValue() >= nextHour.getDateValue())
    {
      nextHour = date;
      nextHour.setTime(date.getHour(), 0, 0);
      if ((date.getHour() % 2) == 0)
        nextHour.addSecs(7200);
      else
        nextHour.addSecs(3600);
    }

    x = x + pixelspace;
    loop++;
  }

  painter.end();
}

void Plot::drawHourlyDate ()
{
  QPainter painter;
  painter.begin(buffer);
  painter.setPen(borderColor);
  painter.setFont(plotFont);

  QFontMetrics fm = painter.fontMetrics();
  
  int x = startX;
  int loop = startIndex;

  QDate oldDay = data->getDate(loop).getDate();

  while(x <= _width && loop < (int) data->count())
  {
    QDate date = data->getDate(loop).getDate();

    if (date.day() != oldDay.day())
    {
      oldDay = date;
      painter.drawLine (x, _height + 1, x, _height + (dateHeight / 3));
      QString s = date.toString("d");
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
  BarData::BarType barType = data->getBarType();

  for (loop = 0; loop < (int) data->count(); loop++)
  {
    QDate date = data->getDate(loop).getDate();
    
    if (barType == BarData::Daily)
    {
      switch (interval)
      {
        case BarData::WeeklyBar:
        case BarData::MonthlyBar:
          if (date.year() != oldDate.year())
          {
            oldDate = date;
	    xGrid.resize(xGrid.size() + 1);
	    xGrid[xGrid.size() - 1] = loop;
          }
          break;
        default:
          if (date.month() != oldDate.month())
          {
            oldDate = date;
	    xGrid.resize(xGrid.size() + 1);
	    xGrid[xGrid.size() - 1] = loop;
          }
          break;
      }
    }
    else
    {
      switch (interval)
      {
        case BarData::Minute5:
        case BarData::Minute15:
        case BarData::Minute30:
        case BarData::Minute60:
          if (date.day() != oldDate.day())
          {
            oldDate = date;
	    xGrid.resize(xGrid.size() + 1);
	    xGrid[xGrid.size() - 1] = loop;
          }
          break;
        case BarData::DailyBar:
          if (date.month() != oldDate.month())
          {
            oldDate = date;
	    xGrid.resize(xGrid.size() + 1);
	    xGrid[xGrid.size() - 1] = loop;
          }
          break;
        case BarData::WeeklyBar:
        case BarData::MonthlyBar:
          if (date.year() != oldDate.year())
          {
            oldDate = date;
	    xGrid.resize(xGrid.size() + 1);
	    xGrid[xGrid.size() - 1] = loop;
          }
          break;
        default:
          break;
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

bool Plot::deleteIndicator (QString d)
{
  return indicators.remove(d);
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
    QString s = strip(scaleArray[loop], 4);
    
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
        s = strip(s.toDouble() / 1000000000, 4);
	s.append("b");
      }
      else
      {
        if (s.toDouble() >= 1000000)
        {
          s = strip(s.toDouble() / 1000000, 4);
	  s.append("m");
        }
        else
        {
          if (s.toDouble() >= 1000)
          {
            s = strip(s.toDouble() / 1000, 4);
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
    double ch = 0;
    double per = 0;
    if (data->count() > 1)
    {
      ch = data->getClose(data->count() - 1) - data->getClose(data->count() - 2);
      per = (ch / data->getClose(data->count() - 2)) * 100;
    }
    s = "CH=";
    s.append(strip(ch, 4));
    s.append(" (");
    s.append(strip(per, 2));
    s.append("%) ");
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
  
    painter.setPen(borderColor);
    
    s = "O=";
    s.append(strip(data->getOpen(data->count() - 1), 4));
    s.append(" H=");
    s.append(strip(data->getHigh(data->count() - 1), 4));
    s.append(" L=");
    s.append(strip(data->getLow(data->count() - 1), 4));
    s.append(" C=");
    s.append(strip(data->getClose(data->count() - 1), 4));
    s.append(" ");
    painter.drawText(pos, 10, s, -1);
    pos = pos + fm.width(s);
  }

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
        s.append(strip(line->getData(line->getSize() - 1), 4));
        s.append(" ");

        painter.setPen(line->getColor());
        painter.drawText(pos, 10, s, -1);
        pos = pos + fm.width(s);
      }
    }
  }
  
  painter.end();
}

void Plot::crossHair (int x, int y, bool f)
{
  crossHairFlag = TRUE;
  getXY(x, y);
  crossHairY = y1;
  crossHairX = x1;
  if (f)
    draw();
}

void Plot::drawCrossHair ()
{
  if (! crosshairs)
    return;
    
  if (! crossHairFlag)
    return;
    
  int y = scaler->convertToY(crossHairY);
  int x = startX + (data->getX(crossHairX) * pixelspace) - (startIndex * pixelspace);

  QPainter painter;
  painter.begin(buffer);
  painter.setPen(QPen(borderColor, 1, QPen::DotLine));
  painter.drawLine (0, y, buffer->width() - SCALE_WIDTH, y);
  painter.drawLine (x, 0, x, buffer->height());
  painter.end();
}

void Plot::updateStatusBar (int x, int y)
{
  int i = convertXToDataIndex(x);
  QString s = data->getDate(i).getDateTimeString(TRUE);
  s.append(" ");
  s.append(strip(scaler->convertToVal(y), 4));
  emit statusMessage(s);
}

void Plot::getXY (int x, int y)
{
  int i = convertXToDataIndex(x);
  x1 = data->getDate(i);
  y1 = scaler->convertToVal(y);
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

        if (line->getScaleFlag())
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

        if (line->getScaleFlag())
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

  if (! scaleToScreen)
  {
    QDictIterator<COPlugin> it(coPlugins);
    for (; it.current(); ++it)
    {
      COPlugin *plug = it.current();
      
      if (plug->getHigh() > scaleHigh)
        scaleHigh = plug->getHigh();
	
      if (plug->getLow() < scaleLow)
        scaleLow = plug->getLow();
    }
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

QString Plot::strip (double d, int p)
{
  QString s = QString::number(d, 'f', p);

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
  chartMenu->clear();
    
  if (mainFlag && data)
  {
    chartMenu->insertItem(tr("&Chart Prefs"), this, SLOT(slotEditChartPrefs()), CTRL+Key_C);
    chartMenu->insertSeparator ();
  }
  
  chartMenu->insertItem(QPixmap(indicator), tr("New &Indicator"), this, SLOT(slotNewIndicator()), CTRL+Key_I);
  chartEditMenu = new QPopupMenu();
  chartMenu->insertItem(QPixmap(edit), tr("Edit Indicator"), chartEditMenu);
  chartMenu->insertSeparator ();

  chartObjectMenu = new QPopupMenu();
  int loop;
  for (loop = 0; loop < (int) coList.count(); loop++)
  {
    int id = chartObjectMenu->insertItem(coList[loop], this, SLOT(slotNewChartObject(int)));
    chartObjectMenu->setItemParameter(id, id);
  }

  int id = chartMenu->insertItem (QPixmap(co), tr("New Chart Object"), chartObjectMenu);
  
  if (! drawMode || ! data)
    chartObjectMenu->setEnabled(FALSE);
  else
    chartObjectMenu->setEnabled(TRUE);
  
  chartMenu->insertItem(QPixmap(deleteitem), tr("Delete &All Chart Objects"), this,
                        SLOT(slotDeleteAllChartObjects()), CTRL+Key_A);

  chartMenu->insertSeparator ();
  chartMenu->insertItem(QPixmap(print), tr("&Print Chart"), this, SLOT(printChart()), CTRL+Key_P);
    
  chartMenu->insertSeparator ();
  id = chartMenu->insertItem(QPixmap(crosshair), tr("Cross&hairs"), this,
                             SLOT(toggleCrosshairs()), CTRL+Key_H);
  chartMenu->setItemChecked(id, crosshairs);
  
  QDictIterator<Indicator> it(indicators);
  for(; it.current(); ++it)
  {
    if (it.currentKey().compare("Main Plot"))
    {
      int id = chartEditMenu->insertItem(QPixmap(indicator), it.currentKey(), this, SLOT(slotEditIndicator(int)));
      chartEditMenu->setItemParameter(id, id);
    }
  }

  chartMenu->exec(QCursor::pos());
}

void Plot::toggleCrosshairs ()
{
  if (crosshairs == FALSE)
    crosshairs = TRUE;
  else
    crosshairs = FALSE;
  emit signalCrosshairsStatus(crosshairs);  
}

void Plot::setCrosshairsStatus (bool status)
{
  crosshairs = status;
  crossHairFlag = FALSE;
  draw();
}

void Plot::slotEditIndicator (int id)
{
  QString s = chartEditMenu->text(id);
  emit signalEditIndicator(s);
}

void Plot::slotNewIndicator ()
{
  emit signalNewIndicator();
}

void Plot::slotEditChartPrefs ()
{
  chartPlugin->prefDialog(this);
}

void Plot::slotMessage (QString d)
{
  emit statusMessage(d);
}

void Plot::slotSliderChanged (int v)
{
  setIndex(v);
  draw();
}

void Plot::slotGridChanged (bool d)
{
  setGridFlag(d);
  draw();
}

void Plot::slotScaleToScreenChanged (bool d)
{
  setScaleToScreen(d);
  draw();
}

void Plot::slotDrawModeChanged (bool d)
{
  setDrawMode(d);
}

void Plot::slotDateFlagChanged (bool d)
{
  setDateFlag(d);
  draw();
}

void Plot::slotLogScaleChanged (bool d)
{
  setLogScale(d);
  draw();
}

void Plot::slotHideMainChanged (bool d)
{
  setHideMainPlot(d);
  draw();
}

//*************************************************************************
//******************** chart object functions *****************************
//*************************************************************************

void Plot::slotNewChartObject (int id)
{
  if (! chartPath.length())
    return;
    
  QDir dir;
  if (! dir.exists(chartPath))
    return;

  QString selection = chartObjectMenu->text(id);
  
  coPlugin = coPlugins[selection];
  if (! coPlugin)
  {
    coPlugin = config.getCOPlugin(selection);
    if (! coPlugin)
      return;
      
    coPlugin->setData(data);
    coPlugin->setScaler(scaler);
    coPlugin->setPixmap(buffer);
    QObject::connect(coPlugin, SIGNAL(signalDraw()), this, SLOT(draw()));
    QObject::connect(coPlugin, SIGNAL(signalRefresh()), this, SLOT(drawRefresh()));
    QObject::connect(coPlugin, SIGNAL(message(QString)), this, SLOT(slotMessage(QString)));
    coPlugins.replace(selection, coPlugin);
  }
  
  QStringList l;
  ChartDb *db = new ChartDb;
  db->openChart(chartPath);
  l = db->getChartObjectsList();
  delete db;
  
  int loop = 0;
  QString name;
  while (1)
  {
    name = QString::number(loop);
    if (l.findIndex(name) != -1)
      loop++;
    else
      break;
  }
  
  QString pl = "Main Plot";
  if (! mainFlag)
  {
    QDictIterator<Indicator> it(indicators);
    it.toFirst();
    pl = it.currentKey();
  }
  
  coPlugin->newObject(pl, name);
  
  setCursor(QCursor(Qt::PointingHandCursor));
  mouseFlag = ClickWait;
}

void Plot::addChartObject (Setting *set)
{
  // routine to convert old chart objects to plugin format
  // remove later when users have updated
  QString s = set->getData("Plugin");
  if (! s.length())
  {
    s = set->getData("ObjectType");
    
    switch (s.toInt())
    {
      case 0:
        set->setData("Plugin", "VerticalLine");
	break;
      case 1:
        set->setData("Plugin", "HorizontalLine");
	break;
      case 2:
        set->setData("Plugin", "TrendLine");
	break;
      case 3:
        set->setData("Plugin", "Text");
	break;
      case 4:
        set->setData("Plugin", "BuyArrow");
	break;
      case 5:
        set->setData("Plugin", "SellArrow");
	break;
      case 6:
        set->setData("Plugin", "FiboLine");
	break;
      default:
	break;
    }
  }

  COPlugin *plug = coPlugins[set->getData("Plugin")];
  if (! plug)
  {
    plug = config.getCOPlugin(set->getData("Plugin"));
    if (! plug)
      return;
      
    plug->setData(data);
    plug->setScaler(scaler);
    plug->setPixmap(buffer);
    QObject::connect(plug, SIGNAL(signalDraw()), this, SLOT(draw()));
    QObject::connect(plug, SIGNAL(signalRefresh()), this, SLOT(drawRefresh()));
    QObject::connect(plug, SIGNAL(message(QString)), this, SLOT(slotMessage(QString)));
    coPlugins.replace(set->getData("Plugin"), plug);
  }
  
  plug->addObject(set);
}

void Plot::drawObjects ()
{
  QDictIterator<COPlugin> it(coPlugins);
  for (; it.current(); ++it)
  {
    COPlugin *plug = it.current();
    plug->draw(startIndex, pixelspace, startX);
  }
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
   
  if (! chartPath.length())
    return;
    
  QDir dir;
  if (! dir.exists(chartPath))
    return;
    
  ChartDb *db = new ChartDb;
  db->openChart(chartPath);
  
  QStringList l = db->getChartObjectsList();

  int loop;  
  for (loop = 0; loop < (int) l.count(); loop++)
    db->deleteChartObject(l[loop]);
  delete db;
  
  QDictIterator<COPlugin> it(coPlugins);
  for (; it.current(); ++it)
  {
    COPlugin *plug = it.current();
    plug->clear();
  }
  
  mouseFlag = None;
  draw();
}

int Plot::convertXToDataIndex (int x)
{
  int i = (x / pixelspace) + startIndex;
  if (i >= (int) data->count())
    i = data->count() - 1;
  if (i < startIndex)
    i = startIndex;
  
  return i;
}

void Plot::setCrosshairsFlag (bool d)
{
  crosshairs = d;
}

