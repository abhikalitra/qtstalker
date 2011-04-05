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

#include "INDICATOR.h"
#include "Globals.h"
#include "Operator.h"
#include "Strip.h"

#include <QtDebug>
#include <QEventLoop>

INDICATOR::INDICATOR ()
{
  _plugin = "INDICATOR";
  
  _method << "CLEAR" << "PLOT" << "PLOT_ALL" << "COLOR_ALL" << "COLOR_COMPARE_INDEX";
  _method << "COLOR_COMPARE_VALUE" << "COLOR_SET" << "DELETE" << "INDEX_DELETE";
  _method << "INDEX_RANGE" << "INDEX_SHIFT" << "NEW" << "STYLE" << "VALUE_GET";
  _method << "VALUE_SET" << "RUN";
}

int INDICATOR::command (Command *command)
{
  int rc = 1;
  switch (_method.indexOf(command->parm("METHOD")))
  {
    case 0:
      rc = clear(command);
      break;
    case 1:
      rc = plot(command);
      break;
    case 2:
      rc = plotAll(command);
      break;
    case 3:
      rc = colorAll(command);
      break;
    case 4:
      rc = colorCompareIndex(command);
      break;
    case 5:
      rc = colorCompareValue(command);
      break;
    case 6:
      rc = colorSet(command);
      break;
    case 7:
      rc = remove(command);
      break;
    case 8:
      rc = indexDelete(command);
      break;
    case 9:
      rc = indexRange(command);
      break;
    case 10:
      rc = indexShift(command);
      break;
    case 11:
      rc = create(command);
      break;
    case 12:
      rc = style(command);
      break;
    case 13:
      rc = valueGet(command);
      break;
    case 14:
      rc = valueSet(command);
      break;
    case 15:
      rc = run(command);
      break;
    default:
      break;
  }

  return rc;
}

int INDICATOR::clear (Command *command)
{
  Indicator *i = command->indicator();
  if (! i)
  {
    qDebug() << _plugin << "::clear: indicator not found";
    return 1;
  }
  
  i->clear();
  
  command->setReturnCode("0");

  return 0;
}

int INDICATOR::plot (Command *command)
{
  // PARMS:
  // NAME
  // Z

  Indicator *i = command->indicator();
  if (! i)
  {
    qDebug() << _plugin << "::plot: no indicator";
    return 1;
  }

  QString name = command->parm("NAME");
  Curve *line = i->line(name);
  if (! line)
  {
    qDebug() << _plugin << "::plot: NAME not found" << name;
    return 1;
  }

  bool ok;
  int z = command->parm("Z").toInt(&ok);
  if (! ok)
  {
    qDebug() << _plugin << "::plot: invalid Z value" << z;
    return 1;
  }

  if (z < 0)
  {
    qDebug() << _plugin << "::plot: invalid Z value must be > -1" << z;
    return 1;
  }

  line->setZ(z);

  command->setReturnCode("0");

  return 0;
}

int INDICATOR::plotAll (Command *command)
{
  // PARMS:
  // NAME
  // STYLE
  // COLOR
  // Z

  Indicator *i = command->indicator();
  if (! i)
  {
    qDebug() << _plugin << "::plotAll: no indicator";
    return 1;
  }

  // verify name
  QString name = command->parm("NAME");
  Curve *line = i->line(name);
  if (! line)
  {
    qDebug() << _plugin << "::plotAll: NAME not found" << name;
    return 1;
  }

  // verify style
  QStringList l = line->list();
  int style = l.indexOf(command->parm("STYLE"));
  if (style == -1)
  {
    qDebug() << _plugin << "::plotAll: invalid STYLE" << command->parm("STYLE");
    return 1;
  }
  line->setType(command->parm("STYLE"));

  // verify color
  QColor color(command->parm("COLOR"));
  if (! color.isValid())
  {
    qDebug() << _plugin << "::plotAll: invalid COLOR" << command->parm("COLOR");
    return 1;
  }
  line->setAllColor(color);

  // verify Z
  bool ok;
  int z = command->parm("Z").toInt(&ok);
  if (! ok)
  {
    qDebug() << _plugin << "::plotAll: invalid Z value" << command->parm("Z");
    return 1;
  }

  if (z < 0)
  {
    qDebug() << _plugin << "::plotAll: invalid z value must be > -1" << z;
    return 1;
  }

  line->setZ(z);

  command->setReturnCode("0");

  return 0;
}

int INDICATOR::colorAll (Command *command)
{
  // PARMS:
  // NAME
  // COLOR

  Indicator *i = command->indicator();
  if (! i)
  {
    qDebug() << _plugin << "::colorAll: no indicator";
    return 1;
  }

  QString name = command->parm("NAME");
  Curve *line = i->line(name);
  if (! line)
  {
    qDebug() << _plugin << "::colorAll: NAME not found" << name;
    return 1;
  }

  QColor color(command->parm("COLOR"));
  if (! color.isValid())
  {
    qDebug() << _plugin << "::colorAll: invalid COLOR" << command->parm("COLOR");
    return 1;
  }

  line->setAllColor(color);

  command->setReturnCode("0");

  return 0;
}

int INDICATOR::colorCompareIndex (Command *command)
{
  // PARMS:
  // NAME
  // OP
  // NAME2
  // NAME3
  // COLOR

  Indicator *i = command->indicator();
  if (! i)
  {
    qDebug() << _plugin << "::colorCompareIndex: no indicator";
    return 1;
  }

  // verify NAME
  int offset = 0;
  QString name = command->parm("NAME");
  QStringList l = name.split(".", QString::SkipEmptyParts);
  if (l.count() == 2)
  {
    name = l.at(0);

    bool ok;
    offset = l.at(1).toInt(&ok);
    if (! ok)
    {
      qDebug() << _plugin << "::colorCompareIndex: invalid NAME" << name;
      return 1;
    }
  }

  Curve *line = i->line(name);
  if (! line)
  {
    qDebug() << _plugin << "::colorCompareIndex: NAME not found" << name;
    return 1;
  }

  // verify OP
  Operator top;
  Operator::Type op = top.stringToOperator(command->parm("OP"));
  if (op == -1)
  {
    qDebug() << _plugin << "::colorCompareIndex: invalid OP" << command->parm("OP");
    return 1;
  }

  // verify NAME2
  int offset2 = 0;
  QString name2 = command->parm("NAME2");
  l = name2.split(".", QString::SkipEmptyParts);
  if (l.count() == 2)
  {
    name2 = l.at(0);

    bool ok;
    offset2 = l.at(1).toInt(&ok);
    if (! ok)
    {
      qDebug() << _plugin << "::colorCompareIndex: invalid NAME2" << name2;
      return 1;
    }
  }

  Curve *line2 = i->line(name2);
  if (! line2)
  {
    qDebug() << _plugin << "::colorCompareIndex: NAME2 not found" << name2;
    return 1;
  }

  // verify NAME3
  int offset3 = 0;
  QString name3 = command->parm("NAME3");
  l = name3.split(".", QString::SkipEmptyParts);
  if (l.count() == 2)
  {
    name3 = l.at(0);

    bool ok;
    offset3 = l.at(1).toInt(&ok);
    if (! ok)
    {
      qDebug() << _plugin << "::colorCompareIndex: invalid NAME3" << name3;
      return 1;
    }
  }

  Curve *line3 = i->line(name3);
  if (! line3)
  {
    qDebug() << _plugin << "::colorCompareIndex: NAME3 not found" << name3;
    return 1;
  }

  QColor color(command->parm("COLOR"));
  if (! color.isValid())
  {
    qDebug() << _plugin << "::colorCompareIndex: invalid COLOR" << command->parm("COLOR");
    return 1;
  }

  // find lowest and highest index values
  int high = 0;
  int low = 0;
  line->keyRange(low, high);

  int tlow = 0;
  int thigh = 0;
  line2->keyRange(tlow, thigh);
  if (tlow < low)
    low = tlow;
  if (thigh > high)
    high = thigh;

  int loop = low;
  for (; loop <= high; loop++)
  {
    CurveBar *bar = line->bar(loop - offset);
    if (! bar)
      continue;

    CurveBar *bar2 = line2->bar(loop - offset2);
    if (! bar2)
      continue;

    CurveBar *bar3 = line3->bar(loop - offset3);
    if (! bar3)
      continue;

    if (top.test(bar->data(), op, bar2->data()))
      bar3->setColor(color);
  }

  command->setReturnCode("0");

  return 0;
}

int INDICATOR::colorCompareValue (Command *command)
{
  // PARMS:
  // NAME
  // OP
  // VALUE
  // NAME2
  // COLOR

  Indicator *i = command->indicator();
  if (! i)
  {
    qDebug() << _plugin << "::colorCompareValue: no indicator";
    return 1;
  }

  // verify NAME
  QString name = command->parm("NAME");
  Curve *line = i->line(name);
  if (! line)
  {
    qDebug() << _plugin << "::colorCompareValue: NAME not found" << name;
    return 1;
  }

  // verify OP
  Operator top;
  Operator::Type op = top.stringToOperator(command->parm("OP"));
  if (op == -1)
  {
    qDebug() << _plugin << "::colorCompareValue: invalid operator" << command->parm("OP");
    return 1;
  }

  // verify VALUE
  bool ok;
  double value = command->parm("VALUE").toDouble(&ok);
  if (! ok)
  {
    qDebug() << _plugin << "::colorCompareValue: invalid VALUE" << command->parm("VALUE");
    return 1;
  }

  // verify NAME2
  QString name2 = command->parm("NAME2");
  Curve *line2 = i->line(name2);
  if (! line2)
  {
    qDebug() << _plugin << "::colorCompareValue: NAME2 not found" << name2;
    return 1;
  }

  QColor color(command->parm("COLOR"));
  if (! color.isValid())
  {
    qDebug() << _plugin << "::colorCompareValue: invalid COLOR" << command->parm("COLOR");
    return 1;
  }

  // find lowest and highest index values
  int high = 0;
  int low = 0;
  line->keyRange(low, high);

  int loop = low;
  for (; loop <= high; loop++)
  {
    CurveBar *bar = line->bar(loop);
    if (! bar)
      continue;

    CurveBar *bar2 = line2->bar(loop);
    if (! bar2)
      continue;

    if (top.test(bar->data(), op, value))
      bar2->setColor(color);
  }

  command->setReturnCode("0");

  return 0;
}

int INDICATOR::colorSet (Command *command)
{
  // PARMS:
  // NAME
  // INDEX
  // COLOR

  Indicator *i = command->indicator();
  if (! i)
  {
    qDebug() << _plugin << "::colorSet: no indicator";
    return 1;
  }

  QString name = command->parm("NAME");
  Curve *line = i->line(name);
  if (! line)
  {
    qDebug() << _plugin << "::colorSet: NAME not found" << name;
    return 1;
  }

  bool ok;
  int index = command->parm("INDEX").toInt(&ok);
  if (! ok)
  {
    qDebug() << _plugin << "::colorSet: invalid INDEX value" << command->parm("INDEX");
    return 1;
  }

  CurveBar *bar = line->bar(index);
  if (! bar)
  {
    qDebug() << _plugin << "::colorSet: bar not found at index" << index;
    return 1;
  }

  QColor color(command->parm("COLOR"));
  if (! color.isValid())
  {
    qDebug() << _plugin << "::colorSet: invalid COLOR" << command->parm("COLOR");
    return 1;
  }

  bar->setColor(color);

  command->setReturnCode("0");

  return 0;
}

int INDICATOR::remove (Command *command)
{
  // PARMS:
  // NAME

  Indicator *i = command->indicator();
  if (! i)
  {
    qDebug() << _plugin << "::remove: no indicator";
    return 1;
  }

  i->deleteLine(command->parm("NAME"));

  command->setReturnCode("0");

  return 0;
}

int INDICATOR::indexDelete (Command *command)
{
  // PARMS:
  // NAME
  // INDEX

  Indicator *i = command->indicator();
  if (! i)
  {
    qDebug() << _plugin << "::indexDelete: no indicator";
    return 1;
  }

  // verify NAME
  Curve *line = i->line(command->parm("NAME"));
  if (! line)
  {
    qDebug() << _plugin << "::indexDelete: NAME not found" << command->parm("NAME");
    return 1;
  }

  // verify INDEX
  bool ok;
  int index = command->parm("INDEX").toInt(&ok);
  if (! ok)
  {
    qDebug() << _plugin << "::indexDelete: invalid INDEX value" << command->parm("INDEX");
    return 1;
  }

  line->deleteBar(index);

  command->setReturnCode("0");

  return 0;
}

int INDICATOR::indexRange (Command *command)
{
  // PARMS:
  // NAME

  Indicator *i = command->indicator();
  if (! i)
  {
    qDebug() << _plugin << "::indexRange: no indicator";
    return 1;
  }

  QString name = command->parm("NAME");
  Curve *line = i->line(name);
  if (! line)
  {
    qDebug() << _plugin << "::indexRange: NAME not found" << name;
    return 1;
  }

  int start = 0;
  int end = 0;
  line->keyRange(start, end);

  command->setReturnData(_plugin + "_RANGE_START", QString::number(start));
  command->setReturnData(_plugin + "_RANGE_END", QString::number(end));

  command->setReturnCode("0");

  return 0;
}

int INDICATOR::indexShift (Command *command)
{
  // PARMS:
  // NAME
  // INPUT
  // PERIOD

  Indicator *i = command->indicator();
  if (! i)
  {
    qDebug() << _plugin << "::indexShift: no indicator";
    return 1;
  }

  QString name = command->parm("NAME");
  Curve *line = i->line(name);
  if (line)
  {
    qDebug() << _plugin << "::indexShift: duplicate name" << name;
    return 1;
  }

  Curve *in = i->line(command->parm("INPUT"));
  if (! in)
  {
    qDebug() << _plugin << "::indexShift: INPUT missing" << command->parm("INPUT");
    return 1;
  }

  bool ok;
  int period = command->parm("PERIOD").toInt(&ok);
  if (! ok)
  {
    qDebug() << _plugin << "::indexShift: invalid PERIOD" << command->parm("PERIOD");
    return 1;
  }

  int high = 0;
  int low = 0;
  in->keyRange(low, high);

  line = new Curve;
  int loop = low;
  for (; loop <= high; loop++)
  {
    CurveBar *bar = in->bar(loop);
    if (! bar)
      continue;

    line->setBar(loop + period, new CurveBar(bar->data()));
  }

  line->setLabel(name);
  i->setLine(name, line);

  command->setReturnCode("0");

  return 0;
}

int INDICATOR::create (Command *command)
{
  // PARMS:
  // NAME

  Indicator *i = command->indicator();
  if (! i)
  {
    qDebug() << _plugin << "::create: no indicator";
    return 1;
  }

  QString name = command->parm("NAME");
  Curve *line = i->line(name);
  if (line)
  {
    qDebug() << _plugin << "::create: duplicate NAME" << name;
    return 1;
  }

  line = new Curve;
  line->setLabel(name);
  i->setLine(name, line);

  command->setReturnCode("0");

  return 0;
}

int INDICATOR::style (Command *command)
{
  // PARMS:
  // NAME
  // STYLE

  Indicator *i = command->indicator();
  if (! i)
  {
    qDebug() << _plugin << "::style: no indicator";
    return 1;
  }

  QString name = command->parm("NAME");
  Curve *line = i->line(name);
  if (! line)
  {
    qDebug() << _plugin << "::style: NAME not found" << name;
    return 1;
  }

  QStringList l = line->list();
  int style = l.indexOf(command->parm("STYLE"));
  if (style == -1)
  {
    qDebug() << _plugin << "::style: invalid STYLE" << command->parm("STYLE");
    return 1;
  }
  line->setType(command->parm("STYLE"));

  command->setReturnCode("0");

  return 0;
}

int INDICATOR::valueGet (Command *command)
{
  // PARMS:
  // NAME
  // INDEX

  Indicator *i = command->indicator();
  if (! i)
  {
    qDebug() << _plugin << "::valueGet: no indicator";
    return 1;
  }

  // verify NAME
  int offsetFlag = FALSE;
  int offset = 0;
  QString name = command->parm("NAME");
  QStringList l = name.split(".", QString::SkipEmptyParts);
  if (l.count() == 2)
  {
    name = l.at(0);

    bool ok;
    offset = l.at(1).toInt(&ok);
    if (! ok)
    {
      qDebug() << _plugin << "::valueGet: invalid NAME" << name;
      return 1;
    }

    offsetFlag = TRUE;
  }

  Curve *line = i->line(name);
  if (! line)
  {
    qDebug() << _plugin << "::valueGet: NAME not found" << name;
    return 1;
  }

  // verify INDEX
  int index = 0;
  if (! offsetFlag)
  {
    bool ok;
    index = command->parm("INDEX").toInt(&ok);
    if (! ok)
    {
      qDebug() << _plugin << "::valueGet: invalid INDEX value" << command->parm("INDEX");
      return 1;
    }
  }

  // get bar
  CurveBar *bar = 0;
  if (offsetFlag)
  {
    int high = 0;
    int low = 0;
    line->keyRange(low, high);

    bar = line->bar(high - offset);
    if (! bar)
    {
      qDebug() << _plugin << "::valueGet: bar not found at offset" << high - offset;
      return 1;
    }
  }
  else
  {
    bar = line->bar(index);
    if (! bar)
    {
      qDebug() << _plugin << "::valueGet: bar not found at index" << index;
      return 1;
    }
  }

  QString s;
  if (bar->isDate())
    s = bar->dateTime().toString(Qt::ISODate);
  else
  {
    Strip strip;
    strip.strip(bar->data(), 4, s);
  }

  command->setReturnData(_plugin + "_GET_VALUE", s);

  command->setReturnCode("0");

  return 0;
}

int INDICATOR::valueSet (Command *command)
{
  // PARMS:
  // NAME
  // INDEX
  // VALUE
  // COLOR

  Indicator *i = command->indicator();
  if (! i)
  {
    qDebug() << _plugin << "::valueSet: no indicator";
    return 1;
  }

  QString name = command->parm("NAME");
  Curve *line = i->line(name);
  if (! line)
  {
    qDebug() << _plugin << "::valueSet: NAME not found" << name;
    return 1;
  }

  bool ok;
  int index = command->parm("INDEX").toInt(&ok);
  if (! ok)
  {
    qDebug() << _plugin << "::valueSet: invalid INDEX value" << command->parm("INDEX");
    return 1;
  }

  double value = command->parm("VALUE").toDouble(&ok);
  if (! ok)
  {
    qDebug() << _plugin << "::valueSet: invalid VALUE" << command->parm("VALUE");
    return 1;
  }

  QColor color(command->parm("COLOR"));
  if (! color.isValid())
    color = QColor(Qt::red);

  line->setBar(index, new CurveBar(color, value));

  command->setReturnCode("0");

  return 0;
}

int INDICATOR::run (Command *command)
{
  // PARMS:
  // NAME

  Indicator *i = command->indicator();
  if (! i)
  {
    qDebug() << _plugin << "::run: no indicator";
    return 1;
  }

  QString name = command->parm("NAME");

  i->clear();
  i->setName(name);
  i->load();
  i->calculate();

  QEventLoop e;
  connect(i, SIGNAL(signalPlot()), &e, SLOT(quit()));
  e.exec();

  command->setReturnCode("0");

  return 0;
}

//*************************************************************
//*************************************************************
//*************************************************************

Plugin * createPlugin ()
{
  INDICATOR *o = new INDICATOR;
  return ((Plugin *) o);
}
