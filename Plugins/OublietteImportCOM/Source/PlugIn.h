/*
 * This file is part of the Oubliette (http://oubliette.sf.net/) import plugin
 * for KeePass (http://keepass.sf.net/).
 *
 * Copyright (C) 2005-2009 Sebastian Schuberth <sschuberth@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

#ifndef PLUGIN_H
#define PLUGIN_H

#include "../../PluginFramework/PluginImpl.h"

class COublietteImportPlugin:public PluginFramework::PluginImpl
{
  public:

    explicit COublietteImportPlugin(IKpUnknown* pAPI)
    :   PluginFramework::PluginImpl(pAPI)
    ,   m_Utilities(m_PluginAPI)
    ,   m_Database(m_PluginAPI)
    {}

    STDMETHODIMP_(DWORD) GetMenuItemCount();
    STDMETHODIMP_(KP_MENU_ITEM*) GetMenuItems();

    STDMETHODIMP_(BOOL) OnMessage(DWORD dwCode,LPARAM lParamW,LPARAM lParamL);

  private:

    bool ImportOublietteFile(LPCTSTR name);

    PluginFramework::Utilities m_Utilities;
    PluginFramework::Database m_Database;
};

#endif // PLUGIN_H
