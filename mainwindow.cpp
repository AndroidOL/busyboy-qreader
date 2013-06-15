#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    this->setWindowTitle("BusyBoy.snap with qr[����H����ʾ����]");
    setMinimumSize(360, 80);
    setWindowIcon(QIcon(":images/icon"));
    clipboard = QApplication::clipboard();
    QxtGlobalShortcut *sc_z = new QxtGlobalShortcut(QKeySequence("Ctrl+Alt+Z"), this);
    connect(sc_z, SIGNAL(activated()),
            this, SLOT(onSnap()));
    QxtGlobalShortcut *sc_x = new QxtGlobalShortcut(QKeySequence("Ctrl+Alt+X"), this);
    connect(sc_x, SIGNAL(activated()),
            this, SLOT(onQr()));
    QxtGlobalShortcut *sc_s = new QxtGlobalShortcut(QKeySequence("Ctrl+Alt+S"), this);
    connect(sc_s, SIGNAL(activated()),
            this, SLOT(onSave()));

    label = new QLabel(this);
    infoLabel = new QLabel(this);
    helpMsg.append("����Ctrl+Alt+Z��ͼ\n");
    helpMsg.append("����Ctrl+Alt+Xʶ���ά��\n");
    helpMsg.append("����Ctrl+Alt+S����ͼƬ\n");
    helpMsg.append("����h��ʾ������\n");
    helpMsg.append("Esc�˳�������\n");
    infoLabel->setText(helpMsg);
    infoLabel->adjustSize();
    isOnSnapping = false;
    rubberBand = new QRubberBand(QRubberBand::Rectangle, label);
    isLeftButtonPressed = false;

    iMsg.append("ԭ������һ������ݰ�����!");
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::keyPressEvent(QKeyEvent *ev){
    if(ev->key() == Qt::Key_Escape){
        if(isOnSnapping == true){
            this->showNormal();
            isOnSnapping = false;
        }else if(isOnSnapping == false){
            QApplication::exit(0);
        }
    }else if(ev->key() == Qt::Key_H){
        this->showNormal();
        label->setText(helpMsg);
        label->adjustSize();
        infoLabel->hide();
        this->adjustSize();
    }else if(ev->key() == Qt::Key_I){
        QMessageBox::about(this,"i", iMsg);
    }
}

void MainWindow::onSnap(){
    fullScreen = QPixmap::grabWindow(QApplication::desktop()->winId());
    this->showNormal();
    this->showFullScreen();
    label->setPixmap(fullScreen);
    label->show();
    label->adjustSize();
    isOnSnapping = true;

}

void MainWindow::mouseMoveEvent(QMouseEvent *ev){
    QPoint pos = ev->pos();

    // ��ͼ״̬��ʾ��ͼ��Ϣ
    if(isOnSnapping){
        int x = pos.x();
        int y = pos.y();
        int w = infoLabel->width();
        int h = infoLabel->height();

        QString info = "";
        info.append("x:").append(QString::number(start.x()));
        info.append("\ny:").append(QString::number(start.y()));
        info.append("\n*\nwidth:").append(QString::number(pos.x() - start.x()));
        info.append("\nheight:").append(QString::number(pos.y() - start.y()));

        infoLabel->setText(info);
        infoLabel->setGeometry(QRect(x, y, w, h));
        infoLabel->adjustSize();
        infoLabel->setStyleSheet("background:#eaeaea;");
        infoLabel->show();
    }else{
        infoLabel->hide();
    }
    if(ev->type() == QEvent::MouseMove&&
            isLeftButtonPressed){
        if(rubberBand){
            rubberBand->setGeometry(QRect(start, ev->pos()).normalized());
            rubberBand->show();
        }
    }
}

void MainWindow::mousePressEvent(QMouseEvent *ev){
    if(ev->button() == Qt::LeftButton&&
            ev->type() == QEvent::MouseButtonPress&&
            isOnSnapping){
        isLeftButtonPressed = true;
        start = ev->pos();
        if(!rubberBand){
            rubberBand = new QRubberBand(QRubberBand::Rectangle,
                                         label);
        }
        rubberBand->setGeometry(QRect(start, QSize()));
        rubberBand->show();
    }
}
void MainWindow::mouseReleaseEvent(QMouseEvent *ev){
    if(isLeftButtonPressed&&
            isOnSnapping){
        isLeftButtonPressed = false;
        end = ev->pos();
        QPixmap pixmap = fullScreen.copy(QRect(start, end));
        clipboard->setImage(pixmap.toImage());
        this->showNormal();
        label->setPixmap(pixmap);
        label->adjustSize();
        this->resize(label->size());
        rubberBand->hide();
        isOnSnapping = false;
    }
}

void MainWindow::onQr(){
    QPixmap pixmap = fullScreen.copy(QRect(start, end));
    if(pixmap.isNull()){
        QMessageBox::warning(this, "��ʾ", "��δ��ͼ");
    }else{
        QZXing decoder;
        QString qrmsg = decoder.decodeImage(pixmap.toImage());
        if(qrmsg.isEmpty()){
            QMessageBox::warning(this, "ʶ��ʧ��", "���ȡ�����Ķ�ά��");
        }else{
            QMessageBox::information(this, "ʶ��ɹ�-�Ѿ����Ƶ����а�",qrmsg);
            clipboard->setText(qrmsg);
        }
    }
}

void MainWindow::onSave(){
    QPixmap pixmap = fullScreen.copy(QRect(start, end));
    if(pixmap.isNull()){
        QMessageBox::warning(this, "��ʾ", "��δ��ͼ");
    }else{
      QString savePath = QFileDialog::getSaveFileName(this,
                                                tr("Open Image"),
                                                ".",
                                                tr("Image Files(*.jpg,*.png,*.bmp)"));
      if(savePath.endsWith(".png") ||
              savePath.endsWith(".jpg") ||
              savePath.endsWith(".bmp")){

      }else{
          if(QFileInfo(savePath).fileName().isEmpty()){
              savePath += "image";
          }
          savePath.append(".png");
      }
      QFileInfo finfo(savePath);
      if(finfo.exists()){
          QMessageBox::StandardButton reply =
                  QMessageBox::question(this,"��ʾ","["+finfo.fileName()+"]�ļ��Ѿ������Ƿ�Ҫ����?",
                                        QMessageBox::Yes |
                                        QMessageBox::No);
          if(reply == QMessageBox::No){
              return;
          }
      }
      pixmap.save(savePath);
    }
}
