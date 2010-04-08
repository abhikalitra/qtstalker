/*
 *  Qtstalker stock charter
 *
 *  Copyright (C) 2001-2010 Stefan S. Stratigakos
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
#include "CODataBase.h"
#include "COFactory.h"
#include "PlotFactory.h"
#include "PlotInfo.h"
#include "Utils.h"

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
#include <iostream>
#include <cmath>

IndicatorPlot::IndicatorPlot (QWidget *w) : QWidget(w)
{
  plotData.startX = 2;
  plotData.pixelspace = 0;
  plotData.startIndex = 0;
  plotData.backgroundColor.setNamedColor("black");
  plotData.borderColor.setNamedColor("white");
  plotData.scaleToScreen = FALSE;
  plotData.infoIndex = 0;
  plotData.x = 0;
  plotData.y = 0;
  plotData.y1 = 0;
  plotData.infoFlag = 1;
  plotData.pos = 0;
  
  interval = Bar::DailyBar;
  mouseFlag = None;
  saveMouseFlag = None;
  chartMenu = 0;
  coSelected = 0;
  menuFlag = TRUE;
  rubberBand = 0;
  newObjectFlag = 0;

  plotData.plotFont.setFamily("Helvetica");
  plotData.plotFont.setPointSize(12);
  plotData.plotFont.setWeight(50);

  coMenu = new QMenu(this);
  coMenu->addAction(QPixmap(edit), tr("&Edit Chart Object"), this, SLOT(slotObjectDialog()), Qt::CTRL+Qt::Key_E);
  coMenu->addAction(QPixmap(delete_xpm), tr("&Delete Chart Object"), this, SLOT(slotChartObjectDeleted()), Qt::CTRL+Qt::Key_D);
  
  chartMenu = new QMenu(this);
  chartMenu->addAction(QPixmap(delete_xpm), tr("Delete &All Chart Objects"), this, SLOT(slotDeleteAllChartObjects()), Qt::CTRL+Qt::Key_A);
  chartMenu->addSeparator ();
  chartMenu->addAction(QPixmap(date), tr("Date"), this, SLOT(toggleDate()), Qt::CTRL+Qt::Key_D);
  chartMenu->addAction(QPixmap(loggridicon), tr("Log Scaling"), this, SLOT(toggleLog()), Qt::CTRL+Qt::Key_L);

  setMouseTracking(TRUE);

  setFocusPolicy(Qt::ClickFocus);
}

IndicatorPlot::~IndicatorPlot ()
{
  indicator.clear();
}

//*********************************************************************
//*************** DRAW FUNCTIONS **************************************
//********************************************************************

void IndicatorPlot::draw ()
{
  if (plotData.buffer.isNull())
    return;

  plotData.buffer.fill(plotData.backgroundColor);

  if (plotData.dateBars.count())
  {
    setScale();
    grid.draw(plotData);
    drawLines();
    drawObjects();

    plotData.infoIndex = convertXToDataIndex(plotData.buffer.width() - plotData.pixelspace);
    PlotInfo info;
    info.drawInfo(plotData, indicator);
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
  PlotFactory fac;
  int loop;
  QList<PlotLine *> plotList;
  indicator.getLines(plotList);
  
  for (loop = 0; loop < plotList.count(); loop++)
  {
    PlotLine *line = plotList.at(loop);

    if (! line->count())
      continue;

    QString s;
    line->getPlugin(s);
    PlotPlugin *plug = fac.getPlot(s);
    if (! plug)
    {
      qDebug() << "IndicatorPlot::drawLines: error loading plugin" << s;
      continue;
    }

    plotData.pos = line->count() - plotData.dateBars.count() + plotData.startIndex;
    plug->draw(line, plotData);
    delete plug;
  }
}

void IndicatorPlot::paintEvent (QPaintEvent *event)
{
  QPainter painter(this);
  
  if (mouseFlag == CursorZoom)
    painter.drawPixmap(0, 0, plotData.buffer);
  else
    painter.drawPixmap(event->rect(), plotData.buffer);
  
  if (mouseFlag == CursorCrosshair)
  {
    int y = plotData.scaler.convertToY(plotData.y1);
    painter.setPen(QPen(plotData.borderColor, 1, Qt::DotLine));
    painter.drawLine (0, y, plotData.buffer.width(), y);
    painter.drawLine (plotData.x, 0, plotData.x, plotData.buffer.height());
    painter.end();
  }
}

void IndicatorPlot::resizeEvent (QResizeEvent *event)
{
  plotData.buffer = QPixmap(event->size());
  draw();
}

void IndicatorPlot::cursorChanged (int d)
{
  switch (d)
  {
    case 0: // normal cursor
      mouseFlag = None;
      break;
    case 1:
      mouseFlag = CursorZoom;
      break;
    case 2:
      mouseFlag = CursorCrosshair;
      break;
    default:
      break;
  }
  
  updateCursor();
  draw();
}

void IndicatorPlot::updateCursor ()
{
  switch (mouseFlag)
  {
    case CursorCrosshair:
      setCursor(QCursor(Qt::CrossCursor));
      break;
    default:
      setCursor(QCursor(Qt::ArrowCursor));
      break;
  }
}

//*********************************************************************
//*************** SCALE FUNCTIONS *************************************
//********************************************************************

void IndicatorPlot::setScaler (Scaler &d)
{
  plotData.scaler = d;
}

Scaler & IndicatorPlot::getScaler ()
{
  return plotData.scaler;
}

void IndicatorPlot::getScalePoints (QList<Setting> &l)
{
  if (! plotData.dateBars.count())
    return;
  
  plotData.infoIndex = convertXToDataIndex(plotData.buffer.width() - plotData.pixelspace);
  PlotInfo info;
  info.getPointInfo(plotData, l, indicator);
}

void IndicatorPlot::setScale ()
{
  double tscaleHigh = -99999999;
  double tscaleLow = 99999999;
  int width = plotData.buffer.width() / plotData.pixelspace;

  QList<PlotLine *> plotList;
  indicator.getLines(plotList);
  
  int loop;
  for (loop = 0; loop < plotList.count(); loop++)
  {
    PlotLine *line = plotList.at(loop);

    if (line->getScaleFlag())
      continue;

    if (! plotData.scaleToScreen)
    {
      if (line->getHigh() > tscaleHigh)
        tscaleHigh = line->getHigh();

      if (line->getLow() < tscaleLow)
        tscaleLow = line->getLow();
    }
    else
    {
      int start = line->count() - plotData.dateBars.count() + plotData.startIndex;
      int end = width + start;
      if (start < 0)
        start = 0;
      if (end > line->count())
        end = line->count();

      double h, l;
      line->getHighLowRange(start, end - 1, h, l);
      if (h > tscaleHigh)
        tscaleHigh = h;
      if (l < tscaleLow)
        tscaleLow = l;
    }
  }

  // do this anyway for scaleToScreen even if we dont use it
  int start = plotData.startIndex;
  int end = width + plotData.startIndex;
  if (start < 0)
    start = 0;
  if (end > plotData.dateBars.count())
    end = plotData.dateBars.count();
  QDateTime sd;
  plotData.dateBars.getDate(start, sd);
  QDateTime ed;
  plotData.dateBars.getDate(end - 1, ed);

  QHash<QString, COPlugin *> coList;
  indicator.getChartObjects(coList);
  QHashIterator<QString, COPlugin *> it(coList);
  while (it.hasNext())
  {
    it.next();
    COPlugin *co = it.value();
    if (plotData.scaleToScreen)
    {
      if (! co->inDateRange(plotData, sd, ed))
        continue;
    }
      
    double h, l;
    if (co->getHighLow(h, l))
      continue;
    
    if (h > tscaleHigh)
      tscaleHigh = h;

    if (l < tscaleLow)
      tscaleLow = l;
  }

  // create a little more room between chart edges and plots
  double t = (tscaleHigh - tscaleLow) * 0.02; // get 2% of the range
  tscaleHigh = tscaleHigh + t;
  tscaleLow = tscaleLow - t;

  // handle log scaling if toggled
  double tlogScaleHigh = 1;
  double tlogRange = 0;
  if (indicator.getLog())
  {
    tlogScaleHigh = tscaleHigh > 0.0 ? log(tscaleHigh) : 1;
    double tlogScaleLow = tscaleLow > 0.0 ? log(tscaleLow) : 0;
    tlogRange = tlogScaleHigh - tlogScaleLow;
  }

  plotData.scaler.set(plotData.buffer.height(), tscaleHigh, tscaleLow, tlogScaleHigh, tlogRange, indicator.getLog());
}

//*********************************************************************
//*************** MOUSE EVENTS ***************************************
//********************************************************************

void IndicatorPlot::mousePressEvent (QMouseEvent *event)
{
  if (! plotData.dateBars.count()  || event->button() != Qt::LeftButton)
  {
    QWidget::mousePressEvent(event);
    return;
  }

  switch (mouseFlag)
  {
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
	  coSelected->setSelected(FALSE);
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
      getXY(event->x(), event->y());
      int rc = coSelected->create2(plotData.x1, plotData.y1);
      if (rc)
	mouseFlag = ClickWait2;
      else
      {
	mouseFlag = saveMouseFlag;
	updateCursor();
        draw();
	
        int i = convertXToDataIndex(event->x());
        PlotInfo info;
        Setting *mess = info.getCursorInfo(i, event->y(), plotData);
        if (mess)
          emit infoMessage(mess);
      }
      break;
    }
    case ClickWait2:
    {
      getXY(event->x(), event->y());
      int rc = coSelected->create3(plotData.x1, plotData.y1);
      if (! rc)
      {
	mouseFlag = saveMouseFlag;
	updateCursor();
        draw();
	
        int i = convertXToDataIndex(event->x());
        PlotInfo info;
        Setting *mess = info.getCursorInfo(i, event->y(), plotData);
        if (mess)
          emit infoMessage(mess);
      }
      break;
    }
    case NewObjectWait:
    {
      mouseFlag = ClickWait;
      emit signalNewExternalChartObjectDone();
      mousePressEvent(event); // recursive call to capture first mouse click again
      break;
    }
    case CursorZoom:
    {
      if (! rubberBand)
	return;
      
      if (rubberBand->width() < plotData.pixelspace)
        return;
      
      // calculate new pixel spacing and position here
      int x = convertXToDataIndex(mouseOrigin.x());
      int ti = rubberBand->width() / plotData.pixelspace;
      ti = this->width() / ti;
      
      delete rubberBand;
      rubberBand = 0;
      
      unsetCursor();
      
      if (ti < plotData.pixelspace)
        return;
      
      emit signalPixelspaceChanged(x, ti);
      break;
    }
    case None:
    {
      QHash<QString, COPlugin *> coList;
      indicator.getChartObjects(coList);
      QHashIterator<QString, COPlugin *> it(coList);
      while (it.hasNext())
      {
        it.next();
        coSelected = it.value();
        QPoint p(event->x(), event->y());
        if (coSelected->isSelected(p))
        {
          mouseFlag = COSelected;
          coSelected->setSelected(TRUE);
          PlotInfo info;
          Setting *mess = info.getCOInfo(coSelected);
          if (mess)
            emit infoMessage(mess);
	  draw();
          return;
        }
      }
      break;
    }
    default:
      QWidget::mousePressEvent(event);
      break;
  }
}

void IndicatorPlot::mouseMoveEvent (QMouseEvent *event)
{
  // ignore moves above the top of the chart - we get draw errors if we don't
  if (! plotData.dateBars.count() || event->y() <= 0)
  {
    QWidget::mouseMoveEvent(event);
    return;
  }

  switch (mouseFlag)
  {
    case CursorCrosshair:
    {
      getXY(event->x(), event->y());

      update();
      
      int i = convertXToDataIndex(event->x());
      PlotInfo info;
      Setting *mess = info.getCursorInfo(i, event->y(), plotData);
      if (mess)
        emit infoMessage(mess);
/*      
      draw();

      getXY(event->x(), event->y());

      int y = plotData.scaler.convertToY(plotData.y1);
      int x = plotData.startX + (plotData.dateBars.getX(plotData.x1) * plotData.pixelspace) - (plotData.startIndex * plotData.pixelspace);

      QPainter painter;
      painter.begin(&plotData.buffer);
      painter.setPen(QPen(plotData.borderColor, 1, Qt::DotLine));
      painter.drawLine (0, y, plotData.buffer.width(), y);
      painter.drawLine (x, 0, x, plotData.buffer.height());
      painter.end();
      
      int i = convertXToDataIndex(event->x());
      PlotInfo info;
      Setting *mess = info.getCursorInfo(i, event->y(), plotData);
      if (mess)
        emit infoMessage(mess);
*/
      
      break;
    }
    case CursorZoom:
    {
      int i = convertXToDataIndex(event->x());
      PlotInfo info;
      Setting *mess = info.getCursorInfo(i, event->y(), plotData);
      if (mess)
        emit infoMessage(mess);
      
      if (rubberBand)
        rubberBand->setGeometry(QRect(mouseOrigin, event->pos()).normalized());
      break;
    }
    case Moving:
    {
      getXY(event->x(), event->y());
      coSelected->moving(plotData.x1, plotData.y1, moveFlag);
      draw();
      break;
    }
    case ClickWait2:
    {
      getXY(event->x(), event->y());
      coSelected->moving(plotData.x1, plotData.y1, 0);
      draw();
      break;
    }
    case None:
    {
      // update the data panel with new data
      if (! plotData.infoFlag)
	return;
      
      // determine if we are over a chart object, if so update cursor
      QPoint p(event->x(), event->y());
      QHash<QString, COPlugin *> coList;
      indicator.getChartObjects(coList);
      QHashIterator<QString, COPlugin *> it(coList);
      int flag = 0;
      while (it.hasNext())
      {
        it.next();
        COPlugin *co = it.value();
        if (co->isSelected(p))
        {
          setCursor(QCursor(Qt::PointingHandCursor));
	  flag = 1;
	  break;
        }
      }
      if (! flag)
        setCursor(QCursor(Qt::ArrowCursor));

      PlotInfo info;
      plotData.infoIndex = convertXToDataIndex(event->x());
      Setting *mess = info.getInfo(p, plotData, indicator);
      if (mess)
        emit infoMessage(mess);
      break;
    }
    default:
      QWidget::mouseMoveEvent(event);
      break;
  }
}

void IndicatorPlot::mouseDoubleClickEvent (QMouseEvent *event)
{
  if (! plotData.dateBars.count())
  {
    QWidget::mouseDoubleClickEvent(event);
    return;
  }

  switch (mouseFlag)
  {
    case None: // center chart on double click mouse position
    {
      int center = (plotData.buffer.width() / plotData.pixelspace) / 2;
      int i = event->x() / plotData.pixelspace;
      if (i < center)
        emit signalIndexChanged(plotData.startIndex - (center - i));
      else
      {
	if (i > center)
          emit signalIndexChanged(plotData.startIndex + (i - center));
      }
      break;
    }
    case COSelected:
      if (coSelected)
        slotObjectDialog();
      break;
    case CursorZoom:
    {
      if (rubberBand)
	return;
      
      rubberBand = new QRubberBand(QRubberBand::Rectangle, this);
      mouseOrigin = event->pos();
      rubberBand->setGeometry(QRect(mouseOrigin, QSize()));
      rubberBand->show();
      setCursor(QCursor(Qt::SizeFDiagCursor));
      break;
    }
    default:
      QWidget::mouseDoubleClickEvent(event);
      break;
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

//*********************************************************************
//*************** MENU FUNCTIONS ***************************************
//********************************************************************

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

void IndicatorPlot::showPopupMenu ()
{
  chartMenu->exec(QCursor::pos());
}

//*********************************************************************
//*************** SET / GET VARIABLES *********************************
//********************************************************************

void IndicatorPlot::clear ()
{
  saveChartObjects();
  indicator.clear();
  plotData.dateBars.clear();
//  mouseFlag = None;
}

void IndicatorPlot::toggleDate ()
{
  int flag = indicator.getDate();
  if (flag == FALSE)
    flag = TRUE;
  else
    flag = FALSE;
  indicator.setDate(flag);

  emit signalDateFlag(flag);
}

void IndicatorPlot::toggleLog ()
{
  int flag = indicator.getLog();
  if (flag == FALSE)
    flag = TRUE;
  else
    flag = FALSE;
  indicator.setLog(flag);

  emit signalLogFlag(flag);
}

void IndicatorPlot::slotSliderChanged (int v)
{
  setIndex(v);
  draw();
}

void IndicatorPlot::slotGridChanged (bool d)
{
  grid.setGridFlag(d);
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

void IndicatorPlot::setData (BarData *l)
{
  if (! l->count())
    return;

  plotData.dateBars.createDateList(l);
  
  exchange = l->getExchange();
  symbol = l->getSymbol();
}

void IndicatorPlot::setScaleToScreen (bool d)
{
  plotData.scaleToScreen = d;
}

void IndicatorPlot::setInfoFlag (bool d)
{
  plotData.infoFlag = d;
}

void IndicatorPlot::setDateFlag (bool d)
{
  indicator.setDate(d);
}

void IndicatorPlot::setLogScale (bool d)
{
  indicator.setLog(d);
}

bool IndicatorPlot::getLogScale ()
{
  return indicator.getLog();
}

void IndicatorPlot::setInterval (Bar::BarLength d)
{
  interval = d;
}

void IndicatorPlot::setBackgroundColor (QColor &d)
{
  plotData.backgroundColor = d;
}

void IndicatorPlot::setBorderColor (QColor &d)
{
  plotData.borderColor = d;
}

void IndicatorPlot::setGridColor (QColor &d)
{
  grid.setGridColor(d);
}

void IndicatorPlot::setPlotFont (QFont &d)
{
  plotData.plotFont = d;
}

void IndicatorPlot::setGridFlag (bool d)
{
  grid.setGridFlag(d);
}

void IndicatorPlot::setMenuFlag (bool d)
{
  menuFlag = d;
}

void IndicatorPlot::setPixelspace (int d)
{
  plotData.pixelspace = d;
}

void IndicatorPlot::setIndex (int d)
{
  plotData.startIndex = d;
}

void IndicatorPlot::setXGrid (QVector<int> &d)
{
  grid.setXGrid(d);
}

int IndicatorPlot::getWidth ()
{
  return plotData.buffer.width();
}

void IndicatorPlot::setIndicator (Indicator &d)
{
  indicator = d;
}

void IndicatorPlot::getIndicator (Indicator &d)
{
  d = indicator;
}

void IndicatorPlot::getDateBar (DateBar &d)
{
  d = plotData.dateBars;
}

//*********************************************************************
//*************** INTERNAL FUNCTIONS **********************************
//********************************************************************

void IndicatorPlot::getXY (int x, int y)
{
  plotData.x = x;
  plotData.y = y;
  int i = convertXToDataIndex(x);
  plotData.dateBars.getDate(i, plotData.x1);
  plotData.y1 = plotData.scaler.convertToVal(y);
}

int IndicatorPlot::convertXToDataIndex (int x)
{
  int i = (x / plotData.pixelspace) + plotData.startIndex;
  if (i >= (int) plotData.dateBars.count())
    i = plotData.dateBars.count() - 1;
  if (i < plotData.startIndex)
    i = plotData.startIndex;

  return i;
}

//*************************************************************************
//******************** chart object functions *****************************
//*************************************************************************

void IndicatorPlot::setExternalChartObjectFlag ()
{
  if (mouseFlag != NewObjectWait)
    return;
  
  mouseFlag = saveMouseFlag;
  newObjectFlag = FALSE;

  QString s = QString::number(coSelected->getID());
  indicator.deleteChartObject(s);
  coSelected = 0;

  updateCursor();
  draw();
}

void IndicatorPlot::newExternalChartObject (QString d)
{
  newObjectFlag = TRUE;
  saveMouseFlag = mouseFlag;
  mouseFlag = NewObjectWait;
  slotNewChartObject(d);
}

void IndicatorPlot::slotNewChartObject (QString selection)
{
  if (! symbol.length())
    return;

  COFactory fac;
  coSelected = fac.getCO(selection);
  if (! coSelected)
  {
    qDebug() << "IndicatorPlot::slotNewChartObject: no co" << selection;
    return;
  }
  
  connect(coSelected, SIGNAL(signalMessage(QString)), this, SIGNAL(signalStatusMessage(QString)));

  Config config;
  QString id = QString::number(config.getInt(Config::LastChartObjectID) + 1);
  config.setData(Config::LastChartObjectID, id);

  coSelected->setID(id.toInt());
  coSelected->setSymbol(symbol);
  coSelected->setExchange(exchange);
  
  QString s;
  indicator.getName(s);
  coSelected->setIndicator(s);
  
  coSelected->create();
  
  indicator.addChartObject(coSelected);

  if (! newObjectFlag)
    mouseFlag = ClickWait;

  setCursor(QCursor(Qt::PointingHandCursor));
}

void IndicatorPlot::drawObjects ()
{
  QHash<QString, COPlugin *> coList;
  indicator.getChartObjects(coList);
  QHashIterator<QString, COPlugin *> it(coList);
  while (it.hasNext())
  {
    it.next();
    COPlugin *co = it.value();
    co->draw(plotData);
  }
}

void IndicatorPlot::slotDeleteAllChartObjects ()
{
  if (! symbol.length())
    return;

  int rc = QMessageBox::warning(this,
		            	tr("Qtstalker: Warning"),
			    	tr("Are you sure you want to delete all chart objects?"),
			    	QMessageBox::Yes,
			    	QMessageBox::No,
			    	QMessageBox::NoButton);

  if (rc == QMessageBox::No)
    return;

  indicator.clearChartObjects();

  CODataBase db;
  BarData bd;
  bd.setExchange(exchange);
  bd.setSymbol(symbol);
  db.deleteChartObjects(&bd);

  mouseFlag = None;

  draw();
}

void IndicatorPlot::slotChartObjectDeleted ()
{
  if (! symbol.length() || ! coSelected)
    return;

  QString s = QString::number(coSelected->getID());
  indicator.deleteChartObject(s);
  coSelected = 0;

  CODataBase db;
  db.deleteChartObject(s);

  mouseFlag = None;

  draw();
}

void IndicatorPlot::saveChartObjects ()
{
  if (! symbol.length())
    return;

  QHash<QString, COPlugin *> coList;
  indicator.getChartObjects(coList);
  QHashIterator<QString, COPlugin *> it(coList);
  while (it.hasNext())
  {
    it.next();
    COPlugin *co = it.value();
    co->save();
  }
}

void IndicatorPlot::loadChartObjects ()
{
  saveChartObjects();

  indicator.clearChartObjects();

  QString s;
  indicator.getName(s);
  CODataBase db;
  BarData bd;
  bd.setExchange(exchange);
  bd.setSymbol(symbol);
  db.getChartObjects(&bd, s, indicator);
}

void IndicatorPlot::slotObjectDialog ()
{
  coSelected->dialog();
  draw();
}

