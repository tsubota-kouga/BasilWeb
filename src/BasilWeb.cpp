
#include "BasilWeb.hpp"


// WebViewer class

// deque<QWebEngineDownloadItem*> WebViewer::downloaditem{};

WebViewer::WebViewer(Basilico* _basil, QTabWidget* tab, QString url):
    Web{},
    toolbar{this},
    urlline{&toolbar},
    default_url{QUrl{url}},
    securityAction{"Security"},
    favoriteAction{"Favorite"},
    progressbar{},
    basil{_basil}
{
    layout = new QGridLayout();
    layout->setContentsMargins(0, 0, 0, 0);
    setLayout(layout);
    setSizePolicy(QSizePolicy{QSizePolicy::Expanding, QSizePolicy::Expanding});

    settingToolBar();

    settingUrlLine();

    settingProgressBar();

    settingWebBrowser(tab);

    // <parent tab>
    connect(&Web, &QWebEngineView::titleChanged, this,
            [=](auto&& title){
            int index = tab->indexOf(this);
            tab->setTabText(index, title);
            });
    connect(&Web, &QWebEngineView::iconChanged, this,
            [=](auto&& icon){
            int index = tab->indexOf(this);
            tab->setTabIcon(index, icon);
            });
    // </parent tab>
    Web.load(default_url);
}

String WebViewer::selectedText()
{
    return Web.selectedText().toStdString();
}

void WebViewer::settingToolBar()
{
    layout->addWidget(&toolbar, 1, 0, 1, 1);
    toolbar.addAction(BasilWeb::leftArrowIcon,
                      "Back",
                      [&]{ Web.back(); });
    toolbar.addAction(BasilWeb::rightArrowIcon,
                      "Forward",
                      [&]{ Web.forward(); });
    toolbar.addAction(BasilWeb::circleArrowIcon,
                      "Reload",
                      [&]{ Web.reload(); });
    toolbar.addAction(BasilWeb::homeIcon,
                      "Home",
                      [&]{ Web.load(default_url); });
    toolbar.addSeparator();
    toolbar.addWidget(&urlline);
    toolbar.addSeparator();

    auto* menu = new QMenu();
    QAction* act;
    act = new QAction{"Quit"};
    connect(act, &QAction::triggered, this, [&]{ basil->getNeoVim().nvim_command("quit"); });
    menu->addAction(act);
    auto* settingMenu = new QToolButton{};
    toolbar.addWidget(settingMenu);
    settingMenu->setMenu(menu);
    settingMenu->setPopupMode(QToolButton::InstantPopup);
}

void WebViewer::settingUrlLine()
{
    connect(&urlline, &QLineEdit::returnPressed, this,
            [&](){
            auto&& path = urlline.text();
            if(path.startsWith("www.")){ path.push_front("http://"); }
            Web.load(path);
            });
    urlline.addAction(&securityAction, QLineEdit::LeadingPosition);
    urlline.addAction(&favoriteAction, QLineEdit::TrailingPosition);
}

void WebViewer::settingProgressBar()
{
    layout->addWidget(&progressbar, 2, 0, 1, 1);
    progressbar.setMinimum(0);
    progressbar.setMaximum(100);
    progressbar.setTextVisible(false);
    connect(&Web, &QWebEngineView::loadProgress, this,
            [&](int value)
            {
            if(value == 0){ progressbar.show(); }
            else if(value == 100){ progressbar.hide(); }
            progressbar.setValue(value);
            });
}

void WebViewer::settingWebBrowser(QTabWidget* tab)
{
    layout->addWidget(&Web, 3, 0);
    setSizePolicy(QSizePolicy{QSizePolicy::Expanding, QSizePolicy::Expanding});
    connect(&Web, &QWebEngineView::loadFinished, this,
            [&](auto&& ok){ if(ok){
            auto&& url = Web.url().toString();
            urlline.setText(url);
                if(url.startsWith("https://")) {
                    securityAction.setIcon(BasilWeb::lockIcon);
                }
                else {
                    securityAction.setIcon(BasilWeb::warningIcon);
                }
                setStar();
            } });
    settingSecurityAction();
    settingFavoriteAction(tab);

    connect(Web.page()->profile(), &QWebEngineProfile::downloadRequested, this,
            [&](auto&& item){
            QMessageBox msgbox;
            msgbox.setText("Download?");
            msgbox.setInformativeText(
                    "Download " + item->url().toString() + "?");
            msgbox.setStandardButtons(QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
            msgbox.setDefaultButton(QMessageBox::Yes);
            int is_download = msgbox.exec();
            if(is_download == QMessageBox::Yes)
            {
                item->accept();
                // downloaditem.push_back(item);
            }
            else{ item->cancel(); }
            });
}

void WebViewer::settingSecurityAction()
{
    securityAction.setCheckable(true);
    connect(&securityAction, &QAction::toggled, this,
            [&](bool b){
                std::cout << "***************************************" << std::endl;
            });
}
void WebViewer::settingFavoriteAction(QTabWidget* tab)
{
    favoriteAction.setCheckable(true);
    connect(&favoriteAction, &QAction::toggled, this,
            [&](bool is_star){
                QJsonArray array{};
                if(BasilWeb::logJson["favorite"].isArray()){
                    array = BasilWeb::logJson.value("favorite").toArray();
                }
                if(is_star){
                    favoriteAction.setIcon(BasilWeb::starIcon);
                    if(!array.contains(urlline.text())){
                        array.append(urlline.text());
                        BasilWeb::logJson["favorite"] = array;
                    }
                }
                else{
                    favoriteAction.setIcon(BasilWeb::starHoleIcon);
                    if(array.contains(urlline.text())){
                        for(auto&& i = 0;i < array.count();i++) {
                            if(array.at(i).toString() == urlline.text()) {
                                array.removeAt(i);
                            }
                        }
                        BasilWeb::logJson["favorite"] = array;
                    }
                }
                std::cout << qPrintable(QJsonDocument{BasilWeb::logJson}.toJson(QJsonDocument::Compact)) << std::endl;
            });
    connect(tab, &QTabWidget::currentChanged, this,
            [=](int index){
                setStar();
            });
}

void WebViewer::setStar()
{
    if(BasilWeb::logJson.value("favorite").isArray()){
        auto&& favorite = BasilWeb::logJson.value("favorite").toArray();
        if(favorite.contains(urlline.text())){
            favoriteAction.setIcon(BasilWeb::starIcon);
            favoriteAction.setChecked(true);
        }
        else{
            favoriteAction.setIcon(BasilWeb::starHoleIcon);
            favoriteAction.setChecked(false);
        }
    }
    else{
        favoriteAction.setIcon(BasilWeb::starHoleIcon);
        favoriteAction.setChecked(false);
    }
}

// BasilWeb class
QIcon BasilWeb::lockIcon{};
QIcon BasilWeb::rightArrowIcon{};
QIcon BasilWeb::leftArrowIcon{};
QIcon BasilWeb::circleArrowIcon{};
QIcon BasilWeb::warningIcon{};
QIcon BasilWeb::homeIcon{};
QIcon BasilWeb::settingIcon{};
QIcon BasilWeb::starIcon{};
QIcon BasilWeb::starHoleIcon{};

QJsonObject BasilWeb::logJson{};

BasilWeb::BasilWeb(Basilico* _basil):
    QWidget{},
    web_layout{},
    BasilPlugin{},
    Tab{},
    addButton{"+"},
    basil{_basil}
{
    installEventFilter(this);

    web_layout.setContentsMargins(0, 0, 0, 0);
    setLayout(&web_layout);
    web_layout.addWidget(&Tab, 0, 0);
    auto&& baseobjstyleSheet = basil->getNeoVim().nvim_get_var("basilweb#base_style_sheet");
    auto&& baseStyleSheet = boost::get<String>(baseobjstyleSheet);
    setStyleSheet(QString::fromStdString(baseStyleSheet));

    auto&& path = QDir{__FILE__};
    path.cd("../..");  // project root path
    auto&& objicontheme = basil->getNeoVim().nvim_get_var("basilweb#icon_theme");
    auto&& icon_theme = boost::get<String>(objicontheme);
    if(icon_theme.size() > 0){ icon_theme.at(0) += ('A' - 'a'); }
    lockIcon = QIcon{path.absolutePath() +
        "/img/lock/lock" + QString::fromStdString(icon_theme) + ".png"};
    rightArrowIcon = QIcon{path.absolutePath() +
        "/img/arrow/rightArrow" + QString::fromStdString(icon_theme) + ".png"};
    leftArrowIcon = QIcon{path.absolutePath() +
        "/img/arrow/leftArrow" + QString::fromStdString(icon_theme) + ".png"};
    circleArrowIcon = QIcon{path.absolutePath() +
        "/img/arrow/circleArrow" + QString::fromStdString(icon_theme) + ".png"};
    warningIcon = QIcon{path.absolutePath() +
        "/img/warning/warning" + QString::fromStdString(icon_theme) + ".png"};
    homeIcon = QIcon{path.absolutePath() +
        "/img/home/home" + QString::fromStdString(icon_theme) + ".png"};
    settingIcon = QIcon{path.absolutePath() +
        "/img/setting/setting" + QString::fromStdString(icon_theme) + ".png"};
    starIcon = QIcon{path.absolutePath() +
        "/img/star/star" + QString::fromStdString(icon_theme) + ".png"};
    starHoleIcon = QIcon{path.absolutePath() +
        "/img/star/starHole" + QString::fromStdString(icon_theme) + ".png"};

    QFile f{path.absolutePath() + "/log/basilweb.json"};
    if(f.open(QIODevice::ReadOnly))
    {
        logJson = QJsonDocument{QJsonDocument::fromJson(f.readAll())}.object();
    }
    f.close();

    setSizePolicy(QSizePolicy{QSizePolicy::Expanding, QSizePolicy::Expanding});

    settingTab();

    settingViewer();

    settingAddButton();
}

BasilWeb::BasilWeb(Basilico* basil, String url):
    BasilWeb{basil}
{
    static_cast<WebViewer*>(Tab.currentWidget())->load(QString::fromStdString(url));
}

std::pair<BasilWeb*, String> BasilWeb::factory(Basilico* basil, Array args)
{
    if(args.size() <= 2)
    {
        return std::make_pair(new BasilWeb{basil}, "tab");
    }
    else
    {
        String url = boost::get<String>(args.at(2));
        return std::make_pair(new BasilWeb{basil, url}, "tab");
    }
}

bool BasilWeb::eventFilter(QObject* obj, QEvent* e)
{
    if(this == obj)
    {
        switch(e->type())
        {
            case QEvent::MouseButtonPress:
                if(static_cast<QMouseEvent*>(e)->button() == Qt::ForwardButton)
                {
                    static_cast<WebViewer*>(Tab.currentWidget())->forward();
                    return true;
                }
                else if(static_cast<QMouseEvent*>(e)->button() == Qt::BackButton)
                {
                    static_cast<WebViewer*>(Tab.currentWidget())->back();
                    return true;
                }
                else
                {
                    return false;
                }
            default:
                return false;
        }
    }
    return false;
}

std::vector<String> BasilWeb::selectedTextVector()
{
    std::vector<String> textVector;
    textVector.reserve(Tab.count());
    for(int i = 0;i < Tab.count();i++)
    {
        textVector.push_back(static_cast<WebViewer*>(Tab.widget(i))->selectedText());
    }
    return textVector;
}

void BasilWeb::execute(Basilico* basil, Array args)
{
    if(boost::get<String>(args.at(1)) == "selectedTextVector")
    {
        auto list = selectedTextVector();
        Array info;
        for(auto&& c:list)
        {
            info.push_back(Object{c});
        }
        basil->getNeoVim().nvim_set_var(
                "basilweb#selected_text_list", info);
    }
}

void BasilWeb::settingTab()
{
    auto&& objStyleSheet = basil->getNeoVim().nvim_get_var("basilweb#tab_style_sheet");
    auto&& styleSheet = boost::get<String>(objStyleSheet);
    Tab.setStyleSheet(QString::fromStdString(styleSheet));
    Tab.setTabsClosable(true);
    Tab.setMovable(true);
    Tab.setCornerWidget(&addButton);
    connect(&Tab, &QTabWidget::tabCloseRequested, this,
            [&](int idx){
            if(Tab.count() == 1){ return; }
            Tab.removeTab(idx);
            });
}

void BasilWeb::settingViewer()
{
    auto&& obj_default_url = basil->getNeoVim().nvim_get_var("basilweb#default_url");
    default_url = boost::get<String>(obj_default_url);
    auto* Viewer = new WebViewer{basil, &Tab, QString::fromStdString(default_url)};

    auto&& pobjStyleSheet = basil->getNeoVim().nvim_get_var("basilweb#progressbar_style_sheet");
    viewerProgressBarStyleSheet = boost::get<String>(pobjStyleSheet);
    Viewer->progressbarSetStyleSheet(viewerProgressBarStyleSheet);

    auto&& tobjstyleSheet = basil->getNeoVim().nvim_get_var("basilweb#toolbar_style_sheet");
    viewerToolBarStyleSheet = boost::get<String>(tobjstyleSheet);
    Viewer->toolbarSetStyleSheet(viewerToolBarStyleSheet);

    Viewer->setParent(&Tab);
    Tab.addTab(Viewer, Viewer->icon(), Viewer->title());
}

void BasilWeb::settingAddButton()
{
    connect(&addButton, &QPushButton::clicked, &Tab,
            [&](){
                auto* Viewer = new WebViewer{basil, &Tab, QString::fromStdString(default_url)};
                Viewer->progressbarSetStyleSheet(viewerProgressBarStyleSheet);
                Viewer->toolbarSetStyleSheet(viewerToolBarStyleSheet);
                Viewer->setParent(&Tab);
                int idx = Tab.addTab(Viewer, Viewer->icon(), Viewer->title());
                Tab.setCurrentIndex(idx);
            });
}
