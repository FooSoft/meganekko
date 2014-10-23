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
#include "FrameMeganekko.h"
#include "DialogCardManager.h"
#include "DialogCard.h"
#include "DialogAbout.h"
#include "DialogOptions.h"

BEGIN_EVENT_TABLE(FrameMeganekko, wxFrame)
    EVT_MENU(XRCID("menuFileNew"),                  FrameMeganekko::OnMenuFileNew)
    EVT_MENU(XRCID("menuFileOpen"),                 FrameMeganekko::OnMenuFileOpen)
    EVT_MENU(XRCID("menuFileSave"),                 FrameMeganekko::OnMenuFileSave)
    EVT_MENU(XRCID("menuFileSaveAs"),               FrameMeganekko::OnMenuFileSaveAs)
    EVT_MENU(XRCID("menuFileExit"),                 FrameMeganekko::OnMenuFileExit)
    EVT_MENU(XRCID("menuToolsCardsManage"),         FrameMeganekko::OnMenuToolsCardsManage)
    EVT_MENU(XRCID("menuToolsCardsExpire"),         FrameMeganekko::OnMenuToolsCardsExpire)
    EVT_MENU(XRCID("menuToolsOptions"),             FrameMeganekko::OnMenuToolsOptions)
    EVT_MENU(XRCID("menuToolsReviewSequential"),    FrameMeganekko::OnMenuToolsReviewSequential)
    EVT_MENU(XRCID("menuToolsReviewStudy"),         FrameMeganekko::OnMenuToolsReviewStudy)
    EVT_MENU(XRCID("menuHelpHomepage"),             FrameMeganekko::OnMenuHelpHomepage)
    EVT_MENU(XRCID("menuHelpAbout"),                FrameMeganekko::OnMenuHelpAbout)
    EVT_BUTTON(XRCID("buttonExpired"),              FrameMeganekko::OnButtonExpired)
    EVT_BUTTON(XRCID("buttonFailed"),               FrameMeganekko::OnButtonFailed)
    EVT_BUTTON(XRCID("buttonUntested"),             FrameMeganekko::OnButtonUntested)
    EVT_BUTTON(XRCID("buttonPending"),              FrameMeganekko::OnButtonPending)
    EVT_CLOSE(FrameMeganekko::OnClose)
END_EVENT_TABLE()

FrameMeganekko::FrameMeganekko(const wxString& filename) :
        m_gaugeExpired(NULL),
        m_gaugeFailed(NULL),
        m_gaugeUntested(NULL),
        m_gaugePending(NULL),
        m_buttonExpired(NULL),
        m_buttonFailed(NULL),
        m_buttonUntested(NULL),
        m_buttonPending(NULL)
{
    wxXmlResource::Get()->LoadFrame(this, NULL, wxT("FrameMeganekko"));
    SetSize(wxSize(640, 480));

    m_gaugeExpired      = XRCCTRL(*this, "gaugeExpired", wxGauge);
    m_gaugeFailed       = XRCCTRL(*this, "gaugeFailed", wxGauge);
    m_gaugeUntested     = XRCCTRL(*this, "gaugeUntested", wxGauge);
    m_gaugePending      = XRCCTRL(*this, "gaugePending", wxGauge);
    m_buttonExpired     = XRCCTRL(*this, "buttonExpired", wxButton);
    m_buttonFailed      = XRCCTRL(*this, "buttonFailed", wxButton);
    m_buttonUntested    = XRCCTRL(*this, "buttonUntested", wxButton);
    m_buttonPending     = XRCCTRL(*this, "buttonPending", wxButton);

    if (!filename.IsEmpty())
    {
        OpenDecks(filename);
    }

    UpdateDecks();
}

void FrameMeganekko::OnMenuFileNew(wxCommandEvent& event)
{
    if (!SavePromptBail())
    {
        NewDecks();
    }
}

void FrameMeganekko::OnMenuFileOpen(wxCommandEvent& event)
{
    if (SavePromptBail())
    {
        return;
    }

    wxString filename = wxFileSelector(
                            wxT("Choose a deck file to open"),
                            wxEmptyString,
                            wxEmptyString,
                            wxT("mnko"),
                            wxT("Meganekko files|*.mnko"),
                            wxOPEN,
                            this
                        );

    if (filename.IsEmpty())
    {
        return;
    }

#ifdef wxGTK
    // is this a bug in gtk wx? the default_extension doesn't appear to do anything
    // so for now just explicitly provide an extension if one isn't specified
    wxFileName temp(filename);
    if (temp.GetExt().IsEmpty())
    {
        temp.SetExt(wxT("mnko"));
        filename = temp.GetFullPath();
    }
#endif

    OpenDecks(filename);
}

void FrameMeganekko::OnMenuFileSave(wxCommandEvent& event)
{
    SaveDecks();
}

void FrameMeganekko::OnMenuFileSaveAs(wxCommandEvent& event)
{
    SaveDecksAs();
}

void FrameMeganekko::OnMenuFileExit(wxCommandEvent&)
{
    Close(true);
}

void FrameMeganekko::OnMenuToolsCardsManage(wxCommandEvent& event)
{
    DialogCardManager* const dialog = new DialogCardManager(this, &m_manager);
    dialog->ShowModal();
    UpdateDecks();
}

void FrameMeganekko::OnMenuToolsCardsExpire(wxCommandEvent& event)
{
    const int count = m_manager.ExpireCards();
    if (count == 0)
    {
        wxMessageBox(wxT("There are no newly expired cards"), wxT("Meganekko"), wxOK | wxICON_INFORMATION, this);
    }
    else
    {
        wxMessageBox(wxString::Format(wxT("There are %d newly expired cards"), count), wxT("Meganekko"), wxOK | wxICON_INFORMATION, this);
        UpdateDecks();
    }
}

void FrameMeganekko::OnMenuToolsOptions(wxCommandEvent& event)
{
    DialogOptions* const dialog = new DialogOptions(this, &m_manager);
    dialog->ShowModal();
    UpdateDecks();
}

void FrameMeganekko::OnMenuToolsReviewSequential(wxCommandEvent& event)
{
    std::vector<FlashCard*> cards;
    m_manager.EnumerateCards(&cards, static_cast<unsigned>(-1) & ~BIT(DECK_TYPE_FAILED), true, false, DECK_SORT_TYPE_TIME_REVIEW_PREVIOUS);

    if (cards.size() > 0)
    {
        DialogCard* const dialog = new DialogCard(this, BIT(DialogCard::CARD_CTRL_QUIZ), cards, m_manager.GetOptions());
        dialog->ShowModal();
        UpdateDecks();
    }
}

void FrameMeganekko::OnMenuToolsReviewStudy(wxCommandEvent& event)
{
    std::vector<FlashCard*> cards;
    m_manager.EnumerateCards(&cards, static_cast<unsigned>(-1), false, true, DECK_SORT_TYPE_TIME_ADDED);

    if (cards.size() > 0)
    {
        const unsigned controls = BIT(DialogCard::CARD_CTRL_ENABLED) | BIT(DialogCard::CARD_CTRL_NAVIGATE);
        DialogCard* const dialog = new DialogCard(this, controls, cards, m_manager.GetOptions());
        dialog->ShowModal();
        UpdateDecks();
    }
}

void FrameMeganekko::OnMenuHelpHomepage(wxCommandEvent& event)
{
    wxLaunchDefaultBrowser(wxT("http://foosoft.net/meganekko"));
}

void FrameMeganekko::OnMenuHelpAbout(wxCommandEvent& event)
{
    DialogAbout* const dialog = new DialogAbout(this);
    dialog->ShowModal();
}

void FrameMeganekko::OnButtonExpired(wxCommandEvent& event)
{
    UseDeck(DECK_TYPE_EXPIRED);
}

void FrameMeganekko::OnButtonFailed(wxCommandEvent& event)
{
    UseDeck(DECK_TYPE_FAILED);
}

void FrameMeganekko::OnButtonUntested(wxCommandEvent& event)
{
    UseDeck(DECK_TYPE_UNTESTED);
}

void FrameMeganekko::OnButtonPending(wxCommandEvent& event)
{
    UseDeck(DECK_TYPE_PENDING);
}

void FrameMeganekko::OnClose(wxCloseEvent& event)
{
    if (event.CanVeto() && SavePromptBail())
    {
        event.Veto();
    }
    else
    {
        Destroy();
    }
}

bool FrameMeganekko::UseDeck(DeckType type)
{
    unsigned controls = BIT(DialogCard::CARD_CTRL_QUIZ);
    if (type == DECK_TYPE_FAILED)
    {
        controls = BIT(DialogCard::CARD_CTRL_LEARNED) | BIT(DialogCard::CARD_CTRL_NAVIGATE);
    }

    std::vector<FlashCard*> cards;
    m_manager.EnumerateCards(&cards, BIT(type), true, false, DECK_SORT_TYPE_SHUFFLE);

    if (cards.size() > 0)
    {
        DialogCard* const dialog = new DialogCard(this, controls, cards, m_manager.GetOptions());
        dialog->ShowModal();
        UpdateDecks();
    }

    return cards.size() > 0;
}

bool FrameMeganekko::SaveDecksAs()
{
    wxString filename = wxFileSelector(
                            wxT("Choose a deck file to save"),
                            wxEmptyString,
                            wxEmptyString,
                            wxT("mnko"),
                            wxT("Meganekko files|*.mnko"),
                            wxSAVE,
                            this
                        );

    if (filename.IsEmpty())
    {
        return false;
    }

    m_filename = filename;

#ifdef wxGTK
    // is this a bug in gtk wx? the default_extension doesn't appear to do anything
    // so for now just explicitly provide an extension if one isn't specified
    wxFileName temp(m_filename);
    if (temp.GetExt().IsEmpty())
    {
        temp.SetExt(wxT("mnko"));
        m_filename = temp.GetFullPath();
    }
#endif

    return SaveDecks(m_filename);
}

bool FrameMeganekko::SaveDecks()
{
    return m_filename.IsEmpty() ? SaveDecksAs() : SaveDecks(m_filename);
}

bool FrameMeganekko::SaveDecks(const wxString& filename)
{
    if (!m_manager.Save(filename.c_str()))
    {
        wxMessageBox(wxString::Format(wxT("Cannot save deck %s"), filename.c_str()), wxT("Meganekko"), wxOK | wxICON_ERROR, this);
        return false;
    }

    m_filename = filename;
    UpdateDecks();
    return true;
}

bool FrameMeganekko::OpenDecks(const wxString& filename)
{
    if (!m_manager.Open(filename.c_str()))
    {
        wxMessageBox(wxString::Format(wxT("Cannot open deck %s"), filename.c_str()), wxT("Meganekko"), wxOK | wxICON_ERROR, this);
        return false;
    }

    m_filename = filename;
    UpdateDecks();
    return true;
}

bool FrameMeganekko::NewDecks()
{
    m_manager.Close();
    m_filename.Clear();
    UpdateDecks();
    return true;
}

void FrameMeganekko::UpdateDecks()
{
    const wxString title = wxString::Format(
        wxT("Meganekko - %s %c"),
        m_filename.IsEmpty() ? wxT("Untitled") : m_filename.c_str(),
        m_manager.IsModified() ? '*' : ' '
    );
    SetTitle(title);

    const int expired = m_manager.GetDeckSize(DECK_TYPE_EXPIRED, false);
    const int failed = m_manager.GetDeckSize(DECK_TYPE_FAILED, false);
    const int untested = m_manager.GetDeckSize(DECK_TYPE_UNTESTED, false);
    const int pending = m_manager.GetDeckSize(DECK_TYPE_PENDING, false);
    const int total = expired + failed + untested + pending;

    float percentExpired = 0;
    float percentFailed = 0;
    float percentUntested = 0;
    float percentPending = 0;

    if (total > 0)
    {
        percentExpired = 100.0f * static_cast<float>(expired) / static_cast<float>(total);
        percentFailed = 100.0f * static_cast<float>(failed) / static_cast<float>(total);
        percentUntested = 100.0f * static_cast<float>(untested) / static_cast<float>(total);
        percentPending = 100.0f * static_cast<float>(pending) / static_cast<float>(total);
    }

    m_gaugeExpired->SetValue(static_cast<int>(percentExpired));
    m_gaugeExpired->SetToolTip(wxString::Format(wxT("%.2f%%"), percentExpired));
    m_buttonExpired->SetLabel(wxString::Format(wxT("&Expired (%d)"), expired));
    m_buttonExpired->Enable(expired > 0);

    m_gaugeFailed->SetValue(static_cast<int>(percentFailed));
    m_gaugeFailed->SetToolTip(wxString::Format(wxT("%.2f%%"), percentFailed));
    m_buttonFailed->SetLabel(wxString::Format(wxT("F&ailed (%d)"), failed));
    m_buttonFailed->Enable(failed > 0);

    m_gaugeUntested->SetValue(static_cast<int>(percentUntested));
    m_gaugeUntested->SetToolTip(wxString::Format(wxT("%.2f%%"), percentUntested));
    m_buttonUntested->SetLabel(wxString::Format(wxT("&Untested (%d)"), untested));
    m_buttonUntested->Enable(untested > 0);

    m_gaugePending->SetValue(static_cast<int>(percentPending));
    m_gaugePending->SetToolTip(wxString::Format(wxT("%.2f%%"), percentPending));
    m_buttonPending->SetLabel(wxString::Format(wxT("&Pending (%d)"), pending));
    m_buttonPending->Enable(pending > 0);
}

bool FrameMeganekko::SavePromptBail()
{
    if (!m_manager.IsModified())
    {
        return false;
    }

    const int result = m_manager.GetOptions().autoSave ? wxYES : wxMessageBox(
        wxT("The flash card database has been modified, do you want to save?"),
        wxT("Meganekko"),
        wxYES_NO | wxCANCEL | wxICON_QUESTION,
        this
    );

    if (result == wxNO || (result == wxYES && SaveDecks()))
    {
        return false;
    }

    return true;
}
