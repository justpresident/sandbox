#include <mainwindow.h>
#include <ui_mainwindow.h>


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    createMenu();
    ui->verticalLayout->setMenuBar(menuBar);

    ui->horizontalLayout->addWidget(ui->saveButton);
    ui->horizontalLayout->addWidget(ui->deleteButton);


    listModel = new QStringListModel(this);
    ui->listView->setModel(listModel);

    connect(&store, SIGNAL(changed(const Store &)), this, SLOT(refresh_data(const Store &)));
    connect(&store, SIGNAL(changed(const Store &)), this, SLOT(save_file(const Store &)));

}

MainWindow::~MainWindow()
{
    delete ui;
    delete listModel;
    if (cypher != NULL)
        delete cypher;
}

void MainWindow::createMenu()
{
    menuBar = new QMenuBar;

    fileMenu = new QMenu(tr("&File"), this);
    openAction = fileMenu->addAction(tr("&Open"));
    saveAction = fileMenu->addAction(tr("&Save"));
    exitAction = fileMenu->addAction(tr("E&xit"));
    menuBar->addMenu(fileMenu);

    connect(exitAction, SIGNAL(triggered()), this, SLOT(close()));
    connect(openAction, SIGNAL(triggered()), this, SLOT(open_file()));
    connect(saveAction, SIGNAL(triggered()), this, SLOT(save_file()));
}

QString choose_file(QFileDialog::FileMode mode) {
    QFileDialog dialog(NULL);
    dialog.setFileMode(mode);
    dialog.setViewMode(QFileDialog::List);

    if (dialog.exec()) {
        QStringList fileNames = dialog.selectedFiles();
        return fileNames[0];
    } else
        return "";
}

QString input_password() {
    return QInputDialog::getText(NULL,"Please input file password", "Password");
}

Cypher * mk_file_cypher(QFileDialog::FileMode mode) {
    QString data_file = choose_file(mode);

    if (data_file == "")
        return NULL;

    QString key = input_password();
    Cypher *cypher = new Cypher(data_file, key);
    return cypher;
}

void MainWindow::open_file() {
    cypher = mk_file_cypher(QFileDialog::ExistingFile);
    if (cypher != NULL)
        store.set_data(cypher->read_data());
}

void MainWindow::save_file() {
    save_file(store);
}

void MainWindow::save_file(const Store &store) {
    if (cypher == NULL)
        cypher = mk_file_cypher(QFileDialog::AnyFile);

    if (cypher != NULL)
        cypher->write_data(store.get_data());
}

void MainWindow::refresh_data(const Store &store) {
    listModel->setStringList(store.get_keys().filter(ui->nameEdit->text()));
    QString value = "";
    if (listModel->stringList().contains(ui->nameEdit->text())) {
        value = store.get(ui->nameEdit->text());
    }
    ui->valueEdit->document()->setPlainText(value);
}

void MainWindow::on_saveButton_clicked()
{
    store.put(ui->nameEdit->text(),ui->valueEdit->document()->toPlainText());
}

void MainWindow::on_deleteButton_clicked()
{
    QString selected_key = listModel->stringList().at(ui->listView->currentIndex().row());
    store.remove(selected_key);
    ui->nameEdit->setText("");
    ui->valueEdit->document()->setPlainText("");
}

void MainWindow::on_listView_clicked(const QModelIndex &index)
{
    QString selected_key = listModel->stringList().at(index.row());
    ui->nameEdit->setText(selected_key);
    ui->valueEdit->document()->setPlainText(store.get(selected_key));
}

void MainWindow::on_nameEdit_textChanged(const QString &arg1 __attribute__((unused)))
{
    refresh_data(store);
}
