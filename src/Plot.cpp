/*
 *  Qtstalker stock charter
 *
 *  Copyright (C) 2001,2002 Stefan S. Stratigakos
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
  pixelspace = 0;
  minPixelspace = 0;
  dateFlag = FALSE;
  gridFlag = TRUE;
  interval = Daily;
  mainFlag = FALSE;
  scaleToScreen = FALSE;
  logScale = FALSE;
  startIndex = 0;
  scaleHigh = -99999999;
  scaleLow = 99999999;
  logScaleHigh = 0;
  logRange = -1;
  mainHigh = -99999999;
  mainLow = 99999999;
  chartType = "None";
  mouseFlag = None;

  plotFont.setFamily("Helvetica");
  plotFont.setPointSize(12);
  plotFont.setWeight(50);

  indicators.setAutoDelete(TRUE);
  paintBars.setAutoDelete(TRUE);
  data = 0;

  setMouseTracking(TRUE);
}

Plot::~Plot ()
{
}

void Plot::clear ()
{
  scaleHigh = -99999999;
  scaleLow = 99999999;
  logScaleHigh = 0;
  logRange = -1;
  mainHigh = -99999999;
  mainLow = 99999999;
  indicators.clear();
  data = 0;
  paintBars.clear();

  if (mainFlag)
    indicators.replace("Main Plot", new Indicator);
}

void Plot::setData (QList<Setting> *l)
{
  if (! l->count())
    return;

  data = l;
  
  if (mainFlag)
  {
    int loop;
    for (loop = 0; loop < (int) data->count(); loop++)
      paintBars.append(new QColor(neutralColor.red(), neutralColor.green(), neutralColor.blue()));
  }

  int loop;
  for (loop = 0; loop < (int) l->count(); loop++)
  {
    Setting *r = l->at(loop);

    if (mainFlag)
    {
      QString s = r->getData("High");
      if (s.length())
      {
        if (s.toFloat() > mainHigh)
          mainHigh = s.toFloat();
      }

      s = r->getData("Low");
      if (s.length())
      {
        if (s.toFloat() < mainLow)
	  mainLow = s.toFloat();
      }
    }
  }
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

    if (! d.compare(tr("Candle")))
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

    if (! d.compare(tr("Point and Figure")))
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

void Plot::draw ()
{
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

    if (dateFlag)
      drawDate();

    setScale();

    drawXGrid();

    drawYGrid();

    if (mainFlag)
    {
      while (1)
      {
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

        if (! chartType.compare(tr("Point and Figure")))
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

    QString type = co->getData(QObject::tr("Type"));

    while (1)
    {
      if (! type.compare(tr("Buy Arrow")))
      {
        drawBuyArrow(co);
	break;
      }

      if (! type.compare(tr("Sell Arrow")))
      {
        drawSellArrow(co);
	break;
      }

      if (! type.compare(tr("Horizontal Line")))
      {
        drawHorizontalLine(co);
	break;
      }

      if (! type.compare(tr("Vertical Line")))
      {
        drawVerticalLine(co);
	break;
      }

      if (! type.compare(tr("Trend Line")))
      {
        drawTrendLine(co);
	break;
      }

      if (! type.compare(tr("Fibonacci Line")))
      {
        drawFibonacciLine(co);
	break;
      }

      if (! type.compare(tr("Text")))
      {
        drawText(co);
	break;
      }

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
      break;
    case RightButton:
      emit rightMouseButton();
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
  Setting *tr = data->at(i);

  if (mainFlag)
    r->parse(tr->getString());

  QString s = tr->getData("Date");
  s.truncate(s.length() - 6);
  r->set("D", s, Setting::Date);
  r->remove("Date");
  r->remove("Volume");
  r->remove("Open Interest");
  if (r->getData("Open").length())
  {
    r->set("O", r->getData("Open"), Setting::Float);
    r->remove("Open");
  }
  if (r->getData("High").length())
  {
    r->set("H", r->getData("High"), Setting::Float);
    r->remove("High");
  }
  if (r->getData("Low").length())
  {
    r->set("L", r->getData("Low"), Setting::Float);
    r->remove("Low");
  }
  if (r->getData("Close").length())
  {
    r->set("C", r->getData("Close"), Setting::Float);
    r->remove("Close");
  }

  QDictIterator<Indicator> it(indicators);
  for (; it.current(); ++it)
  {
    Indicator *ind = it.current();
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

void Plot::setInterval (TimeInterval d)
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

void Plot::drawDate ()
{
  QPainter painter;
  painter.begin(&buffer);
  painter.setPen(borderColor);
  painter.setFont(plotFont);

  QFontMetrics fm = painter.fontMetrics();

  // draw the seperator line
  painter.drawLine (0, buffer.height() - dateHeight, buffer.width() - scaleWidth, buffer.height() - dateHeight);

  int x = startX;
  int loop = startIndex;

  if (interval == Daily)
  {
    Setting *r = data->at(loop);
    QDateTime oldDate = QDateTime::fromString(r->getDateTime("Date"), Qt::ISODate);
    QDateTime oldWeek = oldDate;
    oldWeek = oldWeek.addDays(7 - oldWeek.date().dayOfWeek());

    while(x <= _width && loop < (int) data->count())
    {
      r = data->at(loop);
      QDateTime date = QDateTime::fromString(r->getDateTime("Date"), Qt::ISODate);

      if (date.date().month() != oldDate.date().month())
      {
        oldDate = date;

        painter.drawLine (x, _height + 1, x, _height + 6);

        QString s;
	if (pixelspace < 7)
	{
          if (date.date().month() == 1)
            s = date.toString("yy");
          else
	  {
            s = date.toString("MMM");
	    s.truncate(1);
	  }
	}
	else
	{
          if (date.date().month() == 1)
            s = date.toString("yy");
          else
            s = date.toString("MMM");
	}

        painter.drawText (x - (fm.width(s, -1) / 2), buffer.height() - 2, s, -1);

        oldWeek = date;
        oldWeek = oldWeek.addDays(7 - oldWeek.date().dayOfWeek());
      }
      else
      {
        // if start of new week make a tick
        if (date > oldWeek)
	{
  	  if (pixelspace >= 11)
	  {
            QString s = date.toString("dd");
            painter.drawText (x - (fm.width(s, -1) / 2), buffer.height() - 2, s, -1);
	  }

          painter.drawLine (x, _height + 1, x, _height + 4);
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
      Setting *r = data->at(loop);
      QDateTime oldMonth = QDateTime::fromString(r->getDateTime("Date"), Qt::ISODate);

      while(x <= _width && loop < (int) data->count())
      {
        r = data->at(loop);
        QDateTime date = QDateTime::fromString(r->getDateTime("Date"), Qt::ISODate);

        if (date.date().month() != oldMonth.date().month())
        {
          oldMonth = date;

          painter.drawLine (x, _height + 1, x, _height + 6);

	  QString s;
  	  if (pixelspace < 7)
	  {
            if (date.date().month() == 1)
              s = date.toString("yy");
	    else
	    {
              s = date.toString("MMM");
              s.truncate(1);
	    }
	  }
	  else
	  {
            if (date.date().month() == 1)
              s = date.toString("yyyy");
	    else
              s = date.toString("MMM");
	  }

          painter.drawText (x - (fm.width(s, -1) / 2), buffer.height() - 2, s, -1);
        }

        x = x + pixelspace;
        loop++;
      }
    }
    else
    {
      Setting *r = data->at(loop);
      QDateTime oldYear = QDateTime::fromString(r->getDateTime("Date"), Qt::ISODate);

      while(x <= _width && loop < (int) data->count())
      {
        r = data->at(loop);
        QDateTime date = QDateTime::fromString(r->getDateTime("Date"), Qt::ISODate);

        if (date.date().year() != oldYear.date().year())
        {
          oldYear = date;
          painter.drawLine (x, _height + 1, x, _height + 6);
	  QString s = date.toString("yyyy");
          painter.drawText (x - (fm.width(s, -1) / 2), buffer.height() - 2, s, -1);
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

  int x = startX;
  int loop = startIndex;

  if (interval == Daily)
  {
    Setting *r = data->at(loop);
    QDateTime oldDate = QDateTime::fromString(r->getDateTime("Date"), Qt::ISODate);

    while(x <= _width && loop < (int) data->count())
    {
      r = data->at(loop);
      QDateTime date = QDateTime::fromString(r->getDateTime("Date"), Qt::ISODate);

      if (date.date().month() != oldDate.date().month())
      {
        oldDate = date;
        painter.drawLine (x, 0, x, _height);
      }

      x = x + pixelspace;

      loop++;
    }
  }
  else
  {
    Setting *r = data->at(loop);
    QDateTime oldYear = QDateTime::fromString(r->getDateTime("Date"), Qt::ISODate);

    while(x <= _width && loop < (int) data->count())
    {
      r = data->at(loop);
      QDateTime date = QDateTime::fromString(r->getDateTime("Date"), Qt::ISODate);

      if (date.date().year() != oldYear.date().year())
      {
        oldYear = date;
        painter.drawLine (x, 0, x, _height);
      }

      x = x + pixelspace;
      loop++;
    }
  }

  painter.end();
}

void Plot::addIndicator (QString k, Indicator *i)
{
  indicators.replace(k, i);
}

Indicator * Plot::getIndicator (QString k)
{
  return indicators[k];
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
    int y = convertToY(scaleArray[loop]);
    painter.setPen(QPen(borderColor, 1, QPen::SolidLine));
    painter.drawLine (x, y, x + 4, y);

    // draw the text
    QString s = QString::number(scaleArray[loop], 'f', 2);
    if (s.contains(".00"))
      s.truncate(s.length() - 3);
    else
    {
      QString s2 = s.right(1);
      if (! s2.compare("0"))
        s.truncate(s.length() - 1);
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
    int y = convertToY(scaleArray[loop]);
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

  while ((x2 < _width) && (loop < (int) currentLine->getSize()))
  {
    if (loop > -1)
    {
      y2 = convertToY(currentLine->getData(loop));
      if (y != -1)
        painter.drawLine (x, y, x2, y2);
      x = x2;
      y = y2;
    }

    x2 = x2 + pixelspace;
    loop++;
  }

  painter.end();
}

void Plot::drawHorizontalLine ()
{
  QPainter painter;
  painter.begin(&buffer);
  painter.setFont(plotFont);

  QPen pen;
  pen.setColor(currentLine->getColor());
  painter.setPen(pen);

  int y = convertToY(currentLine->getData(currentLine->getSize() - 1));

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

  while ((x < _width) && (loop < (int) currentLine->getSize()))
  {
    if (loop > -1)
    {
      int y = convertToY(currentLine->getData(loop));
      painter.drawPoint(x, y);
    }

    x = x + pixelspace;
    loop++;
  }

  painter.end();
}

void Plot::drawHistogram ()
{
  QPainter painter;
  painter.begin(&buffer);
  painter.setPen(currentLine->getColor());
  painter.setBrush(currentLine->getColor());

  int loop = currentLine->getSize() - data->count() + startIndex;

  QPointArray pa(4);
  int zero = convertToY(0);
  int x = -1;
  int x2 = startX;
  int y = -1;
  int y2 = -1;

  while ((x < _width) && (loop < (int) currentLine->getSize()))
  {
    if (loop > -1)
    {
      y2 = convertToY(currentLine->getData(loop));
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
}

void Plot::drawHistogramBar ()
{
  QPainter painter;
  painter.begin(&buffer);

  QColor color(currentLine->getColor());

  int x = startX;
  int zero = convertToY(0);

  int loop = currentLine->getSize() - data->count() + startIndex;
  int dataLoop = startIndex;

  while ((x < _width) && (loop < (int) currentLine->getSize()))
  {
    if (loop > -1)
    {
      int y = convertToY(currentLine->getData(loop));

      if (currentLine->getColorBars() == TRUE)
      {
        Setting *r = data->at(dataLoop);
        Setting *pr;
        if (dataLoop < 1)
	  pr = data->at(dataLoop);
        else
	 pr = data->at(dataLoop - 1);

        if (r->getFloat("Close") > pr->getFloat("Close"))
          painter.fillRect(x, y, pixelspace - 1, zero - y, upColor);
        else
          painter.fillRect(x, y, pixelspace - 1, zero - y, downColor);
      }
      else
        painter.fillRect(x, y, pixelspace - 1, zero - y, color);
    }

    x = x + pixelspace;
    loop++;
    dataLoop++;
  }

  painter.end();
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
  Setting *r = data->at(data->count() - 1);
  QDateTime date = QDateTime::fromString(r->getDateTime("Date"), Qt::ISODate);
  s.append(date.toString("yyyyMMdd"));
  s.append(" ");
  painter.drawText(pos, 10, s, -1);
  pos = pos + fm.width(s);

  if (data->count() && mainFlag)
  {
    s = "O=";
    s.append(r->getData("Open"));
    s.append(" H=");
    s.append(r->getData("High"));
    s.append(" L=");
    s.append(r->getData("Low"));
    s.append(" C=");
    s.append(r->getData("Close"));
    s.append(" ");
    painter.drawText(pos, 10, s, -1);
    pos = pos + fm.width(s);

    double ch = 0;
    if (data->count() > 1)
    {
      Setting *pr = data->at(data->count() - 2);
      ch = r->getFloat("Close") - pr->getFloat("Close");
    }
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
  draw();

  QPainter painter;
  painter.begin(&buffer);
  painter.setPen(QPen(borderColor, 1, QPen::DotLine));
  painter.drawLine (0, y, buffer.width() - SCALE_WIDTH, y);
  painter.drawLine (x, 0, x, buffer.height());

  int i = (x / pixelspace) + startIndex;
  if (i >= (int) data->count())
    i = data->count() - 1;
  if (i < startIndex)
    i = startIndex;

  Setting *r = data->at(i);
  QDateTime date = QDateTime::fromString(r->getDateTime("Date"), Qt::ISODate);
  QString s = date.toString("yyyyMMdd");
  s.append(" ");
  s.append(QString::number(convertToVal(y)));
  emit statusMessage(s);

  painter.end();

  paintEvent(0);
}

void Plot::getXY (int x, int y, int f)
{
  int i = (x / pixelspace) + startIndex;
  if (i >= (int) data->count())
    i = data->count() - 1;
  if (i < startIndex)
    i = startIndex;

  Setting *r = data->at(i);

  QDateTime date = QDateTime::fromString(r->getDateTime("Date"), Qt::ISODate);

  if (f == 0)
  {
    x1 = date.toString("yyyyMMdd");
    y1 = QString::number(convertToVal(y));
  }
  else
  {
    x2 = date.toString("yyyyMMdd");
    y2 = QString::number(convertToVal(y));
  }
}

void Plot::newChartObject ()
{
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
      set->set(QObject::tr("Start Date"), x1, Setting::Date);
      set->set(QObject::tr("Start Value"), y1, Setting::Float);
      set->set(QObject::tr("End Date"), x2, Setting::Date);
      set->set(QObject::tr("End Value"), y2, Setting::Float);
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

  mouseFlag = None;

  emit chartObjectCreated(set);

  Indicator *i = indicators[set->getData(tr("Plot"))];
  i->addChartObject(set);

  draw();
  
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
  scaleHigh = -99999999;
  scaleLow = 99999999;

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
        Setting *r = data->at(loop);

	double t = r->getFloat("High");
        if (t > scaleHigh)
	  scaleHigh = t;

	t = r->getFloat("Low");
        if (t < scaleLow)
	  scaleLow = t;

        x = x + pixelspace;
        loop++;
      }
    }
  }

  if (! scaleToScreen)
  {
    if (mainFlag)
    {
      QDictIterator<Indicator> it(indicators);
      for (; it.current(); ++it)
      {
        Indicator *i = it.current();
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
      it.toFirst();
      Indicator *i = it.current();

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
    if (mainFlag)
    {
      QDictIterator<Indicator> it(indicators);
      for (; it.current(); ++it)
      {
        Indicator *i = it.current();
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
    else
    {
      QDictIterator<Indicator> it(indicators);
      it.toFirst();
      Indicator *i = it.current();

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

  scaleHigh = scaleHigh + (scaleHigh * 0.01);
  if (scaleLow < 0)
    scaleLow = scaleLow + (scaleLow * 0.1);
  else
    scaleLow = scaleLow - (scaleLow * 0.01);
  double range = scaleHigh - scaleLow;
  scaler = _height / range;
  
  if (mainFlag && logScale)
  {
    logScaleHigh = scaleHigh > 0.0 ? log(scaleHigh) : 1;
    double logScaleLow = scaleLow > 0.0 ? log(scaleLow) : 0;
    logRange = logScaleHigh - logScaleLow;
  }

  QStringList array;
  array.append(".01");
  array.append(".02");
  array.append(".05");
  array.append(".1");
  array.append(".2");
  array.append(".5");
  array.append("1");
  array.append("2");
  array.append("5");
  array.append("10");
  array.append("25");
  array.append("50");
  array.append("100");
  array.append("250");
  array.append("500");
  array.append("1000");
  array.append("2500");
  array.append("5000");
  array.append("10000");
  array.append("25000");
  array.append("50000");
  array.append("100000");
  array.append("250000");
  array.append("500000");
  array.append("1000000");
  array.append("2500000");
  array.append("5000000");
  array.append("10000000");
  array.append("25000000");
  array.append("50000000");
  array.append("100000000");
  array.append("250000000");
  array.append("500000000");
  array.append("1000000000");
  array.append("2500000000");

  int ticks;
  for (ticks = 2; (ticks * 15) < _height; ticks++)
    ;
  ticks--;
  if (ticks > 10)
    ticks = 10;

  double interval = 0;
  for (loop = 0; loop < (int) array.count(); loop++)
  {
    QString t = array[loop];
    if ((range / t.toDouble()) < ticks)
    {
      interval = t.toDouble();
      break;
    }
  }

  scaleArray.resize(11);

  loop = 0;
  double t = 0 - (ticks * interval);
  while (t <= scaleHigh)
  {
    t = t + interval;

    if (t >= scaleLow)
    {
      scaleArray[loop] = t;
      loop++;
    }
  }

  scaleArray.resize(loop);
}

int Plot::convertToY (double val)
{
  if (mainFlag && logScale)
  {
    if (val <= 0.0)
      return _height;
    else
      return (int) (_height * (logScaleHigh - log(val)) / logRange);
  }
  double t = val - scaleLow;
  int y = (int) (t * scaler);
  y = _height - y;
  if (y > _height)
    y = _height;
  return y;
}

double Plot::convertToVal (int y)
{
  if (mainFlag && logScale) {
    if (y >= _height)
      return scaleLow;
    else
      return exp(logScaleHigh - ((y * logRange) / _height));
  }
  int p = _height - y;
  double val = scaleLow + (p / scaler) ;
  return val;
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
  int x = startX;
  int loop = startIndex;

  Setting *r = data->at(startIndex);
  QDateTime date = QDateTime::fromString(r->getDateTime("Date"), Qt::ISODate);
  if (d.date() < date.date())
    return -1;

  while(x <= _width && loop < (int) data->count())
  {
    r = data->at(loop);
    date = QDateTime::fromString(r->getDateTime("Date"), Qt::ISODate);

    if (date.date() >= d.date())
      break;

    x = x + pixelspace;
    loop++;
  }

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

  Setting *r = data->at(loop);
  double t = r->getFloat("Open");
  int y;
  if (t)
  {
    y = convertToY(t);
    painter.drawLine (x - 2, y, x, y);
  }

  y = convertToY(r->getFloat("Close"));
  painter.drawLine (x + 2, y, x, y);

  int h = convertToY(r->getFloat("High"));

  int l = convertToY(r->getFloat("Low"));
  painter.drawLine (x, h, x, l);

  x = x + pixelspace;
  loop++;

  while ((x < _width) && (loop < (int) data->count()))
  {
    r = data->at(loop);
    Setting *pr = data->at(loop - 1);

    if (r->getFloat("Close") > pr->getFloat("Close"))
      painter.setPen(upColor);
    else
    {
      if (r->getFloat("Close") < pr->getFloat("Close"))
        painter.setPen(downColor);
      else
        painter.setPen(neutralColor);
    }

    t = r->getFloat("Open");
    if (t)
    {
      y = convertToY(t);
      painter.drawLine (x - 2, y, x, y);
    }

    y = convertToY(r->getFloat("Close"));
    painter.drawLine (x + 2, y, x, y);

    h = convertToY(r->getFloat("High"));
    l = convertToY(r->getFloat("Low"));
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

  painter.setPen(neutralColor);

  Setting *r = data->at(loop);
  int h = convertToY(r->getFloat("High"));
  int l = convertToY(r->getFloat("Low"));
  int c = convertToY(r->getFloat("Close"));
  int o = convertToY(r->getFloat("Open"));

  painter.drawLine (x, h, x, l);

  if (r->getFloat("Open") != 0)
  {
    if (c > o)
    {
      painter.fillRect(x - 2, c, 5, o - c, backgroundColor);
      painter.drawRect(x - 2, c, 5, o - c);
    }
    else
      painter.fillRect(x - 2, o, 5, c - o, painter.brush());
  }

  loop++;
  x = x + pixelspace;

  while ((x < _width) && (loop < (int) data->count()))
  {
    r = data->at(loop);
    Setting *pr = data->at(loop - 1);

    if (r->getFloat("Close") > pr->getFloat("Close"))
      painter.setPen(upColor);
    else
    {
      if (r->getFloat("Close") < pr->getFloat("Close"))
        painter.setPen(downColor);
      else
        painter.setPen(neutralColor);
    }

    h = convertToY(r->getFloat("High"));
    l = convertToY(r->getFloat("Low"));
    c = convertToY(r->getFloat("Close"));
    o = convertToY(r->getFloat("Open"));

    painter.drawLine (x, h, x, l);

    if (r->getFloat("Open") != 0)
    {
      if (c > o)
      {
        painter.fillRect(x - 2, c, 5, o - c, backgroundColor);
        painter.drawRect(x - 2, c, 5, o - c);
      }
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
    Setting *r = data->at(loop);

    y2 = convertToY(r->getFloat("Close"));
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

    Setting *r = data->at(loop);

    int y;
    if (r->getFloat("Open") != 0)
    {
      y = convertToY(r->getFloat("Open"));
      painter.drawLine (x - 2, y, x, y);
    }

    y = convertToY(r->getFloat("Close"));
    painter.drawLine (x + 2, y, x, y);

    int h = convertToY(r->getFloat("High"));
    int l = convertToY(r->getFloat("Low"));
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
    Setting *r = data->at(loop);
    Setting *pr = data->at(loop - 1);

    switch (status)
    {
      case 1:
        if (r->getFloat("High") < pr->getFloat("High") && r->getFloat("Low") < pr->getFloat("Low"))
	{
	  painter.setPen(upColor);
          h = convertToY(high);
          l = convertToY(low);
          painter.drawLine (x, h, x, l);

          painter.drawLine (oldx, l, x, l);

	  status = -1;
	  oldx = x;
          low = r->getFloat("Low");
	}
	else
	{
          if (r->getFloat("High") > high)
           high = r->getFloat("High");
	}
	break;
      case -1:
        if (r->getFloat("High") > pr->getFloat("High") && r->getFloat("Low") > pr->getFloat("Low"))
	{
	  painter.setPen(downColor);
          h = convertToY(high);
          l = convertToY(low);
          painter.drawLine (x, h, x, l);

          painter.drawLine (oldx, h, x, h);

	  status = 1;
	  oldx = x;
          high = r->getFloat("High");
	}
	else
	{
          if (r->getFloat("Low") < low)
           low = r->getFloat("Low");
        }
	break;
      default:
        if (r->getFloat("High") < pr->getFloat("High") && r->getFloat("Low") < pr->getFloat("Low"))
	{
	  status = -1;
	  oldx = x;
          high = r->getFloat("High");
          low = r->getFloat("Low");
	}
	else
	{
          if (r->getFloat("High") > pr->getFloat("High") && r->getFloat("Low") > pr->getFloat("Low"))
	  {
	    status = 1;
	    oldx = x;
            high = r->getFloat("High");
            low = r->getFloat("Low");
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
      h = convertToY(high);
      l = convertToY(low);
      painter.drawLine (x, h, x, l);
      painter.drawLine (oldx, l, x, l);
        break;
    case -1:
      painter.setPen(downColor);
      h = convertToY(high);
      l = convertToY(low);
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
  double size = (scaleArray[1] - scaleArray[0]) / 4.0;
  int reversal = 3;
  int symbol;

  Setting *r = data->at(loop);

  double ph = r->getFloat("High");
  double pl = r->getFloat("Low");
  double t2 = r->getFloat("Close");
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
    r = data->at(loop);

    double h = r->getFloat("High");
    double l = r->getFloat("Low");

    if (! symbol)
    {
      if (l <= (pl - size))
      {
        t = (int) (l / size);
        pl = t * size;
      }
      else
      {
        if (h >= (pl + ((reversal + 1) * size)))
        {
	  int y = convertToY(ph);
	  int y2= convertToY(pl);
          painter.fillRect(x, y, x2 - x + pixelspace, y2 - y, downColor);

	  double val = ph - size;
	  while (val > pl)
	  {
	    y = convertToY(val);
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
        if (l <= (ph - ((reversal + 1) * size)))
        {
	  int y = convertToY(ph);
	  int y2= convertToY(pl);
          painter.fillRect(x, y, x2 - x + pixelspace, y2 - y, upColor);

	  double val = ph - size;
	  while (val > pl)
	  {
	    y = convertToY(val);
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

  int y = convertToY(ph);
  int y2= convertToY(pl);
  if (! symbol)
    painter.fillRect(x, y, x2 - x + pixelspace, y2 - y, downColor);
  else
    painter.fillRect(x, y, x2 - x + pixelspace, y2 - y, upColor);
  double val = ph - size;
  while (val > pl)
  {
    y = convertToY(val);
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

  int y = convertToY(co->getFloat(QObject::tr("Value")));

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
  QString v = "0.238";
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
    r = low + (range * v);

  int y = convertToY(r);
  painter.drawLine (startX, y, _width, y);

  QString s = label;
  s.append(" - ");
  s.append(QString::number(r));
  painter.drawText(startX, y - 1, s, -1);

  painter.end();
}

void Plot::drawHorizontalLine (Setting *co)
{
  QPainter painter;
  painter.begin(&buffer);
  painter.setFont(plotFont);

  int y = convertToY(co->getFloat(QObject::tr("Value")));

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

  int y = convertToY(co->getFloat(QObject::tr("Value")));

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

  int y = convertToY(co->getFloat(QObject::tr("Value")));

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

  dt = QDateTime::fromString(co->getDateTime(QObject::tr("Start Date")), Qt::ISODate);

  int x = getXFromDate(dt);
  if (x == -1)
    return;

  int y = convertToY(co->getFloat(QObject::tr("Start Value")));
  int y2 = convertToY(co->getFloat(QObject::tr("End Value")));

  QColor color(co->getData(QObject::tr("Color")));
  painter.setPen(color);

  painter.drawLine (x, y, x2, y2);

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







