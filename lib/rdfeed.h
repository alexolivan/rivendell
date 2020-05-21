// rdfeed.h
//
// Abstract a Rivendell RSS Feed
//
//   (C) Copyright 2002-2020 Fred Gleason <fredg@paravelsystems.com>
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

#ifndef RDFEED_H
#define RDFEED_H

#include <qobject.h>

#include <rdapplication.h>
#include <rdconfig.h>
#include <rdrssschemas.h>
#include <rdsettings.h>
#include <rdstation.h>
#include <rduser.h>
#include <rdweb.h>

#define RDFEED_TOTAL_POST_STEPS 4

class RDFeed : public QObject
{
  Q_OBJECT;
 public:
  enum Error {ErrorOk=0,ErrorNoFile=1,ErrorCannotOpenFile=2,
	      ErrorUnsupportedType=3,ErrorUploadFailed=4,ErrorGeneral=5};
  enum MediaLinkMode {LinkNone=0,LinkDirect=1,LinkCounted=2};
  RDFeed(const QString &keyname,RDConfig *config,QObject *parent=0);
  RDFeed(unsigned id,RDConfig *config,QObject *parent=0);
  ~RDFeed();
  QString keyName() const;
  unsigned id() const;
  bool exists() const;
  bool isSuperfeed() const;
  void setIsSuperfeed(bool state) const;
  QStringList subfeedNames() const;
  QStringList isSubfeedOf() const;
  bool audienceMetrics() const;
  void setAudienceMetrics(bool state);
  QString channelTitle() const;
  void setChannelTitle(const QString &str) const;
  QString channelDescription() const;
  void setChannelDescription(const QString &str) const;
  QString channelCategory() const;
  void setChannelCategory(const QString &str) const;
  QString channelLink() const;
  void setChannelLink(const QString &str) const;
  QString channelCopyright() const;
  void setChannelCopyright(const QString &str) const;
  QString channelEditor() const;
  void setChannelEditor(const QString &str) const;
  QString channelAuthor() const;
  void setChannelAuthor(const QString &str) const;
  QString channelOwnerName() const;
  void setChannelOwnerName(const QString &str) const;
  QString channelOwnerEmail() const;
  void setChannelOwnerEmail(const QString &str) const;
  QString channelWebmaster() const;
  void setChannelWebmaster(const QString &str) const;
  QString channelLanguage() const;
  void setChannelLanguage(const QString &str);
  bool channelExplicit() const;
  void setChannelExplicit(bool state) const;
  int channelImageId() const;
  void setChannelImageId(int img_id) const;
  int defaultItemImageId() const;
  void setDefaultItemImageId(int img_id) const;
  QString baseUrl(const QString &subfeed_key_name) const;
  QString baseUrl(int subfeed_feed_id) const;
  void setBaseUrl(const QString &str) const;
  QString basePreamble() const;
  void setBasePreamble(const QString &str) const;
  QString purgeUrl() const;
  void setPurgeUrl(const QString &str) const;
  QString purgeUsername() const;
  void setPurgeUsername(const QString &str) const;
  QString purgePassword() const;
  void setPurgePassword(const QString &str) const;
  RDRssSchemas::RssSchema rssSchema() const;
  void setRssSchema(RDRssSchemas::RssSchema schema) const;
  QString headerXml() const;
  void setHeaderXml(const QString &str);
  QString channelXml() const;
  void setChannelXml(const QString &str);
  QString itemXml() const;
  void setItemXml(const QString &str);
  QString feedUrl() const;
  bool castOrder() const;
  void setCastOrder(bool state) const;
  int maxShelfLife() const;
  void setMaxShelfLife(int days);
  QDateTime lastBuildDateTime() const;
  void setLastBuildDateTime(const QDateTime &datetime) const;
  QDateTime originDateTime() const;
  void setOriginDateTime(const QDateTime &datetime) const;
  bool enableAutopost() const;
  void setEnableAutopost(bool state) const;
  bool keepMetadata() const;
  void setKeepMetadata(bool state);
  RDSettings::Format uploadFormat() const;
  void setUploadFormat(RDSettings::Format fmt) const;
  int uploadChannels() const;
  void setUploadChannels(int chans) const;
  int uploadQuality() const;
  void setUploadQuality(int qual) const;
  int uploadBitRate() const;
  void setUploadBitRate(int rate) const;
  int uploadSampleRate() const;
  void setUploadSampleRate(int rate) const;
  QString uploadExtension() const;
  void setUploadExtension(const QString &str);
  QString uploadMimetype() const;
  void setUploadMimetype(const QString &str);
  int normalizeLevel() const;
  void setNormalizeLevel(int lvl) const;
  QString redirectPath() const;
  void setRedirectPath(const QString &str);
  RDFeed::MediaLinkMode mediaLinkMode() const;
  void setMediaLinkMode(RDFeed::MediaLinkMode mode) const;
  int importImageFile(const QString &pathname,QString *err_msg,
		      QString desc="") const;
  bool deleteImage(int img_id,QString *err_msg);
  QString audioUrl(RDFeed::MediaLinkMode mode,const QString &cgi_hostname,
		   unsigned cast_id);
  QString imageUrl(int img_id) const;
  bool postXml(QString *err_msg);
  bool postXmlConditional(const QString &caption,QWidget *widget);
  bool deleteXml(QString *err_msg);
  bool deleteImages(QString *err_msg);
  unsigned postCut(RDUser *user,RDStation *station,
		   const QString &cutname,Error *err,bool log_debug,
		   RDConfig *config);
  unsigned postFile(RDUser *user,RDStation *station,const QString &srcfile,
		    Error *err,bool log_debug,RDConfig *config);
  int totalPostSteps() const;
  QString rssXml(QString *err_msg,bool *ok=NULL);
  RDRssSchemas *rssSchemas() const;
  static unsigned create(const QString &keyname,bool enable_users,
			 QString *err_msg,const QString &exemplar="");
  static QString errorString(RDFeed::Error err);
  static QString imageFilename(int feed_id,int img_id,const QString &ext);

 signals:
  void postProgressChanged(int step);

 private:
  unsigned CreateCast(QString *filename,int bytes,int msecs) const;
  QString ResolveChannelWildcards(const QString &tmplt,RDSqlQuery *chan_q);
  QString ResolveItemWildcards(const QString &tmplt,RDSqlQuery *item_q,
			       RDSqlQuery *chan_q);
  QString GetTempFilename() const;
  void LoadSchemas();
  void SetRow(const QString &param,int value) const;
  void SetRow(const QString &param,const QString &value) const;
  void SetRow(const QString &param,const QDateTime &value,
              const QString &format) const;
  QString feed_keyname;
  unsigned feed_id;
  QString feed_cgi_hostname;
  RDConfig *feed_config;
  QByteArray feed_xml;
  int feed_xml_ptr;
  RDRssSchemas *feed_schemas;
  friend size_t __RDFeed_Readfunction_Callback(char *buffer,size_t size,
					       size_t nitems,void *userdata);
};


#endif  // RDFEED_H
