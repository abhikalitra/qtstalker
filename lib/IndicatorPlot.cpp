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

#include "IndicatorPlot.h"
#include "DbPlugin.h"
#include "PrefDialog.h"
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

#include "../pics/indicator.xpm"
#include "../pics/edit.xpm"
#include "../pics/delete.xpm"
#include "../pics/co.xpm"
#include "../pics/print.xpm"
#include "../pics/crosshair.xpm"

IndicatorPlot::IndicatorPlot (QWidget *w) : QWidget(w)
{
  buffer = new QPixmap;
  scaler = 0;
  chartPlugin = 0;
  coPlugin = 0;
  setBackgroundMode(NoBackground);
  startX = 2;
  backgroundColor.setNamedColor("black");
  borderColor.setNamedColor("white");
  gridColor.setNamedColor("#626262");
  pixelspace = 0;
  minPixelspace = 0;
  gridFlag = TRUE;
  interval = BarData::DailyBar;
  mainFlag = FALSE;
  scaleToScreen = FALSE;
  logScale = FALSE;
  startIndex = 0;
  chartType = "None";
  mouseFlag = None;
  hideMainPlot = FALSE;
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

IndicatorPlot::~IndicatorPlot ()
{
  delete buffer;
  delete chartMenu;
  config.closePlugins();
}

void IndicatorPlot::clear ()
{
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

void IndicatorPlot::setData (BarData *l)
{
  if (! l->count())
    return;

  data = l;
  
  createXGrid();
  
  QDictIterator<COPlugin> it(coPlugins);
  for (; it.current(); ++it)
  {
    COPlugin *plug = it.current();
    plug->setData(data);
  }
}

int IndicatorPlot::setChartType (QString d)
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
    qDebug("IndicatorPlot::setChartType:unable to open %s chart plugin", d.latin1());
    return TRUE;
  }
  
  if (chartType.length())
    setChartInput();
      
  chartType = d;
  
  minPixelspace = chartPlugin->getMinPixelspace();
  pixelspace = chartPlugin->getPixelspace();
  startX = chartPlugin->getStartX();
  
  QObject::connect(chartPlugin, SIGNAL(draw()), this, SLOT(draw()));
  
  return FALSE;
}

void IndicatorPlot::setChartInput ()
{
  chartPlugin->setChartInput(data, scaler, buffer);
}

void IndicatorPlot::setMainFlag (bool d)
{
  mainFlag = d;
}

void IndicatorPlot::setScaleToScreen (bool d)
{
  scaleToScreen = d;
}

void IndicatorPlot::setLogScale (bool d)
{
  logScale = d;
}

void IndicatorPlot::setHideMainPlot (bool d)
{
  hideMainPlot = d;
}

bool IndicatorPlot::getHideMainPlot ()
{
  return hideMainPlot;
}

void IndicatorPlot::setChartPath (QString d)
{
  chartPath = d;
}

void IndicatorPlot::setDrawMode (bool d)
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

bool IndicatorPlot::getCrosshairsStatus ()
{
  return crosshairs;
}

void IndicatorPlot::setInfoFlag (bool d)
{
  infoFlag = d;
}

void IndicatorPlot::draw ()
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

    drawInfo();
  }

  paintEvent(0);
  
  emit signalDraw();
}

void IndicatorPlot::drawRefresh ()
{
  paintEvent(0);
}

void IndicatorPlot::drawLines ()
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

void IndicatorPlot::paintEvent (QPaintEvent *)
{
  bitBlt(this, 0, 0, buffer);
}

void IndicatorPlot::resizeEvent (QResizeEvent *event)
{
  buffer->resize(event->size());
  draw();
}

void IndicatorPlot::mousePressEvent (QMouseEvent *event)
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

void IndicatorPlot::contextMenuEvent (QContextMenuEvent *)
{
  if (drawMode && mouseFlag == COSelected && coPlugin)
    coPlugin->showMenu();
  else
    showPopupMenu();
}

void IndicatorPlot::mouseMoveEvent (QMouseEvent *event)
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

void IndicatorPlot::mouseDoubleClickEvent (QMouseEvent *)
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

  if (mouseFlag != COSelected)
    return;
    
  coPlugin->prefDialog();
}

void IndicatorPlot::keyPressEvent (QKeyEvent *key)
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

void IndicatorPlot::setBackgroundColor (QColor d)
{
  backgroundColor = d;
}

void IndicatorPlot::setBorderColor (QColor d)
{
  borderColor = d;
}

void IndicatorPlot::setGridColor (QColor d)
{
  gridColor = d;
}

void IndicatorPlot::setPlotFont (QFont d)
{
  plotFont = d;
}

void IndicatorPlot::setGridFlag (bool d)
{
  gridFlag = d;
}

void IndicatorPlot::setPixelspace (int d)
{
  pixelspace = d;

  if (chartPlugin)
    chartPlugin->setPixelspace(d);
}

void IndicatorPlot::setIndex (int d)
{
  startIndex = d;
}

bool IndicatorPlot::getMainFlag ()
{
  return mainFlag;
}

void IndicatorPlot::drawXGrid ()
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
      painter.drawLine (x, 0, x, buffer->height());
    }
  }

  painter.end();
}

void IndicatorPlot::createXGrid ()
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

void IndicatorPlot::addIndicator (QString d, Indicator *i)
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

Indicator * IndicatorPlot::getIndicator (QString d)
{
  return indicators[d];
}

bool IndicatorPlot::deleteIndicator (QString d)
{
  return indicators.remove(d);
}

void IndicatorPlot::drawYGrid ()
{
  if (! gridFlag)
    return;

  QPainter painter;
  painter.begin(buffer);
  painter.setPen(QPen(gridColor, 1, QPen::DotLine));
  
  QMemArray<double> scaleArray = scaler->getScaleArray();

  int loop;
  for (loop = 0; loop < (int) scaleArray.size(); loop++)
  {
    int y = scaler->convertToY(scaleArray[loop]);
    painter.drawLine (startX, y, buffer->width(), y);
  }

  painter.end();
}

void IndicatorPlot::drawInfo ()
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

void IndicatorPlot::crossHair (int x, int y, bool f)
{
  crossHairFlag = TRUE;
  getXY(x, y);
  crossHairY = y1;
  crossHairX = x1;
  if (f)
    draw();
}

void IndicatorPlot::drawCrossHair ()
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
  painter.drawLine (0, y, buffer->width(), y);
  painter.drawLine (x, 0, x, buffer->height());
  painter.end();
}

void IndicatorPlot::updateStatusBar (int x, int y)
{
  int i = convertXToDataIndex(x);
  QString s = data->getDate(i).getDateTimeString(TRUE);
  s.append(" ");
  s.append(strip(scaler->convertToVal(y), 4));
  emit statusMessage(s);
}

void IndicatorPlot::getXY (int x, int y)
{
  int i = convertXToDataIndex(x);
  x1 = data->getDate(i);
  y1 = scaler->convertToVal(y);
}

void IndicatorPlot::setScale ()
{
  double scaleHigh = -99999999;
  double scaleLow = 99999999;

  if (mainFlag)
  {
    if (! scaleToScreen)
    {
      scaleHigh = data->getMax();
      scaleLow = data->getMin();
    }
    else
    {
      int x = startX;
      int loop = startIndex;
      while ((x < buffer->width()) && (loop < (int) data->count()))
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
        while ((x < buffer->width()) && (loop2 < (int) line->getSize()))
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

  scaler->set(buffer->height(), scaleHigh, scaleLow, logScaleHigh, logRange, logScale);
}

int IndicatorPlot::getPixelspace ()
{
  return pixelspace;
}

int IndicatorPlot::getMinPixelspace ()
{
  return minPixelspace;
}

int IndicatorPlot::getXFromDate (BarDate d)
{
  int x2 = data->getX(d);
  if (x2 == -1)
    return -1;

  int x = startX + (x2 * pixelspace) - (startIndex * pixelspace);

  return x;
}

QString IndicatorPlot::strip (double d, int p)
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

QStringList IndicatorPlot::getIndicators ()
{
  QStringList l;

  QDictIterator<Indicator> it(indicators);
  for (; it.current(); ++it)
    l.append(it.currentKey());

  return l;
}

void IndicatorPlot::printChart ()
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

void IndicatorPlot::showPopupMenu ()
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
    if (it.currentKey().compare("Main IndicatorPlot"))
    {
      int id = chartEditMenu->insertItem(QPixmap(indicator), it.currentKey(), this, SLOT(slotEditIndicator(int)));
      chartEditMenu->setItemParameter(id, id);
    }
  }

  chartMenu->exec(QCursor::pos());
}

void IndicatorPlot::toggleCrosshairs ()
{
  if (crosshairs == FALSE)
    crosshairs = TRUE;
  else
    crosshairs = FALSE;
  emit signalCrosshairsStatus(crosshairs);  
}

void IndicatorPlot::setCrosshairsStatus (bool status)
{
  crosshairs = status;
  crossHairFlag = FALSE;
  draw();
}

void IndicatorPlot::slotEditIndicator (int id)
{
  QString s = chartEditMenu->text(id);
  emit signalEditIndicator(s);
}

void IndicatorPlot::slotNewIndicator ()
{
  emit signalNewIndicator();
}

void IndicatorPlot::slotEditChartPrefs ()
{
  chartPlugin->prefDialog(this);
}

void IndicatorPlot::slotMessage (QString d)
{
  emit statusMessage(d);
}

void IndicatorPlot::slotSliderChanged (int v)
{
  setIndex(v);
  draw();
}

void IndicatorPlot::slotGridChanged (bool d)
{
  setGridFlag(d);
  draw();
}

void IndicatorPlot::slotScaleToScreenChanged (bool d)
{
  setScaleToScreen(d);
  draw();
}

void IndicatorPlot::slotDrawModeChanged (bool d)
{
  setDrawMode(d);
}

void IndicatorPlot::slotLogScaleChanged (bool d)
{
  setLogScale(d);
  draw();
}

void IndicatorPlot::slotHideMainChanged (bool d)
{
  setHideMainPlot(d);
  draw();
}

//*************************************************************************
//******************** chart object functions *****************************
//*************************************************************************

void IndicatorPlot::slotNewChartObject (int id)
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
  QDictIterator<COPlugin> it(coPlugins);
  for(; it.current(); ++it)
    it.current()->getNameList(l);
  
/*  
  QStringList l;
  QString plugin = config.parseDbPlugin(chartPath);
  DbPlugin *db = config.getDbPlugin(plugin);
  if (! db)
  {
    config.closePlugin(plugin);
    return;
  }
  
  db->openChart(chartPath);
  db->getChartObjectsList(l);
  config.closePlugin(plugin);
*/  
  
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

void IndicatorPlot::addChartObject (Setting &set)
{
  // routine to convert old chart objects to plugin format
  // remove later when users have updated
  QString s = set.getData("Plugin");
  if (! s.length())
  {
    s = set.getData("ObjectType");
    
    switch (s.toInt())
    {
      case 0:
        set.setData("Plugin", "VerticalLine");
	break;
      case 1:
        set.setData("Plugin", "HorizontalLine");
	break;
      case 2:
        set.setData("Plugin", "TrendLine");
	break;
      case 3:
        set.setData("Plugin", "Text");
	break;
      case 4:
        set.setData("Plugin", "BuyArrow");
	break;
      case 5:
        set.setData("Plugin", "SellArrow");
	break;
      case 6:
        set.setData("Plugin", "FiboLine");
	break;
      default:
	break;
    }
  }

  COPlugin *plug = coPlugins[set.getData("Plugin")];
  if (! plug)
  {
    plug = config.getCOPlugin(set.getData("Plugin"));
    if (! plug)
      return;
      
    plug->setData(data);
    plug->setScaler(scaler);
    plug->setPixmap(buffer);
    QObject::connect(plug, SIGNAL(signalDraw()), this, SLOT(draw()));
    QObject::connect(plug, SIGNAL(signalRefresh()), this, SLOT(drawRefresh()));
    QObject::connect(plug, SIGNAL(message(QString)), this, SLOT(slotMessage(QString)));
    coPlugins.replace(set.getData("Plugin"), plug);
  }
  
  plug->addObject(set);
}

void IndicatorPlot::drawObjects ()
{
  QDictIterator<COPlugin> it(coPlugins);
  for (; it.current(); ++it)
  {
    COPlugin *plug = it.current();
    plug->draw(startIndex, pixelspace, startX);
  }
}

void IndicatorPlot::slotDeleteAllChartObjects ()
{
  if (! chartPath.length())
    return;
    
  QDir dir;
  if (! dir.exists(chartPath))
    return;

  QDictIterator<COPlugin> it(coPlugins);
  QStringList l;
  for(; it.current(); ++it)
    l.append(it.currentKey());
    
  if (! l.count())
  {
    QMessageBox::information(this,
                             tr("Qtstalker: Delete All Chart Objects"),
			     tr("No chart objects to delete."));
    return;
  }
    
//  QStringList l = config.getPluginList(Config::COPluginPath);

  int loop;
  PrefDialog *dialog = new PrefDialog;
  dialog->setCaption(tr("Delete All Chart Objects"));
  dialog->createPage (tr("Details"));
  for (loop = 0; loop < (int) l.count(); loop++)
    dialog->addCheckItem(l[loop], tr("Details"), FALSE);
  
  int rc = dialog->exec();
  if (rc == QDialog::Rejected)
  {
    delete dialog;
    return;
  }
  
  QStringList l2;
  for (loop = 0; loop < (int) l.count(); loop++)
  {
    if (dialog->getCheck(l[loop]))
      l2.append(l[loop]);
  }
  
  delete dialog;
  
  if (! l2.count())
    return;

  QString plugin = config.parseDbPlugin(chartPath);
  DbPlugin *db = config.getDbPlugin(plugin);
  if (! db)
  {
    config.closePlugin(plugin);
    return;
  }
  db->openChart(chartPath);

  QStringList l3;  
  db->getChartObjects(l3);
  Setting set;
  for (loop = 0; loop < (int) l3.count(); loop++)
  {
    set.parse(l3[loop]);
    QString s = set.getData("Plugin");
    if (l2.findIndex(s) != -1)
      db->deleteChartObject(set.getData("Name"));
  }
  
  config.closePlugin(plugin);
  
  for (loop = 0; loop < (int) l2.count(); loop++)
  {
    COPlugin *plug = coPlugins[l2[loop]];
    if (plug)
      plug->clear();
  }
  
  mouseFlag = None;
  draw();
}

int IndicatorPlot::convertXToDataIndex (int x)
{
  int i = (x / pixelspace) + startIndex;
  if (i >= (int) data->count())
    i = data->count() - 1;
  if (i < startIndex)
    i = startIndex;
  
  return i;
}

void IndicatorPlot::setCrosshairsFlag (bool d)
{
  crosshairs = d;
}

void IndicatorPlot::setScaler (Scaler *d)
{
  scaler = d;
}


