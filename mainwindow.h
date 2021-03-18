#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtWidgets>
#include <QMainWindow>
#include <QDir>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QRegularExpression>
#include <QSysInfo>

class MainWindow : public QMainWindow
{
    Q_OBJECT
private:
    QString m_data_dir_path;
    QString slash;

    QWidget *m_main_widget_wgt;
    QGridLayout *m_main_layout_gl;
    QTableWidget *m_remote_stuff_tw;
    QTableWidget *m_local_stuff_tw;
    QComboBox *m_stuff_type_cb;
    QPushButton *m_add_stuff_pb;
    QPushButton *m_reinstall_all_local_stuff_pb;
    QPushButton *m_remove_pb;
    QLineEdit *m_remote_filter_le;
    QLineEdit *m_local_filter_le;
    QAction *m_set_path;
    QAction *m_refresh;

    QMap<QString, QString> m_remote_plugins;
    QStringList m_remote_themes;

    void set_plugins_table();
    void set_themes_table();
    void get_remote_plugins_info();
    void get_remote_themes_info();

    QNetworkAccessManager m_network_manager;
public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
private slots:
    void update_tables();
    void remove_local_stuff();
    void add_stuff();
    void use_remote_filter(const QString &);
    void use_local_filter(const QString &);
    void reinstall_all_local_stuff();
    void set_path();
};
#endif // MAINWINDOW_H
