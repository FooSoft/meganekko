/*
    Meganekko  Copyright (C) 2008  Alex Yatskov

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "Pch.h"
#include "AppMeganekko.h"
#include "FrameMeganekko.h"

bool AppMeganekko::OnInit()
{
    srand(time(NULL));

    wxInitAllImageHandlers();
    wxFileSystem::AddHandler(new wxInternetFSHandler());
    wxXmlResource::Get()->InitAllHandlers();
    void InitXmlResource();
    InitXmlResource();

    const wxString filename = argc > 1 ? argv[1] : wxEmptyString;
    FrameMeganekko* frame = new FrameMeganekko(filename);
    frame->Show(true);

    return true;
}
