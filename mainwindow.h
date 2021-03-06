#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtGui>

class MyWebView;
class QLineEdit;
class TopicList;

class MainWindow : public QMainWindow
{
  Q_OBJECT

public:
  MainWindow();
  ~MainWindow();

protected slots:

  void adjustLocation();
  void changeLocation();
  void adjustTitle();
  void setProgress(int p);
  void finishLoading(bool);

  void process_res();
  void process_toppage();
  void linkClicked(const QUrl &url);
  void findText();
  void findText(const QString&);
  void debug();
  void openTopic(const QString& t_id);
  void insert_bookmark(const QString& t_id);
  void addNGTopic(const QString& t_id);
protected:
  void mousePressEvent(QMouseEvent* e);
  void mouseMoveEvent(QMouseEvent* e);
  void mouseReleaseEvent(QMouseEvent* e);
  void readSettings();
  void saveSettings();

private:
  QString popupjs, bookmark;
  QStringList ng_topics;
  MyWebView *view;
  QLineEdit *locationEdit, *findEdit;
  int progress;
  TopicList *topic_list;
  void setDock();
  void setView();
  void setToolBar();
  void setStatusBar();
  void setShortcut();
signals:
  void topicOpend(const QString&);
  void mouseGestured(const QString&);
  void mouseGestureUpdated(const QString&);
};
#endif  // MAINWINDOW_H
