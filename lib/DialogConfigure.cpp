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

#include "DialogConfigure.h"
#include "GlobalPlotGroup.h"
#include "GlobalParent.h"
#include "WindowTitle.h"
#include "Global.h"

#include <QtDebug>
#include <QApplication>
#include <QSettings>

DialogConfigure::DialogConfigure (QWidget *p) : Dialog (p)
{
  _keySize = "configure_dialog_window_size";
  _keyPos = "configure_dialog_window_position";
  _modified = 0;

  WindowTitle wt;
  setWindowTitle(wt.title(tr("Configure"), QString()));

  createGUI();
  createGeneralPage();

  loadSettings();

  buttonStatus();
}

void DialogConfigure::createGUI ()
{
  _tabs = new QTabWidget;
  _vbox->insertWidget(0, _tabs);

  // clear room
  _message->hide();
}

void DialogConfigure::createGeneralPage ()
{
  QWidget *w = new QWidget;

  QFormLayout *form = new QFormLayout;
  form->setSpacing(5);
  form->setMargin(10);
  w->setLayout(form);

  QSettings settings(g_localSettings);

  // background color
  QColor color(settings.value("plot_background_color", "black").toString());
  _background = new ColorButton(w, color);
  _background->setToolTip(tr("Plot Background Color"));
  connect(_background, SIGNAL(valueChanged()), this, SLOT(backgroundChanged()));
  form->addRow(tr("Plot Background"), _background);

  _flags.insert("background", 0);

  // app font
  QFont font;
  QStringList l = settings.value("app_font").toString().split(",", QString::SkipEmptyParts);
  if (l.count())
    font.fromString(l.join(","));

  _appFont = new FontButton(w, font);
  _appFont->setToolTip(tr("App Font"));
  connect(_appFont, SIGNAL(valueChanged()), this, SLOT(appFontChanged()));
  form->addRow(tr("App Font"), _appFont);

  _flags.insert("appFont", 0);

  // plot font
  QFont font2;
  l = settings.value("plot_font").toString().split(",", QString::SkipEmptyParts);
  if (l.count())
    font2.fromString(l.join(","));

  _plotFont = new FontButton(w, font2);
  _plotFont->setToolTip(tr("Plot Font"));
  connect(_plotFont, SIGNAL(valueChanged()), this, SLOT(plotFontChanged()));
  form->addRow(tr("Plot Font"), _plotFont);

  _flags.insert("plotFont", 0);

  // tab position
  l.clear();
  l << tr("North") << tr("South") << tr("West") << tr("East");

  _tabPosition = new QComboBox;
  _tabPosition->setToolTip(tr("Indicator Tab Position"));
  _tabPosition->addItems(l);
  _tabPosition->setCurrentIndex(settings.value("plot_tab_position", 0).toInt());
  connect(_tabPosition, SIGNAL(currentIndexChanged(int)), this, SLOT(tabPositionChanged()));
  form->addRow(tr("Tab Position"), _tabPosition);

  _flags.insert("plotTabPosition", 0);

  // antialiasing
  _antiAlias = new QCheckBox;
  _antiAlias->setChecked(settings.value("antialias", TRUE).toBool());
  _antiAlias->setToolTip(tr("Set plot antialiasing.\nApp restart required to take effect"));
  connect(_antiAlias, SIGNAL(toggled(bool)), this, SLOT(antiAliasChanged()));
  form->addRow(tr("Antialias Plotting"), _antiAlias);

  _flags.insert("antialias", 0);

  _tabs->addTab(w, tr("General"));
}

void DialogConfigure::buttonStatus ()
{
  if (_modified)
    _okButton->setEnabled(TRUE);
  else
    _okButton->setEnabled(FALSE);
}

void DialogConfigure::done ()
{
  QSettings settings(g_localSettings);

  if (_flags.value("background"))
  {
    QColor color = _background->color();
    settings.setValue("plot_background_color", color.name());

    QList<QString> pl = g_plotGroup->plots();
    int loop = 0;
    for (; loop < pl.count(); loop++)
    {
      Plot *p = g_plotGroup->plot(pl.at(loop));
      if (! p)
        continue;
      p->setBackgroundColor(color);
    }
  }

  if (_flags.value("appFont"))
  {
    QFont font = _appFont->font();
    settings.setValue("app_font", font.toString());
    QApplication::setFont(font);
  }

  if (_flags.value("plotFont"))
  {
    QFont font = _plotFont->font();
    settings.setValue("plot_font", font.toString());

    QList<QString> pl = g_plotGroup->plots();
    int loop = 0;
    for (; loop < pl.count(); loop++)
    {
      Plot *p = g_plotGroup->plot(pl.at(loop));
      if (! p)
        continue;
      p->setFont(font);
    }
  }

  if (_flags.value("plotTabPosition"))
  {
    settings.setValue("plot_tab_position", _tabPosition->currentIndex());
    g_parent->setTabPosition(Qt::LeftDockWidgetArea, (QTabWidget::TabPosition) _tabPosition->currentIndex());
  }

  if (_flags.value("antialias"))
    settings.setValue("antialias", _antiAlias->isChecked());

  saveSettings();

  accept();
}

void DialogConfigure::backgroundChanged ()
{
  _modified++;
  _flags.insert("background", 1);
  buttonStatus();
}

void DialogConfigure::appFontChanged ()
{
  _modified++;
  _flags.insert("appFont", 1);
  buttonStatus();
}

void DialogConfigure::plotFontChanged ()
{
  _modified++;
  _flags.insert("plotFont", 1);
  buttonStatus();
}

void DialogConfigure::tabPositionChanged ()
{
  _modified++;
  _flags.insert("plotTabPosition", 1);
  buttonStatus();
}

void DialogConfigure::antiAliasChanged ()
{
  _modified++;
  _flags.insert("antialias", 1);
  buttonStatus();
}
