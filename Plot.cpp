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
#include "Indicator.h"
#include <qpainter.h>
#include <qpen.h>
#include <qpoint.h>
#include <qpointarray.h>

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
  startIndex = 0;
  scaleHigh = 0;
  scaleLow = 0;
  mainHigh = 0;
  mainLow = 0;
  chartType = "None";

  plotFont.setFamily("Helvetica");
  plotFont.setPointSize(9);
  plotFont.setWeight(50);

  objectCount = 0;
  objectList.setAutoDelete(TRUE);

  lineCount = 0;
  lineList.setAutoDelete(TRUE);
}

Plot::~Plot ()
{
}

void Plot::clear ()
{
  alerts.truncate(0);
  scaleHigh = 0;
  scaleLow = 0;
  mainHigh = 0;
  mainLow = 0;

  openArray.truncate(0);
  highArray.truncate(0);
  lowArray.truncate(0);
  closeArray.truncate(0);
}

void Plot::clearDate ()
{
  dateArray.truncate(0);
}

void Plot::setData (QList<Setting> l)
{
  dateArray.resize(l.count());

  if (mainFlag)
  {
    openArray.resize(l.count());
    highArray.resize(l.count());
    lowArray.resize(l.count());
    closeArray.resize(l.count());
    alerts.resize(l.count());
    alerts.fill(0, -1);
  }
  
  bool flag = FALSE;
  int loop;
  for (loop = 0; loop < (int) l.count(); loop++)
  {
    Setting *set = l.at(loop);

    QString s = set->getData("Date");
    s.insert(4, "-");
    s.insert(7, "-");
    s.insert(12, ":");
    s.insert(15, ":");
    dateArray[loop] = QDateTime::fromString(s, Qt::ISODate);

    if (mainFlag)
    {
      if (! flag)
      {
        mainHigh = set->getFloat("High");
        mainLow = set->getFloat("Low");
	flag = TRUE;
      }
;
      double t = set->getFloat("High");
      if (t > mainHigh)
        mainHigh = t;

      t = set->getFloat("Low");
      if (t < mainLow)
	mainLow = t;

      openArray[loop] = set->getFloat("Open");
      highArray[loop] = set->getFloat("High");
      lowArray[loop] = set->getFloat("Low");
      closeArray[loop] = set->getFloat("Close");
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

    if (! d.compare(tr("Point And Figure")))
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

void Plot::draw ()
{
  buffer.fill(backgroundColor);

  if (dateArray.size())
  {
    setHeight();

    setWidth();

    if (dateFlag)
      drawDate();

    setScale();

    drawXGrid();

    drawYGrid();

    if (chartType.compare("None"))
    {
      while (1)
      {
        if (! chartType.compare(tr("Bar")))
        {
          drawBars();
          break;
        }

        if (! chartType.compare(tr("Candle")))
        {
          drawCandle();
          break;
        }

        if (! chartType.compare(tr("Line")))
        {
          drawLineChart();
          break;
        }

        if (! chartType.compare(tr("Paint Bar")))
        {
          drawPaintBar();
          break;
        }

        if (! chartType.compare(tr("Point And Figure")))
          drawPointAndFigure();

        break;
      }
    }

    if (chartType.compare(tr("Point And Figure")))
      drawLines();

    if (chartType.compare(tr("Point And Figure")))
      drawObjects();

    drawScale();

    drawInfo();
  }

  paintEvent(0);
}

void Plot::drawObjects ()
{
  QIntDictIterator<Setting> it(objectList);
  while (it.current())
  {
    Setting *co = it.current();

    QString type = co->getData("Show");
    if (! type.compare("False"))
    {
      ++it;
      continue;
    }

    type = co->getData(QObject::tr("Type"));

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

    ++it;
  }
}

void Plot::drawLines ()
{
  // try to order the lines so that any histograms are drawn first
  QStringList l;
  QString s;
  QIntDictIterator<PlotLine> it(lineList);
  while (it.current())
  {
    currentLine = it.current();

    if (currentLine->show == TRUE)
    {
      l.append(QString::number(it.currentKey()));

      if (! currentLine->type.compare("Histogram"))
        s = l[l.count() - 1];
    }

    ++it;
  }
  l.sort();
  if (s.length())
  {
    l.remove(s);
    l.prepend(s);
  }

  int loop;
  for (loop = 0; loop < (int) l.count(); loop++)
  {
    currentLine = lineList[l[loop].toInt()];

    while (1)
    {
      if (! currentLine->type.compare("Histogram"))
      {
        drawHistogram();
	break;
      }

      if (! currentLine->type.compare("Histogram Bar"))
      {
        drawHistogramBar();
	break;
      }

      if (! currentLine->type.compare("Dot"))
      {
        drawDot();
	break;
      }

      if (! currentLine->type.compare("Line") || ! currentLine->type.compare("Dash"))
      {
        drawLine();
	break;
      }

      if (! currentLine->type.compare("Horizontal"))
        drawHorizontalLine();
	
      break;
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
    if (! dateArray.size())
      return;
  }
  else
  {
    if (! lineList.count())
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
    QDateTime oldDate = dateArray[loop];
    QDateTime oldWeek = oldDate;
    oldWeek = oldWeek.addDays(7 - oldWeek.date().dayOfWeek());

    while(x <= _width && loop < (int) dateArray.size())
    {
      QDateTime date = dateArray[loop];

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
      QDateTime oldMonth = dateArray[loop];

      while(x <= _width && loop < (int) dateArray.size())
      {
        QDateTime date = dateArray[loop];

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
      QDateTime oldYear = dateArray[loop];

      while(x <= _width && loop < (int) dateArray.size())
      {
        QDateTime date = dateArray[loop];

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
    QDateTime oldDate = dateArray[loop];

    while(x <= _width && loop < (int) dateArray.size())
    {
      QDateTime date = dateArray[loop];

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
    QDateTime oldYear = dateArray[loop];

    while(x <= _width && loop < (int) dateArray.size())
    {
      QDateTime date = dateArray[loop];

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

void Plot::showLine (int id)
{
  PlotLine *pl = lineList[id];
  if (pl)
    pl->show = TRUE;
}

void Plot::hideLines ()
{
  QIntDictIterator<PlotLine> it(lineList);
  while (it.current())
  {
    PlotLine *pl = it.current();
    pl->show = FALSE;
    ++it;
  }
}

int Plot::addLine (QString c, QString lt, QString l, QMemArray<double> d)
{
  PlotLine *pl = new PlotLine;
  pl->color.setNamedColor(c);
  pl->type = lt;
  pl->label = l;
  pl->data = d;
  pl->show = TRUE;
  pl->high = 0;
  pl->low = 0;

  bool flag = FALSE;
  int loop;
  for (loop = 0; loop < (int) d.size(); loop++)
  {
    if (! flag)
    {
      pl->high = d[loop];
      pl->low = d[loop];
      flag = TRUE;
    }

    if (d[loop] > pl->high)
      pl->high = d[loop];

    if (d[loop] < pl->low)
      pl->low = d[loop];
  }

  lineCount++;
  lineList.insert(lineCount, pl);

  return lineCount;
}

void Plot::clearLines ()
{
  lineList.clear();
}

QMemArray<double> Plot::getLineData (int d)
{
  PlotLine *pl = lineList[d];
  if (pl)
    return pl->data;
  else
    return 0;
}

QString Plot::getLineLabel (int d)
{
  PlotLine *pl = lineList[d];
  if (pl)
    return pl->label;
  else
    return QString::null;
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
  pen.setColor(currentLine->color);

  if (! currentLine->type.compare("Dash"))
    pen.setStyle(Qt::DotLine);
  else
    pen.setStyle(Qt::SolidLine);
  painter.setPen(pen);

  int x = -1;
  int x2 = startX;
  int y = -1;
  int y2 = -1;
  int loop = currentLine->data.size() - dateArray.size() + startIndex;

  while ((x2 < _width) && (loop < (int) currentLine->data.size()))
  {
    if (loop > -1)
    {
      y2 = convertToY(currentLine->data[loop]);
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
  pen.setColor(currentLine->color);
  painter.setPen(pen);

  int y = convertToY(currentLine->data[currentLine->data.size() - 1]);

  painter.drawLine (0, y, _width, y);

  painter.drawText(startX, y - 1, strip(currentLine->data[currentLine->data.size() - 1]));

  painter.end();
}

void Plot::drawDot ()
{
  QPainter painter;
  painter.begin(&buffer);

  QPen pen;
  pen.setColor(currentLine->color);
  painter.setPen(pen);

  int x = startX;
  int loop = currentLine->data.size() - dateArray.size() + startIndex;

  while ((x < _width) && (loop < (int) currentLine->data.size()))
  {
    if (loop > -1)
    {
      int y = convertToY(currentLine->data[loop]);
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
  painter.setPen(currentLine->color);
  painter.setBrush(currentLine->color);

  int loop = currentLine->data.size() - dateArray.size() + startIndex;

  QPointArray pa(4);
  int zero = convertToY(0);
  int x = -1;
  int x2 = startX;
  int y = -1;
  int y2 = -1;

  while ((x < _width) && (loop < (int) currentLine->data.size()))
  {
    if (loop > -1)
    {
      y2 = convertToY(currentLine->data[loop]);
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

  QColor color(currentLine->color);

  int x = startX;
  int zero = convertToY(0);

  int loop = currentLine->data.size() - dateArray.size() + startIndex;

  while ((x < _width) && (loop < (int) currentLine->data.size()))
  {
    if (loop > -1)
    {
      int y = convertToY(currentLine->data[loop]);
      painter.fillRect(x, y, pixelspace - 1, zero - y, color);
    }

    x = x + pixelspace;
    loop++;
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
  s.append(dateArray[dateArray.size() - 1].toString("yyyyMMdd"));
  s.append(" ");
  painter.drawText(pos, 10, s, -1);
  pos = pos + fm.width(s);

  if (closeArray.size())
  {
    s = "O=";
    s.append(QString::number(openArray[openArray.size() - 1]));
    s.append(" H=");
    s.append(QString::number(highArray[highArray.size() - 1]));
    s.append(" L=");
    s.append(QString::number(lowArray[lowArray.size() - 1]));
    s.append(" C=");
    s.append(QString::number(closeArray[closeArray.size() - 1]));
    s.append(" ");
    painter.drawText(pos, 10, s, -1);
    pos = pos + fm.width(s);

    double ch = 0;
    if (dateArray.size() > 1)
      ch = closeArray[closeArray.size() - 1] - closeArray[closeArray.size() - 2];
    s = "CH=";
    s.append(QString::number(ch));
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

  QIntDictIterator<PlotLine> it(lineList);
  while (it.current())
  {
    PlotLine *line = it.current();
    
    if (line->show == FALSE)
    {
      ++it;
      continue;
    }

    if (line->data.size() > 1)
    {
      s = line->label;
      s.append("=");
      s.append(strip(line->data[line->data.size() - 1]));
      s.append(" ");

      painter.setPen(line->color);
      painter.drawText(pos, 10, s, -1);
      pos = pos + fm.width(s);
    }

    ++it;
  }

  painter.end();
}

void Plot::crossHair (int x, int y)
{
  if (x > buffer.width() - SCALE_WIDTH)
    return;

  draw();

  QPainter painter;
  painter.begin(&buffer);
  painter.setPen(QPen(borderColor, 1, QPen::DotLine));
  painter.drawLine (0, y, buffer.width() - SCALE_WIDTH, y);
  painter.drawLine (x, 0, x, buffer.height());

  int i = (x / pixelspace) + startIndex;
  if (i >= (int) dateArray.size())
    i = dateArray.size() - 1;
  if (i < startIndex)
    i = startIndex;

  QString s = dateArray[i].toString("yyyyMMdd");
  s.append(" ");
  s.append(QString::number(convertToVal(y)));
  emit statusMessage(s);

  painter.end();

  paintEvent(0);
}

Setting * Plot::newChartObject (QString d)
{
  Setting *set = new Setting();
  set->set(QObject::tr("Color"), "red", Setting::Color);
  set->set(QObject::tr("Plot"), QObject::tr("Main Plot"), Setting::None);
  set->set("Show", "True", Setting::None);

  QDateTime dt = QDateTime::currentDateTime();

  if (! d.compare(QObject::tr("Buy Arrow")))
  {
    set->set(QObject::tr("Date"), dt.toString("yyyyMMdd"), Setting::Date);
    set->set(QObject::tr("Value"), "0", Setting::Float);
    set->set(QObject::tr("Type"), QObject::tr("Buy Arrow"), Setting::None);
    return set;
  }

  if (! d.compare(QObject::tr("Sell Arrow")))
  {
    set->set(QObject::tr("Date"), dt.toString("yyyyMMdd"), Setting::Date);
    set->set(QObject::tr("Value"), "0", Setting::Float);
    set->set(QObject::tr("Type"), QObject::tr("Sell Arrow"), Setting::None);
    return set;
  }

  if (! d.compare(QObject::tr("Fibonacci Line")))
  {
    set->set(QObject::tr("High"), "0", Setting::Float);
    set->set(QObject::tr("Low"), "0", Setting::Float);
    set->set(QObject::tr("Support"), QObject::tr("False"), Setting::Bool);
    set->set("0.238", QObject::tr("False"), Setting::Bool);
    set->set("0.383", QObject::tr("False"), Setting::Bool);
    set->set("0.5", QObject::tr("False"), Setting::Bool);
    set->set("0.618", QObject::tr("False"), Setting::Bool);
    set->set("1", QObject::tr("False"), Setting::Bool);
    set->set("1.618", QObject::tr("False"), Setting::Bool);
    set->set("2.618", QObject::tr("False"), Setting::Bool);
    set->set("4.236", QObject::tr("False"), Setting::Bool);
    set->set(QObject::tr("Type"), QObject::tr("Fibonacci Line"), Setting::None);
    return set;
  }

  if (! d.compare(QObject::tr("Horizontal Line")))
  {
    set->set(QObject::tr("Value"), "0", Setting::Float);
    set->set(QObject::tr("Type"), QObject::tr("Horizontal Line"), Setting::None);
    return set;
  }

  if (! d.compare(QObject::tr("Vertical Line")))
  {
    set->set(QObject::tr("Date"), dt.toString("yyyyMMdd"), Setting::Date);
    set->set(QObject::tr("Type"), QObject::tr("Vertical Line"), Setting::None);
    return set;
  }

  if (! d.compare(QObject::tr("Trend Line")))
  {
    set->set(QObject::tr("Start Date"), dt.toString("yyyyMMdd"), Setting::Date);
    set->set(QObject::tr("Start Value"), "0", Setting::Float);
    set->set(QObject::tr("End Date"), dt.toString("yyyyMMdd"), Setting::Date);
    set->set(QObject::tr("End Value"), "0", Setting::Float);
    set->set(QObject::tr("Type"), QObject::tr("Trend Line"), Setting::None);
    return set;
  }

  if (! d.compare(QObject::tr("Text")))
  {
    set->set(QObject::tr("Date"), dt.toString("yyyyMMdd"), Setting::Date);
    set->set(QObject::tr("Value"), "0", Setting::Float);
    set->set(QObject::tr("Label"), QObject::tr("Text"), Setting::Text);
    set->set(QObject::tr("Type"), QObject::tr("Text"), Setting::None);
  }

  return set;
}

int Plot::insertChartObject (Setting *d)
{
  Setting *set = new Setting;
  QStringList l = d->getKeyList();
  int loop;
  for (loop = 0; loop < (int) l.count(); loop++)
  {
    SettingItem *item = d->getItem(l[loop]);
    set->set(item->key, item->data, (Setting::Type) item->type);
    set->setList(item->key, item->list);
  }

  objectCount++;
  objectList.insert(objectCount, set);
  return objectCount;
}

void Plot::deleteChartObject (int id)
{
  objectList.remove(id);
}

Setting * Plot::getChartObject (int id)
{
  return objectList[id];
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

void Plot::showChartObject (int id)
{
  Setting *set = objectList[id];
  if (set)
    set->set("Show", "True", Setting::None);
}

void Plot::clearChartObjects ()
{
  objectList.clear();
}

void Plot::hideChartObjects ()
{
  QIntDictIterator<Setting> it(objectList);
  while (it.current())
  {
    Setting *co = it.current();
    co->set("Show", "False", Setting::None);
    ++it;
  }
}

void Plot::setScale ()
{
  scaleHigh = 0;
  scaleLow = 0;
  bool flag = FALSE;

  if (mainFlag)
  {
    if (! scaleToScreen)
    {
      scaleHigh = mainHigh;
      scaleLow = mainLow;
      flag = TRUE;
    }
    else
    {
      int x = startX;
      int loop = startIndex;
      while ((x < _width) && (loop < (int) dateArray.size()))
      {
        if (! flag)
	{
          scaleHigh = highArray[loop];
          scaleLow = lowArray[loop];
	  flag = TRUE;
	}

        if (highArray[loop] > scaleHigh)
	  scaleHigh = highArray[loop];

        if (lowArray[loop] < scaleLow)
	  scaleLow = lowArray[loop];

        x = x + pixelspace;
        loop++;
      }
    }
  }

  if (! scaleToScreen)
  {
    QIntDictIterator<PlotLine> it(lineList);
    while (it.current())
    {
      PlotLine *line = it.current();

      if (line->show == FALSE)
      {
        ++it;
        continue;
      }
      
      if (! flag)
      {
        scaleHigh = line->high;
        scaleLow = line->low;
	flag = TRUE;
      }

      if (line->high > scaleHigh)
        scaleHigh = line->high;

      if (line->low < scaleLow)
        scaleLow = line->low;

      ++it;
    }
  }
  else
  {
    QIntDictIterator<PlotLine> it(lineList);
    while (it.current())
    {
      PlotLine *line = it.current();

      if (line->show == FALSE)
      {
        ++it;
        continue;
      }

      int x = startX;
      int loop = line->data.size() - dateArray.size() + startIndex;
      while ((x < _width) && (loop < (int) line->data.size()))
      {
        if (loop > -1)
        {
          if (! flag)
          {
            scaleHigh = line->data[loop];
            scaleLow = line->data[loop];
	    flag = TRUE;
          }

          if (line->data[loop] > scaleHigh)
            scaleHigh = line->data[loop];

          if (line->data[loop] < scaleLow)
            scaleLow = line->data[loop];
        }

        x = x + pixelspace;
        loop++;
      }

      ++it;
    }
  }

  QIntDictIterator<Setting> it(objectList);
  while (it.current())
  {
    Setting *co = it.current();

    QString type = co->getData("Show");
    if (! type.compare("False"))
    {
      ++it;
      continue;
    }

    type = co->getData(QObject::tr("Type"));
    if (! type.compare(QObject::tr("Vertical Line")))
    {
      ++it;
      continue;
    }

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
      ++it;
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
      ++it;
      continue;
    }

    double v = co->getFloat(QObject::tr("Value"));
    if (v > scaleHigh)
      scaleHigh = v;
    if (v < scaleLow)
      scaleLow = v;

    ++it;
  }

  scaleHigh = scaleHigh + (scaleHigh * 0.01);
  if (scaleLow < 0)
    scaleLow = scaleLow + (scaleLow * 0.1);
  else
    scaleLow = scaleLow - (scaleLow * 0.01);
  double range = scaleHigh - scaleLow;
  scaler = _height / range;

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
  int loop;
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
  double t = val - scaleLow;
  int y = (int) (t * scaler);
  y = _height - y;
  if (y > _height)
    y = _height;
  return y;
}

double Plot::convertToVal (int y)
{
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

  QDateTime date = dateArray[startIndex];
  if (d.date() < date.date())
    return -1;

  while(x <= _width && loop < (int) dateArray.size())
  {
    QDateTime date = dateArray[loop];
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

  double t = openArray[loop];
  int y;
  if (t)
  {
    y = convertToY(t);
    painter.drawLine (x - 2, y, x, y);
  }

  y = convertToY(closeArray[loop]);
  painter.drawLine (x + 2, y, x, y);

  int h = convertToY(highArray[loop]);

  int l = convertToY(lowArray[loop]);
  painter.drawLine (x, h, x, l);

  x = x + pixelspace;
  loop++;

  while ((x < _width) && (loop < (int) dateArray.size()))
  {
    if (closeArray[loop] > closeArray[loop - 1])
      painter.setPen(upColor);
    else
    {
      if (closeArray[loop] < closeArray[loop - 1])
        painter.setPen(downColor);
      else
        painter.setPen(neutralColor);
    }

    t = openArray[loop];
    if (t)
    {
      y = convertToY(t);
      painter.drawLine (x - 2, y, x, y);
    }

    y = convertToY(closeArray[loop]);
    painter.drawLine (x + 2, y, x, y);

    h = convertToY(highArray[loop]);
    l = convertToY(lowArray[loop]);
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

  int h = convertToY(highArray[loop]);
  int l = convertToY(lowArray[loop]);
  int c = convertToY(closeArray[loop]);
  int o = convertToY(openArray[loop]);
  
  painter.drawLine (x, h, x, l);

  if (openArray[loop] != 0)
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

  while ((x < _width) && (loop < (int) dateArray.size()))
  {
    if (closeArray[loop] > closeArray[loop - 1])
      painter.setPen(upColor);
    else
    {
      if (closeArray[loop] < closeArray[loop - 1])
        painter.setPen(downColor);
      else
        painter.setPen(neutralColor);
    }

    h = convertToY(highArray[loop]);
    l = convertToY(lowArray[loop]);
    c = convertToY(closeArray[loop]);
    o = convertToY(openArray[loop]);

    painter.drawLine (x, h, x, l);

    if (openArray[loop] != 0)
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

  while ((x < _width) && (loop < (int) dateArray.size()))
  {
    y2 = convertToY(closeArray[loop]);
    painter.drawLine (x, y, x2, y2);
    x = x2;
    y = y2;

    x2 = x2 + pixelspace;
    loop++;
  }

  painter.end();
}

void Plot::drawPaintBar ()
{
  QPainter painter;
  painter.begin(&buffer);

  int x = startX;
  int loop = startIndex;

  while ((x < _width) && (loop < (int) dateArray.size()))
  {
    switch (alerts[loop])
    {
      case -1:
        painter.setPen(downColor);
        break;
      case 1:
        painter.setPen(upColor);
	break;
      default:
        painter.setPen(neutralColor);
	break;
    }

    int y;
    if (openArray[loop] != 0)
    {
      y = convertToY(openArray[loop]);
      painter.drawLine (x - 2, y, x, y);
    }

    y = convertToY(closeArray[loop]);
    painter.drawLine (x + 2, y, x, y);

    int h = convertToY(highArray[loop]);
    int l = convertToY(lowArray[loop]);
    painter.drawLine (x, h, x, l);

    x = x + pixelspace;
    loop++;
  }

  painter.end();
}

void Plot::setAlerts (QMemArray<int> d)
{
  alerts = d;
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

  double ph = highArray[loop];
  double pl = lowArray[loop];
  double t2 = closeArray[loop];
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
  
  while ((x2 < _width) && (loop < (int) dateArray.size()))
  {
    double h = highArray[loop];
    double l = lowArray[loop];

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

  QString s = co->getData(QObject::tr("Date"));
  s.insert(4, "-");
  s.insert(7, "-");
  s.append("00:00:00");
  QDateTime dt = QDateTime::fromString(s, Qt::ISODate);

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

  QString s = co->getData(QObject::tr("Date"));
  s.insert(4, "-");
  s.insert(7, "-");
  s.append("00:00:00");
  QDateTime dt = QDateTime::fromString(s, Qt::ISODate);

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

  QString s = co->getData(QObject::tr("Date"));
  s.insert(4, "-");
  s.insert(7, "-");
  s.append("00:00:00");
  QDateTime dt = QDateTime::fromString(s, Qt::ISODate);

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

  QString s = co->getData(QObject::tr("End Date"));
  s.insert(4, "-");
  s.insert(7, "-");
  s.append("00:00:00");
  QDateTime dt = QDateTime::fromString(s, Qt::ISODate);

  int x2 = getXFromDate(dt);
  if (x2 == -1)
    return;

  s = co->getData(QObject::tr("Start Date"));
  s.insert(4, "-");
  s.insert(7, "-");
  s.append("00:00:00");
  dt = QDateTime::fromString(s, Qt::ISODate);

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

  QString s = co->getData(QObject::tr("Date"));
  s.insert(4, "-");
  s.insert(7, "-");
  s.append("00:00:00");
  QDateTime dt = QDateTime::fromString(s, Qt::ISODate);

  int x = getXFromDate(dt);
  if (x == -1)
    return;

  QColor color(co->getData(QObject::tr("Color")));
  painter.setPen(color);

  painter.drawLine (x, 0, x, _height);

  painter.end();
}

int Plot::getDataSize ()
{
  return (int) dateArray.size();
}

QDateTime Plot::getDate (int d)
{
  return dateArray[d];
}

double Plot::getOpen (int d)
{
  return openArray[d];
}

double Plot::getHigh (int d)
{
  return highArray[d];
}

double Plot::getLow (int d)
{
  return lowArray[d];
}

double Plot::getClose (int d)
{
  return closeArray[d];
}










