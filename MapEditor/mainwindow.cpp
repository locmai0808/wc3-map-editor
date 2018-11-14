#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "graphicsscene.h"
#include <QDebug>
#include "mapview2.h"
#include "exporttoweb.h"
#include "importpackage.h"
#include "dialogs/dgabout.h"
#include "dialogs/dgmapproperties.h"
#include "dialogs/dgplayerproperties.h"
#include "dialogs/dgassets.h"
#include "dialogs/dgaddtrigger.h"
#include "dialogs/dgimportpkg.h"
#include "aitrigger.h"
#include "newfile.h"
#include <QMediaPlayer>
#include <QDir>
#include "dialogs/dgnoprojectfound.h"
#include "quazip/JlCompress.h"

#define SAVE_PATH QStandardPaths::standardLocations(QStandardPaths::DocumentsLocation).first() + "/MapEditor"

RecordedTile::RecordedTile()
{

}

RecordedTile::RecordedTile(Terrain::Type u, Terrain::Type r, int bs, int a, int b){
    utype = u;
    rtype = r;
    brushSize = bs;
    x = a;
    y = b;
}

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
    // set up UI elements
    ui->setupUi(this);
    readSettings();
    ui->graphicsView->setMouseTracking(true);
    ui->graphicsView_2->setMouseTracking(true);
    curTool = "hand";
    undone = false;


    // Load and display a new file
    MainWindow::newFile();

    // Load all assets using
    MainWindow::setupAssets();

    MainWindow::setupUI();

    // resize minimap
    ui->graphicsView_2->fitInView(0,0,256,192, Qt::KeepAspectRatio);

    // connect signals and slots
    QObject::connect(scene, &GraphicsScene::changedLayout, this, &MainWindow::changeLayout);
    QObject::connect(scene, &GraphicsScene::changedAsset, this, &MainWindow::changeAsset);
    QObject::connect(scene, &GraphicsScene::open_DTrigger, this, &MainWindow::open_DTrigger);
    QObject::connect( ui->menuSound_Properties ,SIGNAL(triggered(QAction*)) , SLOT(subMenuAction(QAction*)) );
    QObject::connect(ui->menuAsset_Editor, SIGNAL(triggered(QAction*)), SLOT(submenuEditor(QAction*)));
    QObject::connect(scene, &GraphicsScene::changecursor, this, &MainWindow::changecursor);

    // default values
    curPlayer = 1;
    scene->curPlayer = 1;
    tileDim.setRect(1,1,32,32);
    // play background music
//    QMediaPlayer * backgroundMusic = new QMediaPlayer();
//    backgroundMusic->setMedia(QUrl("qrc:/data/snd/basic/annoyed2.wav"));
//    backgroundMusic->play();

}

MainWindow::~MainWindow()
{
    delete ui;
}

#ifndef QT_NO_CONTEXTMENU
void MainWindow::contextMenuEvent(QContextMenuEvent *event)
{
    if( event ) {

    }
    /*QMenu menu(this);
    menu.addAction(ui->actionOpen);
    menu.addAction(ui->actionSave);

    menu.exec(event->globalPos());
    */
}
#endif // QT_NO_CONTEXTMENU

void MainWindow::mousePressEvent(QMouseEvent *event){
    if (event->button() == Qt::LeftButton ){
    }
}

void MainWindow::mouseMoveEvent(QMouseEvent *event){
    if (event->button() == Qt::LeftButton ){
    }
}

void MainWindow::mouseReleaseEvent(QMouseEvent *event){
    if (event->button() == Qt::LeftButton ){
    }
}


void MainWindow::closeEvent(QCloseEvent *event)
{
    if (maybeSave()) {
        writeSettings();
        event->accept();
    } else {
        event->ignore();
    }
}

void MainWindow::newFile()
{
    if (maybeSave()) {
        // fill tile here
    }
    QString soundDat = ":/data/default/snd/SoundClips.dat";

    if (!skipNewFile)
    {
        NewFile *newfile = new NewFile();

        QObject::connect(newfile, &NewFile::changeProjectName, this, &MainWindow::changeProjectName);
        QObject::connect(newfile, &NewFile::open, this, &MainWindow::open);

        newfile->exec();

        shortProjectName = newfile->newProjectName;

        if (curProjectName == "")
            this->close();
    }
    else
    {
        curProjectName = ":data/default/";
    }

    sound = new Sound(soundDat ,curProjectName);

    // Set up the map grid
    curMap = MapView2(assets, curProjectName);
    scene = new GraphicsScene(this, &curMap,&assets, curProjectName);
    curMap.displayNewMap(scene);


    // show map + minimap
    ui->graphicsView->setScene(scene);
    ui->graphicsView->show();
    ui->graphicsView_2->setScene(scene);
    ui->graphicsView_2->show();

    // update status
    curPlayer = 1;
    scene->curPlayer = 1;
    ui->tool_p1->setChecked(true);
    ui->tool_hand->setChecked(true);
    on_tool_hand_clicked();
    statusBar()->showMessage("New File created", 2000);
}

bool MainWindow::open()
{
    QFileDialog dialog(this);
    dialog.setDirectory(curPath);
    dialog.setFileMode(QFileDialog::ExistingFile);
    dialog.setAcceptMode(QFileDialog::AcceptOpen);
    dialog.setNameFilter(tr("Map Files (*.map *.mpk *.zip)"));
    if (!maybeSave())
        return false;
    if(dialog.exec() != QDialog::Accepted) {
        curPath = dialog.directory().path();
        return false;
    }

    return loadFile(dialog.selectedFiles().first());
}

bool MainWindow::loadFile(QString &filename) {
    if(filename.split(".").last() == "map") {
        QFile file(filename);
        return loadMapFile(filename, file);
    } else
        return loadPkgFile(filename);
}

void MainWindow::loadScene() {
    scene = new GraphicsScene(this, &curMap, &assets, curProjectName);
    curMap.displayMap(scene);

    ui->graphicsView->setScene(scene);
    ui->graphicsView->setMouseTracking(true);
    ui->graphicsView->show();

    ui->graphicsView_2->setScene(scene);
    ui->graphicsView_2->setMouseTracking(true);
    ui->graphicsView_2->show();

    // connect signals and slots
    QObject::connect(scene, &GraphicsScene::changedLayout, this, &MainWindow::changeLayout);
    QObject::connect(scene, &GraphicsScene::changedAsset, this, &MainWindow::changeAsset);
    QObject::connect(scene, &GraphicsScene::open_DTrigger, this, &MainWindow::open_DTrigger);
    QObject::connect(scene, &GraphicsScene::changecursor, this, &MainWindow::changecursor);
}

QString MainWindow::checkValidProject(QString projectName)
{
    QString currentDirectory = SAVE_PATH;
    QString fullPath = currentDirectory +  "/" + projectName;

    QDir testDir(fullPath);

    if (testDir.exists())
        return fullPath;

    // when it gets here it means projectname is bad
    DgNoProjectFound w(projectName, this);
    w.exec();

    return w.newProjectDir;


}

bool MainWindow::loadMapFile(QString fileName, QIODevice &file)
{
    // check if the file is good
    if(!file.open(QIODevice::ReadOnly)) {
        QMessageBox::warning(0,"error opening map",file.errorString());
        return false;
    }

    QString line = file.readLine();
    line = file.readLine();
    file.close();

    QString validProject = checkValidProject(line);
    curProjectName = validProject;

    //file.open(QIODevice::ReadOnly);

    // load and display map and minimap
    QString mapName = fileName;
    QString texture = curProjectName + "/img/Terrain.png";


    try {
        curMap = MapView2(file, assets, curProjectName, texture);
    } catch (...) {
        return false;
    }

    MainWindow::loadScene();

    setCurrentFile(mapName);
    statusBar()->showMessage(mapName + " loaded!", 2000);

    // reset ui
    curPlayer = 1;
    scene->curPlayer = 1;
    ui->tool_p1->setChecked(true);
    ui->tool_hand->setChecked(true);
    on_tool_hand_clicked();

    return true;
}

// opens up .mpk files
bool MainWindow::loadPkgFile(const QString &pkgFileName){
    QuaZip qz(pkgFileName);
    if(!qz.open(QuaZip::mdUnzip)) {
        QMessageBox::warning(0,"error opening map","MPK file is corrupted.");
        return false;
    }

    qz.setCurrentFile("data.map");

    QuaZipFile qzFile(&qz);

    // check if the file is good
    if(!qzFile.open(QIODevice::ReadOnly)) {
        QMessageBox::warning(0,"error opening map",qzFile.errorString());
    }
    qzFile.close();

    loadMapFile(qzFile.getFileName(), qzFile );

    return true;
}


void MainWindow::setCurrentFile(const QString &fileName)
{
    curFile = fileName;

    QString shownName = curFile;
    if (curFile.isEmpty())
        shownName = "untitled.txt";
    setWindowFilePath(shownName);
}

bool MainWindow::maybeSave()
{
    //if (!ui->textEdit->document()->isModified()) return true;
    return true;
    const QMessageBox::StandardButton ret
        = QMessageBox::warning(this, tr("Application"),
                               tr("The document has been modified.\n"
                                  "Do you want to save your changes?"),
                               QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
    switch (ret) {
    case QMessageBox::Save:
        return save();
    case QMessageBox::Cancel:
        return false;
    default:
        break;
    }
    return true;
}

bool MainWindow::save()
{
    if (curFile.isEmpty()) { // set file to save to
        if (!setSaveFile(&curFile)) // if fails
            return false;
    }

    // check if able to write to file
    QFile file(curFile);
    if (!file.open(QFile::WriteOnly)) {
        QMessageBox::warning(this, "Application", file.errorString());
        return false;
    }

    writeMapFile(&file);

    setCurrentFile(curFile);
    statusBar()->showMessage(tr("File saved"), 2000);
    return true;
}

void MainWindow::undo()
{//Undo the most recent change
    //If there hasn't been any changes, then skip
    if(undoTiles.isEmpty())
        return;

    undone = true;
    RecordedTile rt = undoTiles.pop();
    redoTiles.push(rt);

    if(rt.rtype == Terrain::Water || rt.rtype == Terrain::Rock)
    {
        if(rt.utype != Terrain::Water && rt.utype != Terrain::Rock && rt.utype != Terrain::Grass)
        {
            rt.utype = Terrain::Grass;
            rt.brushSize = 1;
        }
    }
    else
        rt.brushSize = 1;

    scene->setBrushable(true);
    curMap.brush_size(scene, QPointF(rt.x, rt.y), rt.utype, rt.brushSize);
    changeLayout(rt.x, rt.y, rt.utype);

    bool popStack = true;
    while(!undoTiles.isEmpty())
    {//If the desired tiles are already where they should be, then remove from the undo stack
        rt = undoTiles.top();
        for(int i = 0; i <= (rt.brushSize-1)*32; i+=32)
        {
            for(int j = 0; j <= (rt.brushSize-1)*32; j+=32)
                if(!(rt.utype == getTileType
                    ((curMap.getMapLayout())[((rt.y+i)/32) * curMap.getMapDim().width() + (((rt.x+j))/32)])))
                {
                    popStack = false;
                    break;
                }
            if(!popStack)
                break;
        }

        if(popStack)
            undoTiles.pop();
        else
            break;
    }

    undone = false;
}

void MainWindow::redo()
{//Redo the most recent change
    //If there hasn't been any changes, then skip
    if(redoTiles.isEmpty())
        return;

    undone = true;
    RecordedTile rt = redoTiles.pop();
    undoTiles.push(rt);

    if(rt.rtype == Terrain::Water || rt.rtype == Terrain::Rock)
    {
        if(rt.utype != Terrain::Water && rt.utype != Terrain::Rock && rt.utype != Terrain::Grass)
        {
            rt.utype = Terrain::Grass;
            rt.brushSize = 1;
        }
    }
    else
        rt.brushSize = 1;

    scene->setBrushable(true);
    curMap.brush_size(scene, QPointF(rt.x, rt.y), rt.rtype, rt.brushSize);
    changeLayout(rt.x, rt.y, rt.rtype);
    undone = false;
}

void MainWindow::saveAs() {
    if (!setSaveFile(&curFile)) return;
    save();
}

bool MainWindow::setSaveFile(QString* fileName)
{
    if (curMap.getPlayers().size() < 3)
    {
        QMessageBox::warning(this, tr("Application"),
                             tr("Not enough players")
                             );
        return false;
    }

    QFileDialog dialog(this);
    dialog.setDirectory(curPath);
    dialog.setWindowModality(Qt::WindowModal);
    dialog.setAcceptMode(QFileDialog::AcceptSave);
    if (dialog.exec() != QDialog::Accepted) {
        curPath = dialog.directory().path();
        return false;
    }

    *fileName = dialog.selectedFiles().first();
    return true;
}


// this function takes an **opened** map file, writes everything that goes inside, and then closes it
void MainWindow::writeMapFile(QIODevice *file){

    QTextStream stream(file);

    // write descriptive properties
    stream << curMap.getMapName() << endl;
    stream << shortProjectName << endl;
    stream << curMap.getMapDim().width()-2 << " " << curMap.getMapDim().height()-2 << endl;
    stream << curMap.getMapDescription() << endl;
    stream << "General" << endl;

    // write out layout
    QVector<QChar>::iterator itr;
    QVector<QChar> layout = curMap.getMapLayout();
    int x = 0;
    for (itr = layout.begin(); itr != layout.end(); itr++) {
        stream << *itr;
        x++;
        if (x == curMap.getMapDim().width())
        {
            stream << endl;
            x = 0;
        }
    }

    // write players
    QVector<Player> players = curMap.getPlayers();
    stream << curMap.getNumPlayers() << endl;
    for (auto iter = players.begin(); iter != players.end(); iter++) {
         stream << iter->num << " " << iter->gold << " " << iter->lumber << " " << iter->stone << endl;
    }

    // write units
    stream << curMap.getNumUnits() << endl;
    for (int t = 0; t < curMap.getPlayers().size(); t++) {
        QVector<Unit> units = players[t].units;
        QVector<Unit>::iterator itr3;

        for (itr3 = units.begin(); itr3 != units.end(); itr3++) {
            stream << itr3->name << " " << t << " " << itr3->x << " " << itr3->y << endl;
        }
    }

    // write ai triggers
    stream << curMap.getTriggers().length() << endl;
    for( int i = 0; i < curMap.getTriggers().length(); i++){
        stream << curMap.getTriggers().at(i)->infoAI() << endl;
    }

    file->close();
}

// this function saves all assets and stores them inside a zip file
void MainWindow::exportPkg()
{
    QString pkgFileName;

    // set file to save to
    QFileDialog dialog(this);
    dialog.setDirectory(curPath);
    dialog.setWindowModality(Qt::WindowModal);
    dialog.setAcceptMode(QFileDialog::AcceptSave);
    dialog.setDefaultSuffix(".mpk");
    if (dialog.exec() != QDialog::Accepted) {
        curPath = dialog.directory().path();
        return;
    }

    pkgFileName = dialog.selectedFiles().first();

    QFile file(curProjectName + "/data.map");
    if (!file.open(QFile::WriteOnly)) {
        QMessageBox::warning(this, "Application", file.errorString());
        return;
    }

    writeMapFile(&file);

    JlCompress::compressDir(pkgFileName,curProjectName + "/");

/*
    // pkgFileName contains full path; eg. "Users/felix/Desktop/test.map"
    QuaZip qz(pkgFileName);
    qz.open(QuaZip::mdCreate);

    // populate zip file with empty folders
    QuaZipFile qzf(&qz);
    qzf.open(QIODevice::WriteOnly, QuaZipNewInfo("/img/")); qzf.close();
    qzf.open(QIODevice::WriteOnly, QuaZipNewInfo("/res/")); qzf.close();
    qzf.open(QIODevice::WriteOnly, QuaZipNewInfo("/scripts/")); qzf.close();
    qzf.open(QIODevice::WriteOnly, QuaZipNewInfo("/snd/")); qzf.close();
    qzf.open(QIODevice::WriteOnly, QuaZipNewInfo("/upg/")); qzf.close();

    // create map file
    qzf.open(QIODevice::WriteOnly, QuaZipNewInfo("data.map"));
        writeMapFile(&qzf);

    // fill in default assets in curProjectName
    QString file;
    QDirIterator fItr(curProjectName, QDirIterator::Subdirectories);

    while(fItr.hasNext()){
        qDebug() << fItr.next();
        QFile

    }


    // create an index file with all assets
    qzf.open(QIODevice::WriteOnly, QuaZipNewInfo("index.dat"));
        QTextStream stream(&qzf);
        stream << "index.dat" << endl;
        stream << "data.map" << endl;
    qzf.close();

    qz.close();
    */


}

void MainWindow::readSettings()
{
    restoreGeometry(settings.value("main/geometry").toByteArray());
    curPath = settings.value("file/pwd", QDir::homePath()).toString();

}

void MainWindow::writeSettings()
{
    settings.setValue("main/geometry", saveGeometry());
    settings.setValue("file/pwd", curPath);
}

// This function sets up all the UI buttons depending on what map is loaded
void MainWindow::setupUI() {
    // zoom slider and buttons in statusbar
    zMinus = new QToolButton();
    zMinus->setIcon(QIcon(":/toolbar/icons/toolbar/tool_zoom-.bmp"));
    zMinus->setIconSize(QSize(16,16));
    zMinus->setStyleSheet("padding: 0px; border: 1px; margin: 0px;");
    ui->statusBar->addPermanentWidget(zMinus);

    zSlider = new QSlider(Qt::Horizontal);
    zSlider->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    zSlider->setStyleSheet("padding: 0px; border: 0px; margin: 0px;");
    zSlider->setValue(currentScale*100);
    zSlider->setRange(scaleMin*100, scaleMax*100);
    zSlider->setMinimum(scaleMin*100);
    zSlider->setMaximum(scaleMax*100);
    ui->statusBar->addPermanentWidget(zSlider);

    zPlus = new QToolButton();
    zPlus->setIcon(QIcon(":/toolbar/icons/toolbar/tool_zoom+.bmp"));
    zPlus->setIconSize(QSize(16,16));
    zPlus->setStyleSheet("padding: 0px; border: 1px; margin: 0px;");
    ui->statusBar->addPermanentWidget(zPlus);

    QObject::connect(zSlider, SIGNAL(sliderMoved(int)), this, SLOT(on_zSlider_sliderMoved(int)));
    QObject::connect(zPlus, SIGNAL(clicked()), this, SLOT(zoomIn()) );
    QObject::connect(zMinus, SIGNAL(clicked()), this, SLOT(zoomOut()) );

    // player color buttons
    ui->tool_p1->setIcon(curMap.getButtonColorsTx()->getPixTile("blue-light").scaled(16,16));
    ui->tool_p2->setIcon(curMap.getButtonColorsTx()->getPixTile("red-light").scaled(16,16));
    ui->tool_p3->setIcon(curMap.getButtonColorsTx()->getPixTile("green-light").scaled(16,16));
    ui->tool_p4->setIcon(curMap.getButtonColorsTx()->getPixTile("purple-light").scaled(16,16));
    ui->tool_p5->setIcon(curMap.getButtonColorsTx()->getPixTile("orange-light").scaled(16,16));
    ui->tool_p6->setIcon(curMap.getButtonColorsTx()->getPixTile("yellow-light").scaled(16,16));
    ui->tool_p7->setIcon(curMap.getButtonColorsTx()->getPixTile("black-light").scaled(16,16));
    ui->tool_p8->setIcon(curMap.getButtonColorsTx()->getPixTile("white-light").scaled(16,16));

    // terrain buttons
    ui->tool_grass->setIcon(curMap.getTerrain()->getPixTile(Terrain::Grass));
    ui->tool_dirt->setIcon(curMap.getTerrain()->getPixTile(Terrain::Dirt));
    ui->tool_water->setIcon(curMap.getTerrain()->getPixTile(Terrain::Water));
    ui->tool_tree->setIcon(curMap.getTerrain()->getPixTile(Terrain::Tree));
    ui->tool_rock->setIcon(curMap.getTerrain()->getPixTile(Terrain::Rock));
    ui->tool_wall->setIcon(curMap.getTerrain()->getPixTile(Terrain::Wall));

    // other
    ui->tool_hand->setIcon(QIcon(":/toolbar/icons/toolbar/tool_hand.png"));

    // unit buttons
    ui->tool_peasant1->setIcon(curMap.getButtonIconsTx()->getPixTile("peasant"));
    ui->tool_archer->setIcon(curMap.getButtonIconsTx()->getPixTile("archer"));
    ui->tool_ranger->setIcon(curMap.getButtonIconsTx()->getPixTile("ranger"));
    ui->tool_knight->setIcon(curMap.getButtonIconsTx()->getPixTile("knight"));

    // building buttons
    ui->tool_townhall1->setIcon(curMap.getButtonIconsTx()->getPixTile("town-hall"));
    ui->tool_smith->setIcon(curMap.getButtonIconsTx()->getPixTile("human-blacksmith"));
    ui->tool_farm->setIcon(curMap.getButtonIconsTx()->getPixTile("chicken-farm"));
    ui->tool_mill->setIcon(curMap.getButtonIconsTx()->getPixTile("human-lumber-mill"));
    ui->tool_keep->setIcon(curMap.getButtonIconsTx()->getPixTile("keep"));
    ui->tool_castle->setIcon(curMap.getButtonIconsTx()->getPixTile("castle"));
    ui->tool_barracks->setIcon(curMap.getButtonIconsTx()->getPixTile("human-barracks"));
    ui->tool_scouttower->setIcon(curMap.getButtonIconsTx()->getPixTile("scout-tower"));
    ui->tool_cannontower->setIcon(curMap.getButtonIconsTx()->getPixTile("human-cannon-tower"));
    ui->tool_guardtower->setIcon(curMap.getButtonIconsTx()->getPixTile("human-guard-tower"));
    ui->tool_goldmine->setIcon(curMap.getButtonIconsTx()->getPixTile("gold-mine"));
}


// reference  http://www.qtcentre.org/threads/52603-Zoom-effect-by-mouse-Wheel-in-QGraphicsview
void MainWindow::wheelEvent(QWheelEvent *event)
{
    if( (event->modifiers() & Qt::ControlModifier) && event->orientation() == Qt::Vertical )  // if ctrl is held down
    {
        ui->graphicsView->setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
        // static const double scaleFactor = 1.15;
        // static double currentScale = 0.5;  now it's within MainWindow
        // static const double scaleMin = 0.1; // defines the min scale limit.
        // static const double scaleMax = 4.0;

        //bool scrollVertical = event->orientation() == Qt::Vertical;
        int scrollDir = event->delta();
        if(currentScale > scaleMax) {
            currentScale = scaleMax;
        } else if(currentScale < scaleMin) {
            currentScale = scaleMin;
        } else if(scrollDir > 0 && currentScale < scaleMax) {   // zoom out
            zoomOut();
        } else if(scrollDir < 0 && currentScale > scaleMin) {   // zoom in
            zoomIn();
        }
    }
}

void MainWindow::changeLayout(int x, int y, Terrain::Type type)
{
    int newX = x / 32;
    int newY = y / 32;
    int n = newY * curMap.getMapDim().width() + newX;
    statusBar()->showMessage("x: " + QString::number(newX) + ", y: " + QString::number(newY) + ", n: " + QString::number(n));

    QChar c;

    switch (type)
    {
    case Terrain::Water:
        c = ' ';
        break;
    case Terrain::Grass:
        c = 'G';
        break;
    case Terrain::Dirt:
        c = 'D';
        break;
    case Terrain::Rock:
        c = 'R';
        break;
    case Terrain::Tree:
        c = 'F';
        break;
    case Terrain::Wall:
        c = 'W';
        break;
    default:
        qCritical() << "Saving rubble or wall-damaged incomplete";
    break;
    }

    QVector<QChar> layout = curMap.getMapLayout();
    RecordedTile rt(getTileType(curMap.getPreviousTile()), type, scene->getCurBrushSize(), x, y);

    if(!undone && rt.utype != type)
    {//Prevent a duplicate or something not undone from being pushed onto the stack
        if(!redoTiles.isEmpty())
            redoTiles.clear();

        if(undoTiles.isEmpty() || (!undoTiles.isEmpty()
            && (rt.utype != undoTiles.top().utype
                || rt.rtype != undoTiles.top().rtype
                || rt.x != undoTiles.top().x
                || rt.y != undoTiles.top().y)))
            //If there are neither previous tiles nor duplicates,
            //push the previous and new tile of the current x and y
        {
            undoTiles.push(rt);
        }
    }

    layout[n] = c;
    curMap.setMapLayout(layout);
}

Terrain::Type MainWindow::getTileType(QChar tile)
{//Obtain the appropriate tile based on a position on the current map layout
    if(tile == ' ')
        return Terrain::Water;
    else if(tile == 'G')
        return Terrain::Grass;
    else if(tile == 'D')
        return Terrain::Dirt;
    else if(tile == 'R')
        return Terrain::Rock;
    else if(tile == 'F')
        return Terrain::Tree;
    else if(tile == 'W')
        return Terrain::Wall;
    else
        return Terrain::Grass;
}

void MainWindow::activateAI(){
   // qDebug() << QTime::currentTime().toString();
}

void MainWindow::changeAsset(int x, int y, QString asset, int player)
{
    int newX = x / 32;
    int newY = y / 32;

    int n = newY * curMap.getMapDim().width() + newX;
    statusBar()->showMessage("x: " + QString::number(newX) + ", y: " + QString::number(newY) + ", n: " + QString::number(n));

    Unit unit = Unit(asset, newX, newY);

    curMap.addUnit(unit, player);
}

void MainWindow::changeProjectName(QString projectName)
{
    curProjectName = projectName;
}





/* =============================================
 *
 * Slots for button actions
 *
 * =============================================
 */

void MainWindow::on_button_new_clicked()
{
    skipNewFile = false;
    newFile();
}

void MainWindow::on_button_open_clicked()
{
    open();
}

void MainWindow::on_button_save_clicked()
{
    save();
}

void MainWindow::on_button_undo_clicked()
{
    undo();
}

void MainWindow::on_button_redo_clicked()
{
    redo();
}

void MainWindow::on_tool_hand_clicked()
{
    curTool = "hand";
    scene->curTool = "hand";
    statusBar()->showMessage(tr("Hand/Cursor tool selected"), 2000);
    changecursor(curTool);
}

void MainWindow::on_tool_grass_clicked()
{
    curTool = "grass";
    scene->curTool = "grass";
    statusBar()->showMessage(tr("Grass tool selected"), 2000);
    changecursor(curTool);
}

void MainWindow::on_tool_dirt_clicked()
{
    curTool = "dirt";
    scene->curTool = "dirt";
    statusBar()->showMessage(tr("Dirt tool selected"), 2000);
    changecursor(curTool);
}

void MainWindow::on_tool_water_clicked()
{
    curTool = "water";
    scene->curTool = "water";
    statusBar()->showMessage(tr("Water tool selected"), 2000);
    changecursor(curTool);
}

void MainWindow::on_tool_rock_clicked()
{
    curTool = "rock";
    scene->curTool = "rock";
    statusBar()->showMessage(tr("Rock tool selected"), 2000);
    changecursor(curTool);
}

void MainWindow::on_tool_tree_clicked()
{
    curTool = "tree";
    scene->curTool = "tree";
    statusBar()->showMessage(tr("Tree tool selected"), 2000);
    changecursor(curTool);
}

void MainWindow::on_tool_wall_clicked()
{
    curTool = "wall";
    scene->curTool = "wall";
    statusBar()->showMessage(tr("Wall tool selected"), 2000);
    changecursor(curTool);
}


void MainWindow::on_tool_peasant1_clicked()
{
    curTool = "Peasant";
    scene->curTool = "Peasant";
    statusBar()->showMessage(tr("Player 1 Peasant selected"), 2000);
    changecursor(curTool);
}

void MainWindow::on_tool_townhall1_clicked()
{
    curTool = "TownHall";
    scene->curTool = "TownHall";
    statusBar()->showMessage(tr("Player 1 Townhall selected"), 2000);
    changecursor(curTool);
}

void MainWindow::on_tool_goldmine_clicked()
{
    curTool = "GoldMine";
    scene->curTool = "GoldMine";
    statusBar()->showMessage(tr("Goldmine Tool selected"), 2000);
    changecursor(curTool);
}

void MainWindow::on_tool_barracks_clicked()
{
    curTool = "Barracks";
    scene->curTool = "Barracks";
    statusBar()->showMessage(tr("Barracks Tool selected"), 2000);
    changecursor(curTool);
}

void MainWindow::on_tool_smith_clicked()
{
    curTool = "Blacksmith";
    scene->curTool = "Blacksmith";
    statusBar()->showMessage(tr("Black Smith Tool selected"), 2000);
    changecursor(curTool);
}

void MainWindow::on_tool_cannontower_clicked()
{
    curTool = "CannonTower";
    scene->curTool = "CannonTower";
    statusBar()->showMessage(tr("Cannon Tower Tool selected"), 2000);
    changecursor(curTool);
}

void MainWindow::on_tool_castle_clicked()
{
    curTool = "Castle";
    scene->curTool = "Castle";
    statusBar()->showMessage(tr("Castle Tool selected"), 2000);
    changecursor(curTool);
}

void MainWindow::on_tool_farm_clicked()
{
    curTool = "Farm";
    scene->curTool = "Farm";
    statusBar()->showMessage(tr("Farm Tool selected"), 2000);
    changecursor(curTool);
}

void MainWindow::on_tool_guardtower_clicked()
{
    curTool = "GuardTower";
    scene->curTool = "GuardTower";
    statusBar()->showMessage(tr("Guard Tower Tool selected"), 2000);
    changecursor(curTool);
}

void MainWindow::on_tool_scouttower_clicked()
{
    curTool = "ScoutTower";
    scene->curTool = "ScoutTower";
    statusBar()->showMessage(tr("Scout Tower Tool selected"), 2000);
    changecursor(curTool);
}

void MainWindow::on_tool_keep_clicked()
{
    curTool = "Keep";
    scene->curTool = "Keep";
    statusBar()->showMessage(tr("Keep Tool selected"), 2000);
    changecursor(curTool);
}

void MainWindow::on_tool_mill_clicked()
{
    curTool = "LumberMill";
    scene->curTool = "LumberMill";
    statusBar()->showMessage(tr("LumberMill Tool selected"), 2000);
    changecursor(curTool);
}

void MainWindow::on_tool_archer_clicked()
{
    curTool = "Archer";
    scene->curTool = "Archer";
    statusBar()->showMessage(tr("Player 1 Archer selected"), 2000);
    changecursor(curTool);
}

void MainWindow::on_tool_knight_clicked()
{
    curTool = "Knight";
    scene->curTool = "Knight";
    statusBar()->showMessage(tr("Player 1 Knight selected"), 2000);
    changecursor(curTool);
}

void MainWindow::on_tool_ranger_clicked()
{
    curTool = "Ranger";
    scene->curTool = "Ranger";
    statusBar()->showMessage(tr("Player 1 Ranger selected"), 2000);
    changecursor(curTool);
}

// function to generalize all the player button click events
void MainWindow::on_tool_pX_clicked(QAbstractButton* button) {
    curPlayer = button->text().toInt();
    scene->curPlayer = button->text().toInt();
    ui->statusBar->showMessage("Player " + button->text() + " selected");
}

void MainWindow::on_tool_aitrigger_clicked()
{
    curTool = "Trigger";
    scene->curTool = "Trigger";
    statusBar()->showMessage(tr("Trigger selected"),2000);
    changecursor(curTool);
}

void MainWindow::on_actionBrush_size_1_triggered()
{
    scene->CurBrushSize = 1;
}

void MainWindow::on_actionBrush_size_2_triggered()
{
    scene->CurBrushSize = 2;
}

void MainWindow::on_actionBrush_size_3_triggered()
{
    scene->CurBrushSize = 3;
}

void MainWindow::on_actionBrush_size_4_triggered()
{
    scene->CurBrushSize = 4;
}
void MainWindow::on_actionGridlines_toggled(bool arg1)
{
    scene->setGridlines(arg1);
    scene->update();
//    QApplication::processEvents();
}






/* ========================================
 *
 * for Various Dialog Boxes
 *
 * ========================================
 */

void MainWindow::open_DgAbout(){
    DgAbout w(this);
    w.exec();
}

void MainWindow::open_DgMapProperties(){
    DgMapProperties w(&curMap, this);
    w.exec();
}

void MainWindow::open_DgPlayerProperties(){
    DgPlayerProperties w(this, curMap);
    if(w.exec()) {  // if changes were made
        QVector<Player> newPlayers = w.players;

        // copy over Units
        auto oldItr = curMap.getPlayers().begin();
        auto newItr = newPlayers.begin();
        for( ;
                   ( newItr != newPlayers.end() && oldItr != curMap.getPlayers().end() );
                    newItr++, oldItr++ ) {
            //qDebug() << newItr->num;
            newItr->units = oldItr->units;
        }
        curMap.setPlayers(newPlayers);
        loadScene();

        // reset ui
        curPlayer = 1;
        scene->curPlayer = 1;
        ui->tool_p1->setChecked(true);
        ui->tool_hand->setChecked(true);
        on_tool_hand_clicked();

        updateUIPlayers();
    }
}

void MainWindow::open_exporttoweb()
{
    ExportToWeb w(this);
    w.exec();
}

void MainWindow::open_importfromweb()
{
    ImportPackage w(this);
    w.exec();
}

// for the assets editor window
void MainWindow::open_DgAssets(){
    // if doesn't exist, open a new one
    if (MainWindow::wAssets == 0)
        wAssets = new DgAssets(curProjectName, this);

    wAssets->show();
    wAssets->raise();
    wAssets->activateWindow();
}

void MainWindow::open_DTrigger(QGraphicsScene *scene , Tile *tile){
    DgAddTrigger wTrigger(tile, this);
    if ( wTrigger.exec() != QDialog::Accepted ){
        scene->removeItem(tile);
        return;
    }

    AITrigger* trigger = wTrigger.aiTrigger;

    curMap.addTrigger(trigger);
}

void MainWindow::open_DgImportPkg() {
    DgImportPkg w(curProjectName, this);
    w.exec();
}

void MainWindow::subMenuAction(QAction *action){
    DialogSound wSound(this,sound,action->text());

    if ( wSound.exec() != QDialog::Accepted ){

        return;
    }

    sound->writeSound();


}

void MainWindow::setupAssets(){
    // grab all the asset files

    QString path = curProjectName + "/img";
    QString colorFile = curProjectName + "/img/Colors.png";
    QString goldmineTool = curProjectName + "/img/GoldMine.dat";
    QString peasantTool = curProjectName + "/img/Peasant.dat";
    QString archerTool = curProjectName + "/img/Archer.dat";
    QString knightTool = curProjectName + "/img/Knight.dat";
    QString rangerTool = curProjectName + "/img/Ranger.dat";
    QString townhallTool = curProjectName + "/img/TownHall.dat";
    QString barracksTool = curProjectName + "/img/Barracks.dat";
    QString blacksmithTool = curProjectName + "/img/Blacksmith.dat";
    QString cannontowerTool = curProjectName + "/img/CannonTower.dat";
    QString castleTool = curProjectName + "/img/Castle.dat";
    QString farmTool = curProjectName + "/img/Farm.dat";
    QString guardtowerTool = curProjectName + "/img/GuardTower.dat";
    QString keepTool = curProjectName + "/img/Keep.dat";
    QString lumbermillTool = curProjectName + "/img/LumberMill.dat";
    QString scouttowerTool = curProjectName + "/img/ScoutTower.dat";



    int nObjects = 15;

//    assets = new QMap<QString,Texture*>;

    // append them to a vector
    QVector<QString> files;
    files.append(peasantTool);
    files.append(archerTool);
    files.append(knightTool);
    files.append(rangerTool);
    files.append(goldmineTool);
    files.append(townhallTool);
    files.append(barracksTool);
    files.append(blacksmithTool);
    files.append(cannontowerTool);
    files.append(castleTool);
    files.append(farmTool);
    files.append(guardtowerTool);
    files.append(keepTool);
    files.append(lumbermillTool);
    files.append(scouttowerTool);

    // create a texture for each asset
    for(int i = 0; i < nObjects; i++){
        Texture *tex = new Texture(files.at(i),colorFile, curProjectName);
        assets.insert( tex->textureName, tex);
    }
    assets.value("Peasant")->paintAll();
    assets.value("Ranger")->paintAll();
    assets.value("Archer")->paintAll();
    assets.value("Knight")->paintAll();
    assets.value("TownHall")->paintAll();
    assets.value("Barracks")->paintAll();
    assets.value("Blacksmith")->paintAll();
    assets.value("CannonTower")->paintAll();
    assets.value("Castle")->paintAll();
    assets.value("Farm")->paintAll();
    assets.value("GuardTower")->paintAll();
    assets.value("Keep")->paintAll();
    assets.value("LumberMill")->paintAll();
    assets.value("ScoutTower")->paintAll();

}

// for the terrain editor window
void MainWindow::open_terrainEditor(){
    DgTerrainEditor window(curMap.getTerrain(), this);
    window.exec();
}


// checks UI parameters and updates them
void MainWindow::updateUIPlayers(){
    int numPlayers = curMap.getNumPlayers();
    QList<QAbstractButton*> buttons = ui->bgroup_player->buttons();

    // enable all:
    for(int i = 0; i < 8; i++) {
        buttons.at(i)->setEnabled(true);

        // disable some:
        if(i >= numPlayers )
            buttons.at(i)->setDisabled(true);
    }
}

void MainWindow::submenuEditor(QAction* assetEditor){
    if(assetEditor->text() == "Animation")
    {
        open_DgAssets();
    }
    else
    {
        open_terrainEditor();
    }
}

void MainWindow::changecursor(QString currentTool){
    asset = curMap.getAsset(currentTool);
    ui->graphicsView->setDragMode(QGraphicsView::NoDrag);
    if(currentTool == "Peasant" || currentTool == "Archer" || currentTool == "Knight" || currentTool == "Ranger")
        cursorImage = asset->colorPlayerImg[curPlayer][20];
    else if (currentTool == "TownHall" || currentTool == "Barracks" || currentTool == "Farm" || currentTool == "ScoutTower" || currentTool == "LumberMill" || currentTool == "Blacksmith")
        cursorImage = asset->colorPlayerImg[curPlayer][2];
    else if (currentTool == "CannonTower" || currentTool == "Castle" || currentTool == "GuardTower" || currentTool == "Keep")
        cursorImage = asset->colorPlayerImg[curPlayer][1];
    else if (currentTool == "GoldMine")
        cursorImage = asset->imageList[0];
    else if (currentTool == "Trigger")
        cursorImage.load(":/data/default/img/Trigger.png");
    else if (currentTool == "hand")
    {
        cursorImage.load(":/toolbar/icons/toolbar/tool_hand.png");
        pixmap = QPixmap::fromImage(cursorImage);
        QCursor cursorTarget = QCursor(pixmap, 4, 0);
        ui->graphicsView->setCursor(cursorTarget);
        ui->graphicsView->setDragMode(QGraphicsView::RubberBandDrag);
        return;
    }
    else
    {
        ui->graphicsView->setCursor(Qt::ArrowCursor);
        return;
    }
    pixmap = QPixmap::fromImage(cursorImage);
    QPainter painter(&pixmap);
    painter.setOpacity(0);
    painter.end();
    pixmap.setMask(pixmap);
    QCursor cursorTarget = QCursor(pixmap, scene->offsetX, scene->offsetY);
    ui->graphicsView->setCursor(cursorTarget);
}


// zoom slider

void MainWindow::on_zSlider_sliderMoved(int val)
{
    qreal newScale = qPow(scaleFactor, val/100.0);
    currentScale = newScale;

    QMatrix matrix;
    matrix.scale(newScale, newScale);

    ui->graphicsView->setResizeAnchor(ui->graphicsView->resizeAnchor());
    ui->graphicsView->setMatrix(matrix);
}

void MainWindow::zoomOut() {
    if(currentScale > scaleMin) {
        ui->graphicsView->scale(1 / scaleFactor, 1 / scaleFactor);
        currentScale /= scaleFactor;
    }

    zSlider->setValue(currentScale*100.0);
}

void MainWindow::zoomIn() {
    zSlider->setValue(currentScale*100.0);
    if(currentScale < scaleMax) {
        ui->graphicsView->scale(scaleFactor, scaleFactor);
        currentScale *= scaleFactor;
    }
}

void MainWindow::on_actionImport_from_web_triggered()
{
    ImportPackage w(this);
    w.exec();
}
