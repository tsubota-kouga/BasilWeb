#ifndef ___WebView_H_
#define ___WebView_H_

#include <QtWidgets>
#include <QtGui>
#include <QtWebEngineWidgets>

#include "BasilPlugin.hpp"
#include "Basilico.hpp"

class Basilico;

class WebScreen: public QWebEngineView
{
public:
    WebScreen():
        QWebEngineView{}
    {}
};

class WebViewer: public QWidget
{
    QGridLayout* layout;
    QToolBar toolbar;
    QLineEdit urlline;
    QUrl default_url;
    QProgressBar progressbar;
    Basilico* basil;
    // static deque<QWebEngineDownloadItem*> downloaditem;
public:
    WebScreen Web;

    WebViewer(Basilico* basil, QTabWidget* tab, QString url);

    void back(){ Web.back(); }
    void forward(){ Web.forward(); }
    void load(const QUrl& url){ Web.load(url); }
    void load(const QString& url){ Web.load(QUrl{url}); }
    const QIcon icon(){ return Web.icon(); }
    const QString title(){ return Web.title(); }

    void progressbarSetStyleSheet(const String& ss)
    { progressbar.setStyleSheet(QString::fromStdString(ss)); }

    void toolbarSetStyleSheet(const String& ss)
    { toolbar.setStyleSheet(QString::fromStdString(ss)); }

    String selectedText();
private:
    void settingToolBar();

    void settingUrlLine();

    void settingProgressBar();

    void settingWebBrowser();
};

class BasilWeb: public BasilPlugin, public QWidget
{
    QGridLayout web_layout;
    QTabWidget Tab;
    String default_url;
    QPushButton addButton;
    String viewerProgressBarStyleSheet;
    String viewerToolBarStyleSheet;
    Basilico* basil;
public:
    BasilWeb(Basilico* basil);

    BasilWeb(Basilico* basil, String url);

    static std::pair<BasilWeb*, String> factory(Basilico* basil, Array args);

    bool eventFilter(QObject* obj, QEvent* e) override;

    std::vector<String> selectedTextVector();

protected:
    virtual void execute(Basilico* basil, Array args) override;

    void settingTab();

    void settingViewer();

    void settingAddButton();
};
#endif
