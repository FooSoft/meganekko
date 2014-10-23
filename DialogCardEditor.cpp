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
#include "DialogCardEditor.h"

BEGIN_EVENT_TABLE(DialogCardEditor, wxDialog)
    EVT_BUTTON(wxID_OK, DialogCardEditor::OnButtonOk)
END_EVENT_TABLE()

DialogCardEditor::DialogCardEditor(wxWindow* parent, wxString* value) :
    m_textEdit(NULL),
    m_value(value)
{
    wxXmlResource::Get()->LoadDialog(this, parent, wxT("DialogCardEditor"));
    m_textEdit = XRCCTRL(*this, "textEdit", wxTextCtrl);
    m_textEdit->ChangeValue(*value);
    m_textEdit->SetFocus();
    SetSize(500, 300);
}

void DialogCardEditor::OnButtonOk(wxCommandEvent& event)
{
    *m_value = m_textEdit->GetValue();
    event.Skip();
}
