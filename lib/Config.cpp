/*
 *  Qtstalker stock charter
 *
 *  Copyright (C) 2001-2006 Stefan S. Stratigakos
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

#include "Config.h"
#include "UpgradeMessage.h"
#include <qobject.h>
#include <qdir.h>
#include <qlibrary.h>
#include <qsettings.h>
#include <qobject.h>
#include <qfile.h>
#include <qtextstream.h>
#include <qapplication.h>
#include <qfileinfo.h>

Config::Config ()
{
  libs.setAutoDelete(TRUE);
  indicatorPlugins.setAutoDelete(TRUE);
  quotePlugins.setAutoDelete(TRUE);
  version = "0.34";  // only this version of plugin is allowed to be loaded
}

Config::~Config ()
{
  indicatorPlugins.clear();
  quotePlugins.clear();
  libs.clear();
}

void Config::setup ()
{
  checkUpgrade();

  QDir dir(QDir::homeDirPath());
  dir.convertToAbs();
  QString home = dir.absPath();
  home.append("/.qtstalker");
  if (! dir.exists(home, TRUE))
  {
    if (! dir.mkdir(home, TRUE))
      qDebug("Unable to create ~/.qtstalker directory.");
  }
  home.append("/data0");
  if (! dir.exists(home, TRUE))
  {
    if (! dir.mkdir(home, TRUE))
      qDebug("Unable to create ~/.qtstalker/data0 directory.");
  }
  setData(Home, home);

  QString s = home + "/data";
  if (! dir.exists(s, TRUE))
  {
    if (! dir.mkdir(s, TRUE))
      qDebug("Unable to create ~/.qtstalker/data0/data directory.");
  }
  setData(DataPath, s);

  s = home + "/group";
  if (! dir.exists(s, TRUE))
  {
    if (! dir.mkdir(s, TRUE))
      qDebug("Unable to create ~/.qtstalker/data0/group directory.");
  }
  setData(GroupPath, s);

  s = home + "/portfolio";
  if (! dir.exists(s, TRUE))
  {
    if (! dir.mkdir(s, TRUE))
      qDebug("Unable to create ~/.qtstalker/data0/portfolio directory.");
  }
  setData(PortfolioPath, s);

  s = home + "/test";
  if (! dir.exists(s, TRUE))
  {
    if (! dir.mkdir(s, TRUE))
      qDebug("Unable to create ~/.qtstalker/data0/test directory.");
  }
  setData(TestPath, s);

  s = home + "/scanner";
  if (! dir.exists(s, TRUE))
  {
    if (! dir.mkdir(s, TRUE))
      qDebug("Unable to create ~/.qtstalker/data0/scanner directory.");
  }
  setData(ScannerPath, s);

  s = home + "/indicator";
  if (! dir.exists(s, TRUE))
  {
    if (! dir.mkdir(s, TRUE))
      qDebug("Unable to create ~/.qtstalker/data0/indicator directory.");
  }
  setData(IndicatorPath, s);
  
  s = home + "/indicator/Indicators";
  if (! dir.exists(s, TRUE))
  {
    if (! dir.mkdir(s, TRUE))
      qDebug("Unable to create ~/.qtstalker/data0/indicator/Indicators directory.");
    else
    {
      QString str("Indicators");
      setData(IndicatorGroup, str); // set the new default template
      setDefaultIndicators();
    }
  }
  
  s = home + "/plugin";
  if (! dir.exists(s, TRUE))
  {
    if (! dir.mkdir(s, TRUE))
      qDebug("Unable to create ~/.qtstalker/data0/plugin directory.");
  }
  
  s.append("/quote");
  if (! dir.exists(s, TRUE))
  {
    if (! dir.mkdir(s, TRUE))
      qDebug("Unable to create ~/.qtstalker/data0/plugin/quote directory.");
  }
  setData(QuotePluginStorage, s);
  
  getData(AppFont, s);
  if (! s.length())
  {
    QFont font = QApplication::font(0);  
    s = font.family();
    s.append(",");
    s.append(QString::number(font.pointSize()));
    s.append(",");
    s.append(QString::number(font.weight()));
    setData(AppFont, s);
    setData(PlotFont, s);
  }

  // set the version #
  setData(Version, version);
}

void Config::getData (Parm p, QString &s)
{
  QSettings settings;

  s.truncate(0);
  switch (p)
  {
    case Home:
      s = settings.readEntry("/Qtstalker/Home");
      break;
    case DataPath:
      s = settings.readEntry("/Qtstalker/DataPath");
      break;
    case GroupPath:
      s = settings.readEntry("/Qtstalker/GroupPath");
      break;
    case PortfolioPath:
      s = settings.readEntry("/Qtstalker/PortfolioPath");
      break;
    case TestPath:
      s = settings.readEntry("/Qtstalker/TestPath");
      break;
    case ScannerPath:
      s = settings.readEntry("/Qtstalker/ScannerPath");
      break;
    case IndicatorPath:
      s = settings.readEntry("/Qtstalker/IndicatorPath");
      break;
    case BarLength:
      s = settings.readEntry("/Qtstalker/Compression", "6");
      break;
    case Grid:
      s = settings.readEntry("/Qtstalker/Grid", "1");
      break;
    case Bars:
      s = settings.readEntry("/Qtstalker/Bars", "275");
      break;
    case BackgroundColor:
      s = settings.readEntry("/Qtstalker/BackgroundColor", "black");
      break;
    case BorderColor:
      s = settings.readEntry("/Qtstalker/BorderColor", "white");
      break;
    case GridColor:
      s = settings.readEntry("/Qtstalker/GridColor", "#626262");
      break;
    case ScaleToScreen:
      s = settings.readEntry("/Qtstalker/ScaleToScreen", "1");
      break;
    case IndicatorPluginPath:
      s = settings.readEntry("/Qtstalker/IndicatorPluginPath", "/usr/lib/qtstalker/indicator");
      break;
    case QuotePluginPath:
      s = settings.readEntry("/Qtstalker/QuotePluginPath", "/usr/lib/qtstalker/quote");
      break;
    case Group:
      s = settings.readEntry("/Qtstalker/Group");
      break;
    case PlotFont:
      s = settings.readEntry("/Qtstalker/PlotFont");
      break;
    case AppFont:
      s = settings.readEntry("/Qtstalker/AppFont");
      break;
    case NavAreaSize:
      s = settings.readEntry("/Qtstalker/NavAreaSize", "20");
      break;
    case LogScale:
      s = settings.readEntry("/Qtstalker/LogScale", "0");
      break;
    case Height:
      s = settings.readEntry("/Qtstalker/Height", "640");
      break;
    case Width:
      s = settings.readEntry("/Qtstalker/Width", "480");
      break;
    case X:
      s = settings.readEntry("/Qtstalker/X", "0");
      break;
    case Y:
      s = settings.readEntry("/Qtstalker/Y", "0");
      break;
    case Crosshairs:
      s = settings.readEntry("/Qtstalker/Crosshairs", "1");
      break;
    case DrawMode:
      s = settings.readEntry("/Qtstalker/DrawMode", "0");
      break;
    case HelpFilePath:
      s = settings.readEntry("/Qtstalker/HelpFilePath", "/usr/share/doc/qtstalker/html");
      break;
    case LastQuotePlugin:
      s = settings.readEntry("/Qtstalker/LastQuotePlugin");
      break;
    case PlotSizes:
      s = settings.readEntry("/Qtstalker/PlotSizes");
      break;
    case DataPanelSize:
      s = settings.readEntry("/Qtstalker/DataPanelSize");
      break;
    case Menubar:
      s = settings.readEntry("/Qtstalker/Menubar", "1");
      break;
    case IndicatorGroup:
      s = settings.readEntry("/Qtstalker/IndicatorGroup", "Indicators");
      break;
    case QuotePluginStorage:
      s = settings.readEntry("/Qtstalker/QuotePluginStorage");
      break;
    case Version:
      s = settings.readEntry("/Qtstalker/Version");
      break;
    case ShowUpgradeMessage:
      s = settings.readEntry("/Qtstalker/ShowUpgradeMessage", "1");
      break;
    case LastNewIndicator:
      s = settings.readEntry("/Qtstalker/LastNewIndicator", "MA");
      break;
    case PaperTradeMode:
      s = settings.readEntry("/Qtstalker/PaperTradeMode", "0");
      break;
    case Pixelspace:
      s = settings.readEntry("/Qtstalker/Pixelspace", "4");
      break;
    case PS1Button:
      s = settings.readEntry("/Qtstalker/PS1Button", "4");
      break;
    case PS2Button:
      s = settings.readEntry("/Qtstalker/PS2Button", "4");
      break;
    case PS3Button:
      s = settings.readEntry("/Qtstalker/PS3Button", "4");
      break;
    default:
      break;
  }
}

void Config::getData (QString &p, QString &s)
{
  QSettings settings;
  s = settings.readEntry("/Qtstalker/" + p);
}

void Config::setData (Parm p, QString &d)
{
  QSettings settings;

  switch (p)
  {
    case Home:
      settings.writeEntry("/Qtstalker/Home", d);
      break;
    case DataPath:
      settings.writeEntry("/Qtstalker/DataPath", d);
      break;
    case GroupPath:
      settings.writeEntry("/Qtstalker/GroupPath", d);
      break;
    case PortfolioPath:
      settings.writeEntry("/Qtstalker/PortfolioPath", d);
      break;
    case TestPath:
      settings.writeEntry("/Qtstalker/TestPath", d);
      break;
    case ScannerPath:
      settings.writeEntry("/Qtstalker/ScannerPath", d);
      break;
    case IndicatorPath:
      settings.writeEntry("/Qtstalker/IndicatorPath", d);
      break;
    case BarLength:
      settings.writeEntry("/Qtstalker/Compression", d);
      break;
    case Grid:
      settings.writeEntry("/Qtstalker/Grid", d);
      break;
    case Bars:
      settings.writeEntry("/Qtstalker/Bars", d);
      break;
    case BackgroundColor:
      settings.writeEntry("/Qtstalker/BackgroundColor", d);
      break;
    case BorderColor:
      settings.writeEntry("/Qtstalker/BorderColor", d);
      break;
    case GridColor:
      settings.writeEntry("/Qtstalker/GridColor", d);
      break;
    case ScaleToScreen:
      settings.writeEntry("/Qtstalker/ScaleToScreen", d);
      break;
    case IndicatorPluginPath:
      settings.writeEntry("/Qtstalker/IndicatorPluginPath", d);
      break;
    case QuotePluginPath:
      settings.writeEntry("/Qtstalker/QuotePluginPath", d);
      break;
    case Group:
      settings.writeEntry("/Qtstalker/Group", d);
      break;
    case PlotFont:
      settings.writeEntry("/Qtstalker/PlotFont", d);
      break;
    case AppFont:
      settings.writeEntry("/Qtstalker/AppFont", d);
      break;
    case NavAreaSize:
      settings.writeEntry("/Qtstalker/NavAreaSize", d);
      break;
    case LogScale:
      settings.writeEntry("/Qtstalker/LogScale", d);
      break;
    case Height:
      settings.writeEntry("/Qtstalker/Height", d);
      break;
    case Width:
      settings.writeEntry("/Qtstalker/Width", d);
      break;
    case X:
      settings.writeEntry("/Qtstalker/X", d);
      break;
    case Y:
      settings.writeEntry("/Qtstalker/Y", d);
      break;
    case Crosshairs:
      settings.writeEntry("/Qtstalker/Crosshairs", d);
      break;
    case DrawMode:
      settings.writeEntry("/Qtstalker/DrawMode", d);
      break;
    case HelpFilePath:
      settings.writeEntry("/Qtstalker/HelpFilePath", d);
      break;
    case LastQuotePlugin:
      settings.writeEntry("/Qtstalker/LastQuotePlugin", d);
      break;
    case PlotSizes:
      settings.writeEntry("/Qtstalker/PlotSizes", d);
      break;
    case DataPanelSize:
      settings.writeEntry("/Qtstalker/DataPanelSize", d);
      break;
    case Menubar:
      settings.writeEntry("/Qtstalker/Menubar", d);
      break;
    case IndicatorGroup:
      settings.writeEntry("/Qtstalker/IndicatorGroup", d);
      break;
    case QuotePluginStorage:
      settings.writeEntry("/Qtstalker/QuotePluginStorage", d);
      break;
    case Version:
      settings.writeEntry("/Qtstalker/Version", d);
      break;
    case ShowUpgradeMessage:
      settings.writeEntry("/Qtstalker/ShowUpgradeMessage", d);
      break;
    case LastNewIndicator:
      settings.writeEntry("/Qtstalker/LastNewIndicator", d);
      break;
    case PaperTradeMode:
      settings.writeEntry("/Qtstalker/PaperTradeMode", d);
      break;
    case Pixelspace:
      settings.writeEntry("/Qtstalker/Pixelspace", d);
      break;
    case PS1Button:
      settings.writeEntry("/Qtstalker/PS1Button", d);
      break;
    case PS2Button:
      settings.writeEntry("/Qtstalker/PS2Button", d);
      break;
    case PS3Button:
      settings.writeEntry("/Qtstalker/PS3Button", d);
      break;
    default:
      break;
  }
}

void Config::setData (QString &p, QString &d)
{
  QSettings settings;
  settings.writeEntry("/Qtstalker/" + p, d);
}

void Config::getIndicators (QString &d, QStringList &l)
{
  QString s;
  getData(IndicatorPath, s);
  s.append("/" + d);
  l.clear();
  getDirList(s, TRUE, l);
}

void Config::getIndicator (QString &d, Setting &set)
{
  QFile f(d);
  if (! f.open(IO_ReadOnly))
  {
    qDebug("Config::getIndicator:can't open indicator file %s", d.latin1());
    return;
  }
  QTextStream stream(&f);
  
  while(stream.atEnd() == 0)
  {
    QString s = stream.readLine();
    s = s.stripWhiteSpace();
    if (! s.length())
      continue;
      
    QStringList l = QStringList::split("=", s, FALSE);
    if (l.count() < 2)
      continue;

    s = s.remove(0, l[0].length() + 1); // do this in case the data portion has an = sign in it.
    set.setData(l[0], s);      
  }
  
  f.close();
}

void Config::setIndicator (QString &d, Setting &set)
{
  QFile f(d);
  if (! f.open(IO_WriteOnly))
  {
    qDebug("Config::setIndicator:can't write to indicator file %s", d.latin1());
    return;
  }
  QTextStream stream(&f);
  
  int loop;
  QStringList l;
  set.getKeyList(l);
  QString k;
  for (loop = 0; loop < (int) l.count(); loop++)
  {
    set.getData(l[loop], k);
    stream << l[loop] << "=" << k << "\n";
  }
  
  f.close();
}

void Config::deleteIndicator (QString &d)
{
  QDir dir;
  dir.remove(d);
}

void Config::getDirList (QString &path, bool flag, QStringList &l)
{
  l.clear();
  QDir dir(path);

  int loop;
  for (loop = 2; loop < (int) dir.count(); loop++)
  {
    QString s = dir.absPath() + "/" + dir[loop];
    QFileInfo fi(s);
    if (! fi.isDir())
    {
      if (flag)
        l.append(fi.absFilePath());
      else
        l.append(dir[loop]);
    }
  }
}

void Config::getPluginList (Config::Parm d, QStringList &l2) 
{
  l2.clear();
  QStringList l;
  QString s;
  getData(d, s);
  getDirList(s, TRUE, l);
  
  if (! l.count())
    return;
  
  int loop;
  for (loop = 0; loop < (int) l.count(); loop++)
  {
    QFileInfo fi(l[loop]);
    if (! fi.fileName().contains(version))
      continue;

    s = fi.baseName(FALSE);
    s.remove(0, 3);
    l2.append(s);
  }
}

void Config::getIndicatorList (QStringList &l) 
{
  l.clear();
  getPluginList(IndicatorPluginPath, l);
}

IndicatorPlugin * Config::getIndicatorPlugin (QString &p)
{
  IndicatorPlugin *plug = indicatorPlugins[p];
  if (plug)
    return plug;

  QString s;
  getData(IndicatorPluginPath, s);
  s.append("/lib" + p + "." + version);

  QLibrary *lib = new QLibrary(s);
  IndicatorPlugin *(*so)() = 0;
  so = (IndicatorPlugin *(*)()) lib->resolve("createIndicatorPlugin");
  if (so)
  {
    plug = (*so)();
    libs.replace(p, lib);
    indicatorPlugins.replace(p, plug);
    return plug;
  }
  else
  {
    qDebug("Config::getIndicatorPlugin:%s Dll error\n", s.latin1());
    delete lib;
    return 0;
  }
}

QuotePlugin * Config::getQuotePlugin (QString &p)
{
  QuotePlugin *plug = quotePlugins[p];
  if (plug)
    return plug;

  QString s;
  getData(QuotePluginPath, s);
  s.append("/lib" + p + "." + version);

  QLibrary *lib = new QLibrary(s);
  QuotePlugin *(*so)() = 0;
  so = (QuotePlugin *(*)()) lib->resolve("createQuotePlugin");
  if (so)
  {
    plug = (*so)();
    libs.replace(p, lib);
    quotePlugins.replace(p, plug);
    return plug;
  }
  else
  {
    qDebug("Config::getQuotePlugin:%s Dll error\n", s.latin1());
    delete lib;
    return 0;
  }
}

void Config::closePlugins ()
{
  indicatorPlugins.clear();
  quotePlugins.clear();
  libs.clear();
}

void Config::closePlugin (QString &d)
{
  indicatorPlugins.remove(d);
  quotePlugins.remove(d);
  libs.remove(d);
}

void Config::copyIndicatorFile (QString &d, QString &d2)
{
  QFile f(d);
  if (! f.open(IO_ReadOnly))
  {
    qDebug("Config::copyFile:can't open input file %s", d.latin1());
    return;
  }
  QTextStream stream(&f);
  
  QFile f2(d2);
  if (! f2.open(IO_WriteOnly))
  {
    qDebug("Config::copyFile:can't open output file %s", d2.latin1());
    f.close();
    return;
  }
  QTextStream stream2(&f2);
  
  while(stream.atEnd() == 0)
  {
    QString s = stream.readLine();
    stream2 << s << "\n";
  }
  stream2 << "enable=1\n";
    
  f.close();
  f2.close();
}

void Config::checkUpgrade ()
{
  QDir dir(QDir::homeDirPath());
  dir.convertToAbs();
  QString s = dir.absPath() + "/Qtstalker";
  if (! dir.exists(s, TRUE))
    return;

  // check if we need to delete the old qtstalkerrc file before we upgrade
  s = dir.absPath() + "/.qtstalker";
  if (! dir.exists(s, TRUE))
  {
    s = dir.absPath() + "/.qt/qtstalkerrc";
    dir.remove(s, TRUE);
  }

  getData(ShowUpgradeMessage, s);
  if (! s.toInt())
    return;

  UpgradeMessage *dialog = new UpgradeMessage(UpgradeMessage::V031);
  int rc = dialog->exec();

  if (rc == QDialog::Accepted)
  {
    bool flag = dialog->getStatus();
    if (flag)
      s = QString::number(0);
    else
      s = QString::number(1);

    setData(ShowUpgradeMessage, s);
  }

  delete dialog;
}

void Config::setDefaultIndicators ()
{
  QString plugin = "BARS";
  QString name = "Bars";
  int tabRow = 1;
  Setting set;
  QString basePath;
  getData(IndicatorPath, basePath);
  basePath.append("/Indicators/");
  createDefaultIndicator(set, plugin, name, tabRow);
  QString s = "method";
  QString s2 = "Bar";
  set.setData(s,s2);
  s = basePath + name;
  setIndicator(s, set);

  name = "Candles";
  set.clear();
  createDefaultIndicator(set, plugin, name, tabRow);
  s = "method";
  s2 = "Candle";
  set.setData(s,s2);
  s = basePath + name;
  setIndicator(s, set);

  plugin = "VOL";
  name = "Vol";
  tabRow = 2;
  set.clear();
  createDefaultIndicator(set, plugin, name, tabRow);
  s = basePath + name;
  setIndicator(s, set);
}

void Config::createDefaultIndicator (Setting &set, QString &plugin, QString &name, int tabRow)
{
  IndicatorPlugin *plug = getIndicatorPlugin(plugin);
  if (! plug)
  {
    qDebug("Config::createDefaultIndicator - could not open plugin");
    closePlugin(plugin);
    return;
  }

  plug->getIndicatorSettings(set);

  QString s = "Name";
  set.setData(s, name);

  s = "enable";
  QString s2 = "1"; 
  set.setData(s, s2);

  s = "tabRow";
  s2 = QString::number(tabRow);
  set.setData(s, s2);

  s = "logScale";
  s2 = "0";
  set.setData(s, s2);

  s = "dateFlag";
  s2 = "1";
  set.setData(s, s2);
}

