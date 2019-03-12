
#include "BasilWeb.hpp"


// WebViewer class

// deque<QWebEngineDownloadItem*> WebViewer::downloaditem{};

WebViewer::WebViewer(Basilico* _basil, QTabWidget* tab, QString url):
    Web{},
    toolbar{this},
    urlline{&toolbar},
    default_url{QUrl{url}},
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

    settingWebBrowser();

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
    toolbar.addAction(QApplication::style()->standardIcon(QStyle::SP_ArrowBack),
                      "Back",
                      [&]{ Web.back(); });
    toolbar.addAction(QApplication::style()->standardIcon(QStyle::SP_ArrowForward),
                      "Forward",
                      [&]{ Web.forward(); });
    toolbar.addAction(QApplication::style()->standardIcon(QStyle::SP_BrowserReload),
                      "Reload",
                      [&]{ Web.reload(); });
    toolbar.addSeparator();
    toolbar.addWidget(&urlline);

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

void WebViewer::settingWebBrowser()
{
    layout->addWidget(&Web, 3, 0);
    setSizePolicy(QSizePolicy{QSizePolicy::Expanding, QSizePolicy::Expanding});
    connect(&Web, &QWebEngineView::loadFinished, this,
            [&](auto&& ok){ if(ok){
            auto&& url = Web.url().toString();
            urlline.setText(url);
                if(url.startsWith("http://"))
                {
                    urlline.addAction(
                            QApplication::style()->standardIcon(QStyle::SP_MessageBoxWarning),
                            QLineEdit::LeadingPosition);
                }
            } });

    connect(Web.page()->profile(), &QWebEngineProfile::downloadRequested, this,
            [&](auto&& item){
            QMessageBox msgbox;
            msgbox.setText("Download?");
            msgbox.setInformativeText(
                    "Download " + item->url().toString().toStdString() + "?");
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

// BasilWeb class

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
