/*
 *  TestSettings
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



#ifndef TESTSETTINGS_H
#define TESTSETTINGS_H

#include <QLineEdit>
#include <QComboBox>
#include <QCheckBox>
#include <QDoubleSpinBox>
#include <QStringList>
#include <QString>
#include <QSpinBox>
#include <QToolButton>
#include <QGroupBox>
#include <QWidget>
#include <QTextEdit>

#include "IndicatorPlugin.h"
#include "SymbolButton.h"

class TestSettings : public QWidget
{
  Q_OBJECT

  public:
    TestSettings ();
    void getScript (QString &);
    void setScript (QString &);
    void getShellCommand (QString &);
    void setShellCommand (QString &);
    void getComment (QString &);
    void setComment (QString &);
    void getSymbol (QString &);
    void setSymbol (QString &);
    int getEnterField ();
    void setEnterField (int);
    int getExitField ();
    void setExitField (int);
    int getBars ();
    void setBars (int);
    int getBarLength ();
    void setBarLength (int);
    double getEntryComm ();
    void setEntryComm (double);
    double getExitComm ();
    void setExitComm (double);
    int getDelay ();
    void setDelay (int);
    double getAccount ();
    void setAccount (double);
    double getVolumePercentage ();
    void setVolumePercentage (double);
    double getTrailing ();
    void setTrailing (double);
    bool getTrailingCheck ();
    void setTrailingCheck (bool);

  public slots:
    void scriptButtonPressed ();

  private:
    QStringList priceList;
    QStringList barLengthList;
    QLineEdit *script;
    QLineEdit *shellCommand;
    QToolButton *scriptButton;
    SymbolButton *symbolButton;
    QComboBox *enterField;
    QComboBox *exitField;
    QCheckBox *trailingCheck;
    QDoubleSpinBox *trailing;
    QSpinBox *bars;
    QSpinBox *delay;
    QDoubleSpinBox *entryComm;
    QDoubleSpinBox *exitComm;
    QComboBox *barLength;
    QDoubleSpinBox *account;
    QDoubleSpinBox *volumePercentage;
    QTextEdit *comment;
};

#endif

