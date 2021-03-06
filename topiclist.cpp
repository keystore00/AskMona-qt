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
#include <QTimer>
#include <QStringListModel>
#include <QListView>
#include <queue>
#include <ctime>
#include "constants.h"
#include "topicview.h"
#include "util.h"

namespace {
  enum sort_key {k_modified, k_updated, k_res, k_MONA};
  time_t last_reload;
  std::map<QString, enum sort_key> sort_key_map = {{"Default", k_updated}, {"Res.", k_res}, {"MONA", k_MONA}, {"Modified", k_modified}};
  QString fname = "/dat/topic_list.json";
  static const int n_timeout = 1;
}
TopicList::TopicList(QWidget *parent) :
  QWidget(parent)
{
  category_list_str= tr("Misc.,GiveAway,Beginner,News,Discussion,Hobby,Culture,Chat,Economy,Science,Mining,R18,All");
  category_list = category_list_str.split(",");
  setWidgets();
  nam = new QNetworkAccessManager(this);
  QObject::connect(nam, SIGNAL(finished(QNetworkReply*)), this, SLOT(finishedSlot(QNetworkReply*)));

  loadTopics(getDataDir()+fname);
}

TopicList::~TopicList()
{
  saveTopics(getDataDir()+fname);
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
    auto t_obj = TopicObject::New(obj);
    topics[t_obj->t_id] = TopicPair(t_obj, t_obj);
  }
}
void TopicList::saveTopics(const QString& filename)
{
  QJsonArray ja;
  for (const auto &t : topics) {
    ja.push_back(t.second.first->obj);
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
  QIcon reload(getValidFileName("img/reload.png"));
  reload.addPixmap(QPixmap(getValidFileName("img/reload_disabled.png")), QIcon::Disabled);
  reload_button = new QPushButton(reload, "", this);
  reload_button->setToolTip(tr("Update topic information for last N topics."));
  connect(reload_button, &QPushButton::clicked, this, &TopicList::update);
  hlayout->addWidget(reload_button);
  cbox = new QComboBox(this);
  cbox->setView(new QListView(cbox));
  cbox->setToolTip(tr("Topic Order"));
  cbox->addItem(tr("Default"));
  cbox->setItemData(0, tr("Default order in Ask Mona."), Qt::ToolTipRole);
  cbox->addItem(tr("Res."));
  cbox->setItemData(1, tr("Descending order by response count."), Qt::ToolTipRole);
  cbox->addItem(tr("MONA"));
  cbox->setItemData(2, tr("Descending order by MONA received."), Qt::ToolTipRole);
  cbox->addItem(tr("Modified"));
  cbox->setItemData(3, tr("Descending order by modified datetime."), Qt::ToolTipRole);
  connect(cbox, SIGNAL(currentIndexChanged(int)), this, SLOT(comboBoxChanged(int)));
  hlayout->addWidget(cbox);
  cat_box = new QComboBox(this);
  cat_box->setView(new QListView(cat_box));
  cat_box->setToolTip(tr("Filter Category"));
  cat_box->setModel(new QStringListModel(category_list));
  cat_box->setCurrentIndex(category_list.size()-1);
  connect(cat_box, SIGNAL(currentIndexChanged(int)), this, SLOT(comboBoxChanged(int)));
  hlayout->addWidget(cat_box);
  num_update_box = new QSpinBox(this);
  num_update_box->setMinimum(50);
  num_update_box->setMaximum(200);
  num_update_box->setAlignment(Qt::AlignRight);
  num_update_box->setSingleStep(50);
  num_update_box->setValue(50);
  num_update_box->setToolTip(tr("# of topics to be updated."));
  hlayout->addWidget(num_update_box);
  layout->addLayout(hlayout);
  text_area = new TopicView(this);
  text_area->setContextMenuPolicy(Qt::CustomContextMenu);
  text_area->page()->setLinkDelegationPolicy(QWebPage::DelegateAllLinks);
  text_area->page()->mainFrame()->setHtml("Loading...");
  connect(text_area, SIGNAL(linkClicked(QUrl)), this, SLOT(linkClicked(QUrl)));
  layout->addWidget(text_area);
  auto pb = new QPushButton(tr("Mark all as read"), this);
  connect(pb, SIGNAL(clicked()), this, SLOT(mark_all_as_read()));
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
  // sort topics
  auto key = sort_key_map[cbox->currentText()];
  typedef qint64 KeyValueType;
  std::multimap <KeyValueType, TopicPair*> ordered;
  int cat_flag = cat_box->currentIndex() < category_list.size()-1 ? TopicObject::cat_id2flag(cat_box->currentIndex()) : -1;
  for (auto it = topics.begin(); it != topics.end(); ++it) {
    auto last = it->second.first;
    auto current = it->second.second;
    if (!(last->category_flag & cat_flag)) {
      continue;
    }
    KeyValueType key_value = 0;
    switch (key) {
    case k_updated:
      key_value = current->updated;
    case k_modified:
      key_value = current->modified;
      break;
    case k_res:
      key_value = current->count - last->count;
      break;
    case k_MONA:
      key_value = current->receive - last->receive;
      break;
    default:
      key_value = current->t_id;
      break;
    }
    key_value = std::numeric_limits<KeyValueType>::max()-key_value;  // descending order
    ordered.insert(std::pair<KeyValueType, TopicPair*>(key_value, &(it->second)));
  }
  // do update view
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
  if ((time((time_t)NULL) - last_reload) > n_timeout) {
    reload_button->setEnabled(false);
    auto timer = new QTimer(this);
    timer->setSingleShot(true);
    connect(timer, &QTimer::timeout, [this](){reload_button->setEnabled(true);});
    timer->start(n_timeout*1000);
    QNetworkRequest request(QUrl(ask_topic_api+"?limit="+QString::number(num_update_box->value())));
    nam->get(request);
    last_reload = time((time_t)NULL);
  } else {
    qWarning() << "Last update is less than" << n_timeout << "sec. ago.";
  }
}
void TopicList::finishedSlot(QNetworkReply* reply)
{
  QJsonDocument d = QJsonDocument::fromJson(reply->readAll());
  auto res = d.object();
  if (res["status"] != 1) {
    qWarning() << "Failed to update topic list.";
    return;
  } else {
    if (res.contains("topics")) {  // update topic list
      auto array = res["topics"].toArray();
      for (auto t : array) {
        auto t_obj = TopicObject::New(t.toObject());
        auto t_id = t_obj->t_id;
        auto it = topics.find(t_id);
        if (it != topics.end()) {
          it->second.second = t_obj;
        }
      }
    } else if (res.contains("topic")) {  // add topic
      auto topic_obj = TopicObject::New(res["topic"].toObject());
      topics[topic_obj->t_id] = TopicPair(topic_obj, topic_obj);
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
    int pos = 0;
    text_area->page()->mainFrame()->setScrollBarValue(Qt::Vertical, text_area->page()->mainFrame()->contentsSize().height()*pos/100);
    return;
  } else if (g == "UD") {
    update();
    return;
  }
}
const TopicObjectPtr TopicList::getTopic(const QString& t_id)
{
  return getTopic(t_id.toInt());
}
const TopicObjectPtr TopicList::getTopic(int t_id)
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
void TopicList::paintEvent(QPaintEvent *)
{
  QStyleOption opt;
  opt.init(this);
  QPainter p(this);
  style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}
