// add_clock.cpp
//
// Add a Rivendell Clock
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

#include <rdpasswd.h>
#include <rdtextvalidator.h>

#include "add_clock.h"

AddClock::AddClock(QString *logname,QWidget *parent)
  : RDDialog(parent)
{
  setModal(true);

  clock_name=logname;

  setWindowTitle("RDLogManager - "+tr("Add Clock"));

  //
  // Fix the Window Size
  //
  setMinimumSize(sizeHint());
  setMaximumSize(sizeHint());

  //
  // Create Validators
  //
  RDTextValidator *validator=new RDTextValidator();
  validator->addBannedChar('(');
  validator->addBannedChar(')');
  validator->addBannedChar('!');
  validator->addBannedChar('@');
  validator->addBannedChar('#');
  validator->addBannedChar('$');
  validator->addBannedChar('%');
  validator->addBannedChar('^');
  validator->addBannedChar('&');
  validator->addBannedChar('*');
  validator->addBannedChar('{');
  validator->addBannedChar('}');
  validator->addBannedChar('[');
  validator->addBannedChar(']');
  validator->addBannedChar(':');
  validator->addBannedChar(';');
  validator->addBannedChar(34);
  validator->addBannedChar('<');
  validator->addBannedChar('>');
  validator->addBannedChar('.');
  validator->addBannedChar(',');
  validator->addBannedChar('\\');
  validator->addBannedChar('-');
  validator->addBannedChar('_');
  validator->addBannedChar('/');
  validator->addBannedChar('+');
  validator->addBannedChar('=');
  validator->addBannedChar('~');
  validator->addBannedChar('?');
  validator->addBannedChar('|');

  //
  // Clock Name
  //
  clock_name_edit=new QLineEdit(this);
  clock_name_edit->setGeometry(145,11,sizeHint().width()-155,19);
  clock_name_edit->setMaxLength(58);  // MySQL limitation!
  clock_name_edit->setValidator(validator);
  QLabel *clock_name_label=new QLabel(tr("New Clock Name:"),this);
  clock_name_label->setGeometry(10,11,130,19);
  clock_name_label->setFont(labelFont());
  clock_name_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);

  //
  //  Ok Button
  //
  QPushButton *ok_button=new QPushButton(this);
  ok_button->setGeometry(sizeHint().width()-180,sizeHint().height()-60,80,50);
  ok_button->setDefault(true);
  ok_button->setFont(buttonFont());
  ok_button->setText(tr("OK"));
  connect(ok_button,SIGNAL(clicked()),this,SLOT(okData()));

  //
  //  Cancel Button
  //
  QPushButton *cancel_button=new QPushButton(this);
  cancel_button->setGeometry(sizeHint().width()-90,sizeHint().height()-60,
			     80,50);
  cancel_button->setFont(buttonFont());
  cancel_button->setText(tr("Cancel"));
  connect(cancel_button,SIGNAL(clicked()),this,SLOT(cancelData()));

  //
  // Populate Data
  //
  clock_name_edit->setText(*clock_name);
  clock_name_edit->selectAll();
}


AddClock::~AddClock()
{
  delete clock_name_edit;
}


QSize AddClock::sizeHint() const
{
  return QSize(400,105);
} 


QSizePolicy AddClock::sizePolicy() const
{
  return QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
}


void AddClock::okData()
{
  *clock_name=clock_name_edit->text();
  done(0);
}


void AddClock::cancelData()
{
  done(-1);
}


void AddClock::closeEvent(QCloseEvent *e)
{
  cancelData();
}
