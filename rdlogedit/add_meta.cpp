// add_meta.cpp
//
// Add a Rivendell RDCatch Event
//
//   (C) Copyright 2002-2021 Fred Gleason <fredg@paravelsystems.com>
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License version 2 as
//   published by the Free Software Foundation.
//
//   This program is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//   GNU General Public License for more details.
//
//   You should have received a copy of the GNU General Public
//   License along with this program; if not, write to the Free Software
//   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
//

#include <QPushButton>

#include <rd.h>
#include <rdlog_line.h>

#include "add_meta.h"

AddMeta::AddMeta(QWidget *parent)
  : RDDialog(parent)
{
  setWindowTitle("RDLogEdit");

  //
  // Fix the Window Size
  //
  setMinimumSize(sizeHint());
  setMaximumSize(sizeHint());

  //
  // Title Label
  //
  QLabel *label=new QLabel(tr("Insert a:"),this);
  label->setGeometry(0,0,sizeHint().width(),30);
  label->setFont(labelFont());
  label->setAlignment(Qt::AlignCenter);

  //
  // Marker Button
  //
  QPushButton *button=new QPushButton(this);
  button->setGeometry(10,30,sizeHint().width()-20,50);
  button->setFont(buttonFont());
  button->setText(tr("Marker"));
  connect(button,SIGNAL(clicked()),this,SLOT(markerData()));

  //
  // Voice Track Button
  //
  button=new QPushButton(this);
  button->setGeometry(10,80,sizeHint().width()-20,50);
  button->setFont(buttonFont());
  button->setText(tr("Voice Track"));
  connect(button,SIGNAL(clicked()),this,SLOT(trackData()));

  //
  // Chain Button
  //
  button=new QPushButton(this);
  button->setGeometry(10,130,sizeHint().width()-20,50);
  button->setFont(buttonFont());
  button->setText(tr("Log Chain"));
  connect(button,SIGNAL(clicked()),this,SLOT(chainData()));

  //
  //  Cancel Button
  //
  button=new QPushButton(this);
  button->setGeometry(10,sizeHint().height()-60,sizeHint().width()-20,50);
  button->setFont(buttonFont());
  button->setText(tr("Cancel"));
  button->setDefault(true);
  connect(button,SIGNAL(clicked()),this,SLOT(cancelData()));
}


AddMeta::~AddMeta()
{
}


QSize AddMeta::sizeHint() const
{
  return QSize(200,260);
} 


QSizePolicy AddMeta::sizePolicy() const
{
  return QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
}


void AddMeta::closeEvent(QCloseEvent *e)
{
  cancelData();
}


void AddMeta::markerData()
{
  done(RDLogLine::Marker);
}


void AddMeta::chainData()
{
  done(RDLogLine::Chain);
}


void AddMeta::trackData()
{
  done(RDLogLine::Track);
}


void AddMeta::cancelData()
{
  done(-1);
}
