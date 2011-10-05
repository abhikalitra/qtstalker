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

#include "GridButton.h"
#include "Global.h"
#include "GlobalPlotGroup.h"

#include "../pics/grid.xpm"
#include "../pics/color.xpm"

#include <QSettings>
#include <QColorDialog>

GridButton::GridButton ()
{
  setContextMenuPolicy(Qt::CustomContextMenu);

  setIcon(QIcon(grid_xpm));
  setText(tr("Chart &Grid"));
  setStatusTip(tr("Toggle the chart grid. Right click mouse for options"));
  setToolTip(tr("Toggle the chart grid. Right click mouse for options"));
  setCheckable(TRUE);
  setShortcut(QKeySequence(Qt::CTRL + Qt::Key_G));

  QSettings settings(g_localSettings);
  setChecked(settings.value("grid", TRUE).toBool());

  _menu = new QMenu(this);
  _menu->addAction(QPixmap(color_xpm), tr("Grid &Color") + "...", this, SLOT(colorDialog()), Qt::ALT+Qt::Key_C);
  connect(this, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(contextMenu()));

  connect(this, SIGNAL(toggled(bool)), this, SLOT(changed(bool)));
}

void GridButton::changed (bool d)
{
  QSettings settings(g_localSettings);
  settings.setValue("grid", d);
  settings.sync();

  int t = d;
/*
  QHashIterator<QString, Plot *> it(g_plots);
  while (it.hasNext())
  {
    it.next();
    it.value()->setGrid(t);
  }
*/
  QList<QString> pl = g_plotGroup->plots();
  int loop = 0;
  for (; loop < pl.count(); loop++)
  {
    Plot *p = g_plotGroup->plot(pl.at(loop));
    if (! p)
      continue;
    p->setGrid(t);
  }
}

void GridButton::colorDialog ()
{
  QSettings settings(g_localSettings);
  QColor c(settings.value("grid_color", "dimgray").toString());

  QStringList l;
  l << "QtStalker" + g_session + ":" << tr("Grid Color");

  QColorDialog *dialog = new QColorDialog(c, this);
  dialog->setWindowTitle(l.join(" "));
  connect(dialog, SIGNAL(colorSelected(const QColor &)), this, SLOT(colorDialog2(QColor)));
  connect(dialog, SIGNAL(finished(int)), dialog, SLOT(deleteLater()));
  dialog->show();
}

void GridButton::colorDialog2 (QColor c)
{
  QSettings settings(g_localSettings);
  settings.setValue("grid_color", c.name());
  settings.sync();

/*
  QHashIterator<QString, Plot *> it(g_plots);
  while (it.hasNext())
  {
    it.next();
    it.value()->setGridColor(c);
  }
*/
  QList<QString> pl = g_plotGroup->plots();
  int loop = 0;
  for (; loop < pl.count(); loop++)
  {
    Plot *p = g_plotGroup->plot(pl.at(loop));
    if (! p)
      continue;
    p->setGridColor(c);
  }

}

void GridButton::contextMenu ()
{
  _menu->exec(QCursor::pos());
}
