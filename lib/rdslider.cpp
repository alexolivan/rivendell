//   rdslider.cpp
//
//   An audio- and touchscreen-friendly slider widget.
//
//   (C) Copyright 2009-2021 Fred Gleason <fredg@paravelsystems.com>
//
//   This program is free software; you can redistribute it and/or modify
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

#include <QPainter>
#include <QMouseEvent>

#include "rdslider.h"

RDSlider::RDSlider(QWidget *parent=0)
  : QAbstractSlider(parent)
{
  init(RDSlider::Right);
  QWidget::update();
}


RDSlider::RDSlider(RDSlider::Orientation orient,QWidget *parent)
  : QAbstractSlider(parent)
{
  init(orient);
  setOrientation(orient);
}


RDSlider::RDSlider(int minValue,int maxValue,int pageStep,int value,
	   RDSlider::Orientation orient,QWidget *parent)
  : QAbstractSlider(parent)
{
  init(orient);
  setOrientation(orient);
  setRange(minValue,maxValue);
  setPageStep(pageStep);
  setValue(value);
}


RDSlider::Orientation RDSlider::orientation() const
{
  return rdslider_orient;
}


void RDSlider::setOrientation(RDSlider::Orientation orient)
{
  rdslider_orient=orient;
  update();
}


void RDSlider::setTracking(bool enable)
{
  tracking_enabled=enable;
}


bool RDSlider::tracking() const
{
  return tracking_enabled;
}


void RDSlider::setTickInterval(int i)
{
  tick_interval=i;
  update();
}


void RDSlider::setTickmarks(RDSlider::TickSetting s)
{
  tick_setting=s;
  update();
}


void RDSlider::setMinValue(int min_value)
{
  setRange(min_value,maximum());
}


void RDSlider::setMaxValue(int max_value)
{
  setRange(minimum(),max_value);
}


void RDSlider::setRange(int min_value,int max_value)
{
  QAbstractSlider::setRange(min_value,max_value);
  calcKnob();
  update(prev_knob);
  update(curr_knob);
}


void RDSlider::setLineStep(int step)
{
  line_step=step;
}


void RDSlider::setPageStep(int step)
{
  page_step=step;
}


void RDSlider::setKnobColor(const QPalette &pal)
{
  knob_color=pal;
  calcKnob();
  update();
}


void RDSlider::setKnobColor(const QColor &color)
{
  knob_color.setColor(QPalette::Background,color);
  calcKnob();
  update();
}


void RDSlider::setKnobSize(QSize size)
{
  if(size!=curr_knob.size()) {
    knob_size=size;
    calcKnob();
  }
}


void RDSlider::setKnobSize(int w,int h)
{
  setKnobSize(QSize(w,h));
}


void RDSlider::setGeometry(int x,int y,int w,int h)
{
  calcKnob(x,y,w,h);
  QWidget::setGeometry(x,y,w,h);
}


void RDSlider::setGeometry(QRect &rect)
{
  setGeometry(rect.left(),rect.top(),rect.width(),rect.height());
}


void RDSlider::setValue(int setting)
{
  if(!rdslider_moving) { 
   if(setting!=value()) {
      QAbstractSlider::setValue(setting);
      calcKnob();
      update(prev_knob);
      update(curr_knob);
    }
  }
}


void RDSlider::addStep()
{
  setValue(value()+page_step);
}


void RDSlider::subtractStep()
{
  setValue(value()-page_step);
}

   
void RDSlider::paintEvent(QPaintEvent *paintEvent)
{ 
  QPainter *p=new QPainter(this);
  int tick_x,tick_y;

  if((rdslider_orient==RDSlider::Up)||(rdslider_orient==RDSlider::Down)) {
    //
    // The groove
    //
    p->setPen(QColor(palette().dark().color()));
    p->drawLine(width()/2,knob_size.height()/2,width()/2,height()-knob_size.height()/2);
    p->setPen(QColor(palette().shadow().color()));
    p->drawLine(width()/2-1,height()-knob_size.height()/2+1,width()/2-1,knob_size.height()/2-1);
    p->drawLine(width()/2-1,knob_size.height()/2-1,width()/2+1,knob_size.height()/2-1);
    
    //
    // The ticks
    //
    switch(rdslider_orient) {
    case RDSlider::Up:
      if((tick_setting==RDSlider::TicksLeft)||(tick_setting==RDSlider::TicksBothSides)) {
	p->setPen(palette().shadow().color());
	for(int i=minimum();i<=maximum();i+=tick_interval) {
	  tick_y=(geometry().height()-curr_knob.height())-
	    ((geometry().height()-curr_knob.height())*(i-minimum()))/
	    (maximum()-minimum())+curr_knob.height()/2;
	  p->drawLine(0,tick_y,width()/2-2,tick_y);
	}
      }
      if((tick_setting==RDSlider::TicksRight)||(tick_setting==RDSlider::TicksBothSides)) {
	p->setPen(palette().shadow().color());
	for(int i=minimum();i<=maximum();i+=tick_interval) {
	  tick_y=(geometry().height()-curr_knob.height())-
	    ((geometry().height()-curr_knob.height())*(i-minimum()))/
	    (maximum()-minimum())+curr_knob.height()/2;
	  p->drawLine(width()/2+2,tick_y,width(),tick_y);
	}
      }
      break;

    case RDSlider::Down:
      if((tick_setting==RDSlider::TicksLeft)||(tick_setting==RDSlider::TicksBothSides)) {
	p->setPen(palette().shadow().color());
	for(int i=minimum();i<=maximum();i+=tick_interval) {
	  tick_y=(geometry().height()-curr_knob.height()/2)-
	    abs((geometry().height()-curr_knob.height())*i)/
	    (maximum()-minimum());
	  p->drawLine(0,tick_y,width()/2-2,tick_y);
	}
      }
      if((tick_setting==RDSlider::TicksRight)||(tick_setting==RDSlider::TicksBothSides)) {
	p->setPen(palette().shadow().color());
	for(int i=minimum();i<=maximum();i+=tick_interval) {
	  tick_y=(geometry().height()-curr_knob.height()/2)-
	    abs((geometry().height()-curr_knob.height())*i)/
	    (maximum()-minimum());
	  p->drawLine(width()/2+2,tick_y,width(),tick_y);
	}
      }
      break;

    default:
      break;
    }
  }
  if((rdslider_orient==RDSlider::Left)||(rdslider_orient==RDSlider::Right)) {
    //
    // The groove
    //
    p->setPen(QColor(palette().light().color()));
    p->drawLine(knob_size.width()/2,height()/2+1,
		width()-knob_size.width()/2,height()/2+1);
    p->setPen(QColor(palette().dark().color()));
    p->drawLine(knob_size.width()/2,height()/2,
		width()-knob_size.width()/2,height()/2);
    p->setPen(QColor(palette().shadow().color()));
    p->drawLine(width()-knob_size.width()/2+1,height()/2-1,
		knob_size.width()/2-1,height()/2-1);

    //
    // The ticks
    //
    switch(rdslider_orient) {
	case RDSlider::Left:
	  if((tick_setting==RDSlider::TicksAbove)||(tick_setting==RDSlider::TicksBothSides)) {
	    p->setPen(palette().shadow().color());
	    for(int i=minimum();i<=maximum();i+=tick_interval) {
	      tick_x=(geometry().width()-curr_knob.width())-
		((geometry().width()-curr_knob.width())*(i-minimum()))/
		(maximum()-minimum())+curr_knob.width()/2;
	      p->drawLine(tick_x,0,tick_x,height()/2-2);
	    }
	  }
	  if((tick_setting==RDSlider::TicksBelow)||(tick_setting==RDSlider::TicksBothSides)) {
	    p->setPen(palette().shadow().color());
	    for(int i=minimum();i<=maximum();i+=tick_interval) {
	      tick_x=(geometry().width()-curr_knob.width())-
		((geometry().width()-curr_knob.width())*(i-minimum()))/
		(maximum()-minimum())+curr_knob.width()/2;
	      p->drawLine(tick_x,height()/2+2,tick_x,height());
	    }
	  }
	  break;
	case RDSlider::Right:
	  if((tick_setting==RDSlider::TicksAbove)||(tick_setting==RDSlider::TicksBothSides)) {
	    p->setPen(palette().shadow().color());
	    for(int i=minimum();i<=maximum();i+=tick_interval) {
	      tick_x=(geometry().width()-curr_knob.width()/2)-
		abs((geometry().width()-curr_knob.width())*i)/
		(maximum()-minimum());
	      p->drawLine(tick_x,0,tick_x,height()/2-2);
	    }
	  }
	  if((tick_setting==RDSlider::TicksBelow)||(tick_setting==RDSlider::TicksBothSides)) {
	    p->setPen(palette().shadow().color());
	    for(int i=minimum();i<=maximum();i+=tick_interval) {
	      tick_x=(geometry().width()-curr_knob.width()/2)-
		abs((geometry().width()-curr_knob.width())*i)/
		(maximum()-minimum());
	      p->drawLine(tick_x,height()/2+2,tick_x,height());
	    }
	  }
	  break;

	default:
	  break;
    }
  }    

  //
  // The knob
  //
  p->drawPixmap(curr_knob.x(),curr_knob.y(),*knob_map);

  p->end();
}


void RDSlider::mouseMoveEvent(QMouseEvent *mouse)
{
  int curr_x,curr_y;
  int knob_value;

  if(rdslider_moving) {
    prev_knob=curr_knob;
    if((rdslider_orient==RDSlider::Up)||(rdslider_orient==RDSlider::Down)) {
      if(mouse->y()<0) {
	curr_knob=QRect(curr_knob.x(),0,curr_knob.width(),curr_knob.height());
	if(prev_knob!=curr_knob) {
	  switch(rdslider_orient) {
	      case RDSlider::Up:
		if(value()!=maximum()) {
		  QAbstractSlider::setValue(maximum());
		  if(tracking_enabled) {
		    emit valueChanged(value());
		  }
		  else {
		    deferred_change=true;
		  }
		}
		break;
	      case RDSlider::Down:
		if(value()!=minimum()) {
		  QAbstractSlider::setValue(minimum());
		  if(tracking_enabled) {
		    emit valueChanged(value());
		  }
		  else {
		    deferred_change=true;
		  }
		}
		break;

	      default:
		break;
	  }
	  emit sliderMoved(value());
	  update(prev_knob);
	  update(curr_knob);
	}
	return;
      }
      if(mouse->y()>geometry().height()) {
	curr_knob=QRect(curr_knob.x(),height()-curr_knob.height(),
			curr_knob.width(),curr_knob.height());
	if(prev_knob!=curr_knob) {
	  switch(rdslider_orient) {
	      case RDSlider::Up:
		if(value()!=minimum()) {
		  QAbstractSlider::setValue(minimum());
		  if(tracking_enabled) {
		    emit valueChanged(value());
		  }
		  else {
		    deferred_change=true;
		  }
		}
		break;
	      case RDSlider::Down:
		if(value()!=maximum()) {
		  QAbstractSlider::setValue(maximum());
		  if(tracking_enabled) {
		    emit valueChanged(value());
		  }
		  else {
		    deferred_change=true;
		  }
		}
		break;

	      default:
		break;
	  }
	  emit sliderMoved(value());
	  update(prev_knob);
	  update(curr_knob);
	}
	return;
      }
      curr_y=curr_knob.y()-base_y+mouse->y();
      if(curr_y<0) {
	curr_y=0;
      }
      if(curr_y>(height()-curr_knob.height())) {
	curr_y=height()-curr_knob.height();
      }
      curr_knob=QRect(curr_knob.x(),curr_y,
		      curr_knob.width(),curr_knob.height());
      base_y=mouse->y();
      switch(rdslider_orient) {
	  case RDSlider::Up:
	    knob_value=(maximum()-minimum())*
				    (geometry().height()-curr_knob.y()
				     -curr_knob.height())/
				    (geometry().height()-curr_knob.height())
				    +minimum();
	    if(value()!=knob_value) {
	      QAbstractSlider::setValue(knob_value);
	      if(tracking_enabled) {
		emit valueChanged(value());
	      }
	      else {
		deferred_change=true;
	      }
	    }
	    break;
	  case RDSlider::Down:
	    knob_value=((minimum()-maximum())*
				     curr_knob.y()
				     +curr_knob.height()-geometry().height())/
				    (curr_knob.height()-geometry().height())
				    +minimum();
	    if(value()!=knob_value) {
	      QAbstractSlider::setValue(knob_value);
	      if(tracking_enabled) {
		emit valueChanged(value());
	      }
	      else {
		deferred_change=true;
	      }
	    }	    
	    break;

	  default:
	    break;
      }
      emit sliderMoved(value());
      update(prev_knob);
      update(curr_knob);
    }

    if((rdslider_orient==RDSlider::Left)||(rdslider_orient==RDSlider::Right)) {
      if(mouse->x()<0) {
	curr_knob=QRect(0,curr_knob.y(),curr_knob.width(),curr_knob.height());
	if(prev_knob!=curr_knob) {
	  switch(rdslider_orient) {
	      case RDSlider::Left:
		if(value()!=maximum()) {
		  QAbstractSlider::setValue(maximum());
		  if(tracking_enabled) {
		    emit valueChanged(value());
		  }
		  else {
		    deferred_change=true;
		  }
		}
		break;
	      case RDSlider::Right:
		if(value()!=minimum()) {
		  QAbstractSlider::setValue(minimum());
		  if(tracking_enabled) {
		    emit valueChanged(value());
		  }
		  else {
		    deferred_change=true;
		  }
		}
		break;

	      default:
		break;
	  }
	  emit sliderMoved(value());
	  update(prev_knob);
	  update(curr_knob);
	}
	return;
      }
      if(mouse->x()>geometry().width()) {
	curr_knob=QRect(width()-curr_knob.width(),curr_knob.y(),
			curr_knob.width(),curr_knob.height());
	if(prev_knob!=curr_knob) {
	  switch(rdslider_orient) {
	      case RDSlider::Left:
		if(value()!=minimum()) {
		  QAbstractSlider::setValue(minimum());
		  if(tracking_enabled) {
		    emit valueChanged(value());
		  }
		  else {
		    deferred_change=true;
		  }
		}
		break;
	      case RDSlider::Right:
		if(value()!=maximum()) {
		  QAbstractSlider::setValue(maximum());
		  if(tracking_enabled) {
		    emit valueChanged(value());
		  }
		  else {
		    deferred_change=true;
		  }
		}
		break;

	      default:
		break;
	  }
	  emit sliderMoved(value());
	  update(prev_knob);
	  update(curr_knob);
	}
	return;
      }
      curr_x=curr_knob.x()-base_x+mouse->x();
      if(curr_x<0) {
	curr_x=0;
      }
      if(curr_x>(width()-curr_knob.width())) {
	curr_x=width()-curr_knob.width();
      }
      curr_knob=QRect(curr_x,curr_knob.y(),
		      curr_knob.width(),curr_knob.height());
      base_x=mouse->x();
      switch(rdslider_orient) {
	  case RDSlider::Left:
	    knob_value=(maximum()-minimum())*
				    (geometry().width()-curr_knob.x()
				     -curr_knob.width())/
				    (geometry().width()-curr_knob.width())
				    +minimum();
	    if(value()!=knob_value) {
	      QAbstractSlider::setValue(knob_value);
	      if(tracking_enabled) {
		emit valueChanged(value());
	      }
	      else {
		deferred_change=true;
	      }
	    }
	    break;
	  case RDSlider::Right:
	    knob_value=((minimum()-maximum())*curr_knob.x()
			+curr_knob.width()-geometry().width())/
	      (curr_knob.width()-geometry().width())
	      +minimum();
	    if(value()!=knob_value) {
	      QAbstractSlider::setValue(knob_value);
	      if(tracking_enabled) {
		emit valueChanged(value());
	      }
	      else {
		deferred_change=true;
	      }
	    }
	    break;

	  default:
	    break;
      }
      emit sliderMoved(value());
      update(prev_knob);
      update(curr_knob);
    }
  }
}


void RDSlider::mousePressEvent(QMouseEvent *mouse)
{
  if(mouse->button()==Qt::LeftButton) {
    if(curr_knob.contains(mouse->x(),mouse->y())) {
      base_x=mouse->x();
      base_y=mouse->y();
      rdslider_moving=true;
      emit sliderPressed();
      return;
    }
    if(page_up.contains(mouse->x(),mouse->y())) {
      addStep();
      return;
    }
    if(page_down.contains(mouse->x(),mouse->y())) {
      subtractStep();
    }
  }
  else {
    QWidget::mousePressEvent(mouse);
  }
}


void RDSlider::mouseReleaseEvent(QMouseEvent *mouse)
{
  if(mouse->button()==Qt::LeftButton) {
    rdslider_moving=false;
    if(deferred_change) {
      emit valueChanged(value());
      deferred_change=false;
    }
    emit sliderReleased();
  }
}


void RDSlider::init(RDSlider::Orientation orient)
{
  rdslider_orient=orient;
  knob_map=NULL;
  knob_color=palette();
  rdslider_moving=false;
  page_step=10;
  line_step=1;
  tracking_enabled=true;
  deferred_change=false;
  tick_interval=10;
  tick_setting=RDSlider::NoTicks;
  setRange(0,100);
  knob_size=QSize(geometry().width(),10);
}


void RDSlider::calcKnob()
{
  calcKnob(geometry().x(),geometry().y(),
	   geometry().width(),geometry().height());
}


void RDSlider::calcKnob(int x,int y,int w,int h)
{
  int knob_x=0;
  int knob_y=0;
  int knob_w=0;
  int knob_h=0;
  QPainter p;

  if((rdslider_orient==RDSlider::Up)||(rdslider_orient==RDSlider::Down)) {
    //
    // The knob itself
    //
    knob_w=knob_size.width();
    knob_h=knob_size.height();
    knob_x=w/2-knob_w/2;
    switch(rdslider_orient) {
	case RDSlider::Up:
	  knob_y=(h-knob_h)-((h-knob_h)*(value()-minimum()))/
	    (maximum()-minimum());
	  break;
	case RDSlider::Down:
	  knob_y=-(h-knob_h-((h-knob_h)*(value()-minimum())))/
	    (maximum()-minimum());
	  break;

	default:
	  break;
    }
    prev_knob=curr_knob;
    if((knob_w>0)&&(knob_h>0)) {
      curr_knob=QRect(knob_x,knob_y,knob_w,knob_h);
    }
    else {
      curr_knob=QRect(knob_x,knob_y,1,1);
    }
    if(prev_knob!=curr_knob) {
      emit sliderMoved(value());
    }
    
    //
    // The PageUp/PageDown areas
    //
    if(rdslider_orient==RDSlider::Up) {
      page_up=QRect(knob_x,0,knob_w,knob_y);
      page_down=QRect(knob_x,knob_y+knob_h,knob_w,height()-(knob_y+knob_h));
    }
    if(rdslider_orient==RDSlider::Down) {
      page_down=QRect(knob_x,0,knob_w,knob_y);
      page_up=QRect(knob_x,knob_y+knob_h,knob_w,height()-(knob_y+knob_h));
    }
    
    //
    // The knob cap
    //
    if(knob_map!=NULL) {
      delete knob_map;
    }
    knob_map=new QPixmap(curr_knob.size());
    knob_map->fill(knob_color.color(QPalette::Background));
    p.begin(knob_map);
    
    //
    // The outline
    //
    p.setPen(knob_color.light().color());
    p.drawLine(0,curr_knob.height()-1,0,0);
    p.drawLine(0,0,curr_knob.width()-1,0);
    p.drawLine(1,curr_knob.height()-1,1,1);
    p.drawLine(1,1,curr_knob.width()-1,1);
    p.setPen(knob_color.dark().color());
    p.drawLine(curr_knob.width()-1,1,curr_knob.width()-1,curr_knob.height()-1);
    p.drawLine(curr_knob.width()-1,curr_knob.height()-1,0,curr_knob.height()-1);
    p.drawLine(curr_knob.width()-2,1,curr_knob.width()-2,curr_knob.height()-2);
    p.drawLine(curr_knob.width()-2,curr_knob.height()-2,0,curr_knob.height()-2);
    
    //
    // The centerline
    //
    if(curr_knob.height()>=40) {
      p.setPen(knob_color.dark().color());
      p.drawLine(2,curr_knob.height()/2,curr_knob.width()-2,curr_knob.height()/2);
      p.setPen(knob_color.shadow().color());
      p.drawLine(2,curr_knob.height()/2-1,
		 curr_knob.width()-2,curr_knob.height()/2-1);
      p.setPen(knob_color.light().color());
      p.drawLine(2,curr_knob.height()/2+1,
		 curr_knob.width()-2,curr_knob.height()/2+1);
    }
    p.end();
  }
  if((rdslider_orient==RDSlider::Left)||(rdslider_orient==RDSlider::Right)) {
    //
    // The knob itself
    //
    knob_w=knob_size.width();
    knob_h=knob_size.height();
    knob_y=h/2-knob_h/2;
    switch(rdslider_orient) {
	case RDSlider::Left:
	  knob_x=(w-knob_w)-((w-knob_w)*(value()-minimum()))/
	    (maximum()-minimum());
	  break;
	case RDSlider::Right:
	  knob_x=-(w-knob_w-((w-knob_w)*(value()-minimum())))/
	    (maximum()-minimum());
	  break;

	default:
	  break;
    }
    prev_knob=curr_knob;
    if((knob_w>0)&&(knob_h>0)) {
      curr_knob=QRect(knob_x,knob_y,knob_w,knob_h);
    }
    else {
      curr_knob=QRect(knob_x,knob_y,1,1);
    }
    
    //
    // The PageUp/PageDown areas
    //
    if(rdslider_orient==RDSlider::Right) {
      page_up=QRect(knob_x+knob_w,knob_y,x+w,knob_h);
      page_down=QRect(0,knob_y,knob_x,knob_y+knob_h);
    }
    if(rdslider_orient==RDSlider::Left) {
      page_down=QRect(knob_x+knob_w,knob_y,x+w,knob_h);
      page_up=QRect(0,knob_y,knob_x,knob_y+knob_h);
    }

    //
    // The knob cap
    //
    if(knob_map!=NULL) {
      delete knob_map;
    }
    knob_map=new QPixmap(curr_knob.size());
    knob_map->fill(knob_color.color(QPalette::Background));
    p.begin(knob_map);
    
    //
    // The outline
    //
    p.setPen(knob_color.light().color());
    p.drawLine(0,curr_knob.height()-1,0,0);
    p.drawLine(0,0,curr_knob.width()-1,0);
    p.drawLine(1,curr_knob.height()-1,1,1);
    p.drawLine(1,1,curr_knob.width()-1,1);
    p.setPen(knob_color.dark().color());
    p.drawLine(curr_knob.width()-1,1,curr_knob.width()-1,curr_knob.height()-1);
    p.drawLine(curr_knob.width()-1,curr_knob.height()-1,0,curr_knob.height()-1);
    p.drawLine(curr_knob.width()-2,1,curr_knob.width()-2,curr_knob.height()-2);
    p.drawLine(curr_knob.width()-2,curr_knob.height()-2,0,curr_knob.height()-2);
    
    //
    // The centerline
    //
    if(curr_knob.width()>=40) {
      p.setPen(knob_color.dark().color());
      p.drawLine(curr_knob.width()/2,2,
		 curr_knob.width()/2,curr_knob.height()-2);
      p.setPen(knob_color.shadow().color());
      p.drawLine(curr_knob.width()/2-1,2,
		 curr_knob.width()/2-1,curr_knob.height()-2);
      p.setPen(knob_color.light().color());
      p.drawLine(curr_knob.width()/2+1,2,
		 curr_knob.width()/2+1,curr_knob.height()-2);
    }
  p.end();
  }
}

