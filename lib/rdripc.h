// rdripc.h
//
// Connection to the Rivendell Interprocess Communication Daemon
//
//   (C) Copyright 2002-2004,2016-2018 Fred Gleason <fredg@paravelsystems.com>
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

#include <qsqldatabase.h>
#include <qstring.h>
#include <qobject.h>
#include <qlabel.h>
#include <qtimer.h>
#include <qtcpsocket.h>

#include <rdconfig.h>
#include <rdmacro.h>
#include <rdnotification.h>
#include <rdstation.h>

#ifndef RDRIPC_H
#define RDRIPC_H

#define RIPC_MAX_ARGS 100
#define RIPC_MAX_LENGTH 256
#define RIPC_START_DELAY 2000

class RDRipc : public QObject
{
  Q_OBJECT
 public:
 RDRipc(RDStation *station,RDConfig *config,QObject *parent=0);
  ~RDRipc();
  QString user() const;
  QString station() const;
  bool onairFlag() const;
  void setUser(QString user);
  void setIgnoreMask(bool state);
  void connectHost(QString hostname,uint16_t hostport,QString password);
  void sendGpiStatus(int matrix);
  void sendGpoStatus(int matrix);
  void sendGpiMask(int matrix);
  void sendGpoMask(int matrix);
  void sendGpiCart(int matrix);
  void sendGpoCart(int matrix);
  void sendNotification(RDNotification::Type type,
			RDNotification::Action action,const QVariant &id);
  void sendNotification(const RDNotification &notify);
  void sendOnairFlag();
  void sendRml(RDMacro *macro);
  void reloadHeartbeat();
  
 signals:
  void connected(bool state);
  void userChanged();
  void gpiStateChanged(int matrix,int line,bool state);
  void gpoStateChanged(int matrix,int line,bool state);
  void gpiMaskChanged(int matrix,int line,bool state);
  void gpoMaskChanged(int matrix,int line,bool state);
  void gpiCartChanged(int matrix,int line,int off_cartnum,int on_cartnum);
  void gpoCartChanged(int matrix,int line,int off_cartnum,int on_cartnum);
  void notificationReceived(RDNotification *notify);
  void onairFlagChanged(bool state);
  void rmlReceived(RDMacro *rml);
  
 private slots:
  void connectedData();
  void errorData(QAbstractSocket::SocketError err);
  void readyData();

 private:
  void SendCommand(const QString &cmd);
  void DispatchCommand();
  QTcpSocket *ripc_socket;
  QString ripc_user;
  QString ripc_password;
  RDStation *ripc_station;
  RDConfig *ripc_config;
  bool ripc_onair_flag;
  bool ripc_ignore_mask;
  bool debug;
  QString ripc_accum;
  bool ripc_connected;
};


#endif  // RDRIPC_H
