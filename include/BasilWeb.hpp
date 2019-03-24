#ifndef ___WebView_H_
#define ___WebView_H_

#include <QtWidgets>
#include <QtGui>
#include <QtWebEngineWidgets>

#include "BasilPlugin.hpp"
#include "Basilico.hpp"

class Basilico;

class BasilWeb;

class WebScreen: public QWebEngineView
{
public:
    WebScreen(QWidget* parent=nullptr);
protected:
    bool eventFilter(QObject* obj, QEvent* e) override;
    void contextMenuEvent(QContextMenuEvent* e) override;
};

class WebViewer: public QWidget
{
    QGridLayout* layout;
    QToolBar toolbar;
    QLineEdit urlline;
    QUrl default_url;
    QAction securityAction;
    QAction favoriteAction;
    QProgressBar progressbar;
    Basilico* basil;
    QMenu* favoriteMenu;
    // static deque<QWebEngineDownloadItem*> downloaditem;
public:
    BasilWeb* parent;
    WebScreen Web;

    WebViewer(Basilico* basil, BasilWeb* tab, QString url);

    void back(){ Web.back(); }
    void forward(){ Web.forward(); }
    void load(QUrl url){ Web.load(url); }
    void load(QString url){
        SUconverter(url);
        Web.load(QUrl{url});
    }
    void home(){ load(default_url); }
    void SUconverter(QString& url){
        if(url.startsWith("about:")){
            url[5] = '/';  // : -> /
            auto&& path = QDir{__FILE__};
            path.cd("../../");  // root of this project
            url.prepend("file://" + path.absolutePath() + "/");
            url.append(".html");
        }
        else if(url.startsWith("www.")){ url.prepend("http://"); }
    }
    void USconverter(QString& url){
        auto&& path = QDir{__FILE__};
        path.cd("../../");  // root of this project
        if(auto&& prefix = "file://" + path.absolutePath() + "/about/";
                url.startsWith(prefix))
        {
            url.remove(0, prefix.size());
            url.prepend("about:");
            url.remove(url.size() - 5, 5); // size of ".html"
        }
    }
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

    void settingSecurityAction();
    void settingFavoriteAction();

    void setStar();
};

class BasilWeb: public BasilPlugin, public QWidget
{
    QGridLayout web_layout;
public:
    QTabWidget Tab;
private:
    String default_url;
    QPushButton addButton;
    String viewerProgressBarStyleSheet;
    String viewerToolBarStyleSheet;
    Basilico* basil;
    QString jquery;
public:
    BasilWeb(Basilico* basil);

    BasilWeb(Basilico* basil, String url);

    static std::pair<BasilWeb*, String> factory(Basilico* basil, Array args);

    bool eventFilter(QObject* obj, QEvent* e) override;

    std::vector<String> selectedTextVector();

    static QIcon lockIcon;
    static QIcon rightArrowIcon;
    static QIcon leftArrowIcon;
    static QIcon circleArrowIcon;
    static QIcon warningIcon;
    static QIcon homeIcon;
    static QIcon settingIcon;
    static QIcon starIcon;
    static QIcon starHoleIcon;
    static QIcon menuIcon;

    static QJsonObject logJson;

    WebViewer* makeNewTabWindow();

    const QString& getjQuery() const { return jquery; }
protected:
    virtual void execute(Basilico* basil, Array args) override;

    void settingTab(Dictionary& d);

    void settingViewer(Dictionary& d);

    void settingAddButton();

    void setSelected();

    void settingJS();
};
#endif
