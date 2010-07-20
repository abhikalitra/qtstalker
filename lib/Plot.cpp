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
#include "IndicatorDataBase.h"
#include "Globals.h"
#include "IndicatorThread.h"

#include "../pics/loggrid.xpm"
#include "../pics/date.xpm"
#include "../pics/delete.xpm"
#include "../pics/edit.xpm"
#include "../pics/indicator.xpm"

#include <QPainter>
#include <QPen>
#include <QPolygon>
#include <QCursor>
#include <QImage>
#include <QMessageBox>
#include <QtDebug>
#include <iostream>
#include <QColorDialog>
#include <QFontDialog>

Plot::Plot ()
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
  _plotData.interval = BarData::DailyBar;
  _plotData.dateHeight = 30;
  _plotData.scaleWidth = 70;
  _plotData.barWidth = 5;
  _plotData.mouseFlag = None;
  _plotData.x = 0;
  _plotData.y = 0;
  _plotData.plot = this;
  _plotData.plotFont = QFont("Helvetica",9,50,0);
  
  _saveMouseFlag = None;
  _chartMenu = 0;
  _coSelected = 0;
  _menuFlag = TRUE;
  _newObjectFlag = 0;
  _cursor = 0;

  _coMenu = new QMenu(this);
  _coMenu->addAction(QPixmap(edit), tr("&Edit Chart Object"), this, SLOT(objectDialog()), Qt::ALT+Qt::Key_E);
  _coMenu->addAction(QPixmap(delete_xpm), tr("&Delete Chart Object"), this, SLOT(chartObjectDeleted()), Qt::ALT+Qt::Key_D);
  
  _chartMenu = new QMenu(this);
  _chartMenu->addAction(QPixmap(indicator_xpm), tr("&New Indicator"), this, SIGNAL(signalNewIndicator()), Qt::ALT+Qt::Key_N);
  _chartMenu->addAction(QPixmap(edit), tr("Edit &Indicator"), this, SLOT(editIndicator()), Qt::ALT+Qt::Key_I);
  _chartMenu->addAction(QPixmap(), tr("&Move Indicator"), this, SLOT(moveIndicator()), Qt::ALT+Qt::Key_M);
  _chartMenu->addAction(QPixmap(delete_xpm), tr("De&lete Indicator"), this, SLOT(deleteIndicator()), Qt::ALT+Qt::Key_L);
  _chartMenu->addSeparator ();
  _chartMenu->addAction(QPixmap(delete_xpm), tr("Delete &All Chart Objects"), this, SLOT(deleteAllChartObjects()), Qt::ALT+Qt::Key_A);
  _chartMenu->addSeparator ();
  _chartMenu->addAction(QPixmap(date), tr("&Date"), this, SLOT(toggleDate()), Qt::ALT+Qt::Key_D);
  _chartMenu->addAction(QPixmap(loggridicon), tr("Log &Scaling"), this, SLOT(toggleLog()), Qt::ALT+Qt::Key_S);
  _chartMenu->addSeparator ();
  _chartMenu->addAction(tr("&Background Color"), this, SLOT(editBackgroundColor()), Qt::ALT+Qt::Key_B);
  _chartMenu->addAction(tr("Bo&rder Color"), this, SLOT(editBorderColor()), Qt::ALT+Qt::Key_R);
  _chartMenu->addAction(tr("&Font"), this, SLOT(editFont()), Qt::ALT+Qt::Key_F);

  setMouseTracking(TRUE);

  setFocusPolicy(Qt::ClickFocus);

  cursorChanged(0); // set the default cursor

  loadSettings();
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
    _indicator.setScale(_plotData, _dateBars);

    // calculate the right most bar on screen
    _plotData.infoIndex = convertXToDataIndex((_plotData.buffer.width() - _plotData.scaleWidth));

    // draw the grid
    _grid.drawXGrid(_plotData);
    _grid.drawYGrid(_plotData, _indicator.scaler());

    // draw plots
    drawLines();

    // draw chart objects
    drawObjects();

    // draw the date section
    if (_indicator.date())
      _datePlot.draw(_plotData, _dateBars);

    // draw the top left indicator stats of the right most bar on screen
    PlotDrawInfo di;
    di.draw(_plotData, _indicator, _dateBars);

    // draw the scale
    _scalePlot.draw(_plotData, _indicator);
    _scalePlot.drawPoints(_plotData, _indicator);
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

    line->draw(_plotData, _indicator.scaler());
  }
}

void Plot::paintEvent (QPaintEvent *)
{
  QPainter painter(this);
  
//  painter.drawPixmap(0, 0, _plotData.buffer);
//  painter.drawPixmap(event->rect(), _plotData.buffer);

  painter.drawPixmap(0, 0, _plotData.buffer);
  _cursor->draw(painter, _plotData, _dateBars, _indicator);
}

void Plot::resizeEvent (QResizeEvent *event)
{
  _plotData.buffer = QPixmap(event->size());

  emit signalDraw(_indicator.name());
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

  emit signalDraw(_indicator.name());
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
          emit signalDraw(_indicator.name());
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
        emit signalDraw(_indicator.name());
	
        int i = convertXToDataIndex(event->x());
        PlotCursorInfo info;
        Setting *mess = info.infoXY(i, event->y(), _dateBars, _indicator.scaler());
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
        emit signalDraw(_indicator.name());
	
        int i = convertXToDataIndex(event->x());
        PlotCursorInfo info;
        Setting *mess = info.infoXY(i, event->y(), _dateBars, _indicator.scaler());
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
      _cursor->mousePress(_plotData, _dateBars, _indicator);
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
      emit signalDraw(_indicator.name());
      break;
    case ClickWait2:
      getXY(event->x(), event->y());
      _coSelected->moving(_plotData.x1, _plotData.y1, 0);
      emit signalDraw(_indicator.name());
      break;
    case None:
      getXY(event->x(), event->y());
      _cursor->mouseMove(_plotData, _dateBars, _indicator);
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
      _cursor->mouseDoubleClick(_plotData, _dateBars, _indicator);
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
  emit signalDraw(_indicator.name());
}

void Plot::toggleLog ()
{
  int flag = _indicator.getLog();
  if (flag == FALSE)
    flag = TRUE;
  else
    flag = FALSE;
  _indicator.setLog(flag);

  emit signalDraw(_indicator.name());
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
  emit signalDraw(_indicator.name());
}

void Plot::loadIndicator (BarData &data, int index)
{
  setData(data);
  setIndex(index);

  IndicatorDataBase db;
  db.getIndicator(_indicator);

  qRegisterMetaType<Indicator>("Indicator");
  IndicatorThread *r = new IndicatorThread(this, data, _indicator);
  connect(r, SIGNAL(signalDone(Indicator)), this, SLOT(indicatorThreadFinished(Indicator)), Qt::QueuedConnection);
  connect(r, SIGNAL(finished()), r, SLOT(deleteLater()));
  r->start();
}

void Plot::indicatorThreadFinished (Indicator i)
{
  setIndicator(i);
  loadChartObjects();
  if (isVisible())
    draw();
}

void Plot::setData (BarData &data)
{
  if (! data.count())
    return;

  _dateBars.createDateList(data);
  
  _exchange = data.getExchange();
  _symbol = data.getSymbol();
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

void Plot::setInterval (BarData::BarLength d)
{
  _plotData.interval = d;
}

void Plot::setBackgroundColor (QColor d)
{
  _plotData.backgroundColor = d;
  emit signalDraw(_indicator.name());
}

void Plot::setBorderColor (QColor d)
{
  _plotData.borderColor = d;
  emit signalDraw(_indicator.name());
}

void Plot::setGridColor (QColor d)
{
  _grid.setGridColor(d);
  emit signalDraw(_indicator.name());
}

void Plot::setPlotFont (QFont d)
{
  _plotData.plotFont = d;
  emit signalDraw(_indicator.name());
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

void Plot::editBackgroundColor ()
{
  QColor newColor = QColorDialog::getColor(_plotData.backgroundColor, this, tr("Chart Background Color"), 0);
  if (! newColor.isValid())
    return;

  if (_plotData.backgroundColor != newColor)
  {
    Config config;
    config.setData(Config::BackgroundColor, newColor);
    emit signalBackgroundColorChanged(newColor);
  }
}

void Plot::editBorderColor ()
{
  QColor newColor = QColorDialog::getColor(_plotData.borderColor, this, tr("Chart Border Color"), 0);
  if (! newColor.isValid())
    return;

  if (_plotData.borderColor != newColor)
  {
    Config config;
    config.setData(Config::BorderColor, newColor);
    emit signalBorderColorChanged(newColor);
  }
}

void Plot::editFont ()
{
  bool ok;
  QFont newFont = QFontDialog::getFont(&ok, _plotData.plotFont, this, tr("Chart Font"), 0);
  if (! ok)
    return;

  if (_plotData.plotFont != newFont)
  {
    Config config;
    config.setData(Config::PlotFont, newFont);
    emit signalPlotFontChanged(newFont);
  }
}

void Plot::loadSettings ()
{
  Config config;
  QColor c;
  config.getData(Config::BackgroundColor, c);
  if (! c.isValid())
    config.setData(Config::BackgroundColor, _plotData.backgroundColor);
  else
    _plotData.backgroundColor = c;
  
  config.getData(Config::BorderColor, c);
  if (! c.isValid())
    config.setData(Config::BorderColor, _plotData.borderColor);
  else
    _plotData.borderColor = c;

  QString s;
  config.getData(Config::PlotFont, s);
  if (s.isEmpty())
    config.setData(Config::PlotFont, _plotData.plotFont);
  else
    config.getData(Config::PlotFont, _plotData.plotFont);

  _plotData.barSpacing = config.getInt(Config::Pixelspace);

  _plotData.interval = config.getInt(Config::BarLength);
}

void Plot::editIndicator ()
{
  emit signalEditIndicator(_indicator.name());
}

void Plot::deleteIndicator ()
{
  emit signalDeleteIndicator(_indicator.name());
}

void Plot::moveIndicator ()
{
  emit signalMoveIndicator(_indicator.name());
}

void Plot::setRow (int d)
{
  _row = d;
}

int Plot::row ()
{
  return _row;
}

void Plot::setColumn (int d)
{
  _column = d;
}

int Plot::column ()
{
  return _column;
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

  Scaler scaler = _indicator.scaler();
  _plotData.y1 = scaler.convertToVal(y);
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
  emit signalDraw(_indicator.name());
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
    co->draw(_plotData, _dateBars, _indicator.scaler());
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

  emit signalDraw(_indicator.name());
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

  emit signalDraw(_indicator.name());
}

void Plot::saveChartObjects ()
{
  if (! _symbol.length())
    return;

  QList<int> keyList;
  _indicator.coKeys(keyList);

  // we have to lock the mutex here because objects are saved before
  // we load a new indicator
  g_mutex.lock();
  int loop = 0;
  for (; loop < keyList.count(); loop++)
  {
    COPlugin *co = _indicator.chartObject(keyList.at(loop));
    co->save();
  }
  g_mutex.unlock();
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
  emit signalDraw(_indicator.name());
}

void Plot::coSelected (int d)
{
  _coSelected = _indicator.chartObject(d);
  _plotData.mouseFlag = COSelected;
  _coSelected->setSelected(TRUE);
  
  Setting *mess = new Setting;
  _coSelected->getInfo(mess);
  emit signalInfoMessage(mess);
  
  emit signalDraw(_indicator.name());
}

