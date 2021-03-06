// rddgimport.cpp
//
// A Qt-based application for importing Dial Global CDN downloads
//
//   (C) Copyright 2012-2021 Fred Gleason <fredg@paravelsystems.com>
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

#include <errno.h>

#include <QApplication>
#include <QFileDialog>
#include <QMessageBox>
#include <QScrollBar>
#include <QTranslator>

#include <rdaudioimport.h>
#include <rdconf.h>
#include <rddatedecode.h>
#include <rddatedialog.h>
#include <rdescape_string.h>

#include "rddgimport.h"

//
// Icons
//
#include "../../icons/rivendell-22x22.xpm"

MainWidget::MainWidget(RDConfig *c,QWidget *parent)
  : RDWidget(c,parent)
{
  QString sql;
  RDSqlQuery *q;
  QString err_msg;

  dg_group=NULL;
  dg_svc=NULL;

  //
  // Create And Set Icon
  //
  setWindowIcon(QPixmap(rivendell_22x22_xpm));

  //
  // Open the Database
  //
  rda=new RDApplication("RDDgImport","rddgimport",RDDGIMPORT_USAGE,this);
  if(!rda->open(&err_msg)) {
    QMessageBox::critical(this,"RDDgImport - "+tr("Error"),err_msg);
    exit(1);
  }

  //
  // Read Command Options
  //
  for(unsigned i=0;i<rda->cmdSwitch()->keys();i++) {
    if(!rda->cmdSwitch()->processed(i)) {
      QMessageBox::critical(this,"RDDgImport - "+tr("Error"),
			    tr("Unknown command option")+": "+
			    rda->cmdSwitch()->key(i));
      exit(2);
    }
  }

  //
  // Set Window Size
  //
  setMinimumSize(sizeHint());

  SetCaption();

  //
  // Configuration Elements
  //
  connect(rda,SIGNAL(userChanged()),this,SLOT(userChangedData()));
  rda->ripc()->connectHost("localhost",RIPCD_TCP_PORT,rda->config()->password());

  //
  // Service Selector
  //
  dg_service_box=new QComboBox(this);
  connect(dg_service_box,SIGNAL(activated(int)),
	  this,SLOT(serviceActivatedData(int)));
  dg_service_label=new QLabel(tr("Service:"),this);
  dg_service_label->setFont(labelFont());
  dg_service_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);

  //
  // File Selector
  //
  dg_filename_edit=new QLineEdit(this);
  connect(dg_filename_edit,SIGNAL(textChanged(const QString &)),
	  this,SLOT(filenameChangedData(const QString &)));
  dg_filename_label=new QLabel(tr("Filename:"),this);
  dg_filename_label->setFont(labelFont());
  dg_filename_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
  dg_filename_button=new QPushButton(tr("Select"),this);
  dg_filename_button->setFont(subButtonFont());
  connect(dg_filename_button,SIGNAL(clicked()),
	  this,SLOT(filenameSelectedData()));

  //
  // Date Selector
  //
  dg_date_edit=new QDateEdit(this);
  dg_date_edit->setDisplayFormat("MM/dd/yyyy");
  dg_date_edit->setDate(QDate::currentDate());
  dg_date_label=new QLabel(tr("Date:"),this);
  dg_date_label->setFont(labelFont());
  dg_date_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
  dg_date_button=new QPushButton(tr("Select"),this);
  dg_date_button->setFont(subButtonFont());
  connect(dg_date_button,SIGNAL(clicked()),
	  this,SLOT(dateSelectedData()));

  //
  // Progress Bar
  //
  dg_bar=new RDBusyBar(this);
  dg_bar->setDisabled(true);

  //
  // Messages Area
  //
  dg_messages_text=new QTextEdit(this);
  dg_messages_text->setReadOnly(true);
  dg_messages_label=new QLabel(tr("Messages"),this);
  dg_messages_label->setFont(labelFont());
  dg_messages_label->setAlignment(Qt::AlignLeft|Qt::AlignVCenter);

  //
  // Process Button
  //
  dg_process_button=new QPushButton(tr("Process"),this);
  dg_process_button->setFont(buttonFont());
  dg_process_button->setDisabled(true);
  connect(dg_process_button,SIGNAL(clicked()),this,SLOT(processData()));

  //
  // Close Button
  //
  dg_close_button=new QPushButton(tr("Close"),this);
  dg_close_button->setFont(buttonFont());
  connect(dg_close_button,SIGNAL(clicked()),this,SLOT(quitMainWidget()));

  //
  // Load Service List
  //
  sql="select NAME,AUTOSPOT_GROUP from SERVICES order by NAME";
  q=new RDSqlQuery(sql);
  while(q->next()) {
    if(!q->value(1).toString().isEmpty()) {
      dg_service_box->
	insertItem(dg_service_box->count(),q->value(0).toString());
    }
  }
  delete q;
  if(dg_service_box->count()==0) {
    QMessageBox::information(this,tr("RDDgImport"),
			     tr("No AutoSpot-enabled services found!"));
    exit(0);
  }
  serviceActivatedData(0);
}


QSize MainWidget::sizeHint() const
{
  return QSize(400,300);
}


void MainWidget::serviceActivatedData(int index)
{
  if(dg_svc!=NULL) {
    delete dg_svc;
  }
  dg_svc=new RDSvc(dg_service_box->currentText(),rda->station(),rda->config());
  if(dg_group!=NULL) {
    delete dg_group;
  }
  dg_group=new RDGroup(dg_svc->autospotGroup());
}


void MainWidget::filenameChangedData(const QString &str)
{
  dg_process_button->setDisabled(str.isEmpty());
}


void MainWidget::filenameSelectedData()
{
  QString filename=dg_filename_edit->text();
  if(filename.isEmpty()) {
    filename=RDGetHomeDir();
  }
  filename=
    QFileDialog::getOpenFileName(this,"RDDgImport - "+tr("Open File"),filename,
                                 tr("Text Files")+" (*.txt *.TXT);;"+
				 tr("All Files")+" (*.*)");
  if(!filename.isEmpty()) {
    dg_filename_edit->setText(filename);
    filenameChangedData(filename);
  }
}


void MainWidget::dateSelectedData()
{
  QDate date=dg_date_edit->date();
  QDate now=QDate::currentDate();
  RDDateDialog *d=new RDDateDialog(now.year(),now.year()+1,this);
  if(d->exec(&date)==0) {
    dg_date_edit->setDate(date);
  }

  delete d;
}


void MainWidget::processData()
{
  ActivateBar(true);
  if(LoadEvents()) {
    if(ImportAudio()) {
      if(WriteTrafficFile()) {
	QMessageBox::information(this,tr("RDDgImport"),
				 tr("Processing Complete!"));
      }
    }
  }
  ActivateBar(false);
}


void MainWidget::userChangedData()
{
  SetCaption();
}


void MainWidget::quitMainWidget()
{
  qApp->quit();
}


void MainWidget::resizeEvent(QResizeEvent *e)
{
  dg_service_label->setGeometry(10,10,80,20);
  dg_service_box->setGeometry(95,10,size().width()-180,20);
  dg_filename_label->setGeometry(10,37,80,20);
  dg_filename_edit->setGeometry(95,37,size().width()-180,20);
  dg_filename_button->setGeometry(size().width()-70,35,60,25);
  dg_date_label->setGeometry(10,67,80,20);
  dg_date_edit->setGeometry(95,67,100,20);
  dg_date_button->setGeometry(205,65,60,25);
  dg_bar->setGeometry(10,102,size().width()-20,20);
  dg_messages_label->setGeometry(10,129,size().width()-20,20);
  dg_messages_text->setGeometry(10,151,size().width()-20,size().height()-197);
  dg_process_button->setGeometry(10,size().height()-35,70,25);
  dg_close_button->setGeometry(size().width()-60,size().height()-35,50,25);
}


void MainWidget::SetCaption()
{
  QString username=tr("[unknown]");
  username=rda->user()->name();
  setWindowTitle(tr("RDDgImport")+" v"+VERSION+" "+tr("User")+": "+username);
}


bool MainWidget::LoadEvents()
{
  FILE *f=NULL;
  char data[1024];
  QString line;
  QTime time;
  int length;
  QString isci;

  dg_events.clear();
  if((f=fopen(dg_filename_edit->text().toUtf8(),"r"))==NULL) {
    QMessageBox::warning(this,tr("RDDgImport"),
			 tr("Unable to open source file")+"["+
			 QString(strerror(errno))+"].");
    return false;
  }
  while(fgets(data,1024,f)!=NULL) {
    line=QString(data);
    time=GetTime(line.left(8));
    if(time.isValid()) {
      length=GetLength(line.mid(80,3));
      if(length>0) {
	isci=GetIsci(line.mid(10,15));
	if(!isci.isEmpty()) {
	  dg_events.push_back(new Event());
	  dg_events.back()->setTime(time);
	  switch(length) {
	  case 30000:
	    dg_events.back()->setLength(29000);
	    break;

	  case 60000:
	    dg_events.back()->setLength(58000);
	    break;

	  default:
	    LogMessage(tr("WARNING: Non-standard length for ISCI code")+
		       " \""+isci+"\".");
	    break;
	  }
	  dg_events.back()->setIsci(isci);
	  dg_events.back()->setTitle(line.mid(48,25));
	  dg_events.back()->setClient(line.mid(32,11));
	  dg_carts[isci]=0;
	}
      }
    }
  }
  fclose(f);
  return true;
}


bool MainWidget::ImportAudio()
{
  QString err_msg;

  for(std::map<QString,unsigned>::const_iterator it=dg_carts.begin();
      it!=dg_carts.end();it++) {
    Event *evt=GetEvent(it->first);
    if(!CheckSpot(evt->isci())) {
      if(!ImportSpot(evt,&err_msg)) {
	QMessageBox::warning(this,"RDgImport - "+tr("Error"),err_msg);
	return false;
      }
    }
  }
  return true;
}


bool MainWidget::WriteTrafficFile()
{
  FILE *f=NULL;
  QString outname;

  //
  // Open Output File
  //
  outname=RDDateDecode(dg_svc->importPath(RDSvc::Traffic),
		       dg_date_edit->date(),rda->station(),
		       rda->config(),dg_svc->name());
  if((f=fopen(outname.toUtf8(),"w"))==NULL) {
    LogMessage(tr("WARNING: Unable to open traffic output file")+" \""+
	       outname+"\" ["+QString(strerror(errno))+"].");
    return false;
  }

  //
  // Write Records
  //
  for(unsigned i=0;i<dg_events.size();i++) {
    Event *evt=dg_events[i];
    fprintf(f,"%s  ",evt->time().toString("hh:mm:ss").toUtf8().constData());
    fprintf(f,"%06u         ",dg_carts[evt->isci()]);
    fprintf(f,"%-34s ",evt->title().toUtf8().constData());
    if(evt->length()<600000) {
      fprintf(f,"0");
    }
    fprintf(f,"%s ",RDGetTimeLength(evt->length(),true,false).toUtf8().
	    constData());
    fprintf(f,"%-32s ",evt->isci().toUtf8().constData());
    fprintf(f,"%032u",i);
    fprintf(f,"\n");
  }

  //
  // Clean Up
  //
  fclose(f);
  return true;
}


bool MainWidget::CheckSpot(const QString &isci)
{
  QString sql;
  RDSqlQuery *q;
  RDSqlQuery *q1;
  bool ret=false;
  QDate today=QDate::currentDate();
  QDate killdate=dg_date_edit->date().addDays(RDDGIMPORT_KILLDATE_OFFSET);

  QString endDateTimeSQL = "NULL";

  if(killdate.isValid())
    endDateTimeSQL = RDCheckDateTime(QDateTime(killdate,QTime(23,59,59)),
                                    "yyyy-MM-dd hh:mm:ss");

  sql=QString("select CUT_NAME,CUTS.START_DATETIME,CUTS.END_DATETIME ")+
    "from CART left join CUTS on CART.NUMBER=CUTS.CART_NUMBER "+
    "where (CART.GROUP_NAME=\""+RDEscapeString(dg_svc->autospotGroup())+"\")&&"
    "(CUTS.ISCI=\""+RDEscapeString(isci)+"\")";
  q=new RDSqlQuery(sql);
  while(q->next()) {
    dg_carts[isci]=RDCut::cartNumber(q->value(0).toString());
    if(q->value(2).isNull()||(q->value(2).toDateTime().date()<killdate)) {
      sql="update CUTS set ";
      if(q->value(1).isNull()) {
	sql+="START_DATETIME=\""+today.toString("yyyy-MM-dd")+" 00:00:00\",";
      }
      sql+="END_DATETIME="+endDateTimeSQL+" ";
      sql+="where CUT_NAME=\""+q->value(0).toString()+"\"";
      q1=new RDSqlQuery(sql);
      delete q1;
    }
    ret=true;
  }
  delete q;

  return ret;
}


bool MainWidget::ImportSpot(Event *evt,QString *err_msg)
{
  RDCart *cart;
  RDCut *cut;
  unsigned cartnum;
  int cutnum;
  RDAudioImport *conv;
  RDAudioImport::ErrorCode conv_err;
  RDAudioConvert::ErrorCode audio_conv_err;
  RDSettings settings;
  QString dir=RDGetPathPart(dg_filename_edit->text());
  QDateTime start=QDateTime(QDate::currentDate(),QTime(0,0,0));
  QString audiofile;

  //
  // Find File
  //
  audiofile=dir+"/"+evt->isci()+"."+QString(RDDGIMPORT_FILE_EXTENSION).
    toLower();
  if(!QFile::exists(audiofile)) {
    audiofile=dir+"/"+evt->isci()+"."+
      QString(RDDGIMPORT_FILE_EXTENSION).toUpper();
    if(!QFile::exists(audiofile)) {
      LogMessage(tr("Missing audio for")+" "+evt->isci()+" ["+evt->title()+
		 " / "+evt->client()+"].");
      return false;
    }
  }

  //
  // Initialize Audio Importer
  //
  settings.setNormalizationLevel(rda->libraryConf()->ripperLevel()/100);
  settings.setChannels(rda->libraryConf()->defaultChannels());

  if((dg_carts[evt->isci()]=dg_group->nextFreeCart())==0) {
    LogMessage(tr("Unable to allocate new cart for")+" "+evt->isci()+" ["+
	       evt->title()+" / "+evt->client()+"].");
    return false;
  }
  if(evt==NULL) {
    LogMessage(tr("Unable to find event for ISCI code")+" \""+
	       evt->isci()+"\".");
    return false;
  }
  if((cartnum=RDCart::create(dg_group->name(),RDCart::Audio,err_msg,
			     dg_carts[evt->isci()]))==0) {
    return false;
  }
  cart=new RDCart(dg_carts[evt->isci()]);

  if((cutnum=cart->addCut(rda->libraryConf()->defaultLayer(),
			  rda->libraryConf()->defaultBitrate(),
			  rda->libraryConf()->defaultChannels(),
			  evt->isci(),evt->title()))<0) {
    LogMessage(tr("WARNING: Unable to create cut for cart")+" \""+
	       QString().sprintf("%u",dg_carts[evt->isci()])+"\".");
    delete cart;
    return false;
  }
  cut=new RDCut(dg_carts[evt->isci()],cutnum);
  cut->setStartDatetime(start,true);
  cut->setEndDatetime(QDateTime(dg_date_edit->date().
				addDays(RDDGIMPORT_KILLDATE_OFFSET),
				QTime(23,59,59)),true);
  
  conv=new RDAudioImport(this);
  conv->setCartNumber(dg_carts[evt->isci()]);
  conv->setCutNumber(cutnum);
  conv->setSourceFile(audiofile);
  conv->setDestinationSettings(&settings);
  conv->setUseMetadata(false);
  conv_err=conv->
    runImport(rda->user()->name(),rda->user()->password(),&audio_conv_err);
  switch(conv_err) {
  case RDAudioImport::ErrorOk:
    break;
    
  default:
    LogMessage(tr("Importer error")+" ["+audiofile+"]: "+
	       RDAudioImport::errorText(conv_err,audio_conv_err));
  }
  cart->setTitle(evt->title());
  cart->setArtist(evt->client());
  cart->setForcedLength(evt->length());
  cart->setEnforceLength(true);
  delete conv;
  delete cut;
  delete cart;

  return true;
}


void MainWidget::ActivateBar(bool state)
{
  if(state) {
    dg_messages_text->clear();
  }
  dg_bar->setEnabled(state);
  dg_bar->activate(state);
  dg_filename_edit->setDisabled(state);
  dg_filename_button->setDisabled(state);
  dg_date_edit->setDisabled(state);
  dg_date_button->setDisabled(state);
  dg_process_button->setDisabled(state);
  dg_close_button->setDisabled(state);
  qApp->processEvents();
}


Event *MainWidget::GetEvent(const QString &isci)
{
  for(unsigned i=0;i<dg_events.size();i++) {
    if(dg_events[i]->isci()==isci) {
      return dg_events[i];
    }
  }
  return NULL;
}


QTime MainWidget::GetTime(const QString &str) const
{
  QStringList fields;
  QTime ret;
  bool ok=false;

  fields=str.split(":");
  if(fields.size()==3) {
    int hour=fields[0].toInt(&ok);
    if(ok&&(hour>=0)&&(hour<=23)) {
      int minute=fields[1].toInt(&ok);
      if(ok&&(minute>=0)&&(minute<=59)) {
	int second=fields[2].toInt(&ok);
	if(ok&&(second>=0)&&(second<=59)) {
	  ret=QTime(hour,minute,second);
	}
      }
    }
  }
  return ret;
}


int MainWidget::GetLength(const QString &str) const
{
  int ret=0;
  int len;
  bool ok=false;

  if(str.left(1)==":") {
    len=str.right(2).toInt(&ok);
    if(ok&&(len>=0)&&(len<=60)) {
      ret=len*1000;
    }
  }

  return ret;
}


QString MainWidget::GetIsci(const QString &str) const
{
  QString ret;

  if(str.trimmed().length()==15) {
    ret=str.trimmed();
  }
  return ret;
}


void MainWidget::LogMessage(const QString &str)
{
  dg_messages_text->append(str+"\n");
  dg_messages_text->verticalScrollBar()->
    setValue(dg_messages_text->verticalScrollBar()->maximum());
}


int main(int argc,char *argv[])
{
  QApplication::setStyle(RD_GUI_STYLE);
  QApplication a(argc,argv);
  
  //
  // Load Translations
  //
  QString loc=RDApplication::locale();
  if(!loc.isEmpty()) {
    QTranslator qt(0);
    qt.load(QString("/usr/share/qt4/translations/qt_")+loc,
	    ".");
    a.installTranslator(&qt);

    QTranslator rd(0);
    rd.load(QString(PREFIX)+QString("/share/rivendell/librd_")+loc,".");
    a.installTranslator(&rd);

    QTranslator rdhpi(0);
    rdhpi.load(QString(PREFIX)+QString("/share/rivendell/librdhpi_")+loc,".");
    a.installTranslator(&rdhpi);

    QTranslator tr(0);
    tr.load(QString(PREFIX)+QString("/share/rivendell/rdgpimon_")+loc,".");
    a.installTranslator(&tr);
  }

  //
  // Start Event Loop
  //
  RDConfig *config=new RDConfig();
  config->load();
  MainWidget *w=new MainWidget(config);
  w->setGeometry(QRect(QPoint(0,0),w->sizeHint()));
  w->show();
  return a.exec();
}
