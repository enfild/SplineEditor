#include "tcpsplains.h"
#include "./ui_tcpsplains.h"

TCPsplains::TCPsplains(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::TCPsplains)
{
    ui->setupUi(this);
    ui->controlsHint->setAttribute(Qt::WA_TransparentForMouseEvents);
    ui->controlsHint->hide();
    ui->paramPanel->hide();

    connect(ui->actionUndo, &QAction::triggered, this, [this](){
        ui->splineView->undo();
    });

    connect(ui->actionRedo, &QAction::triggered, this, [this](){
        ui->splineView->redo();
    });

    connect(ui->actionNew, &QAction::triggered, this, [this](){
        ui->splineView->setSpline(Spline());
    });

    connect(ui->actionQuit, &QAction::triggered, this, [](){
        qApp->exit();
    });

    connect(ui->actionSave, &QAction::triggered, this, [this](){
        save();
    });

    connect(ui->actionLoad, &QAction::triggered, this, [this](){
        load();
    });

    connect(ui->actionInfo, &QAction::triggered, this, [this](){
        if(ui->controlsHint->isHidden())
        {
            ui->controlsHint->show();
        }
        else
        {
            ui->controlsHint->hide();
        }
    });

    connect(ui->splineView, &SplineView::dotSelected, this, [this](Spline::dot dot){
        ui->paramPanel->show();
        ui->xSpinBox->setValue(dot.x());
        ui->ySpinBox->setValue(dot.y());
        ui->tensionSpinBox->setValue(dot.tension);
        ui->biasSpinBox->setValue(dot.bias);
        ui->continuitySpinBox->setValue(dot.continuity);
    });

    connect(ui->splineView, &SplineView::dotDeselected, this, [this](){
        ui->paramPanel->hide();
    });

    connect(ui->xSpinBox, SIGNAL(valueChanged(double)), this, SLOT(updateDot()));
    connect(ui->ySpinBox, SIGNAL(valueChanged(double)), this, SLOT(updateDot()));
    connect(ui->tensionSpinBox, SIGNAL(valueChanged(double)), this, SLOT(updateDot()));
    connect(ui->biasSpinBox, SIGNAL(valueChanged(double)), this, SLOT(updateDot()));
    connect(ui->continuitySpinBox, SIGNAL(valueChanged(double)), this, SLOT(updateDot()));
}

TCPsplains::~TCPsplains()
{
    delete ui;
}

void TCPsplains::save()
{
    QString filename = QFileDialog::getSaveFileName(this, QString(), QString(), "Splines (*.spline)");
    if(filename.isEmpty())
    {
        return;
    }

    QFile file(filename);
    if(!file.open(QIODevice::WriteOnly))
    {
        QMessageBox::warning(this, tr("File save error"), tr("Can't open/create selected file"));
        return;
    }

    QTextStream stream(&file);
    for(const Spline::dot& dot : ui->splineView->getSpline().getDots())
    {
        QString str;
        stream << QString("%1").arg(dot.x()) << " ";
        stream << QString("%1").arg(dot.y()) << " ";
        stream << QString("%1").arg(dot.tension) << " ";
        stream << QString("%1").arg(dot.bias) << " ";
        stream << QString("%1").arg(dot.continuity) << "\n";
    }
}

void TCPsplains::load()
{
    QString filename = QFileDialog::getOpenFileName(this, QString(), QString(), "Splines (*.spline)");
    if(filename.isEmpty())
    {
        return;
    }

    QFile file(filename);
    if(!file.open(QIODevice::ReadOnly))
    {
        QMessageBox::warning(this, tr("File load error"), tr("Can't open selected file"));
        return;
    }

    Spline spline;
    bool ok = true;

    QTextStream stream(&file);
    QString line;
    while( !(line = stream.readLine()).isEmpty() )
    {
        QStringList dotParams = line.split(" ");
        if(dotParams.size() != 5)
        {
            QMessageBox::warning(this, tr("File format error"), tr("Wrong number of dot parameters"));
            return;
        }
        qreal x = dotParams[0].toDouble(&ok);
        if(!ok)
        {
            QMessageBox::warning(this, tr("File format error"), tr("Invalid X parameter"));
            return;
        }
        qreal y = dotParams[1].toDouble(&ok);
        if(!ok)
        {
            QMessageBox::warning(this, tr("File format error"), tr("Invalid Y parameter"));
            return;
        }
        float t = dotParams[2].toDouble(&ok);
        if(!ok)
        {
            QMessageBox::warning(this, tr("File format error"), tr("Invalid tension parameter"));
            return;
        }
        float b = dotParams[3].toDouble(&ok);
        if(!ok)
        {
            QMessageBox::warning(this, tr("File format error"), tr("Invalid bias parameter"));
            return;
        }
        float c = dotParams[4].toDouble(&ok);
        if(!ok)
        {
            QMessageBox::warning(this, tr("File format error"), tr("Invalid continuity parameter"));
            return;
        }

        spline.add(Spline::dot(x, y, t, b, c));
    }

    if(stream.status() != QTextStream::Ok)
    {
        QMessageBox::warning(this, tr("File read error"), tr("File read error"));
        return;
    }

    ui->splineView->setSpline(spline);
}

void TCPsplains::updateDot()
{
    Spline::dot dot(ui->xSpinBox->value(),
                      ui->ySpinBox->value(),
                      ui->tensionSpinBox->value(),
                      ui->biasSpinBox->value(),
                      ui->continuitySpinBox->value());
    ui->splineView->updateSelectedDot(dot);
}

