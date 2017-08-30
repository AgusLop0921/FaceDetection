#include "registro.h"
#include "ui_registro.h"

#include <QResizeEvent>
#include <QMessageBox>


#include <QDateTime>

Registro::Registro(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Registro)
{
    ui->setupUi(this);

    QPalette palette = this->palette();
    palette.setColor( QPalette::WindowText, QColor( 255, 255, 255 ) );
    this->setPalette( palette );

}

Registro::~Registro()
{
    ui->scene->apagarCamara();
    delete ui;
}


void Registro::closeEvent(QCloseEvent *e)
{
#ifdef Q_OS_ANDROID
    if(e->type() == QEvent::Close)  {

        int respuesta = QMessageBox::question( this, "Atención!", "¿Desea salir de la aplicación?");

        switch(respuesta)
        {
        case QMessageBox::Yes:
            ui->scene->apagarCamara();
            break;

        case QMessageBox::No:
            e->ignore();
//            ui->campoLegajo->setFoco( false );
            break;
        }
    }
#else
    ui->scene->apagarCamara();
#endif
}

void Registro::showEvent(QShowEvent *)
{
    ui->scene->encenderCamara();
}

void Registro::resizeEvent(QResizeEvent *)
{
    this->configurarWidgets();
}

void Registro::mousePressEvent(QMouseEvent *e)
{

}


void Registro::configurarWidgets()
{
    int altoBoton = this->height() / 10;
    int altoLabel = altoBoton;

#ifdef Q_OS_ANDROID
    int altoTextoLabel = altoLabel / 5;
#else
#endif

    int altoCamara = this->height() - altoBoton - altoLabel;
    int anchoPantalla = this->width();

    ui->scene->setGeometry( QRect( 0, altoLabel,
                                   anchoPantalla, altoCamara ) );


//    ui->pbRegistrar->setGeometry( QRect( borde,
//                                         altoPantalla - altoBoton,
//                                         this->width() - 2 * borde,
//                                         altoBoton ) );

}
