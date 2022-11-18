#ifndef RELEASETOOL_H
#define RELEASETOOL_H

#include <QMainWindow>
#include <QProcessEnvironment>
#include <QEnterEvent>
#include <QDragEnterEvent>
#include <QMimeData>
#include <QUrl>
#include <QDir>
#include <QFile>
#include <QFileDialog>
#include <QFileInfo>
#include <QFileInfoList>
#include <QDirIterator>
#include <QProcess>
#include <QTimer>
#include <QDateTime>
#include <QMessageBox>
#include <QTextStream>
#include <QSettings>

const QString COMPILER_PATH = "Compiler path";

namespace Ui {
class ReleaseTool;
}

class ReleaseTool : public QMainWindow
{
    Q_OBJECT

public:
    explicit ReleaseTool(QWidget *parent = 0);
    ~ReleaseTool();

    //��ʼ��
    void Init();

    //ϵͳ��ʼ��
    void SysTemInit();

    //�����ʼ��
    void TitleInit();

    //��ȡqt ��װ·��
    bool GetQtPath(QString &qtPath);

    //��ȡbat����
    bool GetBatContent(QString &qtenv2Path, QStringList &batCmds);

    //���bat����
    bool AddBatContent(QStringList &batCmds, QFileInfo &info);

    //д��bat�ļ�
    bool WriteBatFile(QString &toolBatPath,QStringList &batCmds);

    //ִ��cmd����
    bool ExecuteCmd(QString &cmd);

    QStringList GetFileList(const QString &path,QString arg);

protected:
    void dragEnterEvent (QDragEnterEvent *ev);
    void dropEvent (QDropEvent *ev);

private slots:
    void on_pBtnGetMsvcPath_clicked();

    void on_actionAboutQt_triggered();

    void on_actionExit_triggered();

    void on_actionRelease_triggered();

    void on_actionAboutReleaseTool_triggered();

    void on_actionImport_triggered();

    void on_pBtnLoad_clicked();

private:
    Ui::ReleaseTool *ui;
    QDir m_dir;
    QTimer * m_timer;

    QSettings m_settings;
};

#endif // RELEASETOOL_H
