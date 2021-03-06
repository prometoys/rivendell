// list_sas_resources.cpp
//
// List SAS Resources.
//
//   (C) Copyright 2002-2019 Fred Gleason <fredg@paravelsystems.com>
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License version 2 as
//   published by the Free Software Foundation.
//
//   This program is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//   GNU General Public License for more details.
//
//   You should have received a copy of the GNU General Public
//   License along with this program; if not, write to the Free Software
//   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
//

#include <rd.h>
#include <rddb.h>
#include <rdescape_string.h>

#include "edit_sas_resource.h"
#include "edit_user.h"
#include "list_sas_resources.h"

ListSasResources::ListSasResources(RDMatrix *matrix,int size,QWidget *parent)
  : RDDialog(parent)
{
  setModal(true);

  QString sql;
  QString str;

  list_matrix=matrix;
  list_size=size;
  setWindowTitle("RDAdmin - "+tr("SAS Switches"));

  //
  // Fix the Window Size
  //
  setMinimumSize(sizeHint());

  //
  // Resources List Box
  //
  list_list_view=new Q3ListView(this);
  list_title_label=
    new QLabel(list_list_view,tr("SAS Switches"),this);
  list_title_label->setFont(labelFont());
  list_list_view->setAllColumnsShowFocus(true);
  list_list_view->setItemMargin(5);
  list_list_view->addColumn(tr("GPIO Line"));
  list_list_view->setColumnAlignment(0,Qt::AlignHCenter);
  list_list_view->addColumn(tr("Console"));
  list_list_view->setColumnAlignment(1,Qt::AlignHCenter);
  list_list_view->addColumn(tr("Source"));
  list_list_view->setColumnAlignment(2,Qt::AlignHCenter);
  list_list_view->addColumn(tr("Opto/Relay"));
  list_list_view->setColumnAlignment(3,Qt::AlignHCenter);
  connect(list_list_view,
	  SIGNAL(doubleClicked(Q3ListViewItem *,const QPoint &,int)),
	  this,
	  SLOT(doubleClickedData(Q3ListViewItem *,const QPoint &,int)));

  //
  //  Edit Button
  //
  list_edit_button=new QPushButton(this);
  list_edit_button->setFont(buttonFont());
  list_edit_button->setText(tr("&Edit"));
  connect(list_edit_button,SIGNAL(clicked()),this,SLOT(editData()));

  //
  //  Ok Button
  //
  list_ok_button=new QPushButton(this);
  list_ok_button->setDefault(true);
  list_ok_button->setFont(buttonFont());
  list_ok_button->setText(tr("&OK"));
  connect(list_ok_button,SIGNAL(clicked()),this,SLOT(okData()));

  //
  //  Cancel Button
  //
  list_cancel_button=new QPushButton(this);
  list_cancel_button->setFont(buttonFont());
  list_cancel_button->setText(tr("&Cancel"));
  connect(list_cancel_button,SIGNAL(clicked()),this,SLOT(cancelData()));

  //
  // Load Values
  //
  RefreshList();
}


QSize ListSasResources::sizeHint() const
{
  return QSize(400,250);
} 


QSizePolicy ListSasResources::sizePolicy() const
{
  return QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
}


void ListSasResources::editData()
{
  int engine_num=-1;
  int device_num=-1;
  int relay_num=-1;

  Q3ListViewItem *item=list_list_view->selectedItem();
  if(item==NULL) {
    return;
  }
  if(!item->text(1).isEmpty()) {
    engine_num=item->text(1).toInt();
  }
  if(!item->text(2).isEmpty()) {
    device_num=item->text(2).toInt();
  }
  if(!item->text(3).isEmpty()) {
    relay_num=item->text(3).toInt();
  }
  EditSasResource *dialog=
    new EditSasResource(&engine_num,&device_num,&relay_num,this);
  if(dialog->exec()==0) {
    if(engine_num>=0) {
      item->setText(1,QString().sprintf("%d",engine_num));
    }
    else {
      item->setText(1,"");
    }
    if(device_num>=0) {
      item->setText(2,QString().sprintf("%d",device_num));
    }
    else {
      item->setText(2,"");
    }
    if(relay_num>=0) {
      item->setText(3,QString().sprintf("%d",relay_num));
    }
    else {
      item->setText(3,"");
    }
  }
  delete dialog;
}


void ListSasResources::doubleClickedData(Q3ListViewItem *item,
					    const QPoint &pt,int col)
{
  editData();
}


void ListSasResources::okData()
{
  QString sql;
  RDSqlQuery *q;
  int engine_num=-1;
  int device_num=-1;
  int surface_num=-1;
  int relay_num=-1;

  Q3ListViewItem *item=list_list_view->firstChild();
  while(item!=NULL) {
    engine_num=-1;
    device_num=-1;
    surface_num=-1;
    relay_num=-1;
    if(!item->text(1).isEmpty()) {
      engine_num=item->text(1).toInt();
    }
    if(!item->text(2).isEmpty()) {
      device_num=item->text(2).toInt();
    }
    if(!item->text(3).isEmpty()) {
      relay_num=item->text(3).toInt();
    }
    sql=QString("select ID from VGUEST_RESOURCES where" )+
      "(STATION_NAME=\""+RDEscapeString(list_matrix->station())+"\")&&"+
      QString().sprintf("(MATRIX_NUM=%d)&&",list_matrix->matrix())+
      QString().sprintf("(NUMBER=%d)",item->text(0).toInt());
    q=new RDSqlQuery(sql);
    if(q->first()) {
      sql=QString("update VGUEST_RESOURCES set ")+
	QString().sprintf("ENGINE_NUM=%d,",engine_num)+
	QString().sprintf("DEVICE_NUM=%d,",device_num)+
	QString().sprintf("SURFACE_NUM=%d,",surface_num)+
	QString().sprintf("RELAY_NUM=%d where ",relay_num)+
	"(STATION_NAME=\""+RDEscapeString(list_matrix->station())+"\")&&"+
	QString().sprintf("(MATRIX_NUM=%d)&&",list_matrix->matrix())+
	QString().sprintf("(NUMBER=%d)",item->text(0).toInt());
    }
    else {
      sql=QString("insert into VGUEST_RESOURCES set ")+
	"STATION_NAME=\""+RDEscapeString(list_matrix->station())+"\","+
	QString().sprintf("MATRIX_NUM=%d,",list_matrix->matrix())+
	QString().sprintf("NUMBER=%d,",item->text(0).toInt())+
	QString().sprintf("ENGINE_NUM=%d,",engine_num)+
	QString().sprintf("DEVICE_NUM=%d,",device_num)+
	QString().sprintf("SURFACE_NUM=%d,",surface_num)+
	QString().sprintf("RELAY_NUM=%d",relay_num);
    }
    q=new RDSqlQuery(sql);
    delete q;
    item=item->nextSibling();
  }
  done(0);
}


void ListSasResources::cancelData()
{
  done(-1);
}


void ListSasResources::resizeEvent(QResizeEvent *e)
{
  list_list_view->setGeometry(10,24,size().width()-20,size().height()-94);
  list_title_label->setGeometry(14,5,85,19);
  list_edit_button->setGeometry(10,size().height()-60,80,50);
  list_ok_button->setGeometry(size().width()-180,size().height()-60,80,50);
  list_cancel_button->setGeometry(size().width()-90,size().height()-60,80,50);
}


void ListSasResources::RefreshList()
{
  QString sql;
  RDSqlQuery *q;
  RDSqlQuery *q1;
  Q3ListViewItem *item;
  int n=1;
  int gpis;

  //
  // Populate Resource Records
  //
  sql=QString("select GPIS from MATRICES where ")+
    "(STATION_NAME=\""+RDEscapeString(list_matrix->station())+"\")&&"+
    QString().sprintf("(MATRIX=%d)",list_matrix->matrix());
  q=new RDSqlQuery(sql);
  if(!q->first()) {
    delete q;
    return;
  }
  gpis=q->value(0).toInt();
  delete q;
  for(int i=0;i<gpis;i++) {
    sql=QString("select NUMBER from VGUEST_RESOURCES where ")+
      "(STATION_NAME=\""+RDEscapeString(list_matrix->station())+"\")&&"+
      QString().sprintf("(MATRIX_NUM=%d)&&",list_matrix->matrix())+
      QString().sprintf("(NUMBER=%d)",i+1);
    q=new RDSqlQuery(sql);
    if(!q->first()) {
      sql=QString("insert into VGUEST_RESOURCES set ")+
	QString().sprintf("NUMBER=%d,",i+1)+
	"STATION_NAME=\""+RDEscapeString(list_matrix->station())+"\","+
	QString().sprintf("MATRIX_NUM=%d",list_matrix->matrix());
      q1=new RDSqlQuery(sql);
      delete q1;
    }
    delete q;
  }

  sql=QString("select ")+
    "NUMBER,"+
    "ENGINE_NUM,"+
    "DEVICE_NUM,"+
    "RELAY_NUM "+
    "from VGUEST_RESOURCES where "+
    "(STATION_NAME=\""+RDEscapeString(list_matrix->station())+"\")&&"+
    QString().sprintf("(MATRIX_NUM=%d)",list_matrix->matrix())+
    "order by NUMBER";
  q=new RDSqlQuery(sql);
  list_list_view->clear();
  while(q->next()) {
    while(q->value(0).toInt()>n) {
      item=new Q3ListViewItem(list_list_view);
      item->setText(0,QString().sprintf("%03d",n++));
    }
    item=new Q3ListViewItem(list_list_view);
    item->setText(0,QString().sprintf("%03d",q->value(0).toInt()));
    if(q->value(1).toInt()>=0) {
      item->setText(1,QString().sprintf("%d",q->value(1).toInt()));
    }
    if(q->value(2).toInt()>=0) {
      item->setText(2,QString().sprintf("%d",q->value(2).toInt()));
    }
    if(q->value(3).toInt()>=0) {
      item->setText(3,QString().sprintf("%d",q->value(3).toInt()));
    }
    n++;
  }
  for(int i=n;i<(list_size+1);i++) {
    item=new Q3ListViewItem(list_list_view);
    item->setText(0,QString().sprintf("%03d",i));
  } 
  delete q;
}
