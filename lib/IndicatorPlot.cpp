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

#include "IndicatorPlot.h"
#include "Config.h"
#include "DataBase.h"
#include "Indicator.h"

#include "../pics/delete.xpm"
#include "../pics/loggrid.xpm"
#include "../pics/date.xpm"
#include "../pics/buyarrow.xpm"
#include "../pics/arc.xpm"
#include "../pics/fib.xpm"
#include "../pics/horizontal.xpm"
#include "../pics/sellarrow.xpm"
#include "../pics/text.xpm"
#include "../pics/trend.xpm"
#include "../pics/vertical.xpm"

#include <QPainter>
#include <QPen>
#include <QPolygon>
#include <QCursor>
#include <QImage>
#include <QMessageBox>
#include <QtDebug>
#include <math.h> // only for fabs()





IndicatorPlot::IndicatorPlot (QWidget *w) : QWidget(w)
{
  startX = 2;
  backgroundColor.setNamedColor("black");
  borderColor.setNamedColor("white");
  gridColor.setNamedColor("#626262");
  pixelspace = 0;
  gridFlag = TRUE;
  interval = BarData::DailyBar;
  scaleToScreen = FALSE;
  logScale = FALSE;
  startIndex = 0;
  mouseFlag = None;
  crossHairFlag = FALSE;
  chartMenu = 0;
  drawMode = FALSE;
  crosshairs = TRUE;
  infoFlag = TRUE;
  coSelected = 0;
  dateFlag = TRUE;
  menuFlag = TRUE;

  plotFont.setFamily("Helvetica");
  plotFont.setPointSize(12);
  plotFont.setWeight(50);  
  plotFontMetrics = new QFontMetrics(plotFont);
  
  data = 0;

  chartMenu = new QMenu(this);
  
  setMouseTracking(TRUE);

  setFocusPolicy(Qt::ClickFocus);

  rubberBand = new QRubberBand(QRubberBand::Rectangle, this);
}

IndicatorPlot::~IndicatorPlot ()
{
  qDeleteAll(coList);
  qDeleteAll(plotList);
}

void IndicatorPlot::clear ()
{
  saveChartObjects();
  qDeleteAll(plotList);
  plotList.clear();
  data = 0;
  mouseFlag = None;
  crossHairFlag = FALSE;
  qDeleteAll(coList);
  coList.clear();
}

void IndicatorPlot::setData (BarData *l)
{
  if (! l->count())
    return;

  data = l;
}

void IndicatorPlot::calculate ()
{
  qDeleteAll(plotList);
  plotList.clear();
}

void IndicatorPlot::setChartPath (QString &d)
{
  chartSymbol = d;
}

void IndicatorPlot::setScaleToScreen (bool d)
{
  scaleToScreen = d;
}

void IndicatorPlot::setLogScale (bool d)
{
  logScale = d;
}

void IndicatorPlot::setDateFlag (bool d)
{
  dateFlag = d;
}

void IndicatorPlot::setDrawMode (bool d)
{
  drawMode = d;
  
  if (drawMode)
    setCursor(QCursor(Qt::ArrowCursor));
  else
    setCursor(QCursor(Qt::CrossCursor));

  if (! drawMode && mouseFlag == COSelected && coSelected)
  {
    QPoint p(-1, -1);
    coSelected->pointerClick(p, x1, y1);
    mouseFlag = None;
  }
}

void IndicatorPlot::setInfoFlag (bool d)
{
  infoFlag = d;
}

void IndicatorPlot::setInterval (BarData::BarLength d)
{
  interval = d;
}

void IndicatorPlot::draw ()
{
  if (buffer.isNull())
    return;
  
  buffer.fill(backgroundColor);

  if (data)
  {
    setScale();
    drawXGrid();
    drawYGrid();
    drawInfo();
    drawLines();
    drawObjects();
    drawCrossHair();
    drawInfo();
  }

  update();
  
  emit signalDraw();
}

void IndicatorPlot::drawRefresh ()
{
  update();
}

void IndicatorPlot::drawLines ()
{
  int loop;
  for (loop = 0; loop < plotList.count(); loop++)
  {
    currentLine = plotList.at(loop);

    if (! currentLine->getSize())
      continue;
            
    switch (currentLine->getType())
    {
      case PlotLine::Histogram:
        drawHistogram();
        break;
      case PlotLine::HistogramBar:
        drawHistogramBar();
        break;
      case PlotLine::Dot:
        drawDot();
        break;
      case PlotLine::Line:
      case PlotLine::Dash:
        drawLine();
        break;
      case PlotLine::Horizontal:
        drawHorizontalLine();
        break;
      case PlotLine::Bar:
        drawBar();
        break;
      case PlotLine::Candle:
        drawCandle();
        break;
      default:
        break;
    }
  }
}

void IndicatorPlot::paintEvent (QPaintEvent *)
{
  QPainter p(this);
  p.drawPixmap(0, 0, buffer);
}

void IndicatorPlot::resizeEvent (QResizeEvent *event)
{
  buffer = QPixmap(event->size());
  draw();
}

void IndicatorPlot::contextMenuEvent (QContextMenuEvent *)
{
  if (drawMode && mouseFlag == COSelected && coSelected)
    coSelected->showMenu();
  else
  {
    if (menuFlag)
      showPopupMenu();
  }
}

void IndicatorPlot::addLine (PlotLine *d)
{
  plotList.append(d);
}

//*********************************************************************
//*************** MOUSE EVENTS ***************************************
//********************************************************************

void IndicatorPlot::mousePressEvent (QMouseEvent *event)
{
  if (! data)
    return;

  if (! drawMode)
  {
    if (event->button() == Qt::LeftButton)
    {
      if (crosshairs)
      {
        crossHair(event->x(), event->y(), TRUE);
        updateStatusBar(event->x(), event->y());
        emit leftMouseButton(event->x(), event->y(), 0);
      }
      else
        updateStatusBar(event->x(), event->y());
      return;
    }
  }
    
  if (event->button() == Qt::LeftButton)
  {
    getXY(event->x(), event->y());
    
    if (mouseFlag == None)
    {
      QHashIterator<QString, COBase *> it(coList);
      while (it.hasNext())
      {
        it.next();
        coSelected = it.value();
	QPoint p(event->x(), event->y());
        COBase::Status rc = coSelected->pointerClick(p, x1, y1);
        if (rc != COBase::None)
        {
          mouseFlag = COSelected;
	  return;
        }
      }
    }
    
    if (mouseFlag == COSelected)
    {
      QPoint p(event->x(), event->y());
      COBase::Status rc = coSelected->pointerClick(p, x1, y1);
      if (rc == COBase::Moving)
      {
        mouseFlag = Moving;
	return;
      }
      
      if (rc == COBase::None)
      {
        mouseFlag = None;
	return;
      }
    }
    
    if (mouseFlag == Moving)
    {
      QPoint p(event->x(), event->y());
      coSelected->pointerClick(p, x1, y1);
      mouseFlag = COSelected;
      return;
    }
    
    if (mouseFlag == ClickWait)
    {
      QPoint p(event->x(), event->y());
      COBase::Status rc = coSelected->pointerClick(p, x1, y1);
      if (rc == COBase::None)
      {
        mouseFlag = None;
        emit statusMessage("");
        setCursor(QCursor(Qt::ArrowCursor));
	return;
      }
    }
  }
}

void IndicatorPlot::mouseMoveEvent (QMouseEvent *event)
{
  if (! data)
    return;

  // ignore moves above the top of the chart - we get draw errors if we don't
  if (event->y() <= 0)
    return;

  if (mouseFlag == RubberBand)
  {
    rubberBand->setGeometry(QRect(mouseOrigin, event->pos()).normalized());
    return;
  }
  
  // are we trying to drag a chart object?
  if (drawMode)
  {
    if (mouseFlag == Moving || mouseFlag == ClickWait)
    {
      getXY(event->x(), event->y());
      QPoint p(event->x(), event->y());
      coSelected->pointerMoving(buffer, p, x1, y1);
      return;
    }
  }

  if (! infoFlag)
    return;
  else
    getInfo(event->x());
}

void IndicatorPlot::mouseDoubleClickEvent (QMouseEvent *event)
{
  if (! data)
    return;

  if (mouseFlag == None && drawMode == FALSE)
  {
    mouseOrigin = event->pos();
    rubberBand->setGeometry(QRect(mouseOrigin, QSize()));
    rubberBand->show();
    mouseFlag = RubberBand;
    return;
  }
    
  if (mouseFlag != COSelected)
    return;

  coSelected->prefDialog();
}

void IndicatorPlot::mouseReleaseEvent(QMouseEvent *)
{
  if (mouseFlag == RubberBand)
  {
    rubberBand->hide();
    mouseFlag = None;

   // FIXME calculate new pixel spacing and position here
  }
}

void IndicatorPlot::getInfo (int x)
{
  int i = convertXToDataIndex(x);

  Setting tr;
  QString s, k;
  data->getDateString(i, s);
  k = "D";
  tr.setData(k, s);
  data->getTimeString(i, s);
  k = "T";
  tr.setData(k, s);

  // If there are Buy/Sell objects, then show them in the data panel
/*
  if (coList.count() > 0)
  {
    QDateTime d;
    bar.getDate(d);
    QDateTime coDate;
    int c = 0;
    Setting set;
    QHashIterator<QString, COBase *> it(coList);
    while (it.hasNext())
    {
      it.next();
      it.value()->getDate(coDate);
      if (coDate == d)
      {
        it.value()->getType(s);
        if (s == "BuyArrow" || s == "SellArrow")
        {
          c++;
          QString label = "Trade";
          label += "-" + QString::number(c);
          QString text = s.replace("Arrow", "");
          k = "Identifier";
          set.getData(k, s);
          if (s != "") text += " " + s;
          k = "Price";
          set.getData(k, s);
          if (s != "") text += " " + s;
          tr.setData(label, text);
        }
      }
    }
  }
*/
  
  if (plotList.count())
  {
    int loop;
    for (loop = 0; loop < plotList.count(); loop++)
    {
      PlotLine *line = plotList.at(loop);
      int li = line->getSize() - data->count() + i;
      if (li > -1 && li < line->getSize())
        line->getInfo(li, tr);
    }
  }

  if (tr.count())
  {
    Setting *r = new Setting;
    tr.copy(r);
    emit infoMessage(r);
  }
}

void IndicatorPlot::keyPressEvent (QKeyEvent *key)
{
  // if chart object selected then pass keyevent to it
  if (mouseFlag == COSelected)
  {
    coSelected->keyEvent(key);
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
    case Qt::Key_PageUp:
    case Qt::Key_PageDown:
    case Qt::Key_Up:
    case Qt::Key_Down:
      emit keyPressed(key);
      break;
    default:
      key->ignore();
      break;
  }
}

void IndicatorPlot::setBackgroundColor (QColor &d)
{
  backgroundColor = d;
}

void IndicatorPlot::setBorderColor (QColor &d)
{
  borderColor = d;
}

void IndicatorPlot::setGridColor (QColor &d)
{
  gridColor = d;
}

void IndicatorPlot::setPlotFont (QFont &d)
{
  plotFont = d;
  delete plotFontMetrics;
  plotFontMetrics = new QFontMetrics(plotFont);
}

void IndicatorPlot::setGridFlag (bool d)
{
  gridFlag = d;
}

void IndicatorPlot::setMenuFlag (bool d)
{
  menuFlag = d;
}

void IndicatorPlot::setPixelspace (int d)
{
  pixelspace = d;
}

void IndicatorPlot::setIndex (int d)
{
  startIndex = d;
}

void IndicatorPlot::setXGrid (QVector<int> &d)
{
  xGrid = d;
}

void IndicatorPlot::drawXGrid ()
{
  if (gridFlag == FALSE)
    return;

  QPainter painter;
  painter.begin(&buffer);
  painter.setPen(QPen(gridColor, 1, Qt::DotLine));

  int loop;
  for (loop = 0; loop < (int) xGrid.size(); loop++)
  {
    if (xGrid[loop] >= startIndex)
    {
      int x = startX + (xGrid[loop] * pixelspace) - (startIndex * pixelspace);
      painter.drawLine (x, 0, x, buffer.height());
    }
  }

  painter.end();
}

void IndicatorPlot::drawYGrid ()
{
  if (! gridFlag)
    return;

  QPainter painter;
  painter.begin(&buffer);
  painter.setPen(QPen(gridColor, 1, Qt::DotLine));
  
  QVector<double> scaleArray;
  scaler.getScaleArray(scaleArray);

  int loop;
  for (loop = 0; loop < (int) scaleArray.size(); loop++)
  {
    int y = scaler.convertToY(scaleArray[loop]);
    painter.drawLine (startX, y, buffer.width(), y);
  }

  painter.end();
}

void IndicatorPlot::drawInfo ()
// plot the indicator values at the top of the screen
{
  QPainter painter;
  painter.begin(&buffer);
  painter.setPen(borderColor);
  painter.setFont(plotFont);
  painter.setBackgroundMode(Qt::OpaqueMode);
//  painter.setBackgroundColor(backgroundColor);
  painter.setBackground(QBrush(backgroundColor));
  
  //QFontMetrics fm = painter.fontMetrics();
  int pos = startX;

  QString s;
  data->getDateString(data->count() - 1, s);
  s.append(" ");
  painter.drawText(pos, 10, s);
  pos = pos + plotFontMetrics->width(s);

  if (plotList.count())
  {
    int loop;
    for (loop = 0; loop < plotList.count(); loop++)
    {
      PlotLine *line = plotList.at(loop);
      if (line->getSize() > 1)
      {
        if (line->getType() == PlotLine::Bar || line->getType() == PlotLine::Candle)
        {
          if (data->count())
          {
            double ch = 0;
            double per = 0;
            if (data->count() > 1)
            {
              ch = data->getClose(data->count() - 1) - data->getClose(data->count() - 2);
              per = (ch / data->getClose(data->count() - 2)) * 100;
            }
            s = "CH=";
            QString str;
            strip(ch, 4, str);
            s.append(str);
            s.append(" (");
            strip(per, 2, str);
            s.append(str);
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
            painter.drawText(pos, 10, s);
            pos = pos + plotFontMetrics->width(s);
  
            painter.setPen(borderColor);
    
            s = "O=";
            strip(data->getOpen(data->count() - 1), 4, str);
            s.append(str);
            s.append(" H=");
            strip(data->getHigh(data->count() - 1), 4, str);
            s.append(str);
            s.append(" L=");
            strip(data->getLow(data->count() - 1), 4, str);
            s.append(str);
            s.append(" C=");
            strip(data->getClose(data->count() - 1), 4, str);
            s.append(str);
            s.append(" ");
            painter.drawText(pos, 10, s);
            pos = pos + plotFontMetrics->width(s);
          }
        }
	
	else if (line->getType() == PlotLine::Horizontal)
	{
	  //dont print value of horizontal at the top 
	  //because its already displayed in the chart
	  continue;
	}
	else
        {
          line->getLabel(s);
          s.append("=");
	  QString str;
	  strip(line->getData(line->getSize() - 1), 4, str);
          s.append(str);
          s.append(" ");

          QColor c;
          line->getColor(c);
          painter.setPen(c);
          painter.drawText(pos, 10, s);
          pos = pos + plotFontMetrics->width(s);
        }
      }
    }
  }
  
  painter.end();
}

/******************************************************************/
/********** Crosshair functions ***********************************/
/*****************************************************************/

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
    
  int y = scaler.convertToY(crossHairY);
  int x = startX + (data->getX(crossHairX) * pixelspace) - (startIndex * pixelspace);
  
  QPainter painter;
  painter.begin(&buffer);
  painter.setPen(QPen(borderColor, 1, Qt::DotLine));
  painter.drawLine (0, y, buffer.width(), y);
  painter.drawLine (x, 0, x, buffer.height());
  painter.end();
}

void IndicatorPlot::setCrosshairsStatus (bool status)
{
  crosshairs = status;
  crossHairFlag = FALSE;
  draw();
}

void IndicatorPlot::setCrosshairsFlag (bool d)
{
  crosshairs = d;
}


void IndicatorPlot::getXY (int x, int y)
{
  int i = convertXToDataIndex(x);
  data->getDate(i, x1);
  y1 = scaler.convertToVal(y);
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

void IndicatorPlot::updateStatusBar (int x, int y)
{
  int i = convertXToDataIndex(x);

  QString s;
  data->getDateTimeString(i, s);
  s.append(" ");
  QString str;
  strip(scaler.convertToVal(y), 4, str);
  s.append(str);
  emit statusMessage(s);
}

void IndicatorPlot::setScale ()
{
  double scaleHigh = -99999999;
  double scaleLow = 99999999;

  if (plotList.count())
  {
    int loop;
    for (loop = 0; loop < plotList.count(); loop++)
    {
      PlotLine *line = plotList.at(loop);
      if (line->getType() == PlotLine::Invisible)
	continue;

      if (line->getScaleFlag())
        continue;

      if (! scaleToScreen)
      {
	if (line->getHigh() > scaleHigh)
          scaleHigh = line->getHigh();

        if (line->getLow() < scaleLow)
          scaleLow = line->getLow();
      }
      else
      {        
        int loop2 = line->getSize() - data->count() + startIndex;
        int end = (buffer.width() / pixelspace) + loop2;
        if (loop2 < 0)
          loop2 = 0;
        if (end > line->getSize())
          end = line->getSize();

        double h, l;
        line->getHighLowRange(loop2, end - 1, h, l);
        if (h > scaleHigh)
          scaleHigh = h;
        if (l < scaleLow)
          scaleLow = l;
      }
    }
  }

  if (! scaleToScreen)
  {
    QHashIterator<QString, COBase *> it(coList);
    while (it.hasNext())
    {
      it.next();
      COBase *plug = it.value();
      
      if (plug->getHigh() > scaleHigh)
        scaleHigh = plug->getHigh();
	
      if (plug->getLow() < scaleLow)
        scaleLow = plug->getLow();
    }
  }

  scaleHigh = scaleHigh * 1.001;
  scaleLow = scaleLow * 0.999;

  double logScaleHigh = 1;
  double logRange = 0;
  if (logScale)
  {
    logScaleHigh = scaleHigh > 0.0 ? log(scaleHigh) : 1;
    double logScaleLow = scaleLow > 0.0 ? log(scaleLow) : 0;
    logRange = logScaleHigh - logScaleLow;
  }

  scaler.set(buffer.height(), scaleHigh, scaleLow, logScaleHigh, logRange, logScale);
}

int IndicatorPlot::getXFromDate (QDateTime &d)
{
  int x2 = data->getX(d);
  if (x2 == -1)
    return -1;

  int x = startX + (x2 * pixelspace) - (startIndex * pixelspace);

  return x;
}

void IndicatorPlot::strip (double d, int p, QString &s)
{
  if (fabs(d) < 1)
    s = QString::number(d, 'f', p);
  else
  {
    if (fabs(d) > 1000)
      s = QString::number(d, 'f', 0);
    else
      s = QString::number(d, 'f', 2);
  }
}

void IndicatorPlot::showPopupMenu ()
{
  chartMenu->clear();
    
  chartObjectMenu = new QMenu(tr("Chart Objects")); 
  chartObjectMenu->addAction(QPixmap(buyarrow), tr("Buy Arrow"), this, SLOT(slotNewBuyArrow()));
  chartObjectMenu->addAction(QPixmap(arc), tr("Cycle"), this, SLOT(slotNewCycle()));
  chartObjectMenu->addAction(QPixmap(fib), tr("Fibonacci Line"), this, SLOT(slotNewFiboline()));
  chartObjectMenu->addAction(QPixmap(horizontal), tr("Horizontal Line"), this, SLOT(slotNewHorizontalLine()));
  chartObjectMenu->addAction(QPixmap(sellarrow), tr("Sell Arrow"), this, SLOT(slotNewSellArrow()));
  chartObjectMenu->addAction(QPixmap(text), tr("Text"), this, SLOT(slotNewText()));
  chartObjectMenu->addAction(QPixmap(trend), tr("Trend Line"), this, SLOT(slotNewTrendLine()));
  chartObjectMenu->addAction(QPixmap(vertical), ("Vertical Line"), this, SLOT(slotNewVerticalLine()));

  chartMenu->addMenu(chartObjectMenu);
  if (! drawMode || ! data)
    chartObjectMenu->setEnabled(FALSE);
  else
    chartObjectMenu->setEnabled(TRUE);
  
  chartMenu->addAction(QPixmap(deleteitem), tr("Delete &All Chart Objects"), this, SLOT(slotDeleteAllChartObjects()), Qt::CTRL+Qt::Key_A);
  chartMenu->addSeparator ();
  chartMenu->addAction(QPixmap(date), tr("Date"), this, SLOT(toggleDate()), Qt::CTRL+Qt::Key_D);
  chartMenu->addAction(QPixmap(loggridicon), tr("Log Scaling"), this, SLOT(toggleLog()), Qt::CTRL+Qt::Key_L);
  
  chartMenu->exec(QCursor::pos());
}

void IndicatorPlot::toggleDate ()
{
  if (dateFlag == FALSE)
    dateFlag = TRUE;
  else
    dateFlag = FALSE;

  emit signalDateFlag(dateFlag);
}

void IndicatorPlot::toggleLog ()
{
  if (logScale == FALSE)
    logScale = TRUE;
  else
    logScale = FALSE;

  emit signalLogFlag(logScale);
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

int IndicatorPlot::getWidth ()
{
  return buffer.width();
}

void IndicatorPlot::getPlotList (QList<PlotLine *> &list)
{
  list = plotList;
}

void IndicatorPlot::setPlotList (QList<PlotLine *> &list)
{
  plotList = list;
}

void IndicatorPlot::setIndicator (QString &d)
{
  indicator = d;
}

void IndicatorPlot::setScaler (Scaler &d)
{
  scaler = d;
}

Scaler & IndicatorPlot::getScaler ()
{
  return scaler;
}

//*************************************************************************
//******************** draw line functions ********************************
//*************************************************************************

void IndicatorPlot::drawLine ()
{
  QPainter painter;
  painter.begin(&buffer);

  QPen pen;
  QColor c;
  currentLine->getColor(c);
  pen.setColor(c);

  if (currentLine->getType() == PlotLine::Dash)
    pen.setStyle(Qt::DotLine);
  else
    pen.setStyle(Qt::SolidLine);
  painter.setPen(pen);

  int x = -1;
  int x2 = startX;
  int y = -1;
  int y2 = -1;
  int loop = currentLine->getSize() - data->count() + startIndex;
  
  Scaler scale;
  if (currentLine->getScaleFlag())
  {
    scale.set(scaler.getHeight(),
  	      currentLine->getHigh(),
	      currentLine->getLow(),
	      scaler.getLogScaleHigh(),
	      scaler.getLogRange(),
	      scaler.getLogFlag());
  }

  while ((x2 < buffer.width()) && (loop < (int) currentLine->getSize()))
  {
    if (loop > -1)
    {
      if (currentLine->getScaleFlag())
        y2 = scale.convertToY(currentLine->getData(loop));
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
}

void IndicatorPlot::drawHorizontalLine ()
{
  QPainter painter;
  painter.begin(&buffer);

  QPen pen;
  QColor c;
  currentLine->getColor(c);
  pen.setColor(c);
  painter.setPen(pen);

  int y = scaler.convertToY(currentLine->getData(currentLine->getSize() - 1));

  painter.drawLine (0, y, buffer.width(), y);

  QString s, s2;
  strip(currentLine->getData(currentLine->getSize() - 1), 4, s);
  currentLine->getLabel(s2);
  s = s2 + "=" + s;
  
  painter.setBackgroundMode(Qt::OpaqueMode);
  painter.setBackground(QBrush(backgroundColor));
  painter.setFont(plotFont);
  
  QRect rc = painter.boundingRect(startX, y - (plotFontMetrics->height()/2), 1, 1, 0, s);
  painter.drawText(rc, s);
  painter.drawRect(rc);
  painter.end();
}

void IndicatorPlot::drawDot ()
{
  QPainter painter;
  painter.begin(&buffer);

  QPen pen;
  QColor c;
  currentLine->getColor(c);
  pen.setColor(c);
  painter.setPen(pen);

  int x = startX;
  int loop = currentLine->getSize() - data->count() + startIndex;
  
  Scaler scale;
  if (currentLine->getScaleFlag())
  {
    scale.set(scaler.getHeight(),
  	      currentLine->getHigh(),
	      currentLine->getLow(),
	      scaler.getLogScaleHigh(),
	      scaler.getLogRange(),
	      scaler.getLogFlag());
  }

  while ((x < buffer.width()) && (loop < (int) currentLine->getSize()))
  {
    if (loop > -1)
    {
      int y;
      if (currentLine->getScaleFlag())
        y = scale.convertToY(currentLine->getData(loop));
      else
        y = scaler.convertToY(currentLine->getData(loop));

      painter.drawPoint(x, y);
    }

    x = x + pixelspace;
    loop++;
  }

  painter.end();
}

void IndicatorPlot::drawHistogram ()
{
  QPainter painter;
  painter.begin(&buffer);
  QColor c;
  currentLine->getColor(c);
  painter.setPen(c);
  painter.setBrush(c);

  int loop = currentLine->getSize() - data->count() + startIndex;

  QPolygon pa(4);

  int zero = 0;
  Scaler scale;
  if (currentLine->getScaleFlag())
  {
    scale.set(scaler.getHeight(),
  	      currentLine->getHigh(),
	      currentLine->getLow(),
	      scaler.getLogScaleHigh(),
	      scaler.getLogRange(),
	      scaler.getLogFlag());
    zero = scale.convertToY(0);
  }
  else
    zero = scaler.convertToY(0);

  int x = -1;
  int x2 = startX;
  int y = -1;
  int y2 = -1;

  while ((x < buffer.width()) && (loop < (int) currentLine->getSize()))
  {
    if (loop > -1)
    {
      if (currentLine->getScaleFlag())
        y2 = scale.convertToY(currentLine->getData(loop));
      else
        y2 = scaler.convertToY(currentLine->getData(loop));
      pa.setPoint(0, x, zero);
      pa.setPoint(1, x, y);
      pa.setPoint(2, x2, y2);
      pa.setPoint(3, x2, zero);

      if (y != -1)
        painter.drawPolygon(pa, Qt::OddEvenFill);

      x = x2;
      y = y2;
    }

    x2 = x2 + pixelspace;
    loop++;
  }

  painter.end();
}

void IndicatorPlot::drawHistogramBar ()
{
  QPainter painter;
  painter.begin(&buffer);

  QColor color;
  currentLine->getColor(color);

  int x = startX;
  int zero = 0;
  Scaler scale;
  if (currentLine->getScaleFlag())
  {
    scale.set(scaler.getHeight(),
  	      currentLine->getHigh(),
	      currentLine->getLow(),
	      scaler.getLogScaleHigh(),
	      scaler.getLogRange(),
	      scaler.getLogFlag());
    zero = scale.convertToY(0);
  }
  else
    zero = scaler.convertToY(0);

  int loop = currentLine->getSize() - data->count() + startIndex;

  while ((x < buffer.width()) && (loop < (int) currentLine->getSize()))
  {
    if (loop > -1)
    {
      int y;
      if (currentLine->getScaleFlag())
        y = scale.convertToY(currentLine->getData(loop));
      else
        y = scaler.convertToY(currentLine->getData(loop));

      if (currentLine->getColorFlag() == TRUE)
	currentLine->getColorBar(loop, color);

      painter.fillRect(x, y, pixelspace - 1, zero - y, color);
    }

    x = x + pixelspace;
    loop++;
  }

  painter.end();
}

void IndicatorPlot::drawBar ()
{
  QPainter painter;
  painter.begin(&buffer);

  int loop = currentLine->getSize() - data->count() + startIndex;
  int x = startX;
  double o = 0;
  double h = 0;
  double l = 0;
  double cl = 0;
  bool ff = FALSE;
  QColor c;

  while ((x < buffer.width()) && (loop < (int) currentLine->getSize()))
  {
    if (loop > -1)
    {
      currentLine->getData(loop, c, o, h, l, cl, ff);

      painter.setPen(c);

      int y = scaler.convertToY(o);
      painter.drawLine (x - 2, y, x, y);

      y = scaler.convertToY(cl);
      painter.drawLine (x + 2, y, x, y);

      y = scaler.convertToY(h);
      int y2 = scaler.convertToY(l);
      painter.drawLine (x, y, x, y2);
    }

    x = x + pixelspace;
    loop++;
  }

  painter.end();
}

void IndicatorPlot::drawCandle ()
{
  QPainter painter;
  painter.begin(&buffer);

  int loop = currentLine->getSize() - data->count() + startIndex;
  int x = startX;
  double o = 0;
  double h = 0;
  double l = 0;
  double cl = 0;
  QColor c;
  bool ff = FALSE;

  while ((x < buffer.width()) && (loop < (int) currentLine->getSize()))
  {
    if (loop > -1)
    {
      currentLine->getData(loop, c, o, h, l, cl, ff);

      painter.setPen(c);

      int xh = scaler.convertToY(h);
      int xl = scaler.convertToY(l);
      int xc = scaler.convertToY(cl);
      int xo = scaler.convertToY(o);

      if (! ff)
      {
        painter.drawLine (x, xh, x, xc);
        painter.drawLine (x, xo, x, xl);

        if (xc == xo)
          painter.drawLine (x - 2, xo, x + 2, xo);
        else
          painter.drawRect(x - 2, xc, 5, xo - xc);
      }
      else
      {
        painter.drawLine (x, xh, x, xl);
      
        if (xc == xo)
          painter.drawLine (x - 2, xo, x + 2, xo);
        else
          painter.fillRect(x - 2, xo, 5, xc - xo, c);
      }
    }

    x = x + pixelspace;
    loop++;
  }

  painter.end();
}

//*************************************************************************
//******************** chart object functions *****************************
//*************************************************************************

void IndicatorPlot::slotNewBuyArrow ()
{
  slotNewChartObject((int) COBase::COBuyArrow);
}

void IndicatorPlot::slotNewCycle ()
{
  slotNewChartObject((int) COBase::COCycle);
}

void IndicatorPlot::slotNewFiboline ()
{
  slotNewChartObject((int) COBase::COFiboLine);
}

void IndicatorPlot::slotNewHorizontalLine ()
{
  slotNewChartObject((int) COBase::COHorizontalLine);
}

void IndicatorPlot::slotNewSellArrow ()
{
  slotNewChartObject((int) COBase::COSellArrow);
}

void IndicatorPlot::slotNewText ()
{
  slotNewChartObject((int) COBase::COText);
}

void IndicatorPlot::slotNewTrendLine ()
{
  slotNewChartObject((int) COBase::COTrendLine);
}
    
void IndicatorPlot::slotNewVerticalLine ()
{
  slotNewChartObject((int) COBase::COVerticalLine);
}

void IndicatorPlot::slotNewChartObject (int selection)
{
  if (! chartSymbol.length())
    return;
    
  COBase tco;
  coSelected = tco.getCO(selection);
  if (! coSelected)
    return;
  
  coSelected->setData(data);
  coSelected->setSymbol(chartSymbol);
  QObject::connect(coSelected, SIGNAL(signalDraw()), this, SLOT(draw()));
  QObject::connect(coSelected, SIGNAL(signalRefresh()), this, SLOT(drawRefresh()));
  QObject::connect(coSelected, SIGNAL(message(QString)), this, SLOT(slotMessage(QString)));
  QObject::connect(coSelected, SIGNAL(signalObjectDeleted(QString)), this, SLOT(slotChartObjectDeleted(QString)));

  Config config;
  QString s;
  config.getData(Config::LastChartObjectID, s);
  s = QString::number(s.toInt() + 1);
  coList.insert(s, coSelected);
  config.setData(Config::LastChartObjectID, s);

  setCursor(QCursor(Qt::PointingHandCursor));

  mouseFlag = ClickWait;

  coSelected->newObject(indicator, s);
}

void IndicatorPlot::addChartObject (COSettings &set)
{
  COBase tco;
  QString s;
  set.getType(s);
  COBase *co = tco.getCO(s.toInt());
  if (! co)
    return;

  co->setData(data);
  co->setSymbol(chartSymbol);
  QObject::connect(co, SIGNAL(signalDraw()), this, SLOT(draw()));
  QObject::connect(co, SIGNAL(signalRefresh()), this, SLOT(drawRefresh()));
  QObject::connect(co, SIGNAL(message(QString)), this, SLOT(slotMessage(QString)));
  QObject::connect(co, SIGNAL(signalObjectDeleted(QString)), this, SLOT(slotChartObjectDeleted(QString)));

  set.getID(s);
  coList.insert(s, co);
  co->loadSettings(set);
}

void IndicatorPlot::drawObjects ()
{
  QHashIterator<QString, COBase *> it(coList);
  while (it.hasNext())
  {
    it.next();
    COBase *co = it.value();
    co->draw(buffer, scaler, startIndex, pixelspace, startX);
  }
}

void IndicatorPlot::slotDeleteAllChartObjects ()
{
  if (! chartSymbol.length())
    return;

  int rc = QMessageBox::warning(this,
		            	tr("Qtstalker: Warning"),
			    	tr("Are you sure you want to delete all chart objects?"),
			    	QMessageBox::Yes,
			    	QMessageBox::No,
			    	QMessageBox::NoButton);

  if (rc == QMessageBox::No)
    return;

  qDeleteAll(coList);
  coList.clear();

  DataBase db;
  db.deleteChartObjects(chartSymbol);

  mouseFlag = None;
  draw();
}

void IndicatorPlot::slotChartObjectDeleted (QString d)
{
  if (! chartSymbol.length())
    return;
    
  if (! coList.count())
    return;

  COBase *co = coList[d];
  if (! co)
    return;
  delete co;
  coList.remove(d);
  DataBase db;
  db.deleteChartObject(d);

  mouseFlag = None;

  draw();
}

void IndicatorPlot::saveChartObjects ()
{
  if (! chartSymbol.length())
    return;

  QHashIterator<QString, COBase *> it(coList);
  while (it.hasNext())
  {
    it.next();
    COBase *co = it.value();
    if (co->getSaveFlag())
      co->saveSettings();
  }
}

void IndicatorPlot::loadChartObjects ()
{
  saveChartObjects();

  qDeleteAll(coList);
  coList.clear();

  QList<COSettings> l;
  DataBase db;
  db.getChartObjects(chartSymbol, indicator, l);

  int loop;
  for (loop = 0; loop < l.count(); loop++)
  {
    COSettings set = l.at(loop);
    addChartObject(set);
  }
}


