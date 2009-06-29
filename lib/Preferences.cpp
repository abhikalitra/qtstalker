/*
 *  Qtstalker stock charter
 *
 *  Copyright (C) 2001-2008 Stefan S. Stratigakos
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
#include <QLabel>
#include <QLayout>
#include <QGridLayout>
#include <QVBoxLayout>


Preferences::Preferences (QWidget *w) : QDialog (w, 0)
{
  setWindowTitle(tr("Edit Prefs"));

  QVBoxLayout *vbox = new QVBoxLayout;
  setLayout(vbox);

  tabs = new QTabWidget;
  vbox->addWidget(tabs);

  buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel | QDialogButtonBox::Help);
  connect(buttonBox, SIGNAL(accepted()), this, SLOT(slotSave()));
  connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
//  connect(buttonBox, SIGNAL(helpRequested()), this, SLOT(slotHelp()));
  vbox->addWidget(buttonBox);
  
  loadSettings();
  createGeneralPage();
  createDatabasePage();
  createColorPage();
  createFontPage();
  createMTPage(); // main tool bar
  createCTPage(); // chart tool bar
  createETPage(); // extra tool bar
  
  resize(sz);
}

Preferences::~Preferences ()
{
  Config config;
  config.setData(Config::PrefDlgWindowSize, sz);
}

void Preferences::loadSettings ()
{
  QString s;
  Config config;

  config.getData(Config::ShowMenuBar, s);
  menubar = s.toInt();

  config.getData(Config::ShowExtraToolbar, s);
  extraToolbar = s.toInt();

  config.getData(Config::PSButton1, s);
  ps1Button = s.toInt();

  config.getData(Config::PSButton2, s);
  ps2Button = s.toInt();

  config.getData(Config::PSButton3, s);
  ps3Button = s.toInt();

  config.getData(Config::BackgroundColor, backgroundColor);
  config.getData(Config::BorderColor, borderColor);
  config.getData(Config::GridColor, gridColor);
  config.getData(Config::PlotFont, plotFont);
  config.getData(Config::AppFont, appFont);
  config.getData(Config::PrefDlgWindowSize, sz);
}

void Preferences::createGeneralPage ()
{
  // general parms page

  QWidget *w = new QWidget;

  QVBoxLayout *vbox = new QVBoxLayout;
  w->setLayout(vbox);
  
  QGridLayout *grid = new QGridLayout;
  grid->setMargin(5);
  grid->setSpacing(5);
  grid->setColumnStretch(1, 1); // stretch 2nd col a little
  grid->setColumnStretch(2, 2); // stretch outer right col more
  vbox->addLayout(grid);
  
  vbox->insertStretch(-1, 1);

  // menubar checkbox
  QLabel *label = new QLabel(tr("Main Menubar"));
  grid->addWidget(label, 0, 0);
  
  menubarCheck = new QCheckBox;
  menubarCheck->setChecked(menubar);
  connect(menubarCheck, SIGNAL(stateChanged(int)), this, SLOT(slotModified()));
  grid->addWidget(menubarCheck, 0, 1);

  // extraToolbar checkbox
  label = new QLabel(tr("Extra Toolbar"));
  grid->addWidget(label, 1, 0);
  
  extraToolbarCheck = new QCheckBox;
  extraToolbarCheck->setChecked(extraToolbar);
  connect(extraToolbarCheck, SIGNAL(stateChanged(int)), this, SLOT(slotModified()));
  grid->addWidget(extraToolbarCheck, 1, 1);

  // bar spacing 1
  label = new QLabel(tr("Bar Spacing 1"));
  grid->addWidget(label, 2, 0);
  
  bs1Spinner = new QSpinBox;
  bs1Spinner->setRange(2, 99);
  bs1Spinner->setValue(ps1Button);
  connect(bs1Spinner, SIGNAL(valueChanged(int)), this, SLOT(slotModified()));
  grid->addWidget(bs1Spinner, 2, 1);

  // bar spacing 2
  label = new QLabel(tr("Bar Spacing 2"));
  grid->addWidget(label, 3, 0);
  
  bs2Spinner = new QSpinBox;
  bs2Spinner->setRange(2, 99);
  bs2Spinner->setValue(ps2Button);
  connect(bs2Spinner, SIGNAL(valueChanged(int)), this, SLOT(slotModified()));
  grid->addWidget(bs2Spinner, 3, 1);

  // bar spacing 3
  label = new QLabel(tr("Bar Spacing 3"));
  grid->addWidget(label, 4, 0);
  
  bs3Spinner = new QSpinBox;
  bs3Spinner->setRange(2, 99);
  bs3Spinner->setValue(ps3Button);
  connect(bs3Spinner, SIGNAL(valueChanged(int)), this, SLOT(slotModified()));
  grid->addWidget(bs3Spinner, 4, 1);

  tabs->addTab(w, tr("General"));
}

void Preferences::createDatabasePage ()
{
  // database parms page

  QWidget *w = new QWidget;

  QVBoxLayout *vbox = new QVBoxLayout;
  w->setLayout(vbox);
  
  QGridLayout *grid = new QGridLayout;
  grid->setMargin(5);
  grid->setSpacing(5);
  grid->setColumnStretch(1, 1); // stretch 2nd col a little
  grid->setColumnStretch(2, 2); // stretch outer right col more
  vbox->addLayout(grid);
  
  vbox->insertStretch(-1, 1);

  QLabel *label = new QLabel(tr("Quotes Database"));
  grid->addWidget(label, 0, 0);
  
  QString s;
  QStringList l;
  Config config;
  config.getData(Config::QuotePath, s);
  if (s.length())
    l.append(s);
  dbFile = new FileButton(w, l, s);
  grid->addWidget(dbFile, 0, 1);

  tabs->addTab(w, tr("Quotes"));
}

void Preferences::createColorPage ()
{
  // colors parms page

  QWidget *w = new QWidget;

  QVBoxLayout *vbox = new QVBoxLayout;
  w->setLayout(vbox);
  
  QGridLayout *grid = new QGridLayout;
  grid->setMargin(5);
  grid->setSpacing(5);
  grid->setColumnStretch(1, 1);
  grid->setColumnStretch(2, 2);
  vbox->addLayout(grid);
  
  vbox->insertStretch(-1, 1);

  // background color
  QLabel *label = new QLabel(tr("Chart Background"));
  grid->addWidget(label, 0, 0);
  
  backgroundColorButton = new ColorButton(w, backgroundColor);
  grid->addWidget(backgroundColorButton, 0, 1);
  backgroundColorButton->setColorButton();
  connect(backgroundColorButton, SIGNAL(valueChanged()), this, SLOT(slotModified()));

  // border color
  label = new QLabel(tr("Chart Border"));
  grid->addWidget(label, 1, 0);
  
  borderColorButton = new ColorButton(w, borderColor);
  grid->addWidget(borderColorButton, 1, 1);
  borderColorButton->setColorButton();
  connect(borderColorButton, SIGNAL(valueChanged()), this, SLOT(slotModified()));

  // grid color
  label = new QLabel(tr("Chart Grid"));
  grid->addWidget(label, 2, 0);
  
  gridColorButton = new ColorButton(w, gridColor);
  grid->addWidget(gridColorButton, 2, 1);
  gridColorButton->setColorButton();
  connect(gridColorButton, SIGNAL(valueChanged()), this, SLOT(slotModified()));

  //FIXME: add adjustment possibility for prefered CO-colors.
  // in this way to add a spinbox to set the amount of colors too
  
  tabs->addTab(w, tr("Colors"));
}

void Preferences::createFontPage ()
{
  // font parms page

  QWidget *w = new QWidget;

  QVBoxLayout *vbox = new QVBoxLayout;
  w->setLayout(vbox);
  
  QGridLayout *grid = new QGridLayout;
  grid->setMargin(5);
  grid->setSpacing(5);
  grid->setColumnStretch(1, 1);
  grid->setColumnStretch(2, 2);
  vbox->addLayout(grid);
  
  vbox->insertStretch(-1, 1);

  // plot font
  QLabel *label = new QLabel(tr("Plot Font"));
  grid->addWidget(label, 0, 0);
  
  plotFontButton = new FontButton(w, plotFont);
  grid->addWidget(plotFontButton, 0, 1);
  connect(plotFontButton, SIGNAL(valueChanged()), this, SLOT(slotModified()));

  // app font
  label = new QLabel(tr("App Font"));
  grid->addWidget(label, 1, 0);
  
  appFontButton = new FontButton(w, appFont);
  grid->addWidget(appFontButton, 1, 1);
  connect(appFontButton, SIGNAL(valueChanged()), this, SLOT(slotModified()));

  tabs->addTab(w, tr("Fonts"));
}

void  Preferences::createMTPage()
{
  // main tool bar page

  QWidget *w = new QWidget;

  QVBoxLayout *vbox = new QVBoxLayout;
  w->setLayout(vbox);
  
  QGridLayout *grid = new QGridLayout; // two more cols as needed
  grid->setMargin(5);
  grid->setSpacing(5);
  grid->setColumnStretch(2, 1); // middle spacing col
  grid->setColumnStretch(5, 2); // outer right col
  vbox->addLayout(grid);
  
  vbox->insertStretch(-1, 1);
  
  int i = 0; // count rows
  int j = 0; // "count" cols
  QString s;
  
  QLabel *label = new QLabel(tr("Quit"));
  grid->addWidget(label, i, j);

  Config config;
  quitBtnCheck = new QCheckBox;
  config.getData(Config::ShowQuitBtn, s);
  quitBtnCheck->setChecked(s.toInt());
  connect(quitBtnCheck, SIGNAL(stateChanged(int)), this, SLOT(slotModified()));
  grid->addWidget(quitBtnCheck, i++, j + 1);

  label = new QLabel(tr("Preferences"));
  grid->addWidget(label, i, j);

  prefBtnCheck = new QCheckBox;
  config.getData(Config::ShowPrefBtn, s);
  prefBtnCheck->setChecked(s.toInt());
  connect(prefBtnCheck, SIGNAL(stateChanged(int)), this, SLOT(slotModified()));
  grid->addWidget(prefBtnCheck, i++, j + 1);

  label = new QLabel(tr("Side Panel"));
  grid->addWidget(label, i, j);

  sidePanelBtnCheck = new QCheckBox;
  config.getData(Config::ShowSidePanelBtn, s);
  sidePanelBtnCheck->setChecked(s.toInt());
  connect(sidePanelBtnCheck, SIGNAL(stateChanged(int)), this, SLOT(slotModified()));
  grid->addWidget(sidePanelBtnCheck, i++, j + 1);

  label = new QLabel(tr("Grid"));
  grid->addWidget(label, i, j);

  gridBtnCheck = new QCheckBox;
  config.getData(Config::ShowGridBtn, s);
  gridBtnCheck->setChecked(s.toInt());
  connect(gridBtnCheck, SIGNAL(stateChanged(int)), this, SLOT(slotModified()));
  grid->addWidget(gridBtnCheck, i++, j + 1);

  label = new QLabel(tr("Scale to Screen"));
  grid->addWidget(label, i, j);

  scaleToScreenBtnCheck = new QCheckBox;
  config.getData(Config::ShowScaleToScreenBtn, s);
  scaleToScreenBtnCheck->setChecked(s.toInt());
  connect(scaleToScreenBtnCheck, SIGNAL(stateChanged(int)), this, SLOT(slotModified()));
  grid->addWidget(scaleToScreenBtnCheck, i++, j + 1);

  label = new QLabel(tr("CrossHair"));
  grid->addWidget(label, i, j);

  crosshairBtnCheck = new QCheckBox;
  config.getData(Config::ShowCrosshairBtn, s);
  crosshairBtnCheck->setChecked(s.toInt());
  connect(crosshairBtnCheck, SIGNAL(stateChanged(int)), this, SLOT(slotModified()));
  grid->addWidget(crosshairBtnCheck, i++, j + 1);

  // now fill a second col
  i = 0;
  j = 3;
  
  label = new QLabel(tr("PaperTrade"));
  grid->addWidget(label, i, j);

  paperTradeBtnCheck = new QCheckBox;
  config.getData(Config::ShowPaperTradeBtn, s);  
  paperTradeBtnCheck->setChecked(s.toInt());
  connect(paperTradeBtnCheck, SIGNAL(stateChanged(int)), this, SLOT(slotModified()));
  grid->addWidget(paperTradeBtnCheck, i++, j + 1);

  label = new QLabel(tr("DrawMode"));
  grid->addWidget(label, i, j);

  drawModeBtnCheck= new QCheckBox;
  config.getData(Config::ShowDrawModeBtn, s);
  drawModeBtnCheck->setChecked(s.toInt());
  connect(drawModeBtnCheck, SIGNAL(stateChanged(int)), this, SLOT(slotModified()));
  grid->addWidget(drawModeBtnCheck, i++, j + 1);

  label = new QLabel(tr("NewIndicator"));
  grid->addWidget(label, i, j);

  newIndicatorBtnCheck = new QCheckBox;
  config.getData(Config::ShowNewIndicatorBtn, s);
  newIndicatorBtnCheck->setChecked(s.toInt());
  connect(newIndicatorBtnCheck, SIGNAL(stateChanged(int)), this, SLOT(slotModified()));
  grid->addWidget(newIndicatorBtnCheck, i++, j + 1);

  label = new QLabel(tr("DataWindow"));
  grid->addWidget(label, i, j);

  dataWindowBtnCheck = new QCheckBox;
  config.getData(Config::ShowDataWindowBtn, s);
  dataWindowBtnCheck->setChecked(s.toInt());
  connect(dataWindowBtnCheck, SIGNAL(stateChanged(int)), this, SLOT(slotModified()));
  grid->addWidget(dataWindowBtnCheck, i++, j + 1);

  label = new QLabel(tr("Help"));
  grid->addWidget(label, i, j);

  helpButtonCheck = new QCheckBox;
  config.getData(Config::ShowHelpButton, s);
  helpButtonCheck->setChecked(s.toInt());
  connect(helpButtonCheck, SIGNAL(stateChanged(int)), this, SLOT(slotModified()));
  grid->addWidget(helpButtonCheck, i++, j + 1);

  tabs->addTab(w, tr("MainToolbar"));
}

void  Preferences::createCTPage() 
{
  // chart tool bar page

  QWidget *w = new QWidget;

  QVBoxLayout *vbox = new QVBoxLayout;
  w->setLayout(vbox);
  
  QGridLayout *grid = new QGridLayout;
  grid->setMargin(5);
  grid->setSpacing(5);
  grid->setColumnStretch(1, 1);
  vbox->addLayout(grid);
  
  vbox->insertStretch(-1, 1);

  int i = 0; // count rows
  int j = 0; // "count" cols
  QString s;

  QLabel *label = new QLabel(tr("Compression list"));
  grid->addWidget(label, i, j);

  Config config;
  cmpsComboBoxCheck = new QCheckBox;
  config.getData(Config::ShowCmpsComboBox, s);
  cmpsComboBoxCheck->setChecked(s.toInt());
  connect(cmpsComboBoxCheck, SIGNAL(stateChanged(int)), this, SLOT(slotModified()));
  grid->addWidget(cmpsComboBoxCheck, i++, j + 1);  

  label = new QLabel(tr("Compression Monthly"));
  grid->addWidget(label, i, j);

  cmpsMtyBtnCheck = new QCheckBox;
  config.getData(Config::ShowCmpsMtyBtn, s);
  cmpsMtyBtnCheck->setChecked(s.toInt());
  connect(cmpsMtyBtnCheck, SIGNAL(stateChanged(int)), this, SLOT(slotModified()));
  grid->addWidget(cmpsMtyBtnCheck, i++, j + 1);

  label = new QLabel(tr("Compression Weekly"));
  grid->addWidget(label, i, j);

  cmpsWkyBtnCheck = new QCheckBox;
  config.getData(Config::ShowCmpsWkyBtn, s);
  cmpsWkyBtnCheck->setChecked(s.toInt());
  connect(cmpsWkyBtnCheck, SIGNAL(stateChanged(int)), this, SLOT(slotModified()));
  grid->addWidget(cmpsWkyBtnCheck, i++, j + 1);

  label = new QLabel(tr("Compression Daily"));
  grid->addWidget(label, i, j);

  cmpsDayBtnCheck = new QCheckBox;
  config.getData(Config::ShowCmpsDayBtn, s);
  cmpsDayBtnCheck->setChecked(s.toInt());
  connect(cmpsDayBtnCheck, SIGNAL(stateChanged(int)), this, SLOT(slotModified()));
  grid->addWidget(cmpsDayBtnCheck, i++, j + 1);

  label = new QLabel(tr("Compression 60 Minute"));
  grid->addWidget(label, i, j);

  cmps60BtnCheck = new QCheckBox;
  config.getData(Config::ShowCmps60Btn, s);
  cmps60BtnCheck->setChecked(s.toInt());
  connect(cmps60BtnCheck, SIGNAL(stateChanged(int)), this, SLOT(slotModified()));
  grid->addWidget(cmps60BtnCheck, i++, j + 1);

  label = new QLabel(tr("Compression 15 Minute"));
  grid->addWidget(label, i, j);

  cmps15BtnCheck = new QCheckBox;
  config.getData(Config::ShowCmps15Btn, s);
  cmps15BtnCheck->setChecked(s.toInt());
  connect(cmps15BtnCheck, SIGNAL(stateChanged(int)), this, SLOT(slotModified()));
  grid->addWidget(cmps15BtnCheck, i++, j + 1);

  label = new QLabel(tr("Compression 5 Minute"));
  grid->addWidget(label, i, j);

  cmps5BtnCheck = new QCheckBox;
  config.getData(Config::ShowCmps5Btn, s);
  cmps5BtnCheck->setChecked(s.toInt());
  connect(cmps5BtnCheck, SIGNAL(stateChanged(int)), this, SLOT(slotModified()));
  grid->addWidget(cmps5BtnCheck, i++, j + 1);

  label = new QLabel(tr("BarSpacing spinner"));
  grid->addWidget(label, i, j);

  barSpSpinboxCheck = new QCheckBox;
  config.getData(Config::ShowBarSpSpinbox, s);
  barSpSpinboxCheck->setChecked(s.toInt());
  connect(barSpSpinboxCheck, SIGNAL(stateChanged(int)), this, SLOT(slotModified()));
  grid->addWidget(barSpSpinboxCheck, i++, j + 1);

  label = new QLabel(tr("BarsToLoad field"));
  grid->addWidget(label, i, j);

  barsToLoadFieldCheck = new QCheckBox;
  config.getData(Config::ShowBarsToLoadField, s);
  barsToLoadFieldCheck->setChecked(s.toInt());
  connect(barsToLoadFieldCheck, SIGNAL(stateChanged(int)), this, SLOT(slotModified()));
  grid->addWidget(barsToLoadFieldCheck, i++, j + 1);

  label = new QLabel(tr("Pan Chart slider"));
  grid->addWidget(label, i, j);

  sliderCheck = new QCheckBox;
  config.getData(Config::ShowSlider, s);
  sliderCheck->setChecked(s.toInt());
  connect(sliderCheck, SIGNAL(stateChanged(int)), this, SLOT(slotModified()));
  grid->addWidget(sliderCheck, i++, j + 1);

  tabs->addTab(w, tr("ChartToolbar"));
}

void  Preferences::createETPage() 
{
  // extra tool bar page

  QWidget *w = new QWidget;

  QVBoxLayout *vbox = new QVBoxLayout;
  w->setLayout(vbox);
  
  QGridLayout *grid = new QGridLayout;
  grid->setMargin(5);
  grid->setSpacing(5);
  grid->setColumnStretch(1, 1);
  vbox->addLayout(grid);
  
  vbox->insertStretch(-1, 1);

  int i = 0; // count rows
  int j = 0; // "count" cols
  QString s; // temporary

  QLabel *label = new QLabel(tr("Recent charts"));
  grid->addWidget(label, i, j);

  Config config;
  recentComboBoxCheck = new QCheckBox;
  config.getData(Config::ShowRecentCharts, s);
  recentComboBoxCheck->setChecked(s.toInt());
  connect(recentComboBoxCheck, SIGNAL(stateChanged(int)), this, SLOT(slotModified()));
  grid->addWidget(recentComboBoxCheck, i++, j + 1);  

  tabs->addTab(w, tr("ExtraToolbar"));
}

void Preferences::slotSave ()
{
  Config config;

  bool tbool = menubarCheck->isChecked();
  if (tbool != menubar)
  {
    config.setData(Config::ShowMenuBar, tbool);
    emit signalMenubar(tbool);
    menubar = tbool;
  }

  tbool = extraToolbarCheck->isChecked();
  if (tbool != extraToolbar)
  {
    config.setData(Config::ShowExtraToolbar,tbool);
    emit signalExtraToolbar(tbool);
    extraToolbar = tbool;
   }

  int tint = bs1Spinner->value();
  if (tint != ps1Button)
  {
    config.setData(Config::PSButton1, tint);
    ps1Button = tint;
  }
  
  tint = bs2Spinner->value();
  if (tint != ps2Button)
  {
    config.setData(Config::PSButton2, tint);
    ps2Button = tint;
  }
  
  tint = bs3Spinner->value();
  if (tint != ps3Button)
  {
    config.setData(Config::PSButton3, tint);
    ps3Button = tint;
  }

  bool flag = FALSE;  
  QColor c;
  backgroundColorButton->getColor(c);
  if (c != backgroundColor)
  {
    config.setData(Config::BackgroundColor,c);
    emit signalBackgroundColor(c);
    flag = TRUE;
    backgroundColor = c;
  }

  borderColorButton->getColor(c);
  if (c != borderColor)
  {
    config.setData(Config::BorderColor,c);
    emit signalBorderColor(c);
    flag = TRUE;
    borderColor = c;
  }

  gridColorButton->getColor(c);
  if (c != gridColor)
  {
    config.setData(Config::GridColor,c);
    emit signalGridColor(c);
    flag = TRUE;
    gridColor = c;
  }

  QFont f;
  plotFontButton->getFont(f);
  if (f != plotFont)
  {
    config.setData(Config::PlotFont,f);
    emit signalPlotFont(f);
    flag = TRUE;
    plotFont = f;
  }

  appFontButton->getFont(f);
  if (f != appFont)
  {
    config.setData(Config::AppFont,f);
    emit signalAppFont(f);
    flag = TRUE;
    appFont = f;
  }
  
  // main tool bar settings
  // save all, anyway if changed or not, who cares?
  config.setData(Config::ShowQuitBtn, quitBtnCheck->isChecked());
  
  // prevent the user does things he will regret...
  if(menubarCheck->isChecked())
      config.setData(Config::ShowPrefBtn, prefBtnCheck->isChecked());
  else 
      config.setData(Config::ShowPrefBtn, TRUE);
      
  config.setData(Config::ShowSidePanelBtn, sidePanelBtnCheck->isChecked());
  config.setData(Config::ShowGridBtn, gridBtnCheck->isChecked());
  config.setData(Config::ShowScaleToScreenBtn, scaleToScreenBtnCheck->isChecked());
  config.setData(Config::ShowCrosshairBtn, crosshairBtnCheck->isChecked());
  config.setData(Config::ShowPaperTradeBtn, paperTradeBtnCheck->isChecked());
  config.setData(Config::ShowDrawModeBtn, drawModeBtnCheck->isChecked());
  config.setData(Config::ShowNewIndicatorBtn, newIndicatorBtnCheck->isChecked());
  config.setData(Config::ShowDataWindowBtn, dataWindowBtnCheck->isChecked());
  config.setData(Config::ShowHelpButton, helpButtonCheck->isChecked());
  // chart tool bar settings
  config.setData(Config::ShowSlider, sliderCheck->isChecked());
  config.setData(Config::ShowBarsToLoadField, barsToLoadFieldCheck->isChecked());
  config.setData(Config::ShowBarSpSpinbox, barSpSpinboxCheck->isChecked());
  config.setData(Config::ShowCmps60Btn, cmps60BtnCheck->isChecked());
  config.setData(Config::ShowCmps15Btn, cmps15BtnCheck->isChecked());
  config.setData(Config::ShowCmps5Btn, cmps5BtnCheck->isChecked());
  config.setData(Config::ShowCmpsDayBtn, cmpsDayBtnCheck->isChecked());
  config.setData(Config::ShowCmpsWkyBtn, cmpsWkyBtnCheck->isChecked());
  config.setData(Config::ShowCmpsMtyBtn, cmpsMtyBtnCheck->isChecked());
  config.setData(Config::ShowCmpsComboBox, cmpsComboBoxCheck->isChecked());

  config.setData(Config::ShowRecentCharts, recentComboBoxCheck->isChecked());

  // save database parms
  QStringList l;
  dbFile->getFile(l);
  if (l.count())
    config.setData(Config::QuotePath, l[0]);
  
  emit signalReloadToolBars();
  
  if (flag)
    emit signalLoadChart();

  reject();    
}

void Preferences::slotModified()
{
  // FIXME: The goal is to change the caption of the
  // Cancel-button "Cancel/OK" 	depending on a change
  // of any data by the user
  //qDebug("changed");
//  setCancelButton(tr("&Cancel"));
}


