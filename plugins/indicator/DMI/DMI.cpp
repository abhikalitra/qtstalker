/*
 *  Qtstalker stock charter
 *
 *  Copyright (C) 2001-2004 Stefan S. Stratigakos
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

#include "DMI.h"
#include <math.h>
#include "PrefDialog.h"
#include <qdict.h>

DMI::DMI ()
{
  pluginName = "DMI";
  plotFlag = FALSE;
  setDefaults();
}

DMI::~DMI ()
{
}

void DMI::setDefaults ()
{
  mdiColor.setNamedColor("red");
  pdiColor.setNamedColor("green");
  adxColor.setNamedColor("yellow");
  pdiLineType = PlotLine::Line;
  mdiLineType = PlotLine::Line;
  adxLineType = PlotLine::Line;
  pdiLabel = "+DM";
  mdiLabel = "-DM";
  adxLabel = "ADX";
  period = 14;
  smoothing = 9;
  maType = QSMath::SMA;  
}

void DMI::calculate ()
{
  QSMath *t = new QSMath(data);

  PlotLine *mdi = t->getMDI(period);
  mdi->setColor(mdiColor);
  mdi->setType(mdiLineType);
  mdi->setLabel(mdiLabel);
  output.append(mdi);
  
  PlotLine *pdi = t->getPDI(period);
  pdi->setColor(pdiColor);
  pdi->setType(pdiLineType);
  pdi->setLabel(pdiLabel);
  output.append(pdi);

  PlotLine *adx = t->getADX(mdi, pdi, maType, smoothing);
  adx->setColor(adxColor);
  adx->setType(adxLineType);
  adx->setLabel(adxLabel);
  output.append(adx);
  
  delete t;
}

int DMI::indicatorPrefDialog ()
{
  PrefDialog *dialog = new PrefDialog();
  dialog->setCaption(tr("DMI Indicator"));
  
  dialog->createPage (tr("DMI"));
  dialog->addIntItem(tr("Period"), tr("DMI"), period, 1, 99999999);
  dialog->addIntItem(tr("Smoothing"), tr("DMI"), smoothing, 1, 99999999);
  dialog->addComboItem(tr("Smoothing Type"), tr("DMI"), maTypeList, maType);
  
  dialog->createPage (tr("+DM"));
  dialog->addColorItem(tr("+DM Color"), tr("+DM"), pdiColor);
  dialog->addTextItem(tr("+DM Label"), tr("+DM"), pdiLabel);
  dialog->addComboItem(tr("+DM Line Type"), tr("+DM"), lineTypes, pdiLineType);
  
  dialog->createPage (tr("-DM"));
  dialog->addColorItem(tr("-DM Color"), tr("-DM"), mdiColor);
  dialog->addTextItem(tr("-DM Label"), tr("-DM"), mdiLabel);
  dialog->addComboItem(tr("-DM Line Type"), tr("-DM"), lineTypes, mdiLineType);
  
  dialog->createPage (tr("ADX"));
  dialog->addColorItem(tr("ADX Color"), tr("ADX"), adxColor);
  dialog->addTextItem(tr("ADX Label"), tr("ADX"), adxLabel);
  dialog->addComboItem(tr("ADX Line Type"), tr("ADX"), lineTypes, adxLineType);
  
  int rc = dialog->exec();
  
  if (rc == QDialog::Accepted)
  {
    period = dialog->getInt(tr("Period"));
    smoothing = dialog->getInt(tr("Smoothing"));
    maType = (QSMath::MAType) dialog->getComboIndex(tr("Smoothing Type"));
    
    pdiColor = dialog->getColor(tr("+DM Color"));
    pdiLineType = (PlotLine::LineType) dialog->getComboIndex(tr("+DM Line Type"));
    pdiLabel = dialog->getText(tr("+DM Label"));
    
    mdiColor = dialog->getColor(tr("-DM Color"));
    mdiLineType = (PlotLine::LineType) dialog->getComboIndex(tr("-DM Line Type"));
    mdiLabel = dialog->getText(tr("-DM Label"));
    
    adxColor = dialog->getColor(tr("ADX Color"));
    adxLineType = (PlotLine::LineType) dialog->getComboIndex(tr("ADX Line Type"));
    adxLabel = dialog->getText(tr("ADX Label"));
    
    rc = TRUE;
  }
  else
    rc = FALSE;
  
  delete dialog;
  return rc;
}

void DMI::loadIndicatorSettings (QString file)
{
  setDefaults();
  
  QDict<QString> dict = loadFile(file);
  if (! dict.count())
    return;
  
  QString *s = dict["pdiColor"];
  if (s)
    pdiColor.setNamedColor(s->left(s->length()));
    
  s = dict["mdiColor"];
  if (s)
    mdiColor.setNamedColor(s->left(s->length()));
  
  s = dict["adxColor"];
  if (s)
    adxColor.setNamedColor(s->left(s->length()));
    
  s = dict["period"];
  if (s)
    period = (PlotLine::LineType) s->left(s->length()).toInt();
    
  s = dict["smoothing"];
  if (s)
    smoothing = (PlotLine::LineType) s->left(s->length()).toInt();
    
  s = dict["maType"];
  if (s)
    maType = (QSMath::MAType) s->left(s->length()).toInt();
    
  s = dict["pdiLabel"];
  if (s)
    pdiLabel = s->left(s->length());
    
  s = dict["mdiLabel"];
  if (s)
    mdiLabel = s->left(s->length());
    
  s = dict["adxLabel"];
  if (s)
    adxLabel = s->left(s->length());
        
  s = dict["pdiLineType"];
  if (s)
    pdiLineType = (PlotLine::LineType) s->left(s->length()).toInt();

  s = dict["mdiLineType"];
  if (s)
    mdiLineType = (PlotLine::LineType) s->left(s->length()).toInt();

  s = dict["adxLineType"];
  if (s)
    adxLineType = (PlotLine::LineType) s->left(s->length()).toInt();
}

void DMI::saveIndicatorSettings (QString file)
{
  QDict<QString>dict;
  dict.setAutoDelete(TRUE);
  
  dict.replace("period", new QString(QString::number(period)));
  dict.replace("smoothing", new QString(QString::number(smoothing)));
  dict.replace("maType", new QString(QString::number(maType)));
  dict.replace("pdiColor", new QString(pdiColor.name()));
  dict.replace("mdiColor", new QString(mdiColor.name()));
  dict.replace("adxColor", new QString(adxColor.name()));
  dict.replace("mdiLineType", new QString(QString::number(mdiLineType)));
  dict.replace("pdiLineType", new QString(QString::number(pdiLineType)));
  dict.replace("adxLineType", new QString(QString::number(adxLineType)));
  dict.replace("pdiLabel", new QString(pdiLabel));
  dict.replace("mdiLabel", new QString(mdiLabel));
  dict.replace("adxLabel", new QString(adxLabel));
  dict.replace("plugin", new QString(pluginName));

  saveFile(file, dict);
}

Plugin * create ()
{
  DMI *o = new DMI;
  return ((Plugin *) o);
}

