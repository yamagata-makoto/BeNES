/*
    BeNES - Nintendo Entertaiment System Emulator for BeOS

    * (C) 2000 by makoto yamagata

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/
#include <interface/Alert.h>
#include <interface/Button.h>
#include <interface/CheckBox.h>

#include <memory>
#include <storage/FilePanel.h>
#include <storage/Path.h>

#include "BeOS/BNApplication.h"
#include "BeOS/BNPreference.h"

inline void
ShowMessage(char* msg)
{
  BAlert* alert = new BAlert(NULL, msg, "OK");
  alert->Go();
}

BNApplication::BNApplication()
  : BApplication("application/x-vnd.mkt-BeNES")
  , panel(new BNPanel)
  , screen(new BNScreen)
  , window(new BNWindow)
  , casette(NULL)
{
  current = screen;
}

BNApplication::~BNApplication() {}

void
BNApplication::ReadyToRun()
{
  panel->Show();
}

bool
BNApplication::QuitRequested()
{
  if (NES::Instance().isRunning()) {
    current->QuitRequested();
    panel->Show();
    return false;
  }
  return true;
}

void
BNApplication::MessageReceived(BMessage* msg)
{
  panel->Lock();
  BCheckBox* check = (BCheckBox*)(panel->FindView(CHECKBOX_FULLSCREEN));
  BButton* button = (BButton*)(panel->FindView(BUTTON_EMULATE));
  switch (msg->what) {
    case BNMSG_BEGIN_EMULATE:
      button->SetEnabled(false);
      check->SetEnabled(false);
      break;
    case BNMSG_END_EMULATE:
      button->SetEnabled(true);
      check->SetEnabled(true);
      break;
  }
  panel->Unlock();
}

void
BNApplication::saveDirectory(BEntry& entry)
{
  BDirectory dir;
  BEntry dir_ent;

  entry.GetParent(&dir);
  dir.GetEntry(&dir_ent);

  BPath path;
  dir_ent.GetPath(&path);

  BNPreference::Instance()->setROMdirectory(path.Path());
}

void
BNApplication::RefsReceived(BMessage* msg)
{
  if (BNPreference::Instance()->isFullScreen()) {
    current = screen;
  } else {
    current = window;
  }
  entry_ref ref;
  msg->FindRef("refs", 0, &ref);

  BEntry entry(&ref);
  saveDirectory(entry);
  BPath path;
  entry.GetPath(&path);

  char err[BUFSIZ];

  std::auto_ptr<ROMfile> romfile(new ROMfile);
  if (romfile->open(path.Path(), err) == YES) {
    delete casette;
    casette = MapperFactory::Create(romfile.get());
    if (casette == NULL) {
      char buff[256];
      sprintf(buff, "unknown mapper = %d\n", romfile->getMapperNumber());
      ShowMessage(buff);
      return;
    }
    casette->setROMdata(romfile.get());
    NES::Instance().insertCasette(casette);
    romfile->close();

    current->Show();
  } else {
    ShowMessage(err);
  }
}
