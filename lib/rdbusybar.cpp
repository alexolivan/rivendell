//   rdbusybar.cpp
//
//   A 'progress bar' widget that shows busy state.
//
//   (C) Copyright 2010-2020 Fred Gleason <fredg@paravelsystems.com>
//
//   it under the terms of the GNU Library General Public License 
//   version 2 as published by the Free Software Foundation.
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

#include "rdbusybar.h"

RDBusyBar::RDBusyBar(QWidget *parent,Qt::WindowFlags f)
  : QFrame(parent,f)
{
  bar_pos=0;
  setFrameStyle(QFrame::StyledPanel|QFrame::Sunken);
  QPalette p=palette();
  p.setColor(QPalette::Active,QPalette::Background,
	     p.color(QPalette::Active,QPalette::Base));
  setPalette(p);

  bar_label=new QLabel(this);
  p.setColor(QPalette::Active,QPalette::Background,
	     p.color(QPalette::Active,QPalette::Highlight));
  bar_label->setPalette(p);
  bar_label->setStyleSheet("background-color:"+
		      p.color(QPalette::Active,QPalette::Highlight).name());
  bar_label->hide();

  bar_timer=new QTimer(this);
  connect(bar_timer,SIGNAL(timeout()),this,SLOT(strobe()));
}

void RDBusyBar::activate(bool state)
{
  if(state) {
    Update();
    bar_label->show();
    bar_timer->start(200);
  }
  else {
    bar_label->hide();
    bar_timer->stop();
  }
}


void RDBusyBar::strobe()
{
  bar_pos++;
  bar_pos=bar_pos%5;
  if(bar_label->isVisible()) {
    Update();
  }
}


void RDBusyBar::Update()
{
  int w=rect().width();
  int h=rect().height();
  int fw=frameWidth();
  bar_label->setGeometry(bar_pos*w/5+fw,fw,(w-2*fw)/5,h-2*fw);
}
