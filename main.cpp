#include <QCoreApplication>
#include <iostream>
#include <QVariant>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlDatabase>
#include <QString>
#include <QFile>
#include <QTextStream>

using namespace std;

int main(int argc, char *argv[])
{


    if(argc!=6)
    {
        cout <<"SYNTAX ERROR usage :"<<endl<<"$sqlToMea <user> <password> <baseName> <hostName> <outFileName>"<<endl;
        return 404;
    }
    else
    {
        QCoreApplication a(argc, argv);

        QString user = argv[1];
        QString pwd = argv[2];
        QString base = argv[3];
        QString host = argv[4];
        QString nameFile = argv[5];

        QSqlDatabase laBase;
        laBase=QSqlDatabase::addDatabase("QMYSQL");
        laBase.setHostName(host);
        laBase.setUserName(user);
        laBase.setPassword(pwd);
        laBase.setDatabaseName(base);

        QFile leFichier(nameFile);
        leFichier.open(QFile::WriteOnly);
        QTextStream out(&leFichier);
        out << "<?xml version=\"1.0\" encoding=\"UTF-8\"?><!DOCTYPE xmea PUBLIC \"-//GTHOM//DTD for ENTITY RELATIONSHIP MODEL//EN\" \"http://gthom.btsinfogap.org/gapMea/dtd/mea.dtd\">"<<endl<<"<xmea version=\"1.0\">"<<endl<<"\t"<<"<entities>";

        if(laBase.open())
        {
            QString txtTablesReq ="show tables";
            QSqlQuery tablesReq(txtTablesReq);

            while(tablesReq.next())
            {
              /*  QString txtCreateReq="show create table "+tablesReq.value(0).toString();
                QSqlQuery createReq(txtCreateReq);
                while(createReq.next())
                {
                    QStringList listPK = createReq.value(1).toString().split("PRIMARY KEY (`");
                    for (int i=1; i<listPK.length(); i++)
                    {
                        QStringList listPK2 = listPK[1].split("')");
                        cout << listPK2[1].toStdString();
                    } */
                    QString entityName = tablesReq.value(0).toString();
                    out <<endl<<"\t\t"<<"<entity name=\""+entityName+"\" left=\"0\" top=\"0\" type=\"Entity\">"<<endl<<"\t\t\t"<<"<fields>"<<endl;

                    QString txtFieldsReq = "show fields from "+tablesReq.value(0).toString();
                    QSqlQuery fieldsReq(txtFieldsReq);

                    while(fieldsReq.next())
                    {
                        QString fieldName =  fieldsReq.value(0).toString();
                        QStringList sLFieldType = fieldsReq.value(1).toString().split("(");
                        QString fieldType=sLFieldType[0];
                        if(sLFieldType.count()>1)
                        {
                            QStringList sLFieldSize = sLFieldType[1].split(")");
                            QString fieldSize = sLFieldSize[0];

                            out <<"\t\t\t\t"<<"<property fieldname=\""+fieldName+"\" fieldtype=\""+fieldType+"\" fieldsize=\""+fieldSize+"\" fieldrole=\"\" />"<<endl;
                        }
                        else
                        {
                            out <<"\t\t\t\t"<<"<property fieldname=\""+fieldName+"\" fieldtype=\""+fieldType+"\" fieldsize=\"\" fieldrole=\"\" />"<<endl;
                        }
                    }
                    out <<"\t\t\t"<<"</fields>"<<endl<<"\t\t"<<"</entity>"<<endl;
                }
            //}

            QString txtTablesReq2 ="show tables";
            QSqlQuery tablesReq2(txtTablesReq2);

            out <<"\t"<<"</entities>"<<endl<<endl<<"\t"<<"<links>"<<endl;

            while(tablesReq2.next())
            {
                QString txtLinksReq="show create table "+tablesReq2.value(0).toString();
                QSqlQuery linksReq(txtLinksReq);
                while(linksReq.next())
                {
                    QStringList listFK = linksReq.value(1).toString().split("REFERENCES `");

                    if(listFK.length()>0)
                    {
                        for (int i=1; i<listFK.length(); i++)
                        {
                            QStringList listT2 = listFK[i].split("`");
                            QString t2 = listT2[0];

                            out << "\t\t"<<"<link t1=\""<<tablesReq2.value(0).toString()<<"\" t2=\""<<t2<<"\" type=\"Cif\" card1=\"1,1\" card2=\"0,n\" role=\"\" />"<<endl;
                        }
                    }

                }
            }
            out <<"\t"<<"</links>"<<endl<<"</xmea>";
            leFichier.close();
        }
    }
       return 0;
}
