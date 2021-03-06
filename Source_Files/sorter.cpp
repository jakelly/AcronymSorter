#include "sorter.h"
#include "ui_sorter.h"
#include "changelog.h"
#include <iostream>
#include "Acronyms.h"
#include <vector>
#include <string>
#include <fstream>
#include <QMessageBox>
#include <QFile>
#include <QList>
#include <QDir>
#include <QDebug>
#include <QRegExp>

Sorter::Sorter(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Sorter)
{
    ui->setupUi(this);
    QIcon *ico = new QIcon();
        ico->addPixmap(QPixmap(":/images/Acronym.png"),QIcon::Active,QIcon::On);
        this->setWindowIcon(*ico);
    Load_Config();
    ui->statusBar->showMessage("Config Init Successful... Previous State Loaded!",4000);


}

Sorter::~Sorter()
{
    if(DataBaseFile != defaultfile && QMessageBox::question(this, "Save Database", "Do You Want to Save User Defined Database Prior to Closing Application?"))
        filesave = true;
    Save_Config();

    delete ui;
}


void Sorter::Load_Config()
{
    int debugload = 0;
    int filesaveload = 0;

    QFile config_file(config);
    if(!config_file.exists()) //Initization of Config file
    {
        Save_Config();
    }
    else {
        config_file.open(QIODevice::ReadWrite | QIODevice::Text);
        while (!config_file.atEnd()) {
                   QTextStream load (&config_file);
                   QString v =nullptr;
                   load >> v >> OutputFormatBoxIndex
                        >> v >> SortModeBoxIndex
                        >> v >> filesaveload
                        >> v >> Filehistory
                        >> v >> debugload
                        >> v >> windowWidth
                        >> v >> windowHeight
                        >> v >> w_xpos
                        >> v >> w_ypos
                       ;



    }//while
       ui->OutputFormatBox->setCurrentIndex(OutputFormatBoxIndex);
       ui->SortModeBox->setCurrentIndex(SortModeBoxIndex);
       filesave = filesaveload;
       if(filesave){
       DataBaseFile = Filehistory;
       ui->File_Dir->setText(Filehistory);
       }
       else{ui->File_Dir->setText("Program Default Database");}

       //ui->font_size->setValue(fontsize);
       ui->InputBox->setFontPointSize(fontsize);
       ui->OutputBox->setFontPointSize(fontsize);
       debug = debugload;
       this->setGeometry(w_xpos, w_ypos, windowWidth, windowHeight);



}//else
    if(debug){
        this->setWindowTitle("Sorter :: DEBUG MODE");
        QIcon *ico = new QIcon();
            ico->addPixmap(QPixmap(":/img/debug active.jpg"),QIcon::Active,QIcon::On);
            ui->actionDebug->setIcon(*ico);
        }
       config_file.close();
}//slot

void Sorter::Save_Config(){
    QFile config_file(config);
        config_file.open(QIODevice::ReadWrite | QIODevice::Text);
        qDebug() << config_file.isOpen();
        QTextStream save (&config_file);
        save << "OutputFormatBoxIndex\t" << OutputFormatBoxIndex << "\n"
             << "SorterModeBoxIndex\t"   << SortModeBoxIndex <<"\n"
             << "filesave\t"        << filesave << "\n"
             << "PreviousDatabase\t" << DataBaseFile << "\n"
             << "Debug\t"       << debug << "\n"
             << "WindowWidth\t" << this->width() <<"\n"
             << "windowHeight\t"<< this->height() <<"\n"
             << "windowXpos\t" << this->x() <<"\n"
             << "windowYpos\t" << this->y() << "\n"
             << "FontSize\t"  << fontsize << "\n"
            ;


}

void Sorter::Logic_Diagnostic()
{

    QString Diag_List;
    int databasecount=0;
    QFile file(DataBaseFile);

    if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {   QMessageBox::warning(this, "Warning", "Data File Not Found");
        return;
    }
    file.close();
    file.open(QIODevice::ReadOnly);
    QTextStream FileStream(&file);
    QStringList arg;
    while (!file.atEnd()) {
         QString line = file.readLine();
         for (QString item : line.split("\t"))
          {
             if (item != "\r\n")
             {
                  arg.push_back(item);
             }//if

              else if (item == "\r\n")
             {
                 QString temp = arg[0];
                 Acronyms* acronym = new Acronyms(temp);

                 Diag_List.append(Sorter::print(*acronym));
                 //if(iteration == 2)
             //        Diag_List.replace(Diag_List.length(),1," ");
                 delete acronym;
                 arg.clear();
                 databasecount++;
             }
        }
    }//while
            ui->InputBox->setPlainText(Diag_List);
            Load_List();
            QString output_str = ui->OutputBox->toPlainText();
            QStringList output_list;
            QString input_str = Diag_List;
            QStringList input_list;
            QString combo_str;
            int count = 0;
                for (auto &&item : output_str.split("\n"))
                {
                    output_list.push_back(item);
                 }

                for (auto &&item : input_str.split("\n"))
                {
                    input_list.push_back(item);
                 }
            combo_str = "======================================\n"
                        ":::::::Diagnostic Logic Results:::::::\n"
                        "======================================\n\n";

            QFile Diag(Diag_File);
                Diag.open(QIODevice::ReadWrite | QIODevice::Text);
                qDebug() << Diag.isOpen();
                QTextStream save (&Diag);

                save << "===========================\n"
                        " Diagnosis Results v1.1.2\n"
                        "===========================\n"
                        " Number of Acronyms: ??\n"
                        " Number Found:       ??\n\n\n\n";

                while (count < output_list.count()){

                    save << input_list.at(count) << ","
                         << output_list.at(count) << ","
                         << endl;
                    count++;
                }
                QString qdatabasecount = QString::fromStdString(std::to_string(databasecount));
                QString qcount = QString::fromStdString(std::to_string(count));
                Diag.close();
                Diag.open(QIODevice::ReadWrite | QIODevice::Text);
                        save << "===========================\n"
                                " Diagnosis Results v1.1.2\n"
                                "===========================\n"
                                " Number of Acronyms: "+ qcount+"\n"
                                " Number Found:       "+ qdatabasecount+ "\n\n\n";
                debug = false;
                QFileInfo dir(Diag);
                QString path = QString::fromStdString(dir.absoluteFilePath().toStdString());
                QString msg = ":::Diagnosic Finished:::\n"
                              " Number of Acronyms: "+ qcount+"\n"
                              " Number of Found:    "+ qdatabasecount+ "\n"
                              " Diagnostic File Located: \n"
                              + path;
                QMessageBox::information(this, "Logic Diagnostic", msg);
}

void Sorter::Load_List()
{
    QString input_qtxt = ui->InputBox->toPlainText();
        //these are used to store the findings of the REGEX functions
        QString KnownList; //used to save printed Known Acronyms
        QString *KnownPtr = &KnownList;
        int Known_count{0};
        QString Unknown_txt = input_qtxt;
    if(input_qtxt.size() == 0)
        return;
    ui->OutputBox->clear();


    QFile file(DataBaseFile);

    if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {   QMessageBox::warning(this, "Warning", "Data File Not Found");
        return;
    }

    if(debug){
       qDebug() << "exists?              " << file.exists();
       qDebug() << "writable?            " << file.isWritable();
       qDebug() << "permissions before?  " << file.permissions();
       qDebug() << "permissions set?     " << file.setPermissions(QFileDevice::WriteOther | QFileDevice::ReadOther);
       qDebug() << "permissions after?   " << file.permissions();
       qDebug() << "Filename:            " << file.fileName();
       qDebug() << "opened?              " << file.open(QIODevice::Append);
       qDebug() << "errors?              " << file.errorString();
       qDebug() << "errnum?              " << file.error();
        }
        file.close();
        file.open(QIODevice::ReadOnly);
        QTextStream FileStream(&file);
        QStringList arg; // var used to store lines from the database file read


        //Unknown is a copy of the input QString as Known ACronyms are found they are removed from this copy

        QRegExp csv (".csv");
        QRegExp txt (".txt");
        QString demen = nullptr;
        //dement logic  will want to expand to include more data types !! Big Project!!
        if(DataBaseFile.contains(txt))
        demen = "\t";

        if(DataBaseFile.contains(csv))
        demen = ",";
          while (!file.atEnd()) {
               QString line = file.readLine();
                   int i = 0;
               for (QString item : line.split(demen))
                {
                   if (item != "\r\n")
                   {
                        arg.push_back(item);
                   }//if

                    else if (item == "\r\n")
                   {

                      std::string input_txt = input_qtxt.toStdString();
                      QString qtest_case = arg[0];
                      std::string test_case = qtest_case.toStdString();
                      std::size_t found_pos = input_txt.find(test_case);
                      std::size_t test_length = test_case.length();
                       if (found_pos != input_txt.npos)
//                   QRegExp test (qtest_case);
  //                 if(input_qtxt.contains(test))
                       {
                       if (test_length == 1)
                      {
                           //restructure to always pass the single case acronym to the class builder if found in input box.  the use of a third int val or possible global bool will determine if single case actually exists.  See TODO.txt for sudo code.
                          if(SingleCase(qtest_case, Unknown_txt))
                          {
                          ClassPhrase(arg, *KnownPtr);
                          Known_count++;
                          i = 0;
                          arg.clear();
                          }
                       }

                      if (test_length > 1)
                      {
                          if(MultiCase(qtest_case, Unknown_txt))
                          {
                              ClassPhrase(arg, *KnownPtr);
                              Known_count++;
                              i = 0;
                              arg.clear();
                          }
                      }
                      }//nested if for searching
                       i = 0;
                       arg.clear();
                   }//else

                 }//for


       }//while

          //compare KnownList to Unknown List here

          if(debug)
            {
              qDebug() <<"String Before: " << Unknown_txt;
              qDebug() << "String After: " << input_qtxt;
            }

          //Unknown Class Build
          QString UnknownList;
          QString* UnknownPtr = &UnknownList;
          QStringList CapList;
          std::string Unk_Str_count;
          std::string Known_Str_count;
          QRegExp CAPS  ("[A-Z]{2,}(?=\\W)");
          QRegExp CAPS2 ("[A-Z]{2,}(?=$)");


//R2D2 R2  REGEX Search

          QStringList patterns {"[A-Z]{2,}(?=\\W)","[A-Z]{2,}(?=$)",
                                "(^|\\s[A-Z](?=\\s|$)", "([A-Z]+[^a-z\\s])+(?=\\s|$)"};

                                /*),"(\\s\\d*([A-Z])(?=\\s)"
                                "(^\\d*([A-Z])(?=\\s)", "(\\s([A-Z])(?=$)"
                                "(\\s\\d*([A-Z])(?=$)","(^\\d*([A-Z])(?=$)"};*/


          {//for some scope
          int unk_count=0;

          for (auto i{0}; i < patterns.length(); i++)
          {
              QRegularExpression rx(patterns[i]);
              QRegularExpressionMatch match;
              match = rx.match(Unknown_txt);
              while(match.hasMatch())
                  {
                      CapList.push_back(match.captured());
                      Unknown_txt.remove(match.captured());
                      match = rx.match(Unknown_txt);
                      unk_count++;
                  }
                  //Unknown_txt.replace(CAPS, " ");
              }//for
          Unk_Str_count = std::to_string(unk_count);
          Known_Str_count = std::to_string(Known_count);
          UnknownLoop(CapList, *UnknownPtr);
          }//scope

       //sorter drop down logic
          if(ui->SortModeBox->currentText() == "All")
            {
              QString all;
              all.append(KnownList);
              all.append(UnknownList);
              ui->OutputBox->setPlainText(all);
              ui->statusBar->showMessage(
                          "Search Complete Known Acroynms: "
                          + QString::fromStdString(Known_Str_count)
                          + "\t|\tUnknown Acroynms: "
                          + QString::fromStdString(Unk_Str_count), 5000);
            }
          if(ui->SortModeBox->currentText() == "Known")
              ui->OutputBox->setText(KnownList);

          if(ui->SortModeBox->currentText() == "Unknown")
              ui->OutputBox->append(UnknownList);

          return;
   }

void Sorter::UnknownLoop (QStringList CapList, QString &UnknownList)
{

    QStringList::iterator it = CapList.begin();
    int i =0;
    while (it != CapList.end())
    {
        QString arg = CapList[i];
        UnkClassPhrase(arg, UnknownList);
        ++i;
        ++it;
    }

}


void Sorter::UnkClassPhrase(QString arg, QString &UnknownList)
{
    Acronyms* acronym = new Acronyms(arg);

    UnknownList.append(Sorter::print(*acronym));
    delete acronym;
    return;
}

void Sorter::ClassPhrase (QStringList &arg, QString &KnownList){

                   int def_num{0};
                   if(debug)
                           def_num=7;
                   else
                   {
                   if (arg[7] != "Nodef")
                       def_num = 7;

                   else if (arg[6] != "Nodef")
                       def_num = 6;

                   else if (arg[5] != "Nodef")
                       def_num = 5;

                   else if (arg[4] != "Nodef")
                       def_num = 4;

                   else if (arg[3] != "Nodef")
                       def_num = 3;

                   else if (arg[2] != "Nodef")
                       def_num = 2;

                   else
                       def_num = 1;
                   }

                   Acronyms* acronym = new Acronyms(arg[0],
                                                    arg[1],
                                                    arg[2],
                                                    arg[3],
                                                    arg[4],
                                                    arg[5],
                                                    arg[6],
                                                    arg[7],
                                                    def_num);

                   //ui->OutputBox->appendPlainText("class built");

                    //add global clas variable to 'save' class data for singlecase matches. this can be set based on a global bool set to true indicating singlecase match was found.
                   //if(singlematch)
                   //globalclass= *acronym.
                    KnownList.append(Sorter::print(*acronym));
                    delete acronym;
           }

QString Sorter::print(class Acronyms a)
{
    if(debug)
    {
     return (*a.name + "\n");

    }


    if (*a.def_num == 1) {
        if(ui->OutputFormatBox->currentIndex() == 0)
        return (*a.name + " (" + *a.def1 + "); ");

        if(ui->OutputFormatBox->currentIndex() == 1)
        return (*a.def1 + " (" + *a.name + "); ");

    }if (*a.def_num == 2) {
        if(ui->OutputFormatBox->currentIndex() == 0)
        return                           (*a.name + " ("
                                         + *a.def1 + " / "
                                         + *a.def2 + "); ");

        if(ui->OutputFormatBox->currentIndex() == 1)
        return                           (*a.def1 + " / "
                                         + *a.def2 + " ("
                                         + *a.name + "); ");

    }if (*a.def_num == 3) {
        if(ui->OutputFormatBox->currentIndex() == 0)
          return                         (*a.name
                                         + " (" + *a.def1
                                         + " / " + *a.def2
                                         + " / " + *a.def3
                                         + "); ");
        if(ui->OutputFormatBox->currentIndex() == 1)
        return                           (*a.def1
                                          + " / " + *a.def2
                                          + " / " + *a.def3
                                          + " (" + *a.name + "); ");

    }if (*a.def_num == 4) {
        if(ui->OutputFormatBox->currentIndex() == 0)
        return                         (*a.name
                                       + " (" + *a.def1
                                       + " / " + *a.def2
                                       + " / " + *a.def3
                                       + " / " + *a.def4
                                       + "); ");

        if(ui->OutputFormatBox->currentIndex() == 1)
            return                           (*a.def1
                                              + " / " + *a.def2
                                              + " / " + *a.def3
                                              + " / " + *a.def4
                                              + " (" + *a.name + "); ");


    }if (*a.def_num == 5) {
        if(ui->OutputFormatBox->currentIndex() == 0)
        return (*a.name
                                       + " (" + *a.def1
                                       + " / " + *a.def2
                                       + " / " + *a.def3
                                       + " / " + *a.def4
                                       + " / " + *a.def5
                                       + "); ");
        if(ui->OutputFormatBox->currentIndex() == 1)
        return                           (*a.def1
                                         + " / " + *a.def2
                                         + " / " + *a.def3
                                         + " / " + *a.def4
                                         + " / " + *a.def5
                                         + " (" + *a.name + "); ");

    }if (*a.def_num == 6) {
        if(ui->OutputFormatBox->currentIndex() == 0)
       return (*a.name
                                       + " (" + *a.def1
                                       + " / " + *a.def2
                                       + " / " + *a.def3
                                       + " / " + *a.def4
                                       + " / " + *a.def5
                                       + " / " + *a.def6
                                       + "); ");
        if(ui->OutputFormatBox->currentIndex() == 1)
            return                           (*a.def1
                                             + " / " + *a.def2
                                             + " / " + *a.def3
                                             + " / " + *a.def4
                                             + " / " + *a.def5
                                             + " / " + *a.def6
                                             + " (" + *a.name + "); ");


    }if (*a.def_num == 7) {
        if(ui->OutputFormatBox->currentIndex() == 0)
        return (*a.name
                                       + " (" + *a.def1
                                       + " / " + *a.def2
                                       + " / " + *a.def3
                                       + " / " + *a.def4
                                       + " / " + *a.def5
                                       + " / " + *a.def6
                                       + " / " +*a.def7
                                       + "); ");
        if(ui->OutputFormatBox->currentIndex() == 1)
            return                           (*a.def1
                                             + " / " + *a.def2
                                             + " / " + *a.def3
                                             + " / " + *a.def4
                                             + " / " + *a.def5
                                             + " / " + *a.def6
                                             + " / " + *a.def7
                                             + " (" + *a.name + "); ");
    }

    return "error";
}

bool Sorter::MultiCase(QString test_case, QString &Unknown_txt)
{
       // Multicase REGEX test works 1/1/2019 @ 1730

//        QRegExp PLSpace1   ("(\\s"     + test_case + ")(?=\\s)");   // Space with treiling space //tested
//        QRegExp PLSpace2   ("(\\s\\d*"   + test_case + ")(?=\\s)");   // Space & Digit with trailing space //tested
//        QRegExp PLSpace3   ("(^\\d*"   + test_case + ")(?=\\s)");   // Caret & Digit with trailing space //tested
//        QRegExp PLEndline1 ("(\\s"     + test_case + ")(?=$)");     // Space with trailing endline //tested
//        QRegExp PLEndline2 ("(\\s\\d*" + test_case + ")(?=$)");     // Space & Digit with trailing endline //tested
//        QRegExp PLEndline3 ("(^\\d*" + test_case + ")(?=$)");   // Caret with trailing endline//tested

        // TAC(A)
       QStringList patterns {"\\s("+ QRegularExpression::escape(test_case) + ")\\s", "\\s(" + QRegularExpression::escape(test_case) + ")$",
                             "^("+ QRegularExpression::escape(test_case) + ")\\s"  , "^("+ QRegularExpression::escape(test_case) + ")$"};
        QString input_box_txt = ui->InputBox->toPlainText();

        for (auto i{0}; i < patterns.length(); i++)
        {
            QRegularExpression rx(patterns[i]);
            QRegularExpressionMatch match;
            QRegularExpressionMatch match2;
            match = rx.match(input_box_txt);
            match2 = rx.match(Unknown_txt);

            if(test_case == 'F')
            {
                QRegularExpression rx2("R F");
                QRegularExpressionMatch match_F;
                match_F = rx2.match(input_box_txt);
                if (match_F.hasMatch())
                    return true;
                else {
                    return false;
                }
            }

            if(match.hasMatch())  //Need to build logic to check for repeated Acronyms.
                {
                if(match2.hasMatch()){
                    int j = match2.capturedEnd();
                    int n = match2.capturedLength();
                    j -= n;
                    Unknown_txt.replace(j,n," ");
                    return true;
                }//if#2
                }//if#1
        }//for#1
        return false;
}

//        0.1.1 Regex logic for history sake Also look at how bad I am at this haha!!!!
//        if(input_qtxt.contains(PLSpace1))
//        {
//            input.replace(PLSpace1, " ");
//            return true;
//        }
//        if(input_qtxt.contains(PLSpace2))
//        {
//            input.replace(PLSpace2, " ");
//            return true;
//        }

//        if(input_qtxt.contains(PLSpace3))
//        {
//            input.replace(PLSpace3, " ");
//            return true;
//        }

//        if(input_qtxt.contains(PLEndline1))
//        {
//            input.replace(PLEndline1, " ");
//            return true;
//        }

//        if(input_qtxt.contains(PLEndline2))
//        {
//            input.replace(PLEndline2, " ");
//            return true;
//        }

//        if(input_qtxt.contains(PLEndline3))
//        {
//            input.replace(PLEndline3, " ");
//            return true;
//        }


bool Sorter::SingleCase(QString test_case, QString &input){

    QString input_qtxt = ui->InputBox->toPlainText();
    std::string input_txt = input_qtxt.toStdString();
    //QString Symbols = "!@#$%^&*()_+-=:\";'<>?,./{}[]\\|~`";

    //A&P TEST P
//    QStringList patterns {"\\s(" + test_case + ")(?:\\s)",
//                          "\\d+(" + test_case + ")"};

//    for (auto i{0}; i < patterns.length(); i++)
//    {
//        QRegularExpression rx(patterns[i]);
//        QRegularExpressionMatch match;
//        match = rx.match(input_qtxt);
//        if(match.hasMatch())
//            {
//                input.remove(match.captured());
//                return true;
//            }

    //SingleCase RegEx test works 1/1/209 @2000

    QRegExp PLSpace1   ("(\\s"     + test_case + ")(?=\\s)");   // Space with treiling space //tested
    QRegExp PLSpace2   ("(\\s\\d+"   + test_case + ")(?=\\s)");   // Space & Digit with trailing space //tested
    QRegExp PLSpace3   ("(^\\d+"   + test_case + ")(?=\\s)");   // Caret & Digit with trailing space //tested
    QRegExp PLEndline1 ("(\\s"     + test_case + ")(?=$)");     // Space with trailing endline //tested
    QRegExp PLEndline2 ("(\\s\\$?\\d+" + test_case + ")(?=$)");     // Space & Digit with trailing endline //tested
    QRegExp PLEndline3 ("(^" + test_case + ")(?=\\s)");
    QRegExp PLEndline4 ("(^" + test_case + ")($)");


    //need to a logic that if a singlecase match is made the class will be saved a global boolean set to true and the processing of the acronym section is continued.

    //additional: SingleCase slot's def needs to be updated to allow for a return vaule.  this will offer the option to have more than two possible return values.  currently set to be true or false.  the



    if(input_qtxt.contains(PLSpace1))
    {
        input.replace(PLSpace1, " ");
        return true;
    }

    if (input_qtxt.contains(PLSpace2))
    {
        input.replace(PLSpace2, " ");
        return true;
    }

    if (input_qtxt.contains(PLSpace3))
    {
        input.replace(PLSpace3, " ");
        return true;
    }

    if (input_qtxt.contains(PLEndline1))
    {
        input.replace(PLEndline1, " ");
        return true;
    }

    if (input_qtxt.contains(PLEndline2))
    {
        input.replace(PLEndline2, " ");
        return true;
    }
    if (input_qtxt.contains(PLEndline3))
    {
        input.replace(PLEndline3, " ");
        return true;
    }
    if (input_qtxt.contains(PLEndline4))
    {
        input.replace(PLEndline4, " ");
        return true;
    }

    return false;
}

void Sorter::on_SortBtn_clicked()
{
    Sorter::Load_List();
}

void Sorter::on_OutputFormatBox_currentIndexChanged()
{
    qDebug() << OutputFormatBoxIndex;
    OutputFormatBoxIndex = ui->OutputFormatBox->currentIndex();
    Sorter::Load_List();
}

void Sorter::on_action_Open_triggered()
{
    QString Filename = QFileDialog::getOpenFileName(this, "Select File to Open");
    QFile file(Filename);
    if(!file.open(QIODevice::ReadOnly | QFile::Text)){
       QMessageBox::warning(this, "Warning", "Unable to Open File");
       return;
    }

    QTextStream in(&file);
    QString Text = in.readAll();
    ui->InputBox->setText(Text);
    file.close();
}

void Sorter::on_actionExport_triggered()
{
    QString Filename = QFileDialog::getSaveFileName(this, "Save File as");
    QFile file(Filename);
    if(!file.open(QIODevice::WriteOnly | QFile::Text)){
       QMessageBox::warning(this, "Warning", "Unable to Save File");
       return;
    }
    QTextStream out (&file);
    QString Text = ui->OutputBox->toPlainText();
    out << Text;
    file.close();

}

void Sorter::on_actionClear_triggered()
{
    ui->InputBox->clear();
    ui->OutputBox->clear();
}

void Sorter::on_actionDebug_triggered()
{
    if(debug == false)
    {
        debug = true;
        this->setWindowTitle("Sorter :: DEBUG MODE");
        QIcon *ico = new QIcon();
            ico->addPixmap(QPixmap(":/images/debug active.jpg"),QIcon::Active,QIcon::On);
            ui->actionDebug->setIcon(*ico);

    }
    else {
        debug = false;
        this->setWindowTitle("Sorter");
                QIcon *ico = new QIcon();
                    ico->addPixmap(QPixmap(":/img/debug.png"),QIcon::Active,QIcon::On);
                    ui->actionDebug->setIcon(*ico);

    }
}

void Sorter::on_pushButton_clicked()
{
    int w = ui->centralWidget->width();
    int h = ui->centralWidget->height();
    ui->centralWidget->resize(w + 200 ,h + 200);
}

void Sorter::on_Version_triggered()
{
    ChangeLog version;
    version.setModal(true);
    version.exec();
}

void Sorter::on_actionExit_triggered()
{
    this->close();
}

void Sorter::on_actionReset_Window_triggered()
{
    this->setGeometry(530,388,1500,400);
}

void Sorter::on_actionReset_Window_toggled()
{
    this->setGeometry(530,388,1500,400);
}

void Sorter::on_SortModeBox_currentIndexChanged()
{
    SortModeBoxIndex = ui->SortModeBox->currentIndex();
    if(debug)
    qDebug() << ui->SortModeBox->currentIndex();

    Sorter::Load_List();
}

void Sorter::on_actionRestore_DataBase_File_triggered()
{
    DataBaseFile = defaultfile;
    filesave = false;
    ui->File_Dir->setText("Program Default DataBase");
}

void Sorter::on_actionChoose_Custom_DataBase_triggered()
{
    QString Filename = QFileDialog::getOpenFileName(this, "Select File to Open");
    if(debug)
    qDebug() << Filename;

    DataBaseFile = Filename;
    ui->File_Dir->setText(Filename);
}

void Sorter::on_actionAll_triggered()
{
    ui->OutputFormatBox->setCurrentIndex(0);
    ui->SortModeBox->setCurrentIndex(0);
    debug = false;
    this->setGeometry(530,388,1500,400);
    DataBaseFile = defaultfile;
    filesave = false;
    //ui->font_size->setValue(defaultfontsize);
    ui->File_Dir->setText("Program Default Database");
}

void Sorter::on_fontComboBox_currentFontChanged(const QFont &f)
{
    ui->InputBox->setFont(f);
    ui->OutputBox->setFont(f);
}

void Sorter::on_font_size_valueChanged(int arg1)
{
    fontsize = arg1;
    ui->InputBox->setFontPointSize(arg1);
    ui->OutputBox->setFontPointSize(arg1);
    QString inbox_cpy = ui->InputBox->toPlainText();
    QString outbox_cpy = ui->OutputBox->toPlainText();

    if(!inbox_cpy.isEmpty())
    {
    ui->InputBox->clear();
    ui->InputBox->setPlainText(inbox_cpy);
    }
    if(!outbox_cpy.isEmpty())
    {
        ui->OutputBox->clear();
        ui->OutputBox->setPlainText(outbox_cpy);
    }
}

void Sorter::on_actionRun_Logic_Diagnostic_triggered()
{
    debug = true;
    if(QMessageBox::question(this,"Logic Diagnositic Mode",
   "This will run an Automatic Logic check of the database.\n\n "

   "The purpose of this diagnostic is to check RegEx Logic\n "
   "in hopes to identify errors after review of the diagnostic output files. "))
    {
        Logic_Diagnostic();
       // Logic_Diagnostic(2);

                debug = false;
    }
}
