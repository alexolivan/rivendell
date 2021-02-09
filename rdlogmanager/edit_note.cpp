// edit_note.cpp
//
// Edit a Rivendell LogManager Note
//
//   (C) Copyright 2002-2019 Fred Gleason <fredg@paravelsystems.com>
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

#include "edit_note.h"

EditNote::EditNote(QString *text,QWidget *parent)
  : RDDialog(parent)
{
  edit_text=text;

  setWindowTitle("RDLogManager - "+tr("Edit Log Note"));

  //
  // Fix the Window Size
  //
  setMinimumSize(sizeHint());

  //
  // The Text Editor
  //
  edit_text_edit=new QTextEdit(this);
  edit_text_edit->setText(*edit_text);

  //
  //  OK Button
  //
  edit_ok_button=new QPushButton(this);
  edit_ok_button->setDefault(true);
  edit_ok_button->setFont(buttonFont());
  edit_ok_button->setText(tr("&OK"));
  connect(edit_ok_button,SIGNAL(clicked()),this,SLOT(okData()));

  //
  //  Cancel Button
  //
  edit_cancel_button=new QPushButton(this);
  edit_cancel_button->setFont(buttonFont());
  edit_cancel_button->setText(tr("&Cancel"));
  connect(edit_cancel_button,SIGNAL(clicked()),this,SLOT(cancelData()));
}


QSize EditNote::sizeHint() const
{
  return QSize(400,250);
} 


QSizePolicy EditNote::sizePolicy() const
{
  return QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
}


void EditNote::okData()
{
  *edit_text=edit_text_edit->text();
  done(true);
}


void EditNote::cancelData()
{
  done(false);
}


void EditNote::closeEvent(QCloseEvent *e)
{
  cancelData();
}


void EditNote::resizeEvent(QResizeEvent *e)
{
  edit_text_edit->
    setGeometry(10,10,size().width()-20,size().height()-80);
  edit_ok_button->setGeometry(size().width()-180,size().height()-60,80,50);
  edit_cancel_button->setGeometry(size().width()-90,size().height()-60,80,50);
}
