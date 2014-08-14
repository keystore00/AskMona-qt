#include <QtGui>
#include <QtNetwork>
#include <QtWebKit>
#include <QWebView>
#include <QLineEdit>
#include <QToolBar>
#include <QMenuBar>
#include <QStyle>
#include <QTextEdit>
#include <QWebFrame>
#include <QShortcut>
#include <QApplication>
#include <QDockWidget>
#include <QMessageBox>
#include <QStatusBar>
#include "mainwindow.h"
#include "mywebpage.h"
#include "mywebview.h"
#include "topiclist.h"
#include "constants.h"
#include "util.h"
#include <map>
namespace {
  bool r_clicking = false;
  QPoint origin;
  const int thr = 30;
  enum MGesture {MGUp='U',MGDown='D',MGRight='R',MGLeft='L',MGNone='N'};
  enum MGesture last_gesture = MGNone;
  QString gestures;

  bool isTopPage(const QString& url)
  {
    return url=="http://askmona.org/" || url.startsWith("http://askmona.org/topics/");
  }
}
MainWindow::MainWindow()
{
  setWindowIcon(QIcon(":/img/icon.png"));
  {

    QDir dat_dir(QCoreApplication::applicationDirPath()+"/dat");
    if (!dat_dir.exists()) {
      if (dat_dir.mkpath(".")) {
	qDebug() << "dat directory created.";
      } else {
	qDebug() << "Failed to make dat directory.";
      }
    }
    popupjs = loadEmbeddedOrFile("popup.js");
    bookmark = loadEmbeddedOrFile("bookmark.html");
  }
  progress = 0;
  QNetworkProxyFactory::setUseSystemConfiguration(true);
  setDock();
  setView();
  setCentralWidget(view);
  setToolBar();
  setStatusBar();
  setShortcut();
  readSettings();
}
MainWindow::~MainWindow()
{
  saveSettings();
}
void MainWindow::readSettings()
{
  QSettings settings(getDataDir()+"/settings.ini",QSettings::IniFormat,this);
  restoreGeometry(settings.value("geometry").toByteArray());
  restoreState(settings.value("windowState").toByteArray());
  QDataStream ds(settings.value("ng_topics").toByteArray());
  ds >> ng_topics;
}
void MainWindow::saveSettings()
{
  QSettings settings(getDataDir()+"/settings.ini",QSettings::IniFormat,this);
  settings.setValue("geometry", saveGeometry());
  settings.setValue("windowState", saveState());
  QByteArray ba;
  QDataStream ds(&ba,QIODevice::WriteOnly);
  ds << ng_topics;
  settings.setValue("ng_topics",ba);
}

void MainWindow::adjustLocation()
{
  locationEdit->setText(view->url().toString());
}

void MainWindow::changeLocation()
{
  QUrl url = QUrl(locationEdit->text());
  view->load(url);
  view->setFocus();
}

void MainWindow::adjustTitle()
{
  if (progress <= 0 || progress >= 100)
    setWindowTitle(view->title());
  else
    setWindowTitle(QString("%1 (%2%)").arg(view->title()).arg(progress));
}

void MainWindow::setProgress(int p)
{
  progress = p;
  adjustTitle();
}

void MainWindow::finishLoading(bool)
{
  progress = 100;
  adjustTitle();
  if (view->url().toString() == ask_url_base+"/") {
    topic_list->update();
  }
  QRegularExpression re("http://askmona.org/(\\d+)");
  auto match = re.match(view->url().toString());
  if (match.hasMatch()) { //Ask Mona's topic page
    auto t_id = match.captured(1);
    process_res();
    insert_bookmark(t_id);
    topic_list->addTopic(t_id);
    return;
  }
  if (isTopPage(view->url().toString())) {
    process_toppage();
  }
}
void MainWindow::process_toppage()
{
  QWebElement document = view->page()->mainFrame()->documentElement();
  auto topics = document.findAll("li.tbox");
  for (auto t : topics) {
    auto a = t.findFirst("a");
    auto t_id = a.attribute("href").mid(1);
    if (ng_topics.contains(t_id)) {
      t.setAttribute("style","display:none");
    }
  }
}

void MainWindow::process_res()
{
  typedef std::map<QString,std::map<int,QString> > AnchorMap;
  QStringList anchoring_list;
  AnchorMap am;
  QWebElement document = view->page()->mainFrame()->documentElement();
  auto div_popup = document.findFirst("div#popup");
  QWebElementCollection p = document.findAll("p");
  for (auto it = p.begin(); it != p.end(); ++it) {
    auto p_id = (*it).attribute("id","");
    QRegularExpression re("res_(\\d+)");
    auto match = re.match(p_id);
    if (match.hasMatch()) { //p.res_#
      auto res_id = match.captured(1);
      //AA
      auto inner = (*(it+1)).toPlainText();
      if (inner.contains("　 ")) {
	(*(it+1)).addClass("res_aa");
      }
      //anchor
      QRegularExpression re(">>(\\d+)");
      auto it = re.globalMatch(inner);
      if (it.hasNext()) { // has at least one anchor
	anchoring_list.push_back(res_id);
      }
      while (it.hasNext()) {
	auto match = it.next();
	am[match.captured(1)][res_id.toInt()] = res_id;
      }
    }
  }
  //create anchored href
  for (const auto &a_pair: am) {
    auto &res_id = a_pair.first;
    auto anchored = document.findFirst("div.txt p#res_"+res_id);
    anchored.nextSibling().appendOutside("<p class=\"ras\"></p>");
    auto p = anchored.nextSibling().nextSibling();
    auto anchor_list = a_pair.second;
    for (auto it = anchor_list.begin(); it != anchor_list.end();++it) {
      p.appendInside(QString("<a class=\"raed\" href=\"#res_%1\">&lt;&lt;%1 </a>").arg(it->second));
    }
  }
  //create invisible res
  for (const auto &res_id : anchoring_list) {
    div_popup.appendOutside(QString("<div id=\"a_%1\"></div>").arg(res_id));
    auto hidden = div_popup.nextSibling();
    hidden.setAttribute("style","display:none");
    auto anchoring = document.findFirst("div.txt p#res_"+(res_id));
    auto copied = anchoring.clone();
    copied.removeAttribute("id");
    hidden.appendInside(copied.toOuterXml());
    hidden.appendInside(anchoring.nextSibling().toOuterXml());
    auto third = anchoring.nextSibling().nextSibling();
    if (third.hasClass("ras")) {
      hidden.appendInside(third.toOuterXml());
    }
  }
  div_popup.appendOutside("<div id=\"popup2\"></div>");
  view->page()->mainFrame()->evaluateJavaScript(popupjs + "; null");
}
void MainWindow::insert_bookmark(const QString& t_id)
{
  auto t = topic_list->getTopic(t_id);
  if (!t) {
    //invalid object
    return;
  }
  auto kokomade = QString::number(t->count);
  auto last_res = view->page()->mainFrame()->documentElement().findFirst("p#res_"+kokomade);
  last_res.nextSibling().appendOutside(bookmark);
  view->page()->mainFrame()->evaluateJavaScript("window.location.hash = '#res_"+kokomade+"';null");
}
void MainWindow::linkClicked(const QUrl &url)
{
  view->load(url);
}

void MainWindow::findText()
{
  view->findText(findEdit->text());
}
void MainWindow::findText(const QString&)
{
  findText();
}
void MainWindow::debug()
{
  QTextEdit* textEdit = new QTextEdit(NULL);
  textEdit->setAttribute(Qt::WA_DeleteOnClose);
  textEdit->show();
  textEdit->setPlainText(view->page()->mainFrame()->documentElement().toOuterXml());
}
void MainWindow::openTopic(const QString& t_id)
{
  view->load(ask_url_base+"/"+t_id);
}

void MainWindow::mousePressEvent(QMouseEvent* e)
{
  switch (e->button()) {
  case Qt::BackButton:
    view->triggerPageAction(QWebPage::Back);
    break;
  case Qt::ForwardButton:
    view->triggerPageAction(QWebPage::Forward);
    break;
  case Qt::RightButton:
    r_clicking = true;
    origin = e->pos();
    break;
  default:
    QMainWindow::mousePressEvent(e);
    break;
  }
}
void MainWindow::mouseMoveEvent(QMouseEvent* e)
{
  auto d = origin - e->pos();
  if (r_clicking) {
    auto old_cnt = gestures.size();
    if (d.x() > thr) {
      origin.setX(e->x());
      if (last_gesture != MGLeft) {
	last_gesture = MGLeft;
	gestures.push_back(MGLeft);
      }
    } else if (d.x() < -thr) {
      origin.setX(e->x());
      if (last_gesture != MGRight) {
	last_gesture = MGRight;
	gestures.push_back(MGRight);
      }
    } else if (d.y() < -thr) {
      origin.setY(e->y());
      if (last_gesture != MGDown) {
	last_gesture = MGDown;
	gestures.push_back(MGDown);
      }
    } else if (d.y() > thr) {
      origin.setY(e->y());
      if (last_gesture != MGUp) {
	last_gesture = MGUp;
	gestures.push_back(MGUp);
      }
    }
    if (gestures.size() > old_cnt) {
      emit mouseGestureUpdated(gestures);
    }
  } else {
    QMainWindow::mouseMoveEvent(e);
  }
}
void MainWindow::mouseReleaseEvent(QMouseEvent* e)
{
  switch (e->button()) {
  case Qt::RightButton:
    {
      r_clicking = false;
      emit mouseGestured(gestures);
      //clear gesture
      last_gesture = MGNone;
      gestures = "";
      emit mouseGestureUpdated(gestures);
      break;
    }
  default:
    {
      QMainWindow::mouseReleaseEvent(e);
      break;
    }
  }
}
void MainWindow::setDock()
{
  auto dock = new QDockWidget(tr("Topic List"), this);
  dock->setObjectName("Topic List Dock");
  topic_list = new TopicList(this);
  connect(topic_list,SIGNAL(topicClicked(QString)),this,SLOT(openTopic(QString)));
  connect(this,SIGNAL(mouseGestured(QString)),topic_list,SLOT(handleMouseGesture(QString)));
  dock->setWidget(topic_list);
  addDockWidget(Qt::LeftDockWidgetArea, dock);
}
void MainWindow::setView()
{
  view = new MyWebView(this);
  auto page = new MyWebPage(this);
  connect(page,&MyWebPage::linkHovered,[this](const QString & link, const QString &, const QString &){statusBar()->showMessage(link);});
  view->setPage(page);
  view->page()->setLinkDelegationPolicy(QWebPage::DelegateAllLinks);
  connect(this,SIGNAL(mouseGestured(QString)),view,SLOT(handleMouseGesture(QString)));
  view->load(QUrl("http://askmona.org"));
  QFile css(QCoreApplication::applicationDirPath()+"/user.css");
  if (css.exists()) {
    view->settings()->setUserStyleSheetUrl(QUrl::fromLocalFile((css.fileName())));
    qDebug() << "User style sheet loaded from " << css.fileName();
  } else {
    QFile file(":/user.css");
    file.open(QIODevice::ReadOnly | QIODevice::Text);
    view->settings()->setUserStyleSheetUrl(QUrl("data:text/css;charset=utf-8;base64,"+file.readAll().toBase64()));
  }

  connect(view, SIGNAL(loadFinished(bool)), SLOT(adjustLocation()));
  connect(view, SIGNAL(titleChanged(QString)), SLOT(adjustTitle()));
  connect(view, SIGNAL(loadProgress(int)), SLOT(setProgress(int)));
  connect(view, SIGNAL(loadFinished(bool)), SLOT(finishLoading(bool)));
  connect(view, SIGNAL(addNG(QString)),SLOT(addNGTopic(QString)));

  connect(view, SIGNAL(linkClicked(const QUrl)),SLOT(linkClicked(const QUrl)));
}
void MainWindow::setToolBar()
{
  QToolBar *toolBar = addToolBar(tr("Navigation Buttons"));
  toolBar->setObjectName("Navigation Buttons");
  QIcon backward(":/img/backward.png");
  backward.addPixmap(QPixmap(":/img/backward_disabled.png"),QIcon::Disabled);
  view->pageAction(QWebPage::Back)->setIcon(backward);
  toolBar->addAction(view->pageAction(QWebPage::Back));
  QIcon forward(":/img/forward.png");
  forward.addPixmap(QPixmap(":/img/forward_disabled.png"),QIcon::Disabled);
  view->pageAction(QWebPage::Forward)->setIcon(forward);
  toolBar->addAction(view->pageAction(QWebPage::Forward));
  QIcon reload(":/img/reload.png");
  reload.addPixmap(QPixmap(":/img/reload_disabled.png"),QIcon::Disabled);
  view->pageAction(QWebPage::Reload)->setIcon(reload);
  toolBar->addAction(view->pageAction(QWebPage::Reload));
  QIcon stop(":/img/stop.png");
  stop.addPixmap(QPixmap(":/img/stop_disabled.png"),QIcon::Disabled);
  view->pageAction(QWebPage::Stop)->setIcon(stop);
  toolBar->addAction(view->pageAction(QWebPage::Stop));
  auto qa = new QAction(QIcon(":img/home.png"),"home",this);
  connect(qa,&QAction::triggered,[this](){linkClicked(ask_url_base);});
  toolBar->addAction(qa);

  auto zoomBar = addToolBar(tr("Zoom Buttons"));
  zoomBar->setObjectName("Zoom Buttons");
  auto label = new QLineEdit("100%",zoomBar);
  label->setReadOnly(true);
  connect(view,&MyWebView::zoomFactorChanged,[this,label](qreal factor){label->setText(QString::number(factor*100)+"%");});
  qa = new QAction("Zoom In",this);
  QIcon zoom_in(":/img/zoom_in.png");
  qa->setIcon(zoom_in);
  connect(qa,&QAction::triggered,[this,label](){view->setZoomFactor(view->zoomFactor()+0.1);label->setText(QString::number(view->zoomFactor()*100)+"%");});
  zoomBar->addAction(qa);
  qa = new QAction("Zoom Out",this);
  QIcon zoom_out(":/img/zoom_out.png");
  qa->setIcon(zoom_out);
  connect(qa,&QAction::triggered,[this,label](){view->setZoomFactor(view->zoomFactor()-0.1);label->setText(QString::number(view->zoomFactor()*100)+"%");});
  zoomBar->addAction(qa);
  qa = new QAction("Reset",this);
  QIcon zoom_reset(":/img/zoom_reset.png");
  qa->setIcon(zoom_reset);
  connect(qa,&QAction::triggered,[this,label](){view->setZoomFactor(1.0);label->setText(QString::number(view->zoomFactor()*100)+"%");});
  zoomBar->addAction(qa);
  zoomBar->addWidget(label);

  toolBar = addToolBar(tr("Address Bar"));
  toolBar->setObjectName("Address Bar");
  locationEdit = new QLineEdit(this);
  locationEdit->setSizePolicy(QSizePolicy::Expanding, locationEdit->sizePolicy().verticalPolicy());
  connect(locationEdit, SIGNAL(returnPressed()), SLOT(changeLocation()));
  findEdit = new QLineEdit(this);
  findEdit->setPlaceholderText("find");
  findEdit->setSizePolicy(QSizePolicy::Expanding, locationEdit->sizePolicy().verticalPolicy());
  connect(findEdit, SIGNAL(returnPressed()), SLOT(findText()));
  connect(findEdit, SIGNAL(textChanged(QString)), SLOT(findText(QString)));
  connect(this,SIGNAL(topicOpend(QString)),this,SLOT(openTopic(QString)));
  toolBar->addWidget(locationEdit);
  toolBar->addWidget(findEdit);

  setUnifiedTitleAndToolBarOnMac(true);
}
void MainWindow::setShortcut()
{
  auto ctrl_q = new QShortcut(QKeySequence("CTRL+Q"), this);
  connect(ctrl_q, SIGNAL(activated()), QApplication::instance(), SLOT(quit()));
  auto ctrl_f = new QShortcut(QKeySequence("CTRL+F"), this);
  connect(ctrl_f, SIGNAL(activated()), findEdit, SLOT(selectAll()));
  connect(ctrl_f, SIGNAL(activated()), findEdit, SLOT(setFocus()));
  auto ctrl_g = new QShortcut(QKeySequence("CTRL+G"), this);
  connect(ctrl_g, SIGNAL(activated()), this, SLOT(findText()));
  auto ctrl_d = new QShortcut(QKeySequence("CTRL+D"), this);
  connect(ctrl_d, SIGNAL(activated()), this, SLOT(debug()));
  auto meta_d = new QShortcut(QKeySequence("ALT+D"), this);
  connect(meta_d, SIGNAL(activated()), locationEdit, SLOT(selectAll()));
  connect(meta_d, SIGNAL(activated()), locationEdit, SLOT(setFocus()));
  view->pageAction(QWebPage::Reload)->setShortcut(QKeySequence("CTRL+R"));
}
void MainWindow::setStatusBar()
{
  auto sb = statusBar();
  sb->show();
  connect(this,SIGNAL(mouseGestureUpdated(QString)),sb,SLOT(showMessage(QString)));
}
void MainWindow::addNGTopic(const QString& t_id)
{
  if (!ng_topics.contains(t_id)) {
    ng_topics << t_id;
    process_toppage();
  } else {
    qDebug() << "Dupulicated NG topic request." << t_id;
  }
}
