// edit_hotkeys.h
//
// Edit the Hot Keys Configuration for a  Workstation
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

#ifndef EDIT_HOTKEYS_H
#define EDIT_HOTKEYS_H

#include <vector>

#include <qlineedit.h>
#include <q3listview.h>
#include <qcombobox.h>
#include <qpushbutton.h>
#include <qlabel.h>

#include <rdairplay_conf.h>
#include <rddialog.h>
#include <rdhotkeys.h>
#include <rdhotkeylist.h>

class EditHotkeys : public RDDialog
{
  Q_OBJECT
 public:
  EditHotkeys(const QString &station,const QString &module,QWidget *parent);
  ~EditHotkeys();
  QSize sizeHint() const;
  QSizePolicy sizePolicy() const;

 public slots:
  void SetHotKey( );
  void showCurrentKey( );
  void RefreshList( );
  void Clone_RefreshList(const QString &);
  void SetButtonClicked( );
  void clearCurrentItem( );
  void clearAll_Hotkeys( );

 private slots:
  void save();
  void cancel();

 protected:
  void keyReleaseEvent(QKeyEvent *e);
  void keyPressEvent(QKeyEvent *e);
  QLineEdit *keystroke;

 private:
  void SetRow(const QString &param,QString value) const;
  RDHotkeys *station_hotkeys;
  Q3ListView *list_view;
  QString hotkeystrokes;
  QString hotkey_conf;
  QString hotkey_module;
  QLabel *clone_from_host_label;
  QComboBox *clone_from_host_box;
  QPushButton *set_button;
  QPushButton *clear_button;
  QPushButton *show_original_button;
  QPushButton *clear_all_button;
  QPushButton *save_button;
  QPushButton *cancel_button;
  int  keystrokecount;
  bool keyupdated;
  bool alt_key_hit;
  bool ctrl_key_hit;
  RDHotKeyList *myhotkeylist ;
  int current_station_clone_item;
};

#endif  // EDIT_HOTKEYS_H
