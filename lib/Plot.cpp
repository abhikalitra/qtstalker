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

#include "Plot.h"
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

Plot::Plot (QWidget *w) : QWidget(w)
{
  _plotData.barSpacing = 0;
  _plotData.startIndex = 0;
  _plotData.endIndex = 0;
  _plotData.backgroundColor.setNamedColor("black");
  _plotData.borderColor.setNamedColor("white");
  _plotData.infoIndex = 0;
  _plotData.x = 0;
  _plotData.y = 0;
  _plotData.y1 = 0;
  _plotData.infoFlag = 1;
  _plotData.pos = 0;
  _plotData.interval = Bar::DailyBar;
  _plotData.dateHeight = 30;
  _plotData.scaleWidth = 70;
  _plotData.barWidth = 5;
  
  _mouseFlag = None;
  _saveMouseFlag = None;
  _chartMenu = 0;
  _coSelected = 0;
  _menuFlag = TRUE;
  _rubberBand = 0;
  _newObjectFlag = 0;

  _plotData.plotFont.setFamily("Helvetica");
  _plotData.plotFont.setPointSize(12);
  _plotData.plotFont.setWeight(50);

  _coMenu = new QMenu(this);
  _coMenu->addAction(QPixmap(edit), tr("&Edit Chart Object"), this, SLOT(objectDialog()), Qt::CTRL+Qt::Key_E);
  _coMenu->addAction(QPixmap(delete_xpm), tr("&Delete Chart Object"), this, SLOT(chartObjectDeleted()), Qt::CTRL+Qt::Key_D);
  
  _chartMenu = new QMenu(this);
  _chartMenu->addAction(QPixmap(delete_xpm), tr("Delete &All Chart Objects"), this, SLOT(deleteAllChartObjects()), Qt::CTRL+Qt::Key_A);
  _chartMenu->addSeparator ();
  _chartMenu->addAction(QPixmap(date), tr("Date"), this, SLOT(toggleDate()), Qt::CTRL+Qt::Key_D);
  _chartMenu->addAction(QPixmap(loggridicon), tr("Log Scaling"), this, SLOT(toggleLog()), Qt::CTRL+Qt::Key_L);

  setMouseTracking(TRUE);

  setFocusPolicy(Qt::ClickFocus);
}

Plot::~Plot ()
{
  _indicator.clear();
}

//*********************************************************************
//*************** DRAW FUNCTIONS **************************************
//********************************************************************

void Plot::draw ()
{
  if (_plotData.buffer.isNull())
    return;

  _plotData.buffer.fill(_plotData.backgroundColor);

  if (_dateBars.count())
  {
    // calculate the range of bars to draw
    _plotData.endIndex = _plotData.startIndex + ((_plotData.buffer.width() - _plotData.scaleWidth) / _plotData.barSpacing);
    if (_plotData.endIndex >= _dateBars.count())
      _plotData.endIndex = _dateBars.count() - 1;
    
    // set the current scale
    setScale();

    // calculate the right most bar on screen
    _plotData.infoIndex = convertXToDataIndex((_plotData.buffer.width() - _plotData.scaleWidth));

    // draw the date section
    _datePlot.draw(_plotData, _dateBars);

    // draw the grid
    _grid.drawXGrid(_plotData);
    _grid.drawYGrid(_plotData, _scaler);

    // draw plots
    drawLines();

    // draw chart objects
    drawObjects();

    // draw the top left indicator stats of the right most bar on screen
    PlotInfo info;
    info.drawInfo(_plotData, _indicator, _dateBars);

    // draw the scale
    _scalePlot.draw(_plotData, _scaler);

    // draw the scale markers
    QList<Setting> points;
    info.getPointInfo(_plotData, points, _indicator);
    _scalePlot.drawPoints(_plotData, points, _scaler);
  }

  update();
}

void Plot::drawRefresh ()
{
  update();
}

void Plot::drawLines ()
{
  PlotFactory fac;
  int loop;
  QList<PlotLine *> plotList;
  _indicator.getLines(plotList);
  
  for (loop = 0; loop < plotList.count(); loop++)
  {
    PlotLine *line = plotList.at(loop);

    if (! line->count())
      continue;

    line->draw(_plotData, _scaler);
  }
}

void Plot::paintEvent (QPaintEvent *event)
{
  QPainter painter(this);
  
  if (_mouseFlag == CursorZoom)
    painter.drawPixmap(0, 0, _plotData.buffer);
  else
    painter.drawPixmap(event->rect(), _plotData.buffer);
  
  if (_mouseFlag == CursorCrosshair)
  {
    int y = _scaler.convertToY(_plotData.y1);
    painter.setPen(QPen(_plotData.borderColor, 1, Qt::DotLine));
    painter.drawLine (0, y, _plotData.buffer.width(), y);
    painter.drawLine (_plotData.x, 0, _plotData.x, _plotData.buffer.height());
    painter.end();
  }
}

void Plot::resizeEvent (QResizeEvent *event)
{
  _plotData.buffer = QPixmap(event->size());
//  draw();
  emit signalDraw();
}

void Plot::cursorChanged (int d)
{
  switch (d)
  {
    case 0: // normal cursor
      _mouseFlag = None;
      break;
    case 1:
      _mouseFlag = CursorZoom;
      break;
    case 2:
      _mouseFlag = CursorCrosshair;
      break;
    default:
      break;
  }
  
  updateCursor();
//  draw();
  emit signalDraw();
}

void Plot::updateCursor ()
{
  switch (_mouseFlag)
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

void Plot::setScale ()
{
  int flag = 1;
  double tscaleHigh = -99999999;
  double tscaleLow = 99999999;

  QList<PlotLine *> plotList;
  _indicator.getLines(plotList);
  
  int loop;
  for (loop = 0; loop < plotList.count(); loop++)
  {
    PlotLine *line = plotList.at(loop);

    double h, l;
    if (line->highLowRange(_plotData.startIndex, _plotData.endIndex, h, l))
      continue;

    flag = 0;
    
    if (h > tscaleHigh)
      tscaleHigh = h;
    if (l < tscaleLow)
      tscaleLow = l;
  }

  QDateTime sd;
  _dateBars.getDate(_plotData.startIndex, sd);
  QDateTime ed;
  _dateBars.getDate(_plotData.endIndex, ed);

  QHash<QString, COPlugin *> coList;
  _indicator.getChartObjects(coList);
  QHashIterator<QString, COPlugin *> it(coList);
  while (it.hasNext())
  {
    it.next();
    COPlugin *co = it.value();
    
    if (! co->inDateRange(sd, ed, _dateBars))
      continue;
      
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
  tscaleHigh += t;
  if (tscaleLow != 0)
    tscaleLow -= t;

  // handle log scaling if toggled
  double tlogScaleHigh = 1;
  double tlogRange = 0;
  if (_indicator.getLog())
  {
    tlogScaleHigh = tscaleHigh > 0.0 ? log(tscaleHigh) : 1;
    double tlogScaleLow = tscaleLow > 0.0 ? log(tscaleLow) : 0;
    tlogRange = tlogScaleHigh - tlogScaleLow;
  }

  _scaler.set(_plotData.buffer.height() - _plotData.dateHeight,
              tscaleHigh,
              tscaleLow,
              tlogScaleHigh,
              tlogRange,
              _indicator.getLog());
}

//*********************************************************************
//*************** MOUSE EVENTS ***************************************
//********************************************************************

void Plot::mousePressEvent (QMouseEvent *event)
{
  if (! _dateBars.count()  || event->button() != Qt::LeftButton)
  {
    QWidget::mousePressEvent(event);
    return;
  }

  switch (_mouseFlag)
  {
    case COSelected:
    {
      QPoint p;
      p.setX(event->x());
      p.setY(event->y());
      _moveFlag = _coSelected->isGrabSelected(p);
      if (_moveFlag)
        _mouseFlag = Moving;
      else
      {
        if (! _coSelected->isSelected(p))
        {
          _mouseFlag = None;
	  _coSelected->setSelected(FALSE);
//          draw();
          emit signalDraw();
        }
      }
      break;
    }
    case Moving:
      _mouseFlag = COSelected;
      break;
    case ClickWait:
    {
      getXY(event->x(), event->y());
      int rc = _coSelected->create2(_plotData.x1, _plotData.y1);
      if (rc)
	_mouseFlag = ClickWait2;
      else
      {
	_mouseFlag = _saveMouseFlag;
	updateCursor();
//        draw();
        emit signalDraw();
	
        int i = convertXToDataIndex(event->x());
        PlotInfo info;
        Setting *mess = info.getCursorInfo(i, event->y(), _dateBars, _scaler);
        if (mess)
          emit signalInfoMessage(mess);
      }
      break;
    }
    case ClickWait2:
    {
      getXY(event->x(), event->y());
      int rc = _coSelected->create3(_plotData.x1, _plotData.y1);
      if (! rc)
      {
	_mouseFlag = _saveMouseFlag;
	updateCursor();
//        draw();
        emit signalDraw();
	
        int i = convertXToDataIndex(event->x());
        PlotInfo info;
        Setting *mess = info.getCursorInfo(i, event->y(), _dateBars, _scaler);
        if (mess)
          emit signalInfoMessage(mess);
      }
      break;
    }
    case NewObjectWait:
    {
      _mouseFlag = ClickWait;
      emit signalNewExternalChartObjectDone();
      mousePressEvent(event); // recursive call to capture first mouse click again
      break;
    }
    case CursorZoom:
    {
      if (! _rubberBand)
	return;
      
      if (_rubberBand->width() < _plotData.barSpacing)
        return;
      
      // calculate new pixel spacing and position here
      int x = convertXToDataIndex(_mouseOrigin.x());
      int ti = _rubberBand->width() / _plotData.barSpacing;
      ti = this->width() / ti;
      
      delete _rubberBand;
      _rubberBand = 0;
      
      unsetCursor();
      
      if (ti < _plotData.barSpacing)
        return;
      
      emit signalPixelspaceChanged(x, ti);
      break;
    }
    case None:
    {
      QHash<QString, COPlugin *> coList;
      _indicator.getChartObjects(coList);
      QHashIterator<QString, COPlugin *> it(coList);
      while (it.hasNext())
      {
        it.next();
        _coSelected = it.value();
        QPoint p(event->x(), event->y());
        if (_coSelected->isSelected(p))
        {
          _mouseFlag = COSelected;
          _coSelected->setSelected(TRUE);
          PlotInfo info;
          Setting *mess = info.getCOInfo(_coSelected);
          if (mess)
            emit signalInfoMessage(mess);
//	  draw();
          emit signalDraw();
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

void Plot::mouseMoveEvent (QMouseEvent *event)
{
  // ignore moves above the top of the chart - we get draw errors if we don't
  if (! _dateBars.count() || event->y() <= 0)
  {
    QWidget::mouseMoveEvent(event);
    return;
  }

  switch (_mouseFlag)
  {
    case CursorCrosshair:
    {
      getXY(event->x(), event->y());

      update();
      
      int i = convertXToDataIndex(event->x());
      PlotInfo info;
      Setting *mess = info.getCursorInfo(i, event->y(), _dateBars, _scaler);
      if (mess)
        emit signalInfoMessage(mess);
      break;
    }
    case CursorZoom:
    {
      int i = convertXToDataIndex(event->x());
      PlotInfo info;
      Setting *mess = info.getCursorInfo(i, event->y(), _dateBars, _scaler);
      if (mess)
        emit signalInfoMessage(mess);
      
      if (_rubberBand)
        _rubberBand->setGeometry(QRect(_mouseOrigin, event->pos()).normalized());
      break;
    }
    case Moving:
    {
      getXY(event->x(), event->y());
      _coSelected->moving(_plotData.x1, _plotData.y1, _moveFlag);
//      draw();
      emit signalDraw();
      break;
    }
    case ClickWait2:
    {
      getXY(event->x(), event->y());
      _coSelected->moving(_plotData.x1, _plotData.y1, 0);
//      draw();
      emit signalDraw();
      break;
    }
    case None:
    {
      // update the data panel with new data
      if (! _plotData.infoFlag)
	return;
      
      // determine if we are over a chart object, if so update cursor
      QPoint p(event->x(), event->y());
      QHash<QString, COPlugin *> coList;
      _indicator.getChartObjects(coList);
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
      _plotData.infoIndex = convertXToDataIndex(event->x());
      Setting *mess = info.getInfo(p, _plotData, _indicator, _dateBars);
      if (mess)
        emit signalInfoMessage(mess);
      break;
    }
    default:
      QWidget::mouseMoveEvent(event);
      break;
  }
}

void Plot::mouseDoubleClickEvent (QMouseEvent *event)
{
  if (! _dateBars.count())
  {
    QWidget::mouseDoubleClickEvent(event);
    return;
  }

  switch (_mouseFlag)
  {
    case None: // center chart on double click mouse position
    {
      int center = ((_plotData.buffer.width() - _plotData.scaleWidth) / _plotData.barSpacing) / 2;
      int i = event->x() / _plotData.barSpacing;
      if (i < center)
        emit signalIndexChanged(_plotData.startIndex - (center - i));
      else
      {
	if (i > center)
          emit signalIndexChanged(_plotData.startIndex + (i - center));
      }
      break;
    }
    case COSelected:
      if (_coSelected)
        objectDialog();
      break;
    case CursorZoom:
    {
      if (_rubberBand)
	return;
      
      _rubberBand = new QRubberBand(QRubberBand::Rectangle, this);
      _mouseOrigin = event->pos();
      _rubberBand->setGeometry(QRect(_mouseOrigin, QSize()));
      _rubberBand->show();
      setCursor(QCursor(Qt::SizeFDiagCursor));
      break;
    }
    default:
      QWidget::mouseDoubleClickEvent(event);
      break;
  }
}

//*********************************************************************
//*************** MENU FUNCTIONS ***************************************
//********************************************************************

void Plot::contextMenuEvent (QContextMenuEvent *)
{
  switch (_mouseFlag)
  {
    case COSelected:
      if (_coSelected)
        _coMenu->exec(QCursor::pos());
      break;
    default:
      if (_menuFlag)
        showPopupMenu();
      break;
  }
}

void Plot::showPopupMenu ()
{
  _chartMenu->exec(QCursor::pos());
}

//*********************************************************************
//*************** SET / GET VARIABLES *********************************
//********************************************************************

void Plot::clear ()
{
  saveChartObjects();
  _indicator.clear();
  _dateBars.clear();
}

void Plot::toggleDate ()
{
  int flag = _indicator.getDate();
  if (flag == FALSE)
    flag = TRUE;
  else
    flag = FALSE;
  _indicator.setDate(flag);

  emit signalDateFlag(flag);
}

void Plot::toggleLog ()
{
  int flag = _indicator.getLog();
  if (flag == FALSE)
    flag = TRUE;
  else
    flag = FALSE;
  _indicator.setLog(flag);

  emit signalLogFlag(flag);
}

void Plot::sliderChanged (int v)
{
  setIndex(v);
}

void Plot::gridChanged (bool d)
{
  _grid.setGridFlag(d);
}

void Plot::logScaleChanged (bool d)
{
  setLogScale(d);
  emit signalDraw();
}

void Plot::setData (BarData *l)
{
  if (! l->count())
    return;

  _dateBars.createDateList(l);
  
  _exchange = l->getExchange();
  _symbol = l->getSymbol();
}

void Plot::setInfoFlag (bool d)
{
  _plotData.infoFlag = d;
}

void Plot::setDateFlag (bool d)
{
  _indicator.setDate(d);
}

void Plot::setLogScale (bool d)
{
  _indicator.setLog(d);
}

bool Plot::logScale ()
{
  return _indicator.getLog();
}

void Plot::setInterval (Bar::BarLength d)
{
  _plotData.interval = d;
}

void Plot::setBackgroundColor (QColor d)
{
  _plotData.backgroundColor = d;
}

void Plot::setBorderColor (QColor d)
{
  _plotData.borderColor = d;
}

void Plot::setGridColor (QColor d)
{
  _grid.setGridColor(d);
}

void Plot::setPlotFont (QFont d)
{
  _plotData.plotFont = d;
}

void Plot::setGridFlag (bool d)
{
  _grid.setGridFlag(d);
}

void Plot::setMenuFlag (bool d)
{
  _menuFlag = d;
}

void Plot::setPixelspace (int d)
{
  _plotData.barSpacing = d;
}

void Plot::setIndex (int d)
{
  _plotData.startIndex = d;
}

void Plot::setIndicator (Indicator &d)
{
  _indicator = d;
}

Indicator & Plot::indicator ()
{
  return _indicator;
}

DateBar & Plot::dateBars ()
{
  return _dateBars;
}

int Plot::width ()
{
  return _plotData.buffer.width() - _plotData.scaleWidth;
}

//*********************************************************************
//*************** INTERNAL FUNCTIONS **********************************
//********************************************************************

void Plot::getXY (int x, int y)
{
  _plotData.x = x;
  _plotData.y = y;
  int i = convertXToDataIndex(x);
  _dateBars.getDate(i, _plotData.x1);
  _plotData.y1 = _scaler.convertToVal(y);
}

int Plot::convertXToDataIndex (int x)
{
  int i = (x / _plotData.barSpacing) + _plotData.startIndex;
  if (i >= (int) _dateBars.count())
    i = _dateBars.count() - 1;
  if (i < _plotData.startIndex)
    i = _plotData.startIndex;

  return i;
}

//*************************************************************************
//******************** chart object functions *****************************
//*************************************************************************

void Plot::setExternalChartObjectFlag ()
{
  if (_mouseFlag != NewObjectWait)
    return;
  
  _mouseFlag = _saveMouseFlag;
  _newObjectFlag = FALSE;

  QString s = QString::number(_coSelected->getID());
  _indicator.deleteChartObject(s);
  _coSelected = 0;

  updateCursor();
//  draw();
  emit signalDraw();
}

void Plot::newExternalChartObject (QString d)
{
  _newObjectFlag = TRUE;
  _saveMouseFlag = _mouseFlag;
  _mouseFlag = NewObjectWait;
  newChartObject(d);
}

void Plot::newChartObject (QString selection)
{
  if (! _symbol.length())
    return;

  COFactory fac;
  _coSelected = fac.getCO(selection);
  if (! _coSelected)
  {
    qDebug() << "Plot::slotNewChartObject: no co" << selection;
    return;
  }
  
  connect(_coSelected, SIGNAL(signalMessage(QString)), this, SIGNAL(signalStatusMessage(QString)));

  Config config;
  QString id = QString::number(config.getInt(Config::LastChartObjectID) + 1);
  config.setData(Config::LastChartObjectID, id);

  _coSelected->setID(id.toInt());
  _coSelected->setSymbol(_symbol);
  _coSelected->setExchange(_exchange);
  
  QString s;
  _indicator.getName(s);
  _coSelected->setIndicator(s);
  
  _coSelected->create();
  
  _indicator.addChartObject(_coSelected);

  if (! _newObjectFlag)
    _mouseFlag = ClickWait;

  setCursor(QCursor(Qt::PointingHandCursor));
}

void Plot::drawObjects ()
{
  QHash<QString, COPlugin *> coList;
  _indicator.getChartObjects(coList);
  QHashIterator<QString, COPlugin *> it(coList);
  while (it.hasNext())
  {
    it.next();
    COPlugin *co = it.value();
    co->draw(_plotData, _dateBars, _scaler);
  }
}

void Plot::deleteAllChartObjects ()
{
  if (! _symbol.length())
    return;

  int rc = QMessageBox::warning(this,
		            	tr("Qtstalker: Warning"),
			    	tr("Are you sure you want to delete all chart objects?"),
			    	QMessageBox::Yes,
			    	QMessageBox::No,
			    	QMessageBox::NoButton);

  if (rc == QMessageBox::No)
    return;

  _indicator.clearChartObjects();

  CODataBase db;
  BarData bd;
  bd.setExchange(_exchange);
  bd.setSymbol(_symbol);
  db.deleteChartObjects(&bd);

  _mouseFlag = None;

//  draw();
  emit signalDraw();
}

void Plot::chartObjectDeleted ()
{
  if (! _symbol.length() || ! _coSelected)
    return;

  QString s = QString::number(_coSelected->getID());
  _indicator.deleteChartObject(s);
  _coSelected = 0;

  CODataBase db;
  db.deleteChartObject(s);

  _mouseFlag = None;

//  draw();
  emit signalDraw();
}

void Plot::saveChartObjects ()
{
  if (! _symbol.length())
    return;

  QHash<QString, COPlugin *> coList;
  _indicator.getChartObjects(coList);
  QHashIterator<QString, COPlugin *> it(coList);
  while (it.hasNext())
  {
    it.next();
    COPlugin *co = it.value();
    co->save();
  }
}

void Plot::loadChartObjects ()
{
  saveChartObjects();

  _indicator.clearChartObjects();

  QString s;
  _indicator.getName(s);
  CODataBase db;
  BarData bd;
  bd.setExchange(_exchange);
  bd.setSymbol(_symbol);
  db.getChartObjects(&bd, s, _indicator);
}

void Plot::objectDialog ()
{
  _coSelected->dialog();
//  draw();
  emit signalDraw();
}

