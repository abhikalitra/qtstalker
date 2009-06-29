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

#include "BARS.h"
#include "PrefDialog.h"
#include <QObject>
#include <QInputDialog>
#include <QList>
#include <math.h>
#include <QtDebug>
#include <QMessageBox>



BARS::BARS ()
{
  methodList.append("Bar");
  methodList.append("Candle");
}

BARS::~BARS ()
{
}

void BARS::calculate (BarData *bd, IndicatorParms &parms, QHash<QString, PlotLine *> &tlines)
{
  data = bd;
  QString s("Method");
  QString s2;
  parms.getData(s,s2);

  if (s2 == "Bar")
    calculateBar(parms, tlines);

  if (s2 == "Candle")
    calculateCandle(parms, tlines);
}

void BARS::calculateBar (IndicatorParms &parms, QHash<QString, PlotLine *> &tlines)
{
  PlotLine *line = new PlotLine;
  int loop;
  QColor color;

  QString s = "Bar Up Color";
  QString s2;
  parms.getData(s, s2);
  QColor barUpColor(s2);

  s = "Bar Down Color";
  parms.getData(s, s2);
  QColor barDownColor(s2);

  s = "Bar Neutral Color";
  parms.getData(s, s2);
  QColor barNeutralColor(s2);

  for (loop = 0; loop < (int) data->count(); loop++)
  {
    if (loop > 0)
    {  
      if (data->getClose(loop) > data->getClose(loop - 1))
        color = barUpColor;
      else
      {
        if (data->getClose(loop) < data->getClose(loop - 1))
          color = barDownColor;
        else
          color = barNeutralColor;
      }
    }
    else
      color = barNeutralColor;

    line->append(color, data->getOpen(loop), data->getHigh(loop), data->getLow(loop), data->getClose(loop), FALSE);

    QDateTime dt;
    data->getDate(loop, dt);
    line->append(dt);
  }

  parms.getVariable(s);
  tlines.insert(s, line);
}

void BARS::calculateCandle (IndicatorParms &parms, QHash<QString, PlotLine *> &tlines)
{
  PlotLine *line = new PlotLine;
  int loop;
  QString s = "Candle Color";
  QString s2;
  parms.getData(s, s2);
  QColor color(s2);

  for (loop = 0; loop < (int) data->count(); loop++)
  {
    double c = data->getClose(loop);
    double o = data->getOpen(loop);
    bool fillFlag = FALSE;

    if (o != 0)
    {
      if (c < o)
        fillFlag = TRUE;
    }

    line->append(color, o, data->getHigh(loop), data->getLow(loop), c, fillFlag);

    QDateTime dt;
    data->getDate(loop, dt);
    line->append(dt);
  }

  parms.getVariable(s);
  tlines.insert(s, line);
}

void BARS::prefDialog (IndicatorParms &parms, QStringList &vl)
{
  QString method;
  QString s = "Method";
  parms.getData(s, method);
  if (! method.length())
  {
    bool ok;
    method = QInputDialog::getItem(0,
				   QObject::tr("BARS Method Selection"),
                                   QObject::tr("Select a method:"),
                                   methodList,
                                   0,
                                   TRUE,
                                   &ok);
    if (! ok)
      return;

    parms.setData(s, method);
  }

  QString vnl = QObject::tr("Variable Name");
  QString bucl = QObject::tr("Bar Up Color");
  QString bncl = QObject::tr("Bar Neutral Color");
  QString bdcl = QObject::tr("Bar Down Color");
  QString ccl = QObject::tr("Candle Color");
  PrefDialog *dialog = new PrefDialog(0);
  dialog->setWindowTitle(QObject::tr("BARS Function"));

  parms.getVariable(s);
  dialog->addTextItem(vnl, s);

  QColor color;

  if (method == "Bar")
  {
    parms.getData(bucl, s);
    if (! s.length())
    {
      s = "green";
      parms.setData(bucl, s);
    }
    color.setNamedColor(s);
    dialog->addColorItem(bucl, color);

    parms.getData(bdcl, s);
    if (! s.length())
    {
      s = "red";
      parms.setData(bdcl, s);
    }
    color.setNamedColor(s);
    dialog->addColorItem(bdcl, color);

    parms.getData(bncl, s);
    if (! s.length())
    {
      s = "blue";
      parms.setData(bncl, s);
    }
    color.setNamedColor(s);
    dialog->addColorItem(bncl, color);
  }

  if (method == "Candle")
  {
    parms.getData(ccl, s);
    if (! s.length())
    {
      s = "green";
      parms.setData(ccl, s);
    }
    color.setNamedColor(s);
    dialog->addColorItem(ccl, color);
  }

  int rc = dialog->exec();

  if (rc != QDialog::Accepted)
  {
    delete dialog;
    return;
  }

  dialog->getText(vnl, s);
  if (! s.length())
  {
    delete dialog;
    return;
  }
  if (vl.indexOf(s) != -1)
  {
    QMessageBox::information(0, QObject::tr("Qtstalker: Error"), QObject::tr("Duplicate variable name."));
    delete dialog;
    return;
  }
  parms.setVariable(s);

  if (method == "Bar")
  {
    dialog->getColor(bucl, color);
    s = color.name();
    parms.setData(bucl, s);

    dialog->getColor(bdcl, color);
    s = color.name();
    parms.setData(bdcl, s);

    dialog->getColor(bncl, color);
    s = color.name();
    parms.setData(bncl, s);
  }

  if (method == "Candle")
  {
    dialog->getColor(ccl, color);
    s = color.name();
    parms.setData(ccl, s);
  }

  delete dialog;
}

