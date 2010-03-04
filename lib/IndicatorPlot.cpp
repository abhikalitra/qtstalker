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
#include "PluginFactory.h"
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
  startX = 2;
  backgroundColor.setNamedColor("black");
  borderColor.setNamedColor("white");
  pixelspace = 0;
  interval = BarData::DailyBar;
  scaleToScreen = FALSE;
  startIndex = 0;
  mouseFlag = None;
  crossHairFlag = FALSE;
  chartMenu = 0;
  crosshairs = TRUE;
  infoFlag = TRUE;
  coSelected = 0;
  menuFlag = TRUE;

  plotFont.setFamily("Helvetica");
  plotFont.setPointSize(12);
  plotFont.setWeight(50);

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
}

IndicatorPlot::~IndicatorPlot ()
{
  indicator.clear();
}

void IndicatorPlot::clear ()
{
  saveChartObjects();
  indicator.clear();
  dateBars.clear();
  mouseFlag = None;
  crossHairFlag = FALSE;
}

void IndicatorPlot::setData (BarData *l)
{
  if (! l->count())
    return;

  dateBars.createDateList(l);
}

void IndicatorPlot::setChartPath (QString &d)
{
  chartSymbol = d;
}

void IndicatorPlot::setScaleToScreen (bool d)
{
  scaleToScreen = d;
}

void IndicatorPlot::setInfoFlag (bool d)
{
  infoFlag = d;
}

void IndicatorPlot::setDateFlag (bool d)
{
  indicator.setDate(d);
}

void IndicatorPlot::setLogScale (bool d)
{
  indicator.setLog(d);
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

  if (dateBars.count())
  {
    scaler.setScale(indicator, dateBars, scaleToScreen, startIndex, (buffer.width() / pixelspace),
		    buffer.height(), coPluginPath);
    grid.draw(buffer, startX, startIndex, pixelspace, scaler);
    drawLines();
    drawObjects();
    drawCrossHair();

    PlotInfo info;
    info.drawInfo(buffer, borderColor, backgroundColor, plotFont, startX, dateBars, indicator);
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
  QList<PlotLine *> plotList;
  indicator.getLines(plotList);
  
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

    int startPos = line->count() - dateBars.count() + startIndex;
    plug->draw(line, buffer, startX, pixelspace, startPos, scaler);
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

//*********************************************************************
//*************** MOUSE EVENTS ***************************************
//********************************************************************

void IndicatorPlot::mousePressEvent (QMouseEvent *event)
{
  if (! dateBars.count()  || event->button() != Qt::LeftButton)
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

      QHash<QString, ChartObject *> coList;
      indicator.getChartObjects(coList);
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
          PlotInfo info;
          Setting *mess = info.getCOInfo(coSelected, coPluginPath);
          if (mess)
            emit infoMessage(mess);
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
    case NewObjectWait:
    {
      mouseFlag = ClickWait;
      emit signalNewExternalChartObjectDone();
      slotNewChartObject(newChartObject);
      mousePressEvent(event); // recursive call to simulate 1 click instead of 2
    }
    default:
      break;
  }
}

void IndicatorPlot::mouseMoveEvent (QMouseEvent *event)
{
  // ignore moves above the top of the chart - we get draw errors if we don't
  if (! dateBars.count() || event->y() <= 0)
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
    {
      // update the data panel with new data
      if (infoFlag)
      {
        PlotInfo info;
        QPoint p(event->x(), event->y());
        int index = convertXToDataIndex(event->x());
        Setting *mess = info.getInfo(p, indicator, index, dateBars, coPluginPath);
        if (mess)
          emit infoMessage(mess);
      }
      break;
    }
  }
}

void IndicatorPlot::mouseDoubleClickEvent (QMouseEvent *event)
{
  if (! dateBars.count())
    return;

  switch (mouseFlag)
  {
    case ClickWait:
    case Moving:
    case ClickWait2:
    case NewObjectWait:
      break;
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
  grid.setGridColor(d);
}

void IndicatorPlot::setPlotFont (QFont &d)
{
  plotFont = d;
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
  pixelspace = d;
}

void IndicatorPlot::setIndex (int d)
{
  startIndex = d;
}

void IndicatorPlot::setXGrid (QVector<int> &d)
{
  grid.setXGrid(d);
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
  int x = startX + (dateBars.getX(crossHairX) * pixelspace) - (startIndex * pixelspace);

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
  dateBars.getDate(i, x1);
  y1 = scaler.convertToVal(y);
}

int IndicatorPlot::convertXToDataIndex (int x)
{
  int i = (x / pixelspace) + startIndex;
  if (i >= (int) dateBars.count())
    i = dateBars.count() - 1;
  if (i < startIndex)
    i = startIndex;

  return i;
}

void IndicatorPlot::updateStatusBar (int x, int y)
{
  int i = convertXToDataIndex(x);

  QString s;
  dateBars.getDateTimeString(i, s);
  s.append(" ");
  Utils util;
  QString str;
  util.strip(scaler.convertToVal(y), 4, str);
  s.append(str);
  emit statusMessage(s);
}

int IndicatorPlot::getXFromDate (QDateTime &d)
{
  int x2 = dateBars.getX(d);
  if (x2 == -1)
    return -1;

  int x = startX + (x2 * pixelspace) - (startIndex * pixelspace);

  return x;
}

void IndicatorPlot::showPopupMenu ()
{
  chartMenu->exec(QCursor::pos());
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

int IndicatorPlot::getWidth ()
{
  return buffer.width();
}

void IndicatorPlot::setIndicator (Indicator &d)
{
  indicator = d;
}

void IndicatorPlot::getIndicator (Indicator &d)
{
  d = indicator;
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
  // draw rubber band on widget
  QPainter painter;
  painter.begin(&buffer);

  painter.setPen(QColor("red"));
  painter.drawRect(r);

  painter.end();

  update();
}

void IndicatorPlot::setPlotPluginPath (QString &d)
{
  plotPluginPath = d;
}

void IndicatorPlot::setCOPluginPath (QString &d)
{
  coPluginPath = d;
}

void IndicatorPlot::getDateBar (DateBar &d)
{
  d = dateBars;
}

//*************************************************************************
//******************** chart object functions *****************************
//*************************************************************************

void IndicatorPlot::setExternalChartObjectFlag ()
{
  if (mouseFlag == NewObjectWait)
  {
    mouseFlag = None;
    setCursor(QCursor(Qt::ArrowCursor));
  }
}

void IndicatorPlot::newExternalChartObject (QString d)
{
  mouseFlag = NewObjectWait;
  newChartObject = d;
  setCursor(QCursor(Qt::PointingHandCursor));
  statusMessage(QString(tr("Select point to place object...")));
}

void IndicatorPlot::slotNewChartObject (QString selection)
{
  if (! chartSymbol.length())
    return;

  coSelected = new ChartObject;
  connect(coSelected, SIGNAL(signalMessage(QString)), this, SIGNAL(statusMessage(QString)));

  Config config;
  QString id = QString::number(config.getInt(Config::LastChartObjectID) + 1);
  config.setData(Config::LastChartObjectID, id);

  coSelected->setData(ChartObject::ParmID, id);
  coSelected->setData(ChartObject::ParmSymbol, chartSymbol);
  QString s;
  indicator.getName(s);
  coSelected->setData(ChartObject::ParmIndicator, s);
  coSelected->setData(ChartObject::ParmPlugin, selection);
  
  PluginFactory fac;
  COPlugin *plug = fac.getCO(coPluginPath, selection);
  
  plug->create(coSelected);
  
  indicator.addChartObject(coSelected);
  
  mouseFlag = ClickWait;

  setCursor(QCursor(Qt::PointingHandCursor));
}

void IndicatorPlot::drawObjects ()
{
  PluginFactory fac;
  QHash<QString, ChartObject *> coList;
  indicator.getChartObjects(coList);
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
    
    plug->draw(coDraw, buffer, dateBars, startX, pixelspace, startIndex, scaler);
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

  indicator.clearChartObjects();

  DataBase db;
  db.deleteChartObjects(chartSymbol);

  mouseFlag = None;

  draw();
}

void IndicatorPlot::slotChartObjectDeleted ()
{
  if (! chartSymbol.length() || ! coSelected)
    return;

  QString s;
  coSelected->getData(ChartObject::ParmID, s);
  indicator.deleteChartObject(s);
  coSelected = 0;

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
  QHash<QString, ChartObject *> coList;
  indicator.getChartObjects(coList);
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

  indicator.clearChartObjects();

  QString s;
  indicator.getName(s);
  DataBase db;
  db.getChartObjects(chartSymbol, s, indicator);
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

