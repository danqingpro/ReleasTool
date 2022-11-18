#include "releasetool.h"
#include "ui_releasetool.h"
#include <QDebug>
ReleaseTool::ReleaseTool(QWidget *parent) :
    QMainWindow(parent),
    m_settings(QDir::currentPath ()+"/config.ini",QSettings::IniFormat),
    ui(new Ui::ReleaseTool)
{
    ui->setupUi(this);
    Init();
}

ReleaseTool::~ReleaseTool()
{
    delete ui;
}

void ReleaseTool::Init()
{
    SysTemInit();

    TitleInit();
#ifdef QT_DEBUG
    qDebug()<<ui->tEdtDisPlay->document ()->lineCount();
#endif
}

void ReleaseTool::SysTemInit()
{
    resize (1000,800);

    //接受拖拽事件
    this->setAcceptDrops (true);
//    m_settings.setValue (COMPILER_PATH,QVariant("test"));
//    qDebug()<<m_settings.value (COMPILER_PATH).toString ()<<QDir::currentPath ();

    ui->lEdtCompilerPath->setText (m_settings.value (COMPILER_PATH).toString ());
}

void ReleaseTool::TitleInit()
{
    //<去掉浮动窗口的标题栏>
    if(ui->dockWidget != nullptr)
    {
        QWidget* lTitleBar = ui->dockWidget->titleBarWidget();
        QWidget* lEmptyWidget = new QWidget();
        ui->dockWidget->setTitleBarWidget(lEmptyWidget);
        delete lTitleBar;
    }
    //</去掉浮动窗口的标题栏>

    m_timer = new QTimer(this);
    m_timer->setInterval (1000);

    if(ui->lblTitle != nullptr)
    {
        ui->lblTitle->setText (QStringLiteral("QT 应用程序打包工具"));
        ui->lblTime->setText (QDateTime::currentDateTime ().toString ("yyyy-MM-dd hh:mm:ss"));
        connect (m_timer,&QTimer::timeout,[=]()
        {
            ui->lblTime->setText (QDateTime::currentDateTime ().toString ("yyyy-MM-dd hh:mm:ss"));
        });
        m_timer->start ();
    }

}

bool ReleaseTool::GetQtPath(QString &qtPath)
{
//    QString var =  QProcessEnvironment::systemEnvironment ().value ("PATH");
//    if(var.isEmpty ())
//    {
//        if(ui->tEdtDisPlay != nullptr)
//        {
//            ui->tEdtDisPlay->appendPlainText (QStringLiteral("环境变量配置错误"));
//        }
//        return false;
//    }
//    //var.replace ("\\","/");
//    QStringList list = var.split (";");
//    foreach (QString str, list)
//    {
//        if(str.contains (QT_VERSION_STR) && str.contains ("bin"))
//        {
//            qtPath = str;
//            break;
//        }
//    }
//    if(qtPath.isEmpty ())
//    {
//        return false;
//    }
    if(ui->lEdtCompilerPath->text ().isEmpty ())return false;
    qtPath = ui->lEdtCompilerPath->text ();
    return true;
}

bool ReleaseTool::GetBatContent(QString &qtenv2Path, QStringList &batCmds)
{
    QFile file(qtenv2Path);
    //ui->tEdtDisPlay->appendPlainText (qtenv2Path);
    if(!file.exists ())
    {
        if(ui->tEdtDisPlay != nullptr)
        {
            ui->tEdtDisPlay->appendPlainText (QStringLiteral("编译器路径错误"));
        }
        return false;
    }
    if (!file.open(QFile::ReadOnly | QFile::Text))return false;
    while (!file.atEnd())
    {
        batCmds << QString(file.readLine());
    }
    file.close();
    return true;
}

bool ReleaseTool::AddBatContent(QStringList &batCmds, QFileInfo &info)
{
    batCmds.append(info.filePath().mid(0, 2));
    batCmds.append("\r\n");
    batCmds.append("echo Enter disk " + info.filePath().mid(0, 2));
    batCmds.append("\r\n");
    batCmds.append("cd " + info.absolutePath());
    batCmds.append("\r\n");
    batCmds.append("echo Enter " + info.absolutePath());
    batCmds.append("\r\n");
    batCmds.append("windeployqt " + info.fileName());
    batCmds.append("\r\n");
    batCmds.append("echo "+info.fileName()+"release success");
    return true;
}

bool ReleaseTool::WriteBatFile(QString &toolBatPath,QStringList &batCmds)
{
    QFile toolFile(toolBatPath);
#ifdef QT_DEBUG
    qDebug()<<toolBatPath;
#endif
    if(!toolFile.open (QFile::WriteOnly)) return false;
    QTextStream in(&toolFile);
    foreach (QString cmd,batCmds)
    {
        in << cmd;
    }
    toolFile.close();
    return true;
}

bool ReleaseTool::ExecuteCmd(QString &cmd)
{
    if(ui->tEdtDisPlay != nullptr)
    {
        ui->tEdtDisPlay->appendPlainText (QDateTime::currentDateTime ()
                                          .toString ("yyyy-MM-dd hh:mm:ss.zzz")+
                                          " start...");
    }
    QProcess p(0);
    p.start(cmd);
    p.waitForStarted();
    p.waitForFinished();
    if(ui->tEdtDisPlay != nullptr)
    {
        QString text = QString::fromLocal8Bit(p.readAllStandardOutput());
        if(text.isEmpty ())
        {
            ui->tEdtDisPlay->appendPlainText (QStringLiteral("打包应用程序路径错误"));
        }
        return false;
        text.remove (text.right (4));
        ui->tEdtDisPlay->appendPlainText (text);
        ui->tEdtDisPlay->appendPlainText (QDateTime::currentDateTime ()
                                          .toString ("yyyy-MM-dd hh:mm:ss.zzz")+
                                          " ...end\n");
    }
    return true;
}

QStringList ReleaseTool::GetFileList(const QString &path, QString arg)
{
    QDir dir(path);
    QFileInfoList file_folder = dir.entryInfoList(QDir::Dirs | QDir::Files  | QDir::NoDotAndDotDot );
    QStringList files;
    for(int i = 0; i != file_folder.size(); i++)
    {
        QString name = file_folder[i].absoluteFilePath();
        if(name.endsWith(arg, Qt::CaseInsensitive)) {
            files.append(file_folder[i].absoluteFilePath());
        } else if(file_folder[i].isDir()) {
            files.append(GetFileList(name,arg));
        }
    }

    return files;
}


void ReleaseTool::dragEnterEvent(QDragEnterEvent *ev)
{
    if(!ev->mimeData()->urls()[0].toLocalFile().right(3).compare("exe",Qt::CaseInsensitive))
    {
        ev->acceptProposedAction();//接受鼠标拖入事件
    }
    else
    {
        ev->ignore();//否则不接受鼠标事件
    }
}

void ReleaseTool::dropEvent(QDropEvent *ev)
{
    //窗口部件放下一个对象时,调用该函数
    const QMimeData *qm=ev->mimeData();//获取MIMEData
    foreach (QUrl url, qm->urls ())
    {
        if(ui->lEdtExePath != nullptr)
        {
            ui->lEdtExePath->setText (url.toString ().remove (0,8));
        }
    }
}

void ReleaseTool::on_pBtnGetMsvcPath_clicked()
{
    QString qtPath;
    QString qtenv2Path;
    QStringList batCmds;

    //获取Qt 环境变量 目的是为了找到安装qt 路径下qtenv2.bat文件
    if(!GetQtPath (qtPath))
    {
        if(ui->tEdtDisPlay != nullptr)
        {
            ui->tEdtDisPlay->appendPlainText (QStringLiteral("获取Qt 编译器安装路径错误"));
        }
        return;
    }
    qtenv2Path.append (qtPath);
    qtenv2Path.append ("/qtenv2.bat");

    //获取qtenv2.bat内容
    if(!GetBatContent(qtenv2Path, batCmds))
    {
        if(ui->tEdtDisPlay != nullptr)
        {
            ui->tEdtDisPlay->appendPlainText (QStringLiteral("获取qtenv2.bat内容错误"));
        }
        return;
    }
    QString exePath = ui->lEdtExePath->text ();
    QFileInfo info(exePath);

    //batData 追加exePath内容
    if(exePath.isEmpty ())
    {
        if(ui->tEdtDisPlay != nullptr)
        {
            ui->tEdtDisPlay->appendPlainText (QStringLiteral("请先加载*.exe文件"));
        }
        return;
    }
    else
    {
        if(!AddBatContent(batCmds, info))
        {
            if(ui->tEdtDisPlay != nullptr)
            {
                ui->tEdtDisPlay->appendPlainText (QStringLiteral("追加bat内容错误"));
            }
            return;
        }
    }

    QString toolBatPath;
    toolBatPath = info.absolutePath() + "/tool.bat";
    //写入bat文件
    if(!WriteBatFile(toolBatPath,batCmds))
    {
        if(ui->tEdtDisPlay != nullptr)
        {
            ui->tEdtDisPlay->appendPlainText (QStringLiteral("写入bat文件内容错误"));
        }
        return;
    }

    //执行cmd
    if(!ExecuteCmd(toolBatPath))
    {
        if(ui->tEdtDisPlay != nullptr)
        {
            ui->tEdtDisPlay->appendPlainText (QStringLiteral("执行cmd错误\n打包文件是否包含中文路径或空格"));
        }
        return;
    }
    QFile::remove(toolBatPath);
#ifdef QT_DEBUG
    qDebug()<<ui->tEdtDisPlay->document ()->lineCount();
#endif
}

void ReleaseTool::on_actionAboutQt_triggered()
{
    QMessageBox::aboutQt (this,QStringLiteral("关于Qt"));
}

void ReleaseTool::on_actionExit_triggered()
{
    exit (0);
}

void ReleaseTool::on_actionRelease_triggered()
{
    on_pBtnGetMsvcPath_clicked();
}

void ReleaseTool::on_actionAboutReleaseTool_triggered()
{
    QMessageBox::about (this,QStringLiteral("关于发行工具"),QStringLiteral("Qt 应用程序打包工具第一版"));
}

void ReleaseTool::on_actionImport_triggered()
{
    QString text =QFileDialog::getOpenFileName (this,QStringLiteral("导入"),QDir::currentPath (),"EXE(*.exe)");
    if(ui->lEdtExePath!=nullptr)
    {
        ui->lEdtExePath->setText (text);
    }
    else
    {
        if(ui->tEdtDisPlay!=nullptr)
        {
            ui->tEdtDisPlay->appendPlainText (QStringLiteral("导入错误"));
        }
        return;
    }

}

void ReleaseTool::on_pBtnLoad_clicked()
{
    QString text =QFileDialog::getExistingDirectory (this,QStringLiteral("导入"),"../");
    if(ui->lEdtCompilerPath!=nullptr)
    {
        ui->lEdtCompilerPath->setText (text);
        m_settings.setValue (COMPILER_PATH,QVariant(text));
    }
    else
    {
        if(ui->lEdtCompilerPath!=nullptr)
        {
            ui->tEdtDisPlay->appendPlainText (QStringLiteral("加载编译器安装路径错误"));
        }
        return;
    }
}
