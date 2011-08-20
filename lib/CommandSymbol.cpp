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

#include "CommandSymbol.h"
#include "Strip.h"
#include "QuoteDataBase.h"
#include "Globals.h"
#include "DateRange.h"
#include "BarLength.h"
#include "Script.h"
#include "SettingString.h"
#include "SettingList.h"

#include <QtDebug>

CommandSymbol::CommandSymbol (QObject *p) : Command (p)
{
  _type = "SYMBOL";
}

int CommandSymbol::runScript (void *d)
{
  Script *script = (Script *) d;

  SettingGroup *sg = script->settingGroup(script->currentStep());
  if (! sg)
    return _ERROR;

  // symbol
  BarData bd;
  QString s = sg->get("SYMBOL")->getString();
  if (bd.setKey(s))
  {
    qDebug() << _type << "::runScript: invalid SYMBOL" << s;
    return _ERROR;
  }

  // length
  s = sg->get("LENGTH")->getString();
  BarLength bl;
  QStringList l = bl.list();
  int length = l.indexOf(s);
  if (length == -1)
  {
    qDebug() << _type << "::runScript: invalid LENGTH" << s;
    return _ERROR;
  }
  else
    bd.setBarLength((BarLength::Length) length);

  // range
  s = sg->get("RANGE")->getString();
  DateRange dr;
  l = dr.list();
  int range = l.indexOf(s);
  if (range == -1)
  {
    qDebug() << _type << "::runScript: invalid RANGE" << s;
    return _ERROR;
  }
  else
    bd.setRange(range);

  // date
  s = sg->get("DATE")->getString();
  Curve *dline = script->curve(s);
  if (dline)
  {
    qDebug() << _type << "::runScript: duplicate DATE" << s;
    return _ERROR;
  }
  dline = new Curve;
  dline->setLabel(s);
  script->setCurve(s, dline);

  // open
  s = sg->get("OPEN")->getString();
  Curve *oline = script->curve(s);
  if (oline)
  {
    qDebug() << _type << "::runScript: duplicate OPEN" << s;
    return _ERROR;
  }
  oline = new Curve;
  oline->setLabel(s);
  script->setCurve(s, oline);

  // high
  s = sg->get("HIGH")->getString();
  Curve *hline = script->curve(s);
  if (hline)
  {
    qDebug() << _type << "::runScript: duplicate HIGH" << s;
    return _ERROR;
  }
  hline = new Curve;
  hline->setLabel(s);
  script->setCurve(s, hline);

  // low
  s = sg->get("LOW")->getString();
  Curve *lline = script->curve(s);
  if (lline)
  {
    qDebug() << _type << "::runScript: duplicate LOW" << s;
    return _ERROR;
  }
  lline = new Curve;
  lline->setLabel(s);
  script->setCurve(s, lline);

  // close
  s = sg->get("CLOSE")->getString();
  Curve *cline = script->curve(s);
  if (cline)
  {
    qDebug() << _type << "::runScript: duplicate CLOSE" << s;
    return _ERROR;
  }
  cline = new Curve;
  cline->setLabel(s);
  script->setCurve(s, cline);

  // volume
  s = sg->get("VOLUME")->getString();
  Curve *vline = script->curve(s);
  if (vline)
  {
    qDebug() << _type << "::runScript: duplicate VOLUME" << s;
    return _ERROR;
  }
  vline = new Curve;
  vline->setLabel(s);
  script->setCurve(s, vline);

  // oi
  s = sg->get("OI")->getString();
  Curve *iline = script->curve(s);
  if (iline)
  {
    qDebug() << _type << "::runScript: duplicate OI" << s;
    return _ERROR;
  }
  iline = new Curve;
  iline->setLabel(s);
  script->setCurve(s, iline);

  // load quotes
  QuoteDataBase db;
  if (db.getBars(&bd))
  {
    qDebug() << _type << "::runScript: QuoteDataBase error";
    return _ERROR;
  }

  int loop = 0;
  for (; loop < bd.count(); loop++)
  {
    Bar *b = bd.bar(loop);
    if (! b)
      continue;

    if (dline)
    {
      CurveBar *cb = new CurveBar;
      cb->setDateTime(b->date());
      dline->setBar(loop, cb);
    }

    if (oline)
      oline->setBar(loop, new CurveBar(b->open()));

    if (hline)
      hline->setBar(loop, new CurveBar(b->high()));

    if (lline)
      lline->setBar(loop, new CurveBar(b->low()));

    if (cline)
      cline->setBar(loop, new CurveBar(b->close()));

    if (vline)
      vline->setBar(loop, new CurveBar(b->volume()));

    if (iline)
      iline->setBar(loop, new CurveBar(b->oi()));
  }

  return _OK;
}

SettingGroup * CommandSymbol::settings ()
{
  SettingGroup *sg = new SettingGroup;
  sg->setCommand(_type);

  SettingString *ss = new SettingString(QString());
  ss->setKey("SYMBOL");
  sg->set(ss);

  ss = new SettingString(Setting::_NONE, Setting::_CURVE, QString("date"));
  ss->setKey("DATE");
  sg->set(ss);

  ss = new SettingString(Setting::_NONE, Setting::_CURVE, QString("open"));
  ss->setKey("OPEN");
  sg->set(ss);

  ss = new SettingString(Setting::_NONE, Setting::_CURVE, QString("high"));
  ss->setKey("HIGH");
  sg->set(ss);

  ss = new SettingString(Setting::_NONE, Setting::_CURVE, QString("low"));
  ss->setKey("LOW");
  sg->set(ss);

  ss = new SettingString(Setting::_NONE, Setting::_CURVE, QString("close"));
  ss->setKey("CLOSE");
  sg->set(ss);

  ss = new SettingString(Setting::_NONE, Setting::_CURVE, QString("volume"));
  ss->setKey("VOLUME");
  sg->set(ss);

  ss = new SettingString(Setting::_NONE, Setting::_CURVE, QString("oi"));
  ss->setKey("OI");
  sg->set(ss);

  BarLength bl;
  SettingList *sl = new SettingList(bl.list(), QString());
  sl->setKey("LENGTH");
  sg->set(sl);

  DateRange dr;
  sl = new SettingList(dr.list(), QString());
  sl->setKey("RANGE");
  sg->set(sl);

  return sg;
}
