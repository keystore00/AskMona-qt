#include "topiclist.h"
#include <QLabel>
#include <QTextBrowser>
#include <QTextEdit>
#include <QVBoxLayout>
#include <QCoreApplication>
#include <QDir>
#include <QJsonArray>
#include <QJsonDocument>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QComboBox>
#include <QPushButton>
#include <QScrollBar>
#include <QSpinBox>
#include <QApplication>
#include <queue>
#include <ctime>
#include "constants.h"
#include "topicview.h"

namespace {
  enum sort_key {k_modified,k_res,k_MONA};
  time_t last_reload;
  std::map<QString,enum sort_key> sort_key_map = {{"Default",k_modified},{"Res.",k_res},{"MONA",k_MONA}};
  QString fname = "/dat/topic_list.json";
}
TopicList::TopicList(QWidget *parent) :
  QWidget(parent)
{
  setWidgets();
  nam = new QNetworkAccessManager(this);
  QObject::connect(nam, SIGNAL(finished(QNetworkReply*)),this, SLOT(finishedSlot(QNetworkReply*)));

  loadTopics(QCoreApplication::applicationDirPath()+fname);
}

TopicList::~TopicList()
{
  saveTopics(QCoreApplication::applicationDirPath()+fname);
}

void TopicList::loadTopics(const QString& filename)
{
  QFile jsonFile(filename);
  if (!jsonFile.open(QIODevice::ReadOnly)) {
    qWarning("Couldn't read topic list file.");
    return;
  }
  auto doc = QJsonDocument::fromJson(jsonFile.readAll());
  auto array = doc.array();
  for (const auto &t : array) {
    auto obj = t.toObject();
    topics[obj["t_id"].toInt()] = TopicPair(obj,obj);
  }
}
void TopicList::saveTopics(const QString& filename)
{
  QJsonArray ja;
  for (const auto &t:topics) {
    ja.push_back(t.second.first);
  }
  QFile jsonFile(filename);
  if (!jsonFile.open(QIODevice::WriteOnly)) {
    qWarning("Couldn't write topic list file.");
    return;
  }
  QJsonDocument doc(ja);
  jsonFile.write(doc.toJson());
}

void TopicList::setWidgets()
{
  auto layout = new QVBoxLayout();
  auto hlayout = new QHBoxLayout();
  QIcon reload(":/img/reload.png");
  reload.addPixmap(QPixmap(":/img/reload_disabled.png"),QIcon::Disabled);
  auto pb = new QPushButton(reload,"",this);
  pb->setToolTip("Update topic information for last N topics.");
  connect(pb,SIGNAL(clicked()),this,SLOT(update()));
  hlayout->addWidget(pb);
  cbox = new QComboBox(this);
  cbox->addItem("Default");
  cbox->setItemData(0, "Default order in Ask Mona.", Qt::ToolTipRole);
  cbox->addItem("Res.");
  cbox->setItemData(1, "Descending order by response count.", Qt::ToolTipRole);
  cbox->addItem("MONA");
  cbox->setItemData(2, "Descending order by MONA received.", Qt::ToolTipRole);
  connect(cbox,SIGNAL(currentIndexChanged(int)),this,SLOT(comboBoxChanged(int)));
  hlayout->addWidget(cbox);
  num_update_box = new QSpinBox(this);
  num_update_box->setMinimum(50);
  num_update_box->setMaximum(200);
  num_update_box->setAlignment(Qt::AlignRight);
  num_update_box->setSingleStep(50);
  num_update_box->setValue(50);
  num_update_box->setToolTip("# of topics to be updated.");
  hlayout->addWidget(num_update_box);
  layout->addLayout(hlayout);
  text_area = new TopicView(this);
  text_area->setContextMenuPolicy(Qt::CustomContextMenu);
  text_area->page()->setLinkDelegationPolicy(QWebPage::DelegateAllLinks);
  connect(text_area,SIGNAL(linkClicked(QUrl)),this,SLOT(linkClicked(QUrl)));
  layout->addWidget(text_area);
  pb = new QPushButton("Mark all as read",this);
  connect(pb,SIGNAL(clicked()),this,SLOT(mark_all_as_read()));
  layout->addWidget(pb);
  setLayout(layout);
}

void TopicList::addTopic(const QString& t_id)
{
  QNetworkRequest request(QUrl(ask_res_api+"?t_id="+t_id+"&"+"topic_detail=1"));
  nam->get(request);
}
void TopicList::update_view()
{
  //sort topics
  auto key = sort_key_map[cbox->currentText()];
  typedef unsigned long long KeyValueType;
  std::multimap <KeyValueType,TopicPair*> ordered;
  for (auto it = topics.begin(); it != topics.end(); ++it) {
    auto &last = it->second.first;
    auto &current = it->second.second;
    KeyValueType key_value = 0;
    switch (key) {
    case k_modified:
      key_value = current["modified"].toInt();
      break;
    case k_res:
      key_value = current["count"].toInt() - last["count"].toInt();
      break;
    case k_MONA:
      key_value = current["receive"].toString().toULongLong() - last["receive"].toString().toULongLong();
      break;
    default:
      key_value = current["t_id"].toInt();
      break;
    }
    key_value = std::numeric_limits<KeyValueType>::max()-key_value; //descending order
    ordered.insert(std::pair<KeyValueType, TopicPair*>(key_value,&(it->second)));
  }
  //do updating view
  text_area->set(ordered);
}

void TopicList::linkClicked(const QUrl& url)
{
  auto link = url.toString();
  if (link.startsWith("t_")) {
    emit topicClicked(link.mid(2));
    return;
  }
  if (link.startsWith("x_")) {
    topics.erase(link.mid(2).toInt());
    update_view();
    return;
  }
}

void TopicList::update()
{
  if (last_reload - time((time_t)NULL)) {
    QNetworkRequest request(QUrl(ask_topic_api+"?limit="+QString::number(num_update_box->value())));
    nam->get(request);
    last_reload = time((time_t)NULL);    
  } else {
    qWarning() << "Last update is less than 1 sec. ago.";
  }
}
void TopicList::finishedSlot(QNetworkReply* reply)
{
  QJsonDocument d = QJsonDocument::fromJson(reply->readAll());
  auto res = d.object();
  if (res["status"]!=1) {
    qWarning() << "Failed to update topic list.";
    return;
  } else {
    if (res.contains("topics")) { //update topic list
      auto array = res["topics"].toArray();
      for (auto t : array) {
	auto t_obj = t.toObject();
	auto t_id = t_obj["t_id"].toInt();
	auto it = topics.find(t_id);
	if (it != topics.end()) {
	  it->second.second=t_obj;
	}
      }
    } else if (res.contains("topic")) { //add topic
      auto topic_obj = res["topic"].toObject();
      topics[topic_obj["t_id"].toInt()] = TopicPair(topic_obj,topic_obj);
      update_view();
    }
  }
  update_view();
}
void TopicList::comboBoxChanged(int)
{
  update_view();
}
void TopicList::handleMouseGesture(const QString& g)
{
  if (!underMouse()) {
    return;
  }
  if (g == "LD") {
    int pos = 100;
    text_area->page()->mainFrame()->setScrollBarValue(Qt::Vertical, text_area->page()->mainFrame()->contentsSize().height()*pos/100);
    return;
  } else if (g == "LU") {
    int pos=0;
    text_area->page()->mainFrame()->setScrollBarValue(Qt::Vertical, text_area->page()->mainFrame()->contentsSize().height()*pos/100);
    return;
  } else if (g == "UD") {
    update();
    return;
  }
}
const QJsonObject& TopicList::getTopic(const QString& t_id)
{
  return getTopic(t_id.toInt());
}
const QJsonObject& TopicList::getTopic(int t_id)
{
  return topics[t_id].first;
}
void TopicList::mark_all_as_read()
{
  for (auto &t : topics) {
    t.second.first = t.second.second;
  }
  update_view();
}
