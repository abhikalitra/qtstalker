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
#include "PlotCursorInfo.h"
#include "PlotDrawInfo.h"
#include "PlotCursorFactory.h"

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
  _plotData.y1 = 0;
  _plotData.infoFlag = 1;
  _plotData.pos = 0;
  _plotData.interval = Bar::DailyBar;
  _plotData.dateHeight = 30;
  _plotData.scaleWidth = 70;
  _plotData.barWidth = 5;
  _plotData.mouseFlag = None;
  _plotData.x = 0;
  _plotData.y = 0;
  _plotData.plot = this;
  
  _saveMouseFlag = None;
  _chartMenu = 0;
  _coSelected = 0;
  _menuFlag = TRUE;
  _newObjectFlag = 0;
  _cursor = 0;

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

  cursorChanged(0); // set the default cursor
}

Plot::~Plot ()
{
  _indicator.clear();
  delete _cursor;
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
    if (_indicator.date())
      _datePlot.draw(_plotData, _dateBars);

    // draw the grid
    _grid.drawXGrid(_plotData);
    _grid.drawYGrid(_plotData, _scaler);

    // draw plots
    drawLines();

    // draw chart objects
    drawObjects();

    // draw the top left indicator stats of the right most bar on screen
    PlotDrawInfo di;
    di.draw(_plotData, _indicator, _dateBars);

    // draw the scale
    _scalePlot.draw(_plotData, _scaler, _indicator);
    _scalePlot.drawPoints(_plotData, _scaler, _indicator);
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
  int loop = 0;
  QStringList plotList = _indicator.plotOrder();
  for (; loop < plotList.count(); loop++)
  {
    QString s = plotList.at(loop);
    PlotLine *line = _indicator.line(s);

    if (! line->count())
      continue;

    line->draw(_plotData, _scaler);
  }
}

void Plot::paintEvent (QPaintEvent *)
{
  QPainter painter(this);
  
//  painter.drawPixmap(0, 0, _plotData.buffer);
//  painter.drawPixmap(event->rect(), _plotData.buffer);

  painter.drawPixmap(0, 0, _plotData.buffer);
  _cursor->draw(painter, _plotData, _dateBars, _scaler);
}

void Plot::resizeEvent (QResizeEvent *event)
{
  _plotData.buffer = QPixmap(event->size());

  emit signalDraw();
}

void Plot::cursorChanged (int d)
{
  if (_cursor)
    delete _cursor;
  
  PlotCursorFactory fac;
  _cursor = fac.cursor(d);
  
  connect(_cursor, SIGNAL(signalInfoMessage(Setting *)), this, SIGNAL(signalInfoMessage(Setting *)));
  connect(_cursor, SIGNAL(signalPixelSpaceChanged(int, int)), this, SIGNAL(signalPixelSpaceChanged(int, int)));
  connect(_cursor, SIGNAL(signalCOSelected(int)), this, SLOT(coSelected(int)));
  connect(_cursor, SIGNAL(signalIndexChanged(int)), this, SIGNAL(signalIndexChanged(int)));
  setCursor(QCursor((Qt::CursorShape) _cursor->getCursor()));

  emit signalDraw();
}

//*********************************************************************
//*************** SCALE FUNCTIONS *************************************
//********************************************************************

void Plot::setScale ()
{
  double tscaleHigh = -99999999;
  double tscaleLow = 99999999;

  int loop = 0;
  QStringList plotList = _indicator.plotOrder();
  for (; loop < plotList.count(); loop++)
  {
    QString s = plotList.at(loop);
    PlotLine *line = _indicator.line(s);

    double h, l;
    if (line->highLowRange(_plotData.startIndex, _plotData.endIndex, h, l))
      continue;

    if (h > tscaleHigh)
      tscaleHigh = h;
    if (l < tscaleLow)
      tscaleLow = l;
  }

  QDateTime sd;
  _dateBars.getDate(_plotData.startIndex, sd);
  QDateTime ed;
  _dateBars.getDate(_plotData.endIndex, ed);

  QList<int> keyList;
  _indicator.coKeys(keyList);
  for (loop = 0; loop < keyList.count(); loop++)
  {
    COPlugin *co = _indicator.chartObject(keyList.at(loop));
    
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
  if (_indicator.log())
  {
    tlogScaleHigh = tscaleHigh > 0.0 ? log(tscaleHigh) : 1;
    double tlogScaleLow = tscaleLow > 0.0 ? log(tscaleLow) : 0;
    tlogRange = tlogScaleHigh - tlogScaleLow;
  }

  int height = _plotData.buffer.height();
  if (_indicator.date())
    height -= _plotData.dateHeight;
  
  _scaler.set(height,
              tscaleHigh,
              tscaleLow,
              tlogScaleHigh,
              tlogRange,
              _indicator.log());
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

  switch (_plotData.mouseFlag)
  {
    case COSelected:
    {
      getXY(event->x(), event->y());
      QPoint p(event->x(), event->y());
      _moveFlag = _coSelected->isGrabSelected(p);
      if (_moveFlag)
        _plotData.mouseFlag = Moving;
      else
      {
        if (! _coSelected->isSelected(p))
        {
          _plotData.mouseFlag = None;
	  _coSelected->setSelected(FALSE);
          emit signalDraw();
        }
      }
      break;
    }
    case Moving:
      _plotData.mouseFlag = COSelected;
      break;
    case ClickWait:
    {
      getXY(event->x(), event->y());
      int rc = _coSelected->create2(_plotData.x1, _plotData.y1);
      if (rc)
	_plotData.mouseFlag = ClickWait2;
      else
      {
	_plotData.mouseFlag = _saveMouseFlag;
        setCursor(QCursor(Qt::ArrowCursor));
        emit signalDraw();
	
        int i = convertXToDataIndex(event->x());
        PlotCursorInfo info;
        Setting *mess = info.infoXY(i, event->y(), _dateBars, _scaler);
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
	_plotData.mouseFlag = _saveMouseFlag;
        setCursor(QCursor(Qt::ArrowCursor));
        emit signalDraw();
	
        int i = convertXToDataIndex(event->x());
        PlotCursorInfo info;
        Setting *mess = info.infoXY(i, event->y(), _dateBars, _scaler);
        if (mess)
          emit signalInfoMessage(mess);
      }
      break;
    }
    case NewObjectWait:
      _plotData.mouseFlag = ClickWait;
      emit signalNewExternalChartObjectDone();
      mousePressEvent(event); // recursive call to capture first mouse click again
      break;
    case None:
      getXY(event->x(), event->y());
      _cursor->mousePress(_plotData, _dateBars, _scaler, _indicator);
      break;
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

  switch (_plotData.mouseFlag)
  {
    case Moving:
      getXY(event->x(), event->y());
      _coSelected->moving(_plotData.x1, _plotData.y1, _moveFlag);
      emit signalDraw();
      break;
    case ClickWait2:
      getXY(event->x(), event->y());
      _coSelected->moving(_plotData.x1, _plotData.y1, 0);
      emit signalDraw();
      break;
    case None:
      getXY(event->x(), event->y());
      _cursor->mouseMove(_plotData, _dateBars, _scaler, _indicator);
      break;
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

  switch (_plotData.mouseFlag)
  {
    case None:
      getXY(event->x(), event->y());
      _cursor->mouseDoubleClick(_plotData, _dateBars, _scaler);
      break;
    case COSelected:
      if (_coSelected)
        objectDialog();
      break;
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
  switch (_plotData.mouseFlag)
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
  int flag = _indicator.date();
  if (flag == FALSE)
    flag = TRUE;
  else
    flag = FALSE;
  _indicator.setDate(flag);

  emit signalDateFlag(flag);
  emit signalDraw();
}

void Plot::toggleLog ()
{
  int flag = _indicator.log();
  if (flag == FALSE)
    flag = TRUE;
  else
    flag = FALSE;
  _indicator.setLog(flag);

  emit signalDraw();
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
  return _indicator.log();
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
  _plotData.infoIndex = i;
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
  if (_plotData.mouseFlag != NewObjectWait)
    return;
  
  _plotData.mouseFlag = _saveMouseFlag;
  _newObjectFlag = FALSE;

  _indicator.deleteChartObject(_coSelected->getID());
  _coSelected = 0;

//  updateCursor();
  emit signalDraw();
}

void Plot::newExternalChartObject (QString d)
{
  _newObjectFlag = TRUE;
  _saveMouseFlag = (MouseStatus) _plotData.mouseFlag;
  _plotData.mouseFlag = NewObjectWait;
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
  
  _coSelected->setIndicator(_indicator.name());
  
  _coSelected->create();
  
  _indicator.addChartObject(_coSelected);

  if (! _newObjectFlag)
    _plotData.mouseFlag = ClickWait;

  setCursor(QCursor(Qt::PointingHandCursor));
}

void Plot::drawObjects ()
{
  QList<int> keyList;
  _indicator.coKeys(keyList);
  int loop = 0;
  for (; loop < keyList.count(); loop++)
  {
    COPlugin *co = _indicator.chartObject(keyList.at(loop));
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

  _plotData.mouseFlag = None;

  emit signalDraw();
}

void Plot::chartObjectDeleted ()
{
  if (! _symbol.length() || ! _coSelected)
    return;

  _indicator.deleteChartObject(_coSelected->getID());
  _coSelected = 0;

  CODataBase db;
  db.deleteChartObject(_coSelected->getID());

  _plotData.mouseFlag = None;

  emit signalDraw();
}

void Plot::saveChartObjects ()
{
  if (! _symbol.length())
    return;

  QList<int> keyList;
  _indicator.coKeys(keyList);
  int loop = 0;
  for (; loop < keyList.count(); loop++)
  {
    COPlugin *co = _indicator.chartObject(keyList.at(loop));
    co->save();
  }
}

void Plot::loadChartObjects ()
{
  saveChartObjects();

  _indicator.clearChartObjects();

  CODataBase db;
  BarData bd;
  bd.setExchange(_exchange);
  bd.setSymbol(_symbol);
  db.getChartObjects(&bd, _indicator.name(), _indicator);
}

void Plot::objectDialog ()
{
  _coSelected->dialog();
  emit signalDraw();
}

void Plot::coSelected (int d)
{
  _coSelected = _indicator.chartObject(d);
  _plotData.mouseFlag = COSelected;
  _coSelected->setSelected(TRUE);
  
  Setting *mess = new Setting;
  _coSelected->getInfo(mess);
  emit signalInfoMessage(mess);
  
  emit signalDraw();
}

