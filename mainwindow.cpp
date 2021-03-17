#include "mainwindow.h"

void MainWindow::set_plugins_table()
{
    QDir plugins_dir(m_data_dir_path+slash+"plugins");
    QStringList local_plugins = plugins_dir.entryList(QDir::Filter::Files);

    m_remote_stuff_tw->setRowCount(0);
    m_remote_stuff_tw->setColumnCount(2);
    m_remote_stuff_tw->setHorizontalHeaderLabels(QStringList() << "Name" << "Description");
    int j = 0;
    for(const QString &key : m_remote_plugins.keys())
    {
        m_remote_stuff_tw->insertRow(j);
        m_remote_stuff_tw->setItem(j, 0, new QTableWidgetItem(key));
        m_remote_stuff_tw->setItem(j, 1, new QTableWidgetItem(m_remote_plugins[key]));
        ++j;
    }
    m_remote_stuff_tw->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    m_remote_stuff_tw->horizontalHeader()->setStretchLastSection(true);

    m_local_stuff_tw->setRowCount(0);
    m_local_stuff_tw->setColumnCount(1);
    m_local_stuff_tw->setHorizontalHeaderLabels(QStringList() << "Name");

    for(int i = 0; i < local_plugins.size(); ++i)
    {
        m_local_stuff_tw->insertRow(i);
        m_local_stuff_tw->setItem(i, 0, new QTableWidgetItem(local_plugins.at(i)));
    }

    m_local_stuff_tw->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    m_local_stuff_tw->horizontalHeader()->setStretchLastSection(true);
}

void MainWindow::set_themes_table()
{
    QDir plugins_dir(m_data_dir_path+slash+"user"+slash+"colors");
    QStringList local_plugins = plugins_dir.entryList(QDir::Filter::Files);

    m_remote_stuff_tw->setRowCount(0);
    m_remote_stuff_tw->setColumnCount(1);
    m_remote_stuff_tw->setHorizontalHeaderLabels(QStringList() << "Name");
    for(int j = 0; j < m_remote_themes.size(); ++j)
    {
        m_remote_stuff_tw->insertRow(j);
        m_remote_stuff_tw->setItem(j, 0, new QTableWidgetItem(m_remote_themes.at(j)));
    }
    m_remote_stuff_tw->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    m_remote_stuff_tw->horizontalHeader()->setStretchLastSection(true);

    m_local_stuff_tw->setRowCount(0);
    m_local_stuff_tw->setColumnCount(1);
    m_local_stuff_tw->setHorizontalHeaderLabels(QStringList() << "Name");

    for(int i = 0; i < local_plugins.size(); ++i)
    {
        m_local_stuff_tw->insertRow(i);
        m_local_stuff_tw->setItem(i, 0, new QTableWidgetItem(local_plugins.at(i)));
    }

    m_local_stuff_tw->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    m_local_stuff_tw->horizontalHeader()->setStretchLastSection(true);
}

void MainWindow::get_remote_plugins_info()
{
    QNetworkReply *response = m_network_manager.get(QNetworkRequest(QUrl("https://raw.githubusercontent.com/rxi/lite-plugins/master/README.md")));
    QEventLoop event;
    connect(response, SIGNAL(finished()), &event, SLOT(quit()));
    event.exec();
    QString html = response->readAll();

    QStringList names_list;

    int pos = html.indexOf("[`");
    while(pos != -1)
    {
        int start_name_ind = pos + 2;
        int end_name_ind = html.indexOf("`]", start_name_ind);
        int start_disc_ind = html.indexOf("| ", start_name_ind) + 2;
        int end_disc_ind = html.indexOf("\n", start_disc_ind);
        QString name = html.mid(start_name_ind, end_name_ind - start_name_ind);
        QString descr = html.mid(start_disc_ind, end_disc_ind - start_disc_ind);
        descr = descr.
                remove(QRegExp("\\*.*\\*")).
                remove('`').remove('[').remove(']').
                remove(QRegExp("\\(https.*\\)"));
        m_remote_plugins.insert(name, descr);
        pos = html.indexOf("[`", end_disc_ind);
    }
}

void MainWindow::get_remote_themes_info()
{
    QNetworkReply *response = m_network_manager.get(QNetworkRequest(QUrl("https://raw.githubusercontent.com/rxi/lite-colors/master/README.md")));
    QEventLoop event;
    connect(response, SIGNAL(finished()), &event, SLOT(quit()));
    event.exec();
    QString html = response->readAll();
    html = html.remove(QRegExp("\\*.*\\*"));

    QStringList names_list;

    int pos = html.indexOf("[`");
    while(pos != -1)
    {
        int start_name_ind = pos + 2;
        int end_name_ind = html.indexOf("`]", start_name_ind);
        QString name = html.mid(start_name_ind, end_name_ind - start_name_ind);
        m_remote_themes.push_back(name);
        pos = html.indexOf("[`", end_name_ind);
    }
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setWindowTitle("Lite plugins/themes manager");

    m_main_widget_wgt = new QWidget(this);
    m_main_layout_gl = new QGridLayout(m_main_widget_wgt);

    m_remote_stuff_tw = new QTableWidget(this);
    m_local_stuff_tw = new QTableWidget(this);
    m_stuff_type_cb = new QComboBox(this);
    m_add_stuff_pb = new QPushButton("->", this);
    m_reinstall_all_local_stuff_pb = new QPushButton("Update all local stuff", this);
    m_remove_pb = new QPushButton("Remove local stuff", this);
    m_remote_filter_le = new QLineEdit(this);
    m_local_filter_le = new QLineEdit(this);

    m_main_layout_gl->addWidget(m_remote_filter_le, 0, 0);
    m_main_layout_gl->addWidget(m_local_filter_le, 0, 2);
    m_main_layout_gl->addWidget(m_stuff_type_cb, 0, 1);
    m_main_layout_gl->addWidget(m_remote_stuff_tw, 1, 0, 3, 1);
    m_main_layout_gl->addWidget(m_add_stuff_pb, 1, 1);
    m_main_layout_gl->addWidget(m_local_stuff_tw, 1, 2, 3, 1);
    m_main_layout_gl->addWidget(m_reinstall_all_local_stuff_pb, 2, 1);
    m_main_layout_gl->addWidget(m_remove_pb, 3, 1);

    m_stuff_type_cb->addItems(QStringList() << "Plugins" << "Themes");
    m_stuff_type_cb->setCurrentIndex(0);

    m_local_stuff_tw->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_local_stuff_tw->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_remote_stuff_tw->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_remote_stuff_tw->setSelectionBehavior(QAbstractItemView::SelectRows);

    m_main_widget_wgt->setLayout(m_main_layout_gl);
    setCentralWidget(m_main_widget_wgt);

    connect(m_stuff_type_cb, SIGNAL(currentIndexChanged(int)), this,SLOT(update_tables(int)));
    m_data_dir_path = QFileDialog::getExistingDirectory(0, "Lite data dir path", "/");

    slash = QSysInfo::productType() == "windows" ? "\\" : "/";

    get_remote_plugins_info();
    get_remote_themes_info();
    update_tables(0);

    connect(m_remove_pb, SIGNAL(clicked()), this, SLOT(remove_local_stuff()));
    connect(m_add_stuff_pb, SIGNAL(clicked()), this, SLOT(add_stuff()));
    connect(m_remote_filter_le, SIGNAL(textChanged(const QString &)), this, SLOT(use_remote_filter(const QString &)));
    connect(m_local_filter_le, SIGNAL(textChanged(const QString &)), this, SLOT(use_local_filter(const QString &)));
    connect(m_reinstall_all_local_stuff_pb, SIGNAL(clicked()), this, SLOT(reinstall_all_local_stuff()));
}

MainWindow::~MainWindow()
{
}

void MainWindow::update_tables(int type_index)
{
    m_remote_stuff_tw->clear();
    m_local_stuff_tw->clear();
    if(type_index == 0)
        set_plugins_table();
    else if(type_index == 1)
        set_themes_table();
}

void MainWindow::remove_local_stuff()
{
    QString file_path = m_data_dir_path;
    int type_ind = m_stuff_type_cb->currentIndex();
    if(type_ind == 0)
        file_path +=slash+"plugins"+slash;
    else if(type_ind == 1)
        file_path +=slash+"user" + slash + "colors" + slash;
    QModelIndexList inds = m_local_stuff_tw->selectionModel()->selectedRows();
    if(inds.size() == 0)
    {
        QMessageBox::warning(0, "Remove", "Please select stuff for remove");
        return;
    }
    QStringList stuffs;
    QString sh;
    for(const QModelIndex &i : inds)
    {
         stuffs.push_back(m_local_stuff_tw->item(i.row(), 0)->text());
         sh += m_local_stuff_tw->item(i.row(), 0)->text() + "\n";
    }

    if(QMessageBox::question(0, "Remove", QString("Remove stuffs?:\n%1").arg(sh)) == QMessageBox::Button::Yes)
    {
        for(const QString &s : stuffs)
        {
            if(!QFile::remove(file_path + s))
            {
                QMessageBox::warning(0, "Remove", "Can't remove stuff");
                return;
            }
        }
    }
    update_tables(type_ind);
}

void MainWindow::add_stuff()
{    
    QString url = "https://raw.githubusercontent.com/rxi/lite";
    QString file_path = m_data_dir_path;
    int type_ind = m_stuff_type_cb->currentIndex();
    if(type_ind == 0)
    {
        file_path +=slash+"plugins"+slash;
        url += "-plugins/master/plugins/";
    }
    else if(type_ind == 1)
    {
        file_path +=slash+"user"+slash+"colors"+slash;
        url += "-colors/master/colors/";
    }
    QModelIndexList inds = m_remote_stuff_tw->selectionModel()->selectedRows();
    if(inds.size() == 0)
    {
        QMessageBox::warning(0, "Add", "Please select stuff for add");
        return;
    }
    if(QMessageBox::question(0, "Add", "Warning This operation may overwrite existing material.\n"
                             "Do you want to continue?") == QMessageBox::Button::No)
        return;

    QStringList stuffs;
    for(const QModelIndex &i : inds)
         stuffs.push_back(m_remote_stuff_tw->item(i.row(), 0)->text());

    QMessageBox msg(QMessageBox::Icon::Information, "Download", "Wait for the download to complete");
    msg.show();
    for(const QString &s : stuffs)
    {
        QNetworkReply *response = m_network_manager.get(QNetworkRequest(QUrl(url + s + ".lua")));
        QEventLoop event;
        connect(response, SIGNAL(finished()), &event, SLOT(quit()));
        event.exec();
        QString data = response->readAll();

        QFile stuff(file_path + s + ".lua");
        if(!stuff.open(QIODevice::WriteOnly))
        {
            QMessageBox::warning(0, "Add", "Can't add stuff");
            return;
        }
        stuff.write(data.toUtf8());
        stuff.close();
    }
    update_tables(type_ind);
}

void MainWindow::use_remote_filter(const QString &text)
{
    if(text == "")
    {
        update_tables(m_stuff_type_cb->currentIndex());
        return;
    }
    for(int i = 0; i < m_remote_stuff_tw->rowCount(); ++i)
    {
        bool exist = true;
        for(int j = 0; j< m_remote_stuff_tw->columnCount(); ++j)
        {
            exist &= m_remote_stuff_tw->item(i, j)->text().contains(text);
            if(exist) break;
        }
        if(!exist)
            m_remote_stuff_tw->hideRow(i);
    }
}

void MainWindow::use_local_filter(const QString &text)
{
    if(text == "")
    {
        update_tables(m_stuff_type_cb->currentIndex());
        return;
    }
    for(int i = 0; i < m_local_stuff_tw->rowCount(); ++i)
    {
        bool exist = true;
        for(int j = 0; j< m_local_stuff_tw->columnCount(); ++j)
        {
            exist &= m_local_stuff_tw->item(i, j)->text().contains(text);
            if(exist) break;
        }
        if(!exist)
            m_local_stuff_tw->hideRow(i);
    }
}

void MainWindow::reinstall_all_local_stuff()
{
    QString url = "https://raw.githubusercontent.com/rxi/lite";
    QString file_path = m_data_dir_path;
    int type_ind = m_stuff_type_cb->currentIndex();
    if(type_ind == 0)
    {
        file_path +=slash+"plugins"+slash;
        url += "-plugins/master/plugins/";
    }
    else if(type_ind == 1)
    {
        file_path +=slash+"user"+slash+"colors"+slash;
        url += "-colors/master/colors/";
    }
    if(QMessageBox::question(0, "ReDownload", "Warning This operation may overwrite existing material.\n"
                             "Do you want to continue?") == QMessageBox::Button::No)
        return;

    QStringList remote_stuffs;
    QStringList local_stuffs;
    QStringList for_update;
    QString sh;
    for(int i = 0; i < m_remote_stuff_tw->rowCount(); ++i)
        remote_stuffs.push_back(m_remote_stuff_tw->item(i, 0)->text());
    for(int i = 0; i < m_local_stuff_tw->rowCount(); ++i)
        local_stuffs.push_back(m_local_stuff_tw->item(i, 0)->text());
    for(const QString &i : remote_stuffs)
    {
        if(local_stuffs.indexOf(i + ".lua") != -1)
        {
            for_update.push_back(i);
            sh += i + "\n";
        }
    }
    if(QMessageBox::question(0, "ReDownload", QString("Update these stuffs?:\n%1").arg(sh)) == QMessageBox::Button::No)
        return;
    QMessageBox msg(QMessageBox::Icon::Information, "Download", "Wait for the download to complete");
    msg.show();
    for(const QString &s : for_update)
    {
        QNetworkReply *response = m_network_manager.get(QNetworkRequest(QUrl(url + s + ".lua")));
        QEventLoop event;
        connect(response, SIGNAL(finished()), &event, SLOT(quit()));
        event.exec();
        QString data = response->readAll();

        QFile stuff(file_path + s + ".lua");
        if(!stuff.open(QIODevice::WriteOnly))
        {
            QMessageBox::warning(0, "Add", "Can't add stuff");
            return;
        }
        stuff.write(data.toUtf8());
        stuff.close();
    }
    update_tables(type_ind);
}
