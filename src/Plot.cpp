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
  setBackgroundMode(NoBackground);
  scaleWidth = SCALE_WIDTH;
  dateHeight = DATE_HEIGHT;
  _height = 0;
  _width = 0;
  startX = 2;
  backgroundColor.setNamedColor("black");
  borderColor.setNamedColor("white");
  gridColor.setNamedColor("#626262");
  upColor.setNamedColor("green");
  downColor.setNamedColor("red");
  neutralColor.setNamedColor("blue");
  candleColor.setNamedColor("green");
  pixelspace = 0;
  minPixelspace = 0;
  dateFlag = FALSE;
  gridFlag = TRUE;
  interval = Daily;
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
  PAFBoxSize = 0;
  PAFReversal = 3;
  chartMenu = 0;

  plotFont.setFamily("Helvetica");
  plotFont.setPointSize(12);
  plotFont.setWeight(50);

  indicators.setAutoDelete(TRUE);
  paintBars.setAutoDelete(TRUE);
  data = 0;

  setMouseTracking(TRUE);

  setFocusPolicy(QWidget::ClickFocus);

  // set up the popup menu
  chartMenu = new QPopupMenu();
  chartMenu->insertItem(QPixmap(indicator), tr("New Indicator"), this, SLOT(slotNewIndicator()));
  chartDeleteMenu = new QPopupMenu();
  chartEditMenu = new QPopupMenu();
  chartMenu->insertItem(QPixmap(edit), tr("Edit Indicator"), chartEditMenu);
  chartMenu->insertItem (QPixmap(deletefile), tr("Delete Indicator"), chartDeleteMenu);
  chartMenu->insertSeparator ();
  chartObjectDeleteMenu = new QPopupMenu();
  chartObjectEditMenu = new QPopupMenu();

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

  chartMenu->insertItem (QPixmap(edit), tr("Edit Chart Object"), chartObjectEditMenu);
  chartMenu->insertItem (QPixmap(deletefile), tr("Delete Chart Object"), chartObjectDeleteMenu);

  chartMenu->insertSeparator ();
  chartMenu->insertItem(QPixmap(print), tr("Print Chart"), this, SLOT(printChart()));
}

Plot::~Plot ()
{
}

void Plot::clear ()
{
  mainHigh = -99999999;
  mainLow = 99999999;
  indicators.clear();
  data = 0;
  paintBars.clear();

  if (mainFlag)
    indicators.replace("Main Plot", new Indicator);
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
    
    int loop;
    for (loop = 0; loop < (int) data->count(); loop++)
      paintBars.append(new QColor(neutralColor.red(), neutralColor.green(), neutralColor.blue()));
  }

  createXGrid();
}

void Plot::setChartType (QString d)
{
  while (1)
  {
    if (! d.compare(tr("Bar")))
    {
      minPixelspace = 4;
      startX = 2;
      chartType = d;
      dateFlag = TRUE;
      break;
    }

    if (! d.compare(tr("Candle")) || ! d.compare(tr("Candle 2")))
    {
      minPixelspace = 6;
      startX = 2;
      chartType = d;
      dateFlag = TRUE;
      break;
    }

    if (! d.compare(tr("Line")))
    {
      minPixelspace = 3;
      startX = 0;
      chartType = d;
      dateFlag = TRUE;
      break;
    }

    if (! d.compare(tr("Paint Bar")))
    {
      minPixelspace = 4;
      startX = 2;
      chartType = d;
      dateFlag = TRUE;
      break;
    }

    if (! d.compare(tr("P&F")))
    {
      minPixelspace = 4;
      startX = 0;
      chartType = d;
      dateFlag = TRUE;
      break;
    }

    if (! d.compare(tr("Swing")))
    {
      minPixelspace = 4;
      startX = 0;
      chartType = d;
      dateFlag = TRUE;
      break;
    }

    break;
  }

  if (minPixelspace > pixelspace)
    pixelspace = minPixelspace;
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

void Plot::draw ()
{
  crossHairFlag = FALSE;

  buffer.fill(backgroundColor);

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

    setHeight();

    setWidth();

    setScale();

    drawXGrid();

    drawYGrid();

    if (mainFlag)
    {
      while (1)
      {
        if (hideMainPlot == TRUE)
	{
          drawLines();
          drawObjects();
          break;
	}

        if (! chartType.compare(tr("Bar")))
        {
          drawBars();
          drawLines();
          drawObjects();
          break;
        }

        if (! chartType.compare(tr("Candle")))
        {
          drawCandle();
          drawLines();
          drawObjects();
          break;
        }

        if (! chartType.compare(tr("Candle 2")))
        {
          drawCandle2();
          drawLines();
          drawObjects();
          break;
        }
        
	if (! chartType.compare(tr("Line")))
        {
          drawLineChart();
          drawLines();
          drawObjects();
          break;
        }

        if (! chartType.compare(tr("Paint Bar")))
        {
          drawPaintBar();
          drawLines();
          drawObjects();
          break;
        }

        if (! chartType.compare(tr("P&F")))
        {
          drawPointAndFigure();
	  break;
	}

        if (! chartType.compare(tr("Swing")))
        {
          drawSwing();
	  break;
	}

	break;
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

void Plot::drawObjects ()
{
  Indicator *i;
  if (mainFlag)
    i = indicators["Main Plot"];
  else
  {
    QDictIterator<Indicator> it(indicators);
    it.toFirst();
    i = it.current();
  }

  QStringList l = i->getChartObjects();

  int loop;
  for (loop = 0; loop < (int) l.count(); loop++)
  {
    Setting *co = i->getChartObject(l[loop]);

    switch (co->getInt("ObjectType"))
    {
      case Plot::VerticalLine:
        drawVerticalLine(co);
	break;
      case Plot::HorizontalLine:
        drawHorizontalLine(co);
	break;
      case Plot::TrendLine:
        drawTrendLine(co);
	break;
      case Plot::Text:
        drawText(co);
	break;
      case Plot::BuyArrow:
        drawBuyArrow(co);
	break;
      case Plot::SellArrow:
        drawSellArrow(co);
	break;
      case Plot::FibonacciLine:
        drawFibonacciLine(co);
	break;
    }
  }
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

        while (1)
        {
          if (! currentLine->getType().compare("Histogram"))
          {
            drawHistogram();
	    break;
          }

          if (! currentLine->getType().compare("Histogram Bar"))
          {
            drawHistogramBar();
	    break;
          }

          if (! currentLine->getType().compare("Dot"))
          {
            drawDot();
	    break;
          }

          if (! currentLine->getType().compare("Line") || ! currentLine->getType().compare("Dash"))
          {
            drawLine();
	    break;
          }

          if (! currentLine->getType().compare("Horizontal"))
            drawHorizontalLine();

          break;
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
    for (loop = 0; loop < i->getLines(); loop++)
    {
      currentLine = i->getLine(loop);

      while (1)
      {
        if (! currentLine->getType().compare("Histogram"))
        {
          drawHistogram();
	  break;
        }

        if (! currentLine->getType().compare("Histogram Bar"))
        {
          drawHistogramBar();
	  break;
        }

        if (! currentLine->getType().compare("Dot"))
        {
          drawDot();
	  break;
        }
	
        if (! currentLine->getType().compare("Line") || ! currentLine->getType().compare("Dash"))
        {
          drawLine();
	  break;
        }

        if (! currentLine->getType().compare("Horizontal"))
          drawHorizontalLine();

        break;
      }
    }
  }
}

void Plot::paintEvent (QPaintEvent *)
{
  bitBlt(this, 0, 0, &buffer);

  // redraw the crosshair
  if (crossHairFlag)
  {
    QPainter painter;
    painter.begin(this);
    painter.setPen(QPen(borderColor, 1, QPen::DotLine));
    painter.drawLine (0, crossHairY, buffer.width() - SCALE_WIDTH, crossHairY);
    painter.drawLine (crossHairX, 0, crossHairX, buffer.height());
    painter.end();
  }
}

void Plot::resizeEvent (QResizeEvent *event)
{
  buffer.resize(event->size());

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

  if (event->x() > buffer.width() - SCALE_WIDTH)
    return;

  if (mouseFlag != None)
  {
    if (event->button() == LeftButton)
    {
      switch (mouseFlag)
      {
        case ClickWait2:
	  getXY(event->x(), event->y(), 1);
          newChartObject();
	  break;
        default:
	  getXY(event->x(), event->y(), 0);
          if (objectFlag == TrendLine || objectFlag == FibonacciLine)
	  {
            mouseFlag = ClickWait2;
            if (objectFlag == TrendLine)
             emit statusMessage(tr("Select end point of trend line..."));
	    else
             emit statusMessage(tr("Select low point of fibonacci line..."));
	  }
	  else
	    newChartObject();
	  break;
      }

      return;
    }

    return;
  }

  switch(event->button())
  {
    case LeftButton:
      crossHair(event->x(), event->y());
      updateStatusBar(event->x(), event->y());
      emit leftMouseButton(event->x(), event->y(), mainFlag);
      break;
    case RightButton:
      showPopupMenu();
      break;
    default:
      break;
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

  if (event->x() > buffer.width() - SCALE_WIDTH)
    return;

  int i = (event->x() / pixelspace) + startIndex;
  if (i >= (int) data->count())
    i = data->count() - 1;
  if (i < startIndex)
    i = startIndex;

  Setting *r = new Setting;

  r->set("D", data->getDate(i).toString("yyyyMMdd"), Setting::Date);
  
  if (mainFlag && data->getType() == BarData::Bars)
  {
    r->set("O", strip(data->getOpen(i)), Setting::Float);
    r->set("H", strip(data->getHigh(i)), Setting::Float);
    r->set("L", strip(data->getLow(i)), Setting::Float);
    r->set("C", strip(data->getClose(i)), Setting::Float);
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
        r->set(line->getLabel(), strip(line->getData(li)), Setting::Float);
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

void Plot::setInterval (Plot::TimeInterval d)
{
  interval = d;
}

void Plot::setScaleWidth (int d)
{
  if (d > 999 || d < SCALE_WIDTH)
    return;
  else
    scaleWidth = d;
}

void Plot::setDateHeight (int d)
{
  if (d > 999 || d < DATE_HEIGHT)
    return ;
  else
    dateHeight = d;
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

void Plot::setUpColor (QColor d)
{
  upColor = d;
}

void Plot::setDownColor (QColor d)
{
  downColor = d;
}

void Plot::setNeutralColor (QColor d)
{
  neutralColor = d;
}

void Plot::setCandleColor (QColor d)
{
  candleColor = d;
}

void Plot::setPlotFont (QFont d)
{
  plotFont = d;
}

void Plot::setDateFlag (bool d)
{
  dateFlag = d;
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

void Plot::setPAFBoxSize (double d)
{
  PAFBoxSize = d;
}

void Plot::setPAFReversal (int d)
{
  PAFReversal = d;
}

bool Plot::getMainFlag ()
{
  return mainFlag;
}

void Plot::drawDate ()
{
  QPainter painter;
  painter.begin(&buffer);
  painter.setPen(borderColor);
  painter.setFont(plotFont);

  QFontMetrics fm = painter.fontMetrics();

  // clear date area
  painter.fillRect(0, buffer.height() - dateHeight, buffer.width() - scaleWidth, dateHeight, backgroundColor);

  // draw the seperator line
  painter.drawLine (0, buffer.height() - dateHeight, buffer.width() - scaleWidth, buffer.height() - dateHeight);

  int x = startX;
  int loop = startIndex;

  if (interval == Daily)
  {
    QDateTime oldDate = data->getDate(loop);
    QDateTime oldWeek = oldDate;
    oldWeek = oldWeek.addDays(7 - oldWeek.date().dayOfWeek());

    while(x <= _width && loop < (int) data->count())
    {
      QDateTime date = data->getDate(loop);

      if (date.date().month() != oldDate.date().month())
      {
        oldDate = date;

        painter.drawLine (x, _height + 1, x, _height + dateHeight - fm.height() - 1);

        QString s = date.toString("MMM'yy");
        painter.drawText (x - (fm.width(s, -1) / 2), buffer.height() - 2, s, -1);

        oldWeek = date;
        oldWeek = oldWeek.addDays(7 - oldWeek.date().dayOfWeek());
      }
      else
      {
        // if start of new week make a tick
        if (date > oldWeek)
	{
          painter.drawLine (x, _height + 1, x, _height + 4);

          QString s = date.toString("d");
          painter.drawText (x - (fm.width(s, -1) / 2),
	   		    buffer.height() - dateHeight + fm.height() + 1,
			    s,
			    -1);

  	  oldWeek = date;
          oldWeek = oldWeek.addDays(7 - oldWeek.date().dayOfWeek());
	}
      }

      x = x + pixelspace;
      loop++;
    }
  }
  else
  {
    if (interval == Weekly)
    {
      QDateTime oldMonth = data->getDate(loop);

      while(x <= _width && loop < (int) data->count())
      {
        QDateTime date = data->getDate(loop);

        if (date.date().month() != oldMonth.date().month())
        {
          oldMonth = date;

          painter.drawLine (x, _height + 1, x, _height + (dateHeight / 3));

	  QString s;
   	  s = date.toString("MMM'yy");

	  if (fm.width(s, -1) > 4 * pixelspace)
	  {
            if (date.date().month() == 1)
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
    }
    else
    {
      QDateTime oldYear = data->getDate(loop);

      while(x <= _width && loop < (int) data->count())
      {
        QDateTime date = data->getDate(loop);

        if (date.date().year() != oldYear.date().year())
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
    }
  }

  painter.end();
}

void Plot::drawXGrid ()
{
  if (gridFlag == FALSE)
    return;

  QPainter painter;
  painter.begin(&buffer);
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

  QDateTime oldDate = data->getDate(loop);

  for (loop = 0; loop < (int) data->count(); loop++)
  {
    QDateTime date = data->getDate(loop);

    if (interval == Daily)
    {
      if (date.date().month() != oldDate.date().month())
      {
        oldDate = date;
	xGrid.resize(xGrid.size() + 1);
	xGrid[xGrid.size() - 1] = loop;
      }
    }
    else
    {
      if (date.date().year() != oldDate.date().year())
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
  _height = buffer.height();
  if (dateFlag)
    _height = _height - dateHeight - 1;
}

void Plot::setWidth ()
{
  _width = buffer.width() - scaleWidth - startX;
}

void Plot::drawScale ()
{
  QPainter painter;
  painter.begin(&buffer);
  painter.setFont(plotFont);

  QFontMetrics fm = painter.fontMetrics();

  painter.fillRect(buffer.width() - scaleWidth, 0, scaleWidth, _height + 1, backgroundColor);

  int x = buffer.width() - scaleWidth;

  int loop;
  for (loop = 0; loop < (int) scaleArray.size(); loop++)
  {
    int y = scaler.convertToY(scaleArray[loop]);
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

  painter.end();
}

void Plot::drawYGrid ()
{
  if (! gridFlag)
    return;

  QPainter painter;
  painter.begin(&buffer);
  painter.setPen(QPen(gridColor, 1, QPen::DotLine));

  int loop;
  for (loop = 0; loop < (int) scaleArray.size(); loop++)
  {
    int y = scaler.convertToY(scaleArray[loop]);
    painter.drawLine (startX, y, _width, y);
  }

  painter.end();
}

void Plot::drawLine ()
{
  QPainter painter;
  painter.begin(&buffer);

  QPen pen;
  pen.setColor(currentLine->getColor());

  if (! currentLine->getType().compare("Dash"))
    pen.setStyle(Qt::DotLine);
  else
    pen.setStyle(Qt::SolidLine);
  painter.setPen(pen);

  int x = -1;
  int x2 = startX;
  int y = -1;
  int y2 = -1;
  int loop = currentLine->getSize() - data->count() + startIndex;

  Scaler *scale = new Scaler;
  scale->set(_height,
  	     currentLine->getHigh(),
	     currentLine->getLow(),
	     scaler.getLogScaleHigh(),
	     scaler.getLogRange(),
	     dateFlag,
	     logScale);

  while ((x2 < _width) && (loop < (int) currentLine->getSize()))
  {
    if (loop > -1)
    {
      if (currentLine->getScaleFlag())
        y2 = scale->convertToY(currentLine->getData(loop));
      else
        y2 = scaler.convertToY(currentLine->getData(loop));

      if (y != -1)
        painter.drawLine (x, y, x2, y2);
      x = x2;
      y = y2;
    }

    x2 = x2 + pixelspace;
    loop++;
  }

  painter.end();

  delete scale;
}

void Plot::drawHorizontalLine ()
{
  QPainter painter;
  painter.begin(&buffer);
  painter.setFont(plotFont);

  QPen pen;
  pen.setColor(currentLine->getColor());
  painter.setPen(pen);

  int y = scaler.convertToY(currentLine->getData(currentLine->getSize() - 1));

  painter.drawLine (0, y, _width, y);

  painter.drawText(startX, y - 1, strip(currentLine->getData(currentLine->getSize() - 1)));

  painter.end();
}

void Plot::drawDot ()
{
  QPainter painter;
  painter.begin(&buffer);

  QPen pen;
  pen.setColor(currentLine->getColor());
  painter.setPen(pen);

  int x = startX;
  int loop = currentLine->getSize() - data->count() + startIndex;

  Scaler *scale = new Scaler;
  scale->set(_height,
  	     currentLine->getHigh(),
	     currentLine->getLow(),
	     scaler.getLogScaleHigh(),
	     scaler.getLogRange(),
	     dateFlag,
	     logScale);

  while ((x < _width) && (loop < (int) currentLine->getSize()))
  {
    if (loop > -1)
    {
      int y;
      if (currentLine->getScaleFlag())
        y = scale->convertToY(currentLine->getData(loop));
      else
        y = scaler.convertToY(currentLine->getData(loop));

      painter.drawPoint(x, y);
    }

    x = x + pixelspace;
    loop++;
  }

  painter.end();

  delete scale;
}

void Plot::drawHistogram ()
{
  QPainter painter;
  painter.begin(&buffer);
  painter.setPen(currentLine->getColor());
  painter.setBrush(currentLine->getColor());

  Scaler *scale = new Scaler;
  scale->set(_height,
  	     currentLine->getHigh(),
	     currentLine->getLow(),
	     scaler.getLogScaleHigh(),
	     scaler.getLogRange(),
	     dateFlag,
	     logScale);

  int loop = currentLine->getSize() - data->count() + startIndex;

  QPointArray pa(4);

  int zero = 0;
  if (currentLine->getScaleFlag())
    zero = scale->convertToY(0);
  else
    zero = scaler.convertToY(0);

  int x = -1;
  int x2 = startX;
  int y = -1;
  int y2 = -1;

  while ((x < _width) && (loop < (int) currentLine->getSize()))
  {
    if (loop > -1)
    {
      if (currentLine->getScaleFlag())
        y2 = scale->convertToY(currentLine->getData(loop));
      else
        y2 = scaler.convertToY(currentLine->getData(loop));
      pa.setPoint(0, x, zero);
      pa.setPoint(1, x, y);
      pa.setPoint(2, x2, y2);
      pa.setPoint(3, x2, zero);

      if (y != -1)
        painter.drawPolygon(pa, TRUE, 0, -1);

      x = x2;
      y = y2;
    }

    x2 = x2 + pixelspace;
    loop++;
  }

  painter.end();

  delete scale;
}

void Plot::drawHistogramBar ()
{
  QPainter painter;
  painter.begin(&buffer);

  QColor color(currentLine->getColor());

  Scaler *scale = new Scaler;
  scale->set(_height,
  	     currentLine->getHigh(),
	     currentLine->getLow(),
	     scaler.getLogScaleHigh(),
	     scaler.getLogRange(),
	     dateFlag,
	     logScale);

  int x = startX;
  int zero = 0;
  if (currentLine->getScaleFlag())
    zero = scale->convertToY(0);
  else
    zero = scaler.convertToY(0);

  int loop = currentLine->getSize() - data->count() + startIndex;

  while ((x < _width) && (loop < (int) currentLine->getSize()))
  {
    if (loop > -1)
    {
      int y;
      if (currentLine->getScaleFlag())
        y = scale->convertToY(currentLine->getData(loop));
      else
        y = scaler.convertToY(currentLine->getData(loop));

      if (currentLine->getColorFlag() == TRUE)
	color.setNamedColor(currentLine->getColorBar(loop));

      painter.fillRect(x, y, pixelspace - 1, zero - y, color);
    }

    x = x + pixelspace;
    loop++;
  }

  painter.end();

  delete scale;
}

void Plot::drawInfo ()
{
  QPainter painter;
  painter.begin(&buffer);
  painter.setPen(borderColor);
  painter.setFont(plotFont);

  QFontMetrics fm = painter.fontMetrics();
  int pos = startX;

  QString s = "D=";
  QDateTime date = data->getDate(data->count() - 1);
  s.append(date.toString("yyyyMMdd"));
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
      painter.setPen(downColor);
    else
    {
      if (ch > 0)
        painter.setPen(upColor);
      else
        painter.setPen(neutralColor);
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
  painter.drawLine (0, y, buffer.width() - SCALE_WIDTH, y);
  painter.drawLine (x, 0, x, buffer.height());
  painter.end();
}

void Plot::updateStatusBar (int x, int y)
{
  int i = (x / pixelspace) + startIndex;
  if (i >= (int) data->count())
    i = data->count() - 1;
  if (i < startIndex)
    i = startIndex;

  QDateTime date = data->getDate(i);
  QString s = date.toString("yyyyMMdd");
  s.append(" ");
  s.append(strip(scaler.convertToVal(y)));
  emit statusMessage(s);
}

void Plot::getXY (int x, int y, int f)
{
  int i = (x / pixelspace) + startIndex;
  if (i >= (int) data->count())
    i = data->count() - 1;
  if (i < startIndex)
    i = startIndex;

  QDateTime date = data->getDate(i);

  if (f == 0)
  {
    x1 = date.toString("yyyyMMdd");
    y1 = strip(scaler.convertToVal(y));
  }
  else
  {
    x2 = date.toString("yyyyMMdd");
    y2 = strip(scaler.convertToVal(y));
  }
}

void Plot::newChartObject ()
{
  bool cancelFlag = FALSE;
  
  Setting *set = new Setting();
  if (mainFlag)
    set->set(QObject::tr("Plot"), QObject::tr("Main Plot"), Setting::None);
  else
  {
    QDictIterator<Indicator> it(indicators);
    it.toFirst();
    set->set(QObject::tr("Plot"), it.currentKey(), Setting::None);
  }
  set->set("Name", objectName, Setting::None);
  set->set("ObjectType", QString::number(objectFlag), Setting::None);

  switch(objectFlag)
  {
    case BuyArrow:
      set->set(QObject::tr("Date"), x1, Setting::Date);
      set->set(QObject::tr("Value"), y1, Setting::Float);
      set->set(QObject::tr("Type"), QObject::tr("Buy Arrow"), Setting::None);
      set->set(QObject::tr("Color"), upColor.name(), Setting::Color);
      break;
    case SellArrow:
      set->set(QObject::tr("Date"), x1, Setting::Date);
      set->set(QObject::tr("Value"), y1, Setting::Float);
      set->set(QObject::tr("Type"), QObject::tr("Sell Arrow"), Setting::None);
      set->set(QObject::tr("Color"), downColor.name(), Setting::Color);
      break;
    case FibonacciLine:
      set->set(QObject::tr("High"), y1, Setting::Float);
      set->set(QObject::tr("Low"), y2, Setting::Float);
      set->set(QObject::tr("Support"), QObject::tr("False"), Setting::Bool);
      set->set("0", QObject::tr("True"), Setting::Bool);
      set->set("0.238", QObject::tr("True"), Setting::Bool);
      set->set("0.383", QObject::tr("True"), Setting::Bool);
      set->set("0.5", QObject::tr("True"), Setting::Bool);
      set->set("0.618", QObject::tr("True"), Setting::Bool);
      set->set("1", QObject::tr("False"), Setting::Bool);
      set->set("1.618", QObject::tr("False"), Setting::Bool);
      set->set("2.618", QObject::tr("False"), Setting::Bool);
      set->set("4.236", QObject::tr("False"), Setting::Bool);
      set->set(QObject::tr("Type"), QObject::tr("Fibonacci Line"), Setting::None);
      set->set(QObject::tr("Color"), borderColor.name(), Setting::Color);
      break;
    case HorizontalLine:
      set->set(QObject::tr("Value"), y1, Setting::Float);
      set->set(QObject::tr("Type"), QObject::tr("Horizontal Line"), Setting::None);
      set->set(QObject::tr("Color"), borderColor.name(), Setting::Color);
      break;
    case VerticalLine:
      set->set(QObject::tr("Date"), x1, Setting::Date);
      set->set(QObject::tr("Type"), QObject::tr("Vertical Line"), Setting::None);
      set->set(QObject::tr("Color"), borderColor.name(), Setting::Color);
      break;
    case TrendLine:
      if (x2 <= x1)
      {
	cancelFlag = TRUE;
        QMessageBox::information(this, tr("Qtstalker: Error"),
    			         tr("Trendline end point <= start point."));
        break;
      }
      
      set->set(QObject::tr("Start Date"), x1, Setting::Date);
      set->set(QObject::tr("Start Value"), y1, Setting::Float);
      set->set(QObject::tr("Start Bar"), QObject::tr("Close"), Setting::InputField);
      set->set(QObject::tr("End Date"), x2, Setting::Date);
      set->set(QObject::tr("End Value"), y2, Setting::Float);
      set->set(QObject::tr("End Bar"), QObject::tr("Close"), Setting::InputField);
      set->set(QObject::tr("Use Bar"), QObject::tr("False"), Setting::Bool);
      set->set(QObject::tr("Type"), QObject::tr("Trend Line"), Setting::None);
      set->set(QObject::tr("Color"), borderColor.name(), Setting::Color);
      break;
    case Text:
      set->set(QObject::tr("Date"), x1, Setting::Date);
      set->set(QObject::tr("Value"), y1, Setting::Float);
      set->set(QObject::tr("Label"), QObject::tr("Text"), Setting::Text);
      set->set(QObject::tr("Type"), QObject::tr("Text"), Setting::None);
      set->set(QObject::tr("Color"), borderColor.name(), Setting::Color);
      break;
    default:
      break;
  }

  if (! cancelFlag)
  {
    emit chartObjectCreated(set);

    Indicator *i = indicators[set->getData(tr("Plot"))];
    i->addChartObject(set);

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
	if (! line->getType().compare(tr("Invisible")))
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
	if (! line->getType().compare(tr("Invisible")))
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

  Indicator *i;
  if (mainFlag)
    i = indicators["Main Plot"];
  else
  {
    QDictIterator<Indicator> it(indicators);
    it.toFirst();
    i = it.current();
  }
  QStringList l = i->getChartObjects();

  int loop;
  for (loop = 0; loop < (int) l.count(); loop++)
  {
    Setting *co = i->getChartObject(l[loop]);

    QString type = co->getData(QObject::tr("Type"));
    if (! type.compare(QObject::tr("Vertical Line")))
      continue;

    if (! type.compare(QObject::tr("Trend Line")))
    {
      double v = co->getFloat(QObject::tr("Start Value"));
      double v2 = co->getFloat(QObject::tr("End Value"));
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

    if (! type.compare(QObject::tr("Fibonacci Line")))
    {
      double v = co->getFloat(QObject::tr("High"));
      double v2 = co->getFloat(QObject::tr("Low"));
      if (v > scaleHigh)
        scaleHigh = v;
      if (v2 < scaleLow)
        scaleLow = v2;
      continue;
    }

    double v = co->getFloat(QObject::tr("Value"));
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

  scaler.set(_height,
  	     scaleHigh,
	     scaleLow,
	     logScaleHigh,
	     logRange,
	     dateFlag,
	     logScale);

  scaleArray = scaler.getScaleArray();
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

int Plot::getXFromDate (QDateTime d)
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

void Plot::drawBars ()
{
  QPainter painter;
  painter.begin(&buffer);

  int x = startX;
  int loop = startIndex;

  // set first bar as neutral
  painter.setPen(neutralColor);

  double t = data->getOpen(loop);
  int y;
  if (t)
  {
    y = scaler.convertToY(t);
    painter.drawLine (x - 2, y, x, y);
  }

  y = scaler.convertToY(data->getClose(loop));
  painter.drawLine (x + 2, y, x, y);

  int h = scaler.convertToY(data->getHigh(loop));

  int l = scaler.convertToY(data->getLow(loop));
  painter.drawLine (x, h, x, l);

  x = x + pixelspace;
  loop++;

  while ((x < _width) && (loop < (int) data->count()))
  {
    if (data->getClose(loop) > data->getClose(loop - 1))
      painter.setPen(upColor);
    else
    {
      if (data->getClose(loop) < data->getClose(loop - 1))
        painter.setPen(downColor);
      else
        painter.setPen(neutralColor);
    }

    t = data->getOpen(loop);
    if (t)
    {
      y = scaler.convertToY(t);
      painter.drawLine (x - 2, y, x, y);
    }

    y = scaler.convertToY(data->getClose(loop));
    painter.drawLine (x + 2, y, x, y);

    h = scaler.convertToY(data->getHigh(loop));
    l = scaler.convertToY(data->getLow(loop));
    painter.drawLine (x, h, x, l);

    x = x + pixelspace;
    loop++;
  }

  painter.end();
}

void Plot::drawCandle ()
{
  QPainter painter;
  painter.begin(&buffer);

  int x = startX;
  int loop = startIndex;

  painter.setPen(candleColor);

  while ((x < _width) && (loop < (int) data->count()))
  {
    int h = scaler.convertToY(data->getHigh(loop));
    int l = scaler.convertToY(data->getLow(loop));
    int c = scaler.convertToY(data->getClose(loop));
    int o = scaler.convertToY(data->getOpen(loop));

    painter.drawLine (x, h, x, l);

    if (data->getOpen(loop) != 0)
    {
      if (c < o)
      {
        painter.fillRect(x - 2, c, 5, o - c, backgroundColor);
        painter.drawRect(x - 2, c, 5, o - c);
      }
      else if (c == o)
        painter.drawLine (x - 2, o, x + 2, o);
      else
        painter.fillRect(x - 2, o, 5, c - o, painter.pen().color());
    }

    x = x + pixelspace;
    loop++;
  }

  painter.end();
}

void Plot::drawCandle2 ()
{
  QPainter painter;
  painter.begin(&buffer);

  int x = startX;
  int loop = startIndex;

  painter.setPen(neutralColor);

  int h = scaler.convertToY(data->getHigh(loop));
  int l = scaler.convertToY(data->getLow(loop));
  int c = scaler.convertToY(data->getClose(loop));
  int o = scaler.convertToY(data->getOpen(loop));

  painter.drawLine (x, h, x, l);

  if (data->getOpen(loop) != 0)
  {
    if (c < o)
    {
      painter.fillRect(x - 2, c, 5, o - c, backgroundColor);
      painter.drawRect(x - 2, c, 5, o - c);
    }
    else if (c == o)
      painter.drawLine (x - 2, o, x + 2, o);
    else
      painter.fillRect(x - 2, o, 5, c - o, painter.brush());
  }

  loop++;
  x = x + pixelspace;

  while ((x < _width) && (loop < (int) data->count()))
  {
    if (data->getClose(loop) > data->getClose(loop - 1))
      painter.setPen(upColor);
    else
    {
      if (data->getClose(loop) < data->getClose(loop - 1))
        painter.setPen(downColor);
      else
        painter.setPen(neutralColor);
    }

    h = scaler.convertToY(data->getHigh(loop));
    l = scaler.convertToY(data->getLow(loop));
    c = scaler.convertToY(data->getClose(loop));
    o = scaler.convertToY(data->getOpen(loop));

    painter.drawLine (x, h, x, l);

    if (data->getOpen(loop) != 0)
    {
      if (c < o)
      {
        painter.fillRect(x - 2, c, 5, o - c, backgroundColor);
        painter.drawRect(x - 2, c, 5, o - c);
      }
      else if (c == o)
        painter.drawLine (x - 2, o, x + 2, o);
      else
        painter.fillRect(x - 2, o, 5, c - o, painter.pen().color());
    }

    x = x + pixelspace;
    loop++;
  }

  painter.end();
}

void Plot::drawLineChart ()
{
  QPainter painter;
  painter.begin(&buffer);

  int x = -1;
  int x2 = startX;
  int y = -1;
  int y2 = -1;
  int loop = startIndex;

  painter.setPen(upColor);

  while ((x < _width) && (loop < (int) data->count()))
  {
    y2 = scaler.convertToY(data->getClose(loop));
    if (y != -1)
      painter.drawLine (x, y, x2, y2);
    x = x2;
    y = y2;

    x2 = x2 + pixelspace;
    loop++;
  }

  painter.end();
}

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

void Plot::drawPaintBar ()
{
  QPainter painter;
  painter.begin(&buffer);

  int x = startX;
  int loop = startIndex;
  
  while ((x < _width) && (loop < (int) data->count()))
  {
    QColor *color = paintBars.at(loop);
    painter.setPen(QColor(color->red(), color->green(), color->blue()));

    int y;
    if (data->getOpen(loop) != 0)
    {
      y = scaler.convertToY(data->getOpen(loop));
      painter.drawLine (x - 2, y, x, y);
    }

    y = scaler.convertToY(data->getClose(loop));
    painter.drawLine (x + 2, y, x, y);

    int h = scaler.convertToY(data->getHigh(loop));
    int l = scaler.convertToY(data->getLow(loop));
    painter.drawLine (x, h, x, l);

    x = x + pixelspace;
    loop++;
  }

  painter.end();
}

void Plot::drawSwing ()
{
  QPainter painter;
  painter.begin(&buffer);
  painter.setPen(neutralColor);

  int status = 0;
  int x = startX;
  int loop = startIndex;
  int h;
  int l;
  double high = -99999999;
  double low = 99999999;

  int oldx = x;
  loop++;
  x = x + pixelspace;

  while ((x < _width) && (loop < (int) data->count()))
  {
    switch (status)
    {
      case 1:
        if (data->getHigh(loop) < data->getHigh(loop - 1) && data->getLow(loop) < data->getLow(loop - 1))
	{
	  painter.setPen(upColor);
          h = scaler.convertToY(high);
          l = scaler.convertToY(low);
          painter.drawLine (x, h, x, l);

          painter.drawLine (oldx, l, x, l);

	  status = -1;
	  oldx = x;
          low = data->getLow(loop);
	}
	else
	{
          if (data->getHigh(loop) > high)
           high = data->getHigh(loop);
	}
	break;
      case -1:
        if (data->getHigh(loop) > data->getHigh(loop - 1) && data->getLow(loop) > data->getLow(loop - 1))
	{
	  painter.setPen(downColor);
          h = scaler.convertToY(high);
          l = scaler.convertToY(low);
          painter.drawLine (x, h, x, l);

          painter.drawLine (oldx, h, x, h);

	  status = 1;
	  oldx = x;
          high = data->getHigh(loop);
	}
	else
	{
          if (data->getLow(loop) < low)
           low = data->getLow(loop);
        }
	break;
      default:
        if (data->getHigh(loop) < data->getHigh(loop - 1) && data->getLow(loop) < data->getLow(loop - 1))
	{
	  status = -1;
	  oldx = x;
          high = data->getHigh(loop);
          low = data->getLow(loop);
	}
	else
	{
          if (data->getHigh(loop) > data->getHigh(loop - 1) && data->getLow(loop) > data->getLow(loop - 1))
	  {
	    status = 1;
	    oldx = x;
            high = data->getHigh(loop);
            low = data->getLow(loop);
	  }
        }
	break;
    }

    x = x + pixelspace;
    loop++;
  }

  // draw the leftover
  switch (status)
  {
    case 1:
      painter.setPen(upColor);
      h = scaler.convertToY(high);
      l = scaler.convertToY(low);
      painter.drawLine (x, h, x, l);
      painter.drawLine (oldx, l, x, l);
        break;
    case -1:
      painter.setPen(downColor);
      h = scaler.convertToY(high);
      l = scaler.convertToY(low);
      painter.drawLine (x, h, x, l);
      painter.drawLine (oldx, h, x, h);
      break;
    default:
      break;
  }

  painter.end();
}

void Plot::drawPointAndFigure ()
{
  QPainter painter;
  painter.begin(&buffer);
  painter.setPen(backgroundColor);

  int x = startX;
  int x2 = startX;
  int loop = startIndex;
  double size = PAFBoxSize;
  if (size == 0)
    size = (scaleArray[1] - scaleArray[0]) / 4.0;
  int symbol;

  double ph = data->getHigh(loop);
  double pl = data->getLow(loop);
  double t2 = data->getClose(loop);
  if (((ph - pl) / 2) + pl > t2)
    symbol = TRUE;
  else
    symbol = FALSE;

  int t = (int) (ph / size);
  ph = t * size;

  t = (int) (pl / size);
  pl = t * size;

  loop++;
  x2 = x2 + pixelspace;

  while ((x2 < _width) && (loop < (int) data->count()))
  {
    double h = data->getHigh(loop);
    double l = data->getLow(loop);

    if (! symbol)
    {
      if (l <= (pl - size))
      {
        t = (int) (l / size);
        pl = t * size;
      }
      else
      {
        if (h >= (pl + ((PAFReversal + 1) * size)))
        {
	  int y = scaler.convertToY(ph);
	  int y2= scaler.convertToY(pl);
          painter.fillRect(x, y, x2 - x + pixelspace, y2 - y, downColor);

	  double val = ph - size;
	  while (val > pl)
	  {
	    y = scaler.convertToY(val);
            painter.drawLine (x, y, x2 + pixelspace, y);
            val = val - size;
	  }

	  x = x2 + pixelspace;

          symbol = TRUE;
          pl = pl + size;
          int t = (int) (h / size);
          ph = t * size;
        }
      }
    }
    else
    {
      if (h >= (ph + size))
      {
        int t = (int) (h / size);
        ph = t * size;
      }
      else
      {
        if (l <= (ph - ((PAFReversal + 1) * size)))
        {
	  int y = scaler.convertToY(ph);
	  int y2= scaler.convertToY(pl);
          painter.fillRect(x, y, x2 - x + pixelspace, y2 - y, upColor);

	  double val = ph - size;
	  while (val > pl)
	  {
	    y = scaler.convertToY(val);
            painter.drawLine (x, y, x2 + pixelspace, y);
            val = val - size;
	  }

	  x = x2 + pixelspace;

          symbol = FALSE;
          ph = ph - size;
          t = (int) (l / size);
          pl = t * size;
        }
      }
    }

    x2 = x2 + pixelspace;
    loop++;
  }

  int y = scaler.convertToY(ph);
  int y2= scaler.convertToY(pl);
  if (! symbol)
    painter.fillRect(x, y, x2 - x + pixelspace, y2 - y, downColor);
  else
    painter.fillRect(x, y, x2 - x + pixelspace, y2 - y, upColor);
  double val = ph - size;
  while (val > pl)
  {
    y = scaler.convertToY(val);
    painter.drawLine (x, y, x2 + pixelspace, y);
    val = val - size;
  }

  painter.end();
}

void Plot::drawBuyArrow (Setting *co)
{
  QPainter painter;
  painter.begin(&buffer);

  QDateTime dt = QDateTime::fromString(co->getDateTime(QObject::tr("Date")), Qt::ISODate);

  int x = getXFromDate(dt);
  if (x == -1)
    return;

  int y = scaler.convertToY(co->getFloat(QObject::tr("Value")));

  QColor color(co->getData(QObject::tr("Color")));

  QPointArray array;
  array.setPoints(7, x, y,
                  x + 4, y + 4,
                  x + 1, y + 4,
                  x + 1, y + 10,
	          x - 1, y + 10,
	          x - 1, y + 4,
                  x - 4, y + 4);
  painter.setBrush(color);
  painter.drawPolygon(array, TRUE, 0, -1);

  painter.end();
}

void Plot::drawFibonacciLine (Setting *co)
{
  QColor color(co->getData(QObject::tr("Color")));
  double high = co->getFloat(QObject::tr("High"));
  double low = co->getFloat(QObject::tr("Low"));

  bool support = FALSE;
  QString s = co->getData(QObject::tr("Support"));
  if (! s.compare(QObject::tr("True")))
    support = TRUE;

  QString label;
  QString v = "0";
  s = co->getData(v);
  if (! s.compare(QObject::tr("True")))
  {
    label = "0%";
    if (! support)
    {
      v.prepend("-");
      label.prepend("-");
    }
    drawFibonacciLine2(color, label, high, low, v.toFloat());
  }

  v = "0.238";
  s = co->getData(v);
  if (! s.compare(QObject::tr("True")))
  {
    label = "23.8%";
    if (! support)
    {
      v.prepend("-");
      label.prepend("-");
    }
    drawFibonacciLine2(color, label, high, low, v.toFloat());
  }

  v = "0.383";
  s = co->getData(v);
  if (! s.compare(QObject::tr("True")))
  {
    label = "38.3%";
    if (! support)
    {
      v.prepend("-");
      label.prepend("-");
    }
    drawFibonacciLine2(color, label, high, low, v.toFloat());
  }

  v = "0.5";
  s = co->getData(v);
  if (! s.compare(QObject::tr("True")))
  {
    label = "50%";
    if (! support)
    {
      v.prepend("-");
      label.prepend("-");
    }
    drawFibonacciLine2(color, label, high, low, v.toFloat());
  }

  v = "0.618";
  s = co->getData(v);
  if (! s.compare(QObject::tr("True")))
  {
    label = "61.8%";
    if (! support)
    {
      v.prepend("-");
      label.prepend("-");
    }
    drawFibonacciLine2(color, label, high, low, v.toFloat());
  }

  v = "1";
  s = co->getData(v);
  if (! s.compare(QObject::tr("True")))
  {
    label = "100%";
    if (! support)
    {
      v.prepend("-");
      label.prepend("-");
    }
    drawFibonacciLine2(color, label, high, low, v.toFloat());
  }

  v = "1.618";
  s = co->getData(v);
  if (! s.compare(QObject::tr("True")))
  {
    label = "161.8%";
    if (! support)
    {
      v.prepend("-");
      label.prepend("-");
    }
    drawFibonacciLine2(color, label, high, low, v.toFloat());
  }

  v = "2.618";
  s = co->getData(v);
  if (! s.compare(QObject::tr("True")))
  {
    label = "261.8%";
    if (! support)
    {
      v.prepend("-");
      label.prepend("-");
    }
    drawFibonacciLine2(color, label, high, low, v.toFloat());
  }

  v = "4.236";
  s = co->getData(v);
  if (! s.compare(QObject::tr("True")))
  {
    label = "423.6%";
    if (! support)
    {
      v.prepend("-");
      label.prepend("-");
    }
    drawFibonacciLine2(color, label, high, low, v.toFloat());
  }
}

void Plot::drawFibonacciLine2 (QColor color, QString label, double high, double low, double v)
{
  QPainter painter;
  painter.begin(&buffer);
  painter.setFont(plotFont);
  painter.setPen(color);

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

  int y = scaler.convertToY(r);
  painter.drawLine (startX, y, _width, y);

  QString s = label;
  s.append(" - ");
  s.append(strip(r));
  painter.drawText(startX, y - 1, s, -1);

  painter.end();
}

void Plot::drawHorizontalLine (Setting *co)
{
  QPainter painter;
  painter.begin(&buffer);
  painter.setFont(plotFont);

  int y = scaler.convertToY(co->getFloat(QObject::tr("Value")));

  QColor color(co->getData(QObject::tr("Color")));
  painter.setPen(color);

  painter.drawLine (startX, y, _width, y);
  painter.drawText(startX, y - 1, co->getData(QObject::tr("Value")), -1);

  painter.end();
}

void Plot::drawSellArrow (Setting *co)
{
  QPainter painter;
  painter.begin(&buffer);

  QDateTime dt = QDateTime::fromString(co->getDateTime(QObject::tr("Date")), Qt::ISODate);

  int x = getXFromDate(dt);
  if (x == -1)
    return;

  int y = scaler.convertToY(co->getFloat(QObject::tr("Value")));

  QColor color(co->getData(QObject::tr("Color")));

  QPointArray array;
  array.setPoints(7, x, y,
                  x + 4, y - 4,
	          x + 1, y - 4,
	          x + 1, y - 10,
	          x - 1, y - 10,
	          x - 1, y - 4,
                  x - 4, y - 4);
  painter.setBrush(color);
  painter.drawPolygon(array, TRUE, 0, -1);

  painter.end();
}

void Plot::drawText (Setting *co)
{
  QPainter painter;
  painter.begin(&buffer);
  painter.setFont(plotFont);

  QDateTime dt = QDateTime::fromString(co->getDateTime(QObject::tr("Date")), Qt::ISODate);

  int x = getXFromDate(dt);
  if (x == -1)
    return;

  int y = scaler.convertToY(co->getFloat(QObject::tr("Value")));

  QColor color(co->getData(QObject::tr("Color")));
  painter.setPen(color);

  painter.drawText(x, y, co->getData(QObject::tr("Label")));

  painter.end();
}

void Plot::drawTrendLine (Setting *co)
{
  QPainter painter;
  painter.begin(&buffer);

  QDateTime dt = QDateTime::fromString(co->getDateTime(QObject::tr("End Date")), Qt::ISODate);

  int x2 = getXFromDate(dt);
  if (x2 == -1)
    return;
  int i2 = data->getX(dt);

  dt = QDateTime::fromString(co->getDateTime(QObject::tr("Start Date")), Qt::ISODate);

  int x = getXFromDate(dt);
  if (x == -1)
    return;
  int i = data->getX(dt);

  int y;
  if (! co->getData(QObject::tr("Use Bar")).compare(QObject::tr("True")))
  {
    QString s = co->getData(QObject::tr("Start Bar"));
    
    while (1)
    {
      if (! s.compare(QObject::tr("Open")))
      {
        y = scaler.convertToY(data->getOpen(i));
	break;
      }

      if (! s.compare(QObject::tr("High")))
      {
        y = scaler.convertToY(data->getHigh(i));
	break;
      }

      if (! s.compare(QObject::tr("Low")))
      {
        y = scaler.convertToY(data->getLow(i));
	break;
      }

      if (! s.compare(QObject::tr("Close")))
      {
        y = scaler.convertToY(data->getClose(i));
	break;
      }

      return;
    }
  }
  else
    y = scaler.convertToY(co->getFloat(QObject::tr("Start Value")));

  int y2;
  if (! co->getData(QObject::tr("Use Bar")).compare(QObject::tr("True")))
  {
    QString s = co->getData(QObject::tr("End Bar"));

    while (1)
    {
      if (! s.compare(QObject::tr("Open")))
      {
        y2 = scaler.convertToY(data->getOpen(i2));
	break;
      }

      if (! s.compare(QObject::tr("High")))
      {
        y2 = scaler.convertToY(data->getHigh(i2));
	break;
      }

      if (! s.compare(QObject::tr("Low")))
      {
        y2 = scaler.convertToY(data->getLow(i2));
	break;
      }

      if (! s.compare(QObject::tr("Close")))
      {
        y2 = scaler.convertToY(data->getClose(i2));
	break;
      }

      return;
    }
  }
  else
    y2 = scaler.convertToY(co->getFloat(QObject::tr("End Value")));

  QColor color(co->getData(QObject::tr("Color")));
  painter.setPen(color);

  painter.drawLine (x, y, x2, y2);

  int ydiff = y - y2;
  int xdiff = x2 - x;
  dt = data->getDate(data->count() - 1);
  int end = getXFromDate(dt);
  while (x2 < end)
  {
    x = x2;
    y = y2;
    x2 = x2 + xdiff;
    y2 = y2 - ydiff;
    painter.drawLine (x, y, x2, y2);
  }

  painter.end();
}

void Plot::drawVerticalLine (Setting *co)
{
  QPainter painter;
  painter.begin(&buffer);

  QDateTime dt = QDateTime::fromString(co->getDateTime(QObject::tr("Date")), Qt::ISODate);

  int x = getXFromDate(dt);
  if (x == -1)
    return;

  QColor color(co->getData(QObject::tr("Color")));
  painter.setPen(color);

  painter.drawLine (x, 0, x, _height);

  painter.end();
}

QStringList Plot::getIndicators ()
{
  QStringList l;

  QDictIterator<Indicator> it(indicators);
  for (; it.current(); ++it)
    l.append(it.currentKey());

  if (mainFlag)
    l.remove("Main Plot");

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
      objectFlag = VerticalLine;
      emit statusMessage(tr("Select point to place vertical line..."));
      break;
    }

    if (! d.compare(tr("Horizontal Line")))
    {
      mouseFlag = ClickWait;
      objectFlag = HorizontalLine;
      emit statusMessage(tr("Select point to place horizontal line..."));
      break;
    }

    if (! d.compare(tr("Text")))
    {
      mouseFlag = ClickWait;
      objectFlag = Text;
      emit statusMessage(tr("Select point to place text..."));
      break;
    }

    if (! d.compare(tr("Trend Line")))
    {
      mouseFlag = ClickWait;
      objectFlag = TrendLine;
      emit statusMessage(tr("Select start point of trend line..."));
      break;
    }

    if (! d.compare(tr("Buy Arrow")))
    {
      mouseFlag = ClickWait;
      objectFlag = BuyArrow;
      emit statusMessage(tr("Select point to place buy arrow..."));
      break;
    }

    if (! d.compare(tr("Sell Arrow")))
    {
      mouseFlag = ClickWait;
      objectFlag = SellArrow;
      emit statusMessage(tr("Select point to place sell arrow..."));
      break;
    }

    if (! d.compare(tr("Fibonacci Line")))
    {
      mouseFlag = ClickWait;
      objectFlag = FibonacciLine;
      emit statusMessage(tr("Select high point of fibonacci line..."));
      break;
    }

    break;
  }
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

    QPixmap pix = buffer;

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

  chartEditMenu->clear();
  chartDeleteMenu->clear();
  chartObjectEditMenu->clear();
  chartObjectDeleteMenu->clear();

  QDictIterator<Indicator> it(indicators);
  for(; it.current(); ++it)
  {
    Indicator *i = it.current();

    if (it.currentKey().compare("Main Plot"))
    {
      int id = chartDeleteMenu->insertItem(QPixmap(indicator), i->getData("Name"), this, SLOT(slotDeleteIndicator(int)));
      chartDeleteMenu->setItemParameter(id, id);

      id = chartEditMenu->insertItem(QPixmap(indicator), i->getData("Name"), this, SLOT(slotEditIndicator(int)));
      chartEditMenu->setItemParameter(id, id);
    }

    QStringList l = i->getChartObjects();
    int loop;
    for (loop = 0; loop < (int) l.count(); loop++)
    {
      QPixmap icon;
      Setting *co = i->getChartObject(l[loop]);

      while (1)
      {
        if (co->getInt("ObjectType") == Plot::VerticalLine)
	{
          icon = QPixmap(vertical);
	  break;
	}

        if (co->getInt("ObjectType") == Plot::HorizontalLine)
	{
          icon = QPixmap(horizontal);
	  break;
	}

        if (co->getInt("ObjectType") == Plot::TrendLine)
	{
          icon = QPixmap(trend);
	  break;
	}

        if (co->getInt("ObjectType") == Plot::Text)
	{
          icon = QPixmap(text);
	  break;
	}

        if (co->getInt("ObjectType") == Plot::BuyArrow)
	{
          icon = QPixmap(buyarrow);
	  break;
	}

        if (co->getInt("ObjectType") == Plot::SellArrow)
	{
          icon = QPixmap(sellarrow);
	  break;
	}

        if (co->getInt("ObjectType") == Plot::FibonacciLine)
	{
          icon = QPixmap(fib);
	  break;
	}

	break;
      }

      int id = chartObjectEditMenu->insertItem(icon, l[loop], this, SLOT(slotEditChartObject(int)));
      chartObjectEditMenu->setItemParameter(id, id);

      id = chartObjectDeleteMenu->insertItem(icon, l[loop], this, SLOT(slotDeleteChartObject(int)));
      chartObjectDeleteMenu->setItemParameter(id, id);
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

void Plot::slotEditChartObject (int id)
{
  QString s = chartObjectEditMenu->text(id);

  Indicator *i;
  if (mainFlag)
    i = getIndicator("Main Plot");
  else
  {
    QDictIterator<Indicator> it(indicators);
    it.toFirst();
    i = it.current();
  }

  emit signalEditChartObject(i->getChartObject(s), this);
}

void Plot::slotDeleteChartObject (int id)
{
  QString s = chartObjectDeleteMenu->text(id);

  Indicator *i;
  if (mainFlag)
    i = getIndicator("Main Plot");
  else
  {
    QDictIterator<Indicator> it(indicators);
    it.toFirst();
    i = it.current();
  }

  i->deleteChartObject(s);

  emit signalDeleteChartObject(s, this);
}

void Plot::slotNewIndicator ()
{
  emit signalNewIndicator();
}

void Plot::slotNewChartObject (int id)
{
  QString s = chartObjectMenu->text(id);
  emit signalNewChartObject(s, this);
}

QStringList Plot::getChartTypes ()
{
  QStringList l;
  l.append(tr("Bar"));
  l.append(tr("Paint Bar"));
  l.append(tr("Line"));
  l.append(tr("Candle"));
  l.append(tr("Candle 2"));
  l.append(tr("P&F"));
  l.append(tr("Swing"));
  return l;
}




