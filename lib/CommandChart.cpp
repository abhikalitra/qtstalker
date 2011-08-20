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

#include "CommandChart.h"
#include "Globals.h"
#include "Script.h"
#include "IPCMessage.h"
#include "MessageSend.h"
#include "SettingString.h"
#include "SettingBool.h"
#include "SettingInteger.h"

#include <QtDebug>
#include <QSettings>

CommandChart::CommandChart (QObject *p) : Command (p)
{
  _type = "CHART";
}

int CommandChart::runScript (void *d)
{
  Script *script = (Script *) d;

  SettingGroup *sg = script->settingGroup(script->currentStep());
  if (! sg)
    return _ERROR;

  QStringList data;
  data << sg->get("NAME")->toString() << sg->get("DATE")->toString();
  data << sg->get("LOG")->toString() << sg->get("ROW")->toString() << sg->get("COL")->toString();

  IPCMessage ipcm(script->session(), _type, "*", script->file());

  MessageSend ms(this);
  ms.send(ipcm, data);

  return _OK;
}

int CommandChart::message (IPCMessage &mess, QString &d)
{
  QStringList l = d.split(",");
  if (l.count() != 5)
  {
    qDebug() << "CommandChart::message: invalid # parms" << l;
    return _ERROR;
  }

  Plot *plot = g_plotGroup->plot(l.at(0));
  if (! plot)
  {
    plot = new Plot(l.at(0), g_plotGroup);
    plot->setBarSpacing(g_controlPanel->barSpaceButton()->getPixelSpace());
    plot->loadSettings();
    plot->setScriptFile(mess.scriptFile());
    plot->setRow(l.at(3).toInt());
    plot->setCol(l.at(4).toInt());

    connect(plot, SIGNAL(signalInfoMessage(Message)), g_infoPanel, SLOT(showInfo(Message)));
    connect(g_controlPanel->barSpaceButton(), SIGNAL(signalPixelSpace(int)), plot, SLOT(setBarSpacing(int)));
    connect(g_controlPanel, SIGNAL(signalValueChanged(int)), plot, SLOT(setStartIndex(int)));
    connect(plot, SIGNAL(signalIndex(int)), g_controlPanel, SLOT(setStartValue(int)));

    g_plotGroup->setPlot(plot);
  }

  plot->clear();

  SettingBool v(FALSE);
  v.fromString(l.at(1));
  plot->showDate(v.getBool());

  v.fromString(l.at(2));
  plot->setLogScaling(v.getBool());

  plot->update();

  return _OK;
}

SettingGroup * CommandChart::settings ()
{
  SettingGroup *sg = new SettingGroup;
  sg->setCommand(_type);

  SettingString *ss = new SettingString(Setting::_NONE, Setting::_CHART, QString("chart1"));
  ss->setKey("NAME");
  sg->set(ss);

  SettingBool *sb = new SettingBool(Setting::_NONE, Setting::_BOOL, TRUE);
  sb->setKey("DATE");
  sg->set(sb);

  sb = new SettingBool(Setting::_NONE, Setting::_BOOL, FALSE);
  sb->setKey("LOG");
  sg->set(sb);

  SettingInteger *si = new SettingInteger(0, 0, 0, 2, 0);
  si->setKey("ROW");
  sg->set(si);

  si = new SettingInteger(0, 0, 0, 99, 0);
  si->setKey("COL");
  sg->set(si);

  return sg;
}
