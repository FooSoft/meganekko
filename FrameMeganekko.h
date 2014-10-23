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

#pragma once
#include "FlashCardManager.h"

class FrameMeganekko : public wxFrame
{
public:
    FrameMeganekko(const wxString& filename);

    void                OnMenuFileNew(wxCommandEvent& event);
    void                OnMenuFileOpen(wxCommandEvent& event);
    void                OnMenuFileSave(wxCommandEvent& event);
    void                OnMenuFileSaveAs(wxCommandEvent& event);
    void                OnMenuFileExit(wxCommandEvent& event);
    void                OnMenuToolsCardsManage(wxCommandEvent& event);
    void                OnMenuToolsCardsExpire(wxCommandEvent& event);
    void                OnMenuToolsOptions(wxCommandEvent& event);
    void                OnMenuToolsReviewSequential(wxCommandEvent& event);
    void                OnMenuToolsReviewStudy(wxCommandEvent& event);
    void                OnMenuHelpHomepage(wxCommandEvent& event);
    void                OnMenuHelpAbout(wxCommandEvent& event);
    void                OnButtonExpired(wxCommandEvent& event);
    void                OnButtonFailed(wxCommandEvent& event);
    void                OnButtonUntested(wxCommandEvent& event);
    void                OnButtonPending(wxCommandEvent& event);
    void                OnClose(wxCloseEvent& event);

private:
    bool                UseDeck(DeckType type);
    bool                SaveDecksAs();
    bool                SaveDecks();
    bool                SaveDecks(const wxString& filename);
    bool                OpenDecks(const wxString& filename);
    bool                NewDecks();
    void                UpdateDecks();
    bool                SavePromptBail();

    DECLARE_EVENT_TABLE()

    FlashCardManager    m_manager;
    wxString            m_filename;

    wxGauge*            m_gaugeExpired;
    wxGauge*            m_gaugeFailed;
    wxGauge*            m_gaugeUntested;
    wxGauge*            m_gaugePending;
    wxButton*           m_buttonExpired;
    wxButton*           m_buttonFailed;
    wxButton*           m_buttonUntested;
    wxButton*           m_buttonPending;
};
