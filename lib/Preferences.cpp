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

#include "Preferences.h"
#include "Config.h"
#include "HelpWindow.h"
#include <qlabel.h>
#include <qlayout.h>


Preferences::Preferences (QWidget *w) : QTabDialog (w, "Preferences", FALSE, WDestructiveClose)
{
  helpFile = "preferences.html";

  setCaption(tr("Edit Prefs"));

  loadSettings();
  createGeneralPage();
  createColorPage();
  createFontPage();

  setApplyButton(tr("&Apply"));  
  connect(this, SIGNAL(applyButtonPressed()), this, SLOT(slotSave()));
  
  setCancelButton(tr("&Cancel"));  
//  connect(this, SIGNAL(cancelButtonPressed()), this, SLOT(exitDialog()));
  
  setOkButton(QString::null);  
  
  setHelpButton();
  QObject::connect(this, SIGNAL(helpButtonPressed()), this, SLOT(slotHelp()));
}

Preferences::~Preferences ()
{
}

void Preferences::loadSettings ()
{
  Config config;
  QString s;

  config.getData(Config::Menubar, s);
  menubar = s.toInt();

  config.getData(Config::PS1Button, s);
  ps1Button = s.toInt();
  
  config.getData(Config::PS2Button, s);
  ps2Button = s.toInt();
  
  config.getData(Config::PS3Button, s);
  ps3Button = s.toInt();
  
  config.getData(Config::BackgroundColor, s);
  backgroundColor.setNamedColor(s);

  config.getData(Config::BorderColor, s);
  borderColor.setNamedColor(s);

  config.getData(Config::GridColor, s);
  gridColor.setNamedColor(s);

  config.getData(Config::PlotFont, s);
  QStringList l = QStringList::split(",", s, FALSE);
  if (l.count() == 3)
  {
    QFont f(l[0], l[1].toInt(), l[2].toInt());
    plotFont = f;
  }

  config.getData(Config::AppFont, s);
  l = QStringList::split(",", s, FALSE);
  if (l.count() == 3)
  {
    QFont f(l[0], l[1].toInt(), l[2].toInt());
    appFont = f;
  }
}

void Preferences::createGeneralPage ()
{
  // general parms page

  QWidget *w = new QWidget(this);
  QVBoxLayout *vbox = new QVBoxLayout(w);
  
  QGridLayout *grid = new QGridLayout(vbox, 1, 2);
  grid->setMargin(5);
  grid->setSpacing(5);
  grid->setColStretch(1, 1);
  
  vbox->insertStretch(-1, 1);

  // menubar checkbox
  QLabel *label = new QLabel(tr("Main Menubar"), w);
  grid->addWidget(label, 0, 0);
  
  menubarCheck = new QCheckBox(w);
  menubarCheck->setChecked(menubar);
  grid->addWidget(menubarCheck, 0, 1);

  // bar spacing 1
  label = new QLabel(tr("Bar Spacing 1"), w);
  grid->addWidget(label, 1, 0);
  
  bs1Spinner = new QSpinBox(2, 99, 1, w);
  bs1Spinner->setValue(ps1Button);
  grid->addWidget(bs1Spinner, 1, 1);

  // bar spacing 2
  label = new QLabel(tr("Bar Spacing 2"), w);
  grid->addWidget(label, 2, 0);
  
  bs2Spinner = new QSpinBox(2, 99, 1, w);
  bs2Spinner->setValue(ps2Button);
  grid->addWidget(bs2Spinner, 2, 1);

  // bar spacing 3
  label = new QLabel(tr("Bar Spacing 3"), w);
  grid->addWidget(label, 3, 0);
  
  bs3Spinner = new QSpinBox(2, 99, 1, w);
  bs3Spinner->setValue(ps3Button);
  grid->addWidget(bs3Spinner, 3, 1);

  addTab(w, tr("General"));
}

void Preferences::createColorPage ()
{
  // colors parms page

  QWidget *w = new QWidget(this);
  QVBoxLayout *vbox = new QVBoxLayout(w);
  
  QGridLayout *grid = new QGridLayout(vbox, 1, 2);
  grid->setMargin(5);
  grid->setSpacing(5);
  grid->setColStretch(1, 1);
  
  vbox->insertStretch(-1, 1);

  // background color
  QLabel *label = new QLabel(tr("Chart Background"), w);
  grid->addWidget(label, 0, 0);
  
  backgroundColorButton = new ColorButton(w, backgroundColor);
  grid->addWidget(backgroundColorButton, 0, 1);
  backgroundColorButton->setColorButton();

  // border color
  label = new QLabel(tr("Chart Border"), w);
  grid->addWidget(label, 1, 0);
  
  borderColorButton = new ColorButton(w, borderColor);
  grid->addWidget(borderColorButton, 1, 1);
  borderColorButton->setColorButton();

  // grid color
  label = new QLabel(tr("Chart Grid"), w);
  grid->addWidget(label, 2, 0);
  
  gridColorButton = new ColorButton(w, gridColor);
  grid->addWidget(gridColorButton, 2, 1);
  gridColorButton->setColorButton();

  addTab(w, tr("Colors"));
}

void Preferences::createFontPage ()
{
  // font parms page

  QWidget *w = new QWidget(this);
  QVBoxLayout *vbox = new QVBoxLayout(w);
  
  QGridLayout *grid = new QGridLayout(vbox, 1, 2);
  grid->setMargin(5);
  grid->setSpacing(5);
  grid->setColStretch(1, 1);
  
  vbox->insertStretch(-1, 1);

  // plot font
  QLabel *label = new QLabel(tr("Plot Font"), w);
  grid->addWidget(label, 0, 0);
  
  plotFontButton = new FontButton(w, plotFont);
  grid->addWidget(plotFontButton, 0, 1);

  // app font
  label = new QLabel(tr("App Font"), w);
  grid->addWidget(label, 1, 0);
  
  appFontButton = new FontButton(w, appFont);
  grid->addWidget(appFontButton, 1, 1);

  addTab(w, tr("Fonts"));
}

void Preferences::slotSave ()
{
  Config config;

  bool tbool = menubarCheck->isChecked();
  if (tbool != menubar)
  {
    QString s = QString::number(tbool);
    config.setData(Config::Menubar, s);
    emit signalMenubar(tbool);
    menubar = tbool;
  }

  int tint = bs1Spinner->value();
  if (tint != ps1Button)
  {
    QString s = QString::number(tint);
    config.setData(Config::PS1Button, s);
    ps1Button = tint;
  }
  
  tint = bs2Spinner->value();
  if (tint != ps2Button)
  {
    QString s = QString::number(tint);
    config.setData(Config::PS2Button, s);
    ps2Button = tint;
  }
  
  tint = bs3Spinner->value();
  if (tint != ps3Button)
  {
    QString s = QString::number(tint);
    config.setData(Config::PS3Button, s);
    ps3Button = tint;
  }

  bool flag = FALSE;  
  QColor c;
  backgroundColorButton->getColor(c);
  if (c != backgroundColor)
  {
    QString s = c.name();
    config.setData(Config::BackgroundColor, s);
    emit signalBackgroundColor(c);
    flag = TRUE;
    backgroundColor = c;
  }

  borderColorButton->getColor(c);
  if (c != borderColor)
  {
    QString s = c.name();
    config.setData(Config::BorderColor, s);
    emit signalBorderColor(c);
    flag = TRUE;
    borderColor = c;
  }

  gridColorButton->getColor(c);
  if (c != gridColor)
  {
    QString s = c.name();
    config.setData(Config::GridColor, s);
    emit signalGridColor(c);
    flag = TRUE;
    gridColor = c;
  }

  QFont f;
  plotFontButton->getFont(f);
  if (f != plotFont)
  {
    QString s = f.family() + "," + QString::number(f.pointSize()) + "," + QString::number(f.weight());
    config.setData(Config::PlotFont, s);
    emit signalPlotFont(f);
    flag = TRUE;
    plotFont = f;
  }

  appFontButton->getFont(f);
  if (f != appFont)
  {
    QString s = f.family() + "," + QString::number(f.pointSize()) + "," + QString::number(f.weight());
    config.setData(Config::AppFont, s);
    emit signalAppFont(f);
    flag = TRUE;
    appFont = f;
  }

  if (flag)    
    emit signalLoadChart();
}

void Preferences::slotHelp ()
{
  HelpWindow *hw = new HelpWindow(this, helpFile);
  hw->show();
}


