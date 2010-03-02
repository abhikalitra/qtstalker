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
#include "PluginFactory.h"

#include "../pics/loggrid.xpm"
#include "../pics/date.xpm"
#include "../pics/delete.xpm"
#include "../pics/edit.xpm"

#include <QPainter>
#include <QPen>
#include <QPolygon>
#include <QCursor>
#include <QImage>
#include <QMessageBox>
#include <QtDebug>
#include <math.h> // only for fabs()
#include <iostream>


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
  crosshairs = TRUE;
  infoFlag = TRUE;
  coSelected = 0;
  dateFlag = TRUE;
  menuFlag = TRUE;

  plotFont.setFamily("Helvetica");
  plotFont.setPointSize(12);
  plotFont.setWeight(50);

  data = 0;

  coMenu = new QMenu(this);
  coMenu->addAction(QPixmap(edit), tr("&Edit Chart Object"), this, SLOT(slotObjectDialog()), Qt::CTRL+Qt::Key_E);
  coMenu->addAction(QPixmap(deleteitem), tr("&Delete Chart Object"), this, SLOT(slotChartObjectDeleted()), Qt::CTRL+Qt::Key_D);
  
  chartMenu = new QMenu(this);
  chartMenu->addAction(QPixmap(deleteitem), tr("Delete &All Chart Objects"), this, SLOT(slotDeleteAllChartObjects()), Qt::CTRL+Qt::Key_A);
  chartMenu->addSeparator ();
  chartMenu->addAction(QPixmap(date), tr("Date"), this, SLOT(toggleDate()), Qt::CTRL+Qt::Key_D);
  chartMenu->addAction(QPixmap(loggridicon), tr("Log Scaling"), this, SLOT(toggleLog()), Qt::CTRL+Qt::Key_L);

  setMouseTracking(TRUE);

  setFocusPolicy(Qt::ClickFocus);

  externalChartObjectFlag = FALSE;
  
  Config config;
  config.getData(Config::PlotPluginPath, plotPluginPath);
  config.getData(Config::COPluginPath, coPluginPath);
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
  PluginFactory fac;
  int loop;
  for (loop = 0; loop < plotList.count(); loop++)
  {
    PlotLine *line = plotList.at(loop);

    if (! line->count())
      continue;

    QString s;
    line->getPlugin(s);
    PlotPlugin *plug = fac.getPlot(plotPluginPath, s);
    if (! plug)
    {
      qDebug() << "IndicatorPlot::drawLines: error loading plugin" << s;
      continue;
    }

    int startPos = line->count() - data->count() + startIndex;
    plug->draw(line, data, buffer, startX, pixelspace, startPos, scaler);
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

void IndicatorPlot::addLine (PlotLine *d)
{
  plotList.append(d);
}

//*********************************************************************
//*************** MOUSE EVENTS ***************************************
//********************************************************************

void IndicatorPlot::mousePressEvent (QMouseEvent *event)
{
  if (! data  || event->button() != Qt::LeftButton)
    return;

  getXY(event->x(), event->y());

  switch (mouseFlag)
  {
    case None:
    {
      if (crosshairs)
      {
        crossHair(event->x(), event->y(), TRUE);
        updateStatusBar(event->x(), event->y());
        emit leftMouseButton(event->x(), event->y(), 0);
      }
      else
        updateStatusBar(event->x(), event->y());

      QHashIterator<QString, ChartObject *> it(coList);
      while (it.hasNext())
      {
        it.next();
        coSelected = it.value();
        QPoint p(event->x(), event->y());
        if (coSelected->isSelected(p))
        {
          mouseFlag = COSelected;
          coSelected->setData(ChartObject::ParmSelected, TRUE);
          getCOInfo();
	  draw();
          return;
        }
      }
      break;
    }
    case COSelected:
    {
      QPoint p;
      p.setX(event->x());
      p.setY(event->y());
      moveFlag = coSelected->isGrabSelected(p);
      if (moveFlag)
        mouseFlag = Moving;
      else
      {
        if (! coSelected->isSelected(p))
        {
          mouseFlag = None;
	  coSelected->setData(ChartObject::ParmSelected, FALSE);
          draw();
        }
      }
      break;
    }
    case Moving:
      mouseFlag = COSelected;
      break;
    case ClickWait:
    {
      if (externalChartObjectFlag)
      {
	QString s;
	coSelected->getData(ChartObject::ParmID, s);
	emit signalNewExternalChartObjectDone(s.toInt());
      }
      
      PluginFactory fac;
      QString s;
      coSelected->getData(ChartObject::ParmPlugin, s);
      COPlugin *plug = fac.getCO(coPluginPath, s);
      int rc = plug->create2(coSelected, x1, y1);
      if (rc)
	mouseFlag = ClickWait2;
      else
      {
        mouseFlag = None;
        draw();
        emit statusMessage(QString());
        setCursor(QCursor(Qt::ArrowCursor));
      }
      break;
    }
    case ClickWait2:
    {
      PluginFactory fac;
      QString s;
      coSelected->getData(ChartObject::ParmPlugin, s);
      COPlugin *plug = fac.getCO(coPluginPath, s);
      int rc = plug->create3(coSelected, x1, y1);
      if (! rc)
      {
        mouseFlag = None;
        draw();
        emit statusMessage(QString());
        setCursor(QCursor(Qt::ArrowCursor));
      }
      break;
    }
    default:
      break;
  }
}

void IndicatorPlot::mouseMoveEvent (QMouseEvent *event)
{
  // ignore moves above the top of the chart - we get draw errors if we don't
  if (! data || event->y() <= 0)
    return;

  switch (mouseFlag)
  {
    case RubberBand:
      rubberBand->setGeometry(QRect(mouseOrigin, event->pos()).normalized());
      break;
    case Moving:
    {
      getXY(event->x(), event->y());
      PluginFactory fac;
      QString s;
      coSelected->getData(ChartObject::ParmPlugin, s);
      COPlugin *plug = fac.getCO(coPluginPath, s);
      plug->moving(coSelected, x1, y1, moveFlag);
      draw();
      break;
    }
    case ClickWait2:
    {
      getXY(event->x(), event->y());
      PluginFactory fac;
      QString s;
      coSelected->getData(ChartObject::ParmPlugin, s);
      COPlugin *plug = fac.getCO(coPluginPath, s);
      plug->moving(coSelected, x1, y1, 0);
      draw();
      break;
    }
    default:
      // update the data panel with new data
      if (infoFlag)
        getInfo(event->x(), event->y());
      break;
  }
}

void IndicatorPlot::mouseDoubleClickEvent (QMouseEvent *event)
{
  if (! data)
    return;

  switch (mouseFlag)
  {
    case COSelected:
      if (coSelected)
        slotObjectDialog();
      break;
    default:
    {
      rubberBand = new QRubberBand(QRubberBand::Rectangle, this);
      mouseOrigin = event->pos();
      rubberBand->setGeometry(QRect(mouseOrigin, QSize()));
      rubberBand->show();
      mouseFlag = RubberBand;
      setCursor(QCursor(Qt::SizeFDiagCursor));
      break;
    }
  }
}

void IndicatorPlot::mouseReleaseEvent(QMouseEvent *)
{
  switch (mouseFlag)
  {
    case RubberBand:
    {
      if (rubberBand->width() < pixelspace)
        return;
      // calculate new pixel spacing and position here
      int x = convertXToDataIndex(mouseOrigin.x());
      int ti = rubberBand->width() / pixelspace;
      ti = this->width() / ti;
      delete rubberBand;
      mouseFlag = None;
      unsetCursor();
      if (ti < pixelspace)
        return;
      emit signalPixelspaceChanged(x, ti);
      break;
    }
    default:
      break;
  }
}

void IndicatorPlot::contextMenuEvent (QContextMenuEvent *)
{
  switch (mouseFlag)
  {
    case COSelected:
      if (coSelected)
        coMenu->exec(QCursor::pos());
      break;
    default:
      if (menuFlag)
        showPopupMenu();
      break;
  }
}

void IndicatorPlot::getInfo (int x, int y)
{
  Setting tr;
  PluginFactory fac;

  // determine if we are over a chart object, if so we display parms in the data panel
  QHashIterator<QString, ChartObject *> it(coList);
  while (it.hasNext())
  {
    it.next();
    ChartObject *co = it.value();
    QPoint p(x, y);
    if (co->isSelected(p))
    {
      QString s;
      co->getData(ChartObject::ParmPlugin, s);
      COPlugin *plug = fac.getCO(coPluginPath, s);
      plug->getInfo(co, tr);
      if (tr.count())
      {
        Setting *r = new Setting;
        tr.copy(r);
        emit infoMessage(r);
        return;
      }
    }
  }

  int i = convertXToDataIndex(x);

  QString s, k;
  data->getDateString(i, s);
  k = "D";
  tr.setData(k, s);
  data->getTimeString(i, s);
  k = "T";
  tr.setData(k, s);

  if (plotList.count())
  {
    int loop;
    for (loop = 0; loop < plotList.count(); loop++)
    {
      PlotLine *line = plotList.at(loop);
      line->getPlugin(s);

      if (s == "Horizontal")
	continue;
      
      int li = line->count() - data->count() + i;

      if (s == "OHLC" || s == "Candle")
      {
	s = "O";
	QString d = QString::number(data->getOpen(li));
	tr.setData(s, d);
	
	s = "H";
	d = QString::number(data->getHigh(li));
	tr.setData(s, d);
	
	s = "L";
	d = QString::number(data->getLow(li));
	tr.setData(s, d);
	
	s = "C";
	d = QString::number(data->getClose(li));
	tr.setData(s, d);
      }
      else
      {
        if (li > -1 && li < line->count())
          line->getInfo(li, tr);
      }
    }
  }

  if (tr.count())
  {
    Setting *r = new Setting;
    tr.copy(r);
    emit infoMessage(r);
  }
}

void IndicatorPlot::getCOInfo ()
{
  Setting tr;
  PluginFactory fac;
  QString s;
  coSelected->getData(ChartObject::ParmPlugin, s);
  COPlugin *plug = fac.getCO(coPluginPath, s);
  plug->getInfo(coSelected, tr);

  if (tr.count())
  {
    Setting *r = new Setting;
    tr.copy(r);
    emit infoMessage(r);
  }
}

void IndicatorPlot::keyPressEvent (QKeyEvent *key)
{
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

// plot the indicator values at the top of the screen
void IndicatorPlot::drawInfo ()
{
  QPainter painter;
  painter.begin(&buffer);
  painter.setPen(borderColor);
  painter.setFont(plotFont);
  painter.setBackgroundMode(Qt::OpaqueMode);
  painter.setBackground(QBrush(backgroundColor));

  QFontMetrics fm(plotFont);
  int pos = startX;

  QString s;
  data->getDateString(data->count() - 1, s);
  s.append(" ");
  painter.drawText(pos, 10, s);
  pos = pos + fm.width(s);

  if (plotList.count())
  {
    int loop;
    for (loop = 0; loop < plotList.count(); loop++)
    {
      PlotLine *line = plotList.at(loop);
      
      if (! line->count())
	continue;
      
      QString ts;
      line->getPlugin(ts);
      if (ts == "Horizontal")
	continue;
      
      QColor c;
      double d = line->getData(line->count() - 1, c);
	
      line->getLabel(s);
      s.append("=");
      QString str;
      strip(d, 4, str);
      s.append(str);
      s.append(" ");

      painter.setPen(c);
      painter.drawText(pos, 10, s);
      pos = pos + fm.width(s);
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
        int loop2 = line->count() - data->count() + startIndex;
        int end = (buffer.width() / pixelspace) + loop2;
        if (loop2 < 0)
          loop2 = 0;
        if (end > line->count())
          end = line->count();

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
    PluginFactory fac;
    QHashIterator<QString, ChartObject *> it(coList);
    while (it.hasNext())
    {
      it.next();
      ChartObject *co = it.value();
      
      QString s;
      co->getData(ChartObject::ParmPlugin, s);
      COPlugin *plug = fac.getCO(coPluginPath, s);
      plug->getHighLow(co);
      
      double h = co->getDouble(ChartObject::ParmHigh);
      double l = co->getDouble(ChartObject::ParmLow);
      
      if (h > scaleHigh)
        scaleHigh = h;

      if (l < scaleLow)
        scaleLow = l;
    }
  }

  // create a little more room between chart edges and plots
  double t = (scaleHigh - scaleLow) * 0.02; // get 2% of the range
  scaleHigh = scaleHigh + t;
  scaleLow = scaleLow - t;

  // handle log scaling if toggled
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

void IndicatorPlot::drawRubberBand (QRect &r)
{
  // remove the old band by refreshing with pixmap
//  QPainter painter(this);
//  painter.drawPixmap(0, 0, buffer);

  // draw rubber band on widget
  QPainter painter;
  painter.begin(&buffer);

  painter.setPen(QColor("red"));
  painter.drawRect(r);

  painter.end();

  update();
}

//*************************************************************************
//******************** chart object functions *****************************
//*************************************************************************

void IndicatorPlot::setExternalChartObjectFlag (int id)
{
  externalChartObjectFlag = FALSE;
  QString s;  
  coSelected->getData(ChartObject::ParmID, s);
  if (s.toInt() != id)
  {
    mouseFlag = None;
    delete coSelected;
    coList.remove(s);
    setCursor(QCursor(Qt::ArrowCursor));
  }
}

void IndicatorPlot::newExternalChartObject (QString d)
{
  externalChartObjectFlag = TRUE;
  slotNewChartObject(d);
}

void IndicatorPlot::slotNewChartObject (QString selection)
{
  if (! chartSymbol.length())
    return;

  coSelected = new ChartObject;
  connect(coSelected, SIGNAL(signalMessage(QString)), this, SIGNAL(statusMessage(QString)));

  Config config;
  QString id = QString::number(config.getInt(Config::LastChartObjectID) + 1);
  coList.insert(id, coSelected);
  config.setData(Config::LastChartObjectID, id);

  coSelected->setData(ChartObject::ParmID, id);
  coSelected->setData(ChartObject::ParmSymbol, chartSymbol);
  coSelected->setData(ChartObject::ParmIndicator, indicator);
  coSelected->setData(ChartObject::ParmPlugin, selection);
  
  PluginFactory fac;
  COPlugin *plug = fac.getCO(coPluginPath, selection);
  
  plug->create(coSelected);

  mouseFlag = ClickWait;

  setCursor(QCursor(Qt::PointingHandCursor));
}

void IndicatorPlot::drawObjects ()
{
  PluginFactory fac;
  QHashIterator<QString, ChartObject *> it(coList);
  while (it.hasNext())
  {
    it.next();
    coDraw = it.value();
    
    QString s;
    coDraw->getData(ChartObject::ParmPlugin, s);
    COPlugin *plug = fac.getCO(coPluginPath, s);
    if (! plug)
    {
      qDebug() << "IndicatorPlot::drawObjects: plugin load error" << s;
      continue;
    }
    
    plug->draw(coDraw, buffer, data, startX, pixelspace, startIndex, scaler);
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

void IndicatorPlot::slotChartObjectDeleted ()
{
  if (! chartSymbol.length() || ! coList.count() || ! coSelected)
    return;

  QString s;
  coSelected->getData(ChartObject::ParmID, s);
  delete coSelected;
  coSelected = 0;
  coList.remove(s);
  DataBase db;
  db.deleteChartObject(s);

  mouseFlag = None;

  draw();
}

void IndicatorPlot::saveChartObjects ()
{
  if (! chartSymbol.length())
    return;

  DataBase db;
  QHashIterator<QString, ChartObject *> it(coList);
  while (it.hasNext())
  {
    it.next();
    ChartObject *co = it.value();
    if (co->getInt(ChartObject::ParmSave))
      db.setChartObject(co);
  }
}

void IndicatorPlot::loadChartObjects ()
{
  saveChartObjects();

  qDeleteAll(coList);
  coList.clear();

  DataBase db;
  db.getChartObjects(chartSymbol, indicator, coList);
}

void IndicatorPlot::slotObjectDialog ()
{
  PluginFactory fac;
  QString s;
  coSelected->getData(ChartObject::ParmPlugin, s);
  COPlugin *plug = fac.getCO(coPluginPath, s);
  plug->dialog(coSelected);
  draw();
}

void IndicatorPlot::addChartObject (ChartObject *co)
{
  QString s;
  co->getData(ChartObject::ParmID, s);
  coList.insert(s, co);
}

