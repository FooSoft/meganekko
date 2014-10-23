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
#include "DialogCardManager.h"
#include "FlashCardManager.h"

BEGIN_EVENT_TABLE(DialogCardManager, wxDialog)
    EVT_MENU(ID_MENU_CARD_ADD,                          DialogCardManager::OnMenuCardAdd)
    EVT_MENU(ID_MENU_CARD_REMOVE,                       DialogCardManager::OnMenuCardRemove)
    EVT_MENU(ID_MENU_CARD_ENABLE,                       DialogCardManager::OnMenuCardEnable)
    EVT_BUTTON(XRCID("buttonAdd"),                      DialogCardManager::OnMenuCardAdd)
    EVT_BUTTON(XRCID("buttonRemove"),                   DialogCardManager::OnMenuCardRemove)
    EVT_LISTBOX(XRCID("checkListCards"),                DialogCardManager::OnCheckListCardsIndexChanged)
    EVT_CHECKLISTBOX(XRCID("checkListCards"),           DialogCardManager::OnCheckListCardsChecked)
    EVT_TEXT_ENTER(XRCID("textFilter"),                 DialogCardManager::OnCardSummaryChanged)
    EVT_TEXT(XRCID("textQuestion"),                     DialogCardManager::OnCardTextChanged)
    EVT_TEXT(XRCID("textAnswer"),                       DialogCardManager::OnCardTextChanged)
    EVT_CHOICE(XRCID("choiceSearch"),                   DialogCardManager::OnCardSummaryChanged)
    EVT_CHOICE(XRCID("choiceSort"),                     DialogCardManager::OnCardSummaryChanged)
    EVT_NOTEBOOK_PAGE_CHANGING(XRCID("notebookCard"),   DialogCardManager::OnNotebookCardPageChanged)
END_EVENT_TABLE()

DialogCardManager::DialogCardManager(wxWindow* parent, FlashCardManager* manager) :
        m_manager(manager),
        m_textQuestion(NULL),
        m_htmlQuestion(NULL),
        m_textAnswer(NULL),
        m_htmlAnswer(NULL),
        m_textFilter(NULL),
        m_choiceSearch(NULL),
        m_choiceSort(NULL),
        m_listCards(NULL),
        m_notebookCard(NULL),
        m_staticDeck(NULL),
        m_staticRemembered(NULL),
        m_staticForgotten(NULL),
        m_staticBungled(NULL),
        m_staticAdded(NULL),
        m_staticReviewPrevious(NULL),
        m_staticReviewNext(NULL)
{
    wxXmlResource::Get()->LoadDialog(this, parent, wxT("DialogCardManager"));
    const FlashCardOptions& options = m_manager->GetOptions();

    m_textQuestion          = XRCCTRL(*this, "textQuestion", wxTextCtrl);
    m_htmlQuestion          = XRCCTRL(*this, "htmlQuestion", wxHtmlWindow);
    m_textAnswer            = XRCCTRL(*this, "textAnswer", wxTextCtrl);
    m_htmlAnswer            = XRCCTRL(*this, "htmlAnswer", wxHtmlWindow);
    m_textFilter            = XRCCTRL(*this, "textFilter", wxTextCtrl);
    m_choiceSearch          = XRCCTRL(*this, "choiceSearch", wxChoice);
    m_choiceSort            = XRCCTRL(*this, "choiceSort", wxChoice);
    m_listCards             = XRCCTRL(*this, "checkListCards", wxCheckListBox);
    m_notebookCard          = XRCCTRL(*this, "notebookCard", wxNotebook);
    m_staticDeck            = XRCCTRL(*this, "staticDeck", wxStaticText);
    m_staticRemembered      = XRCCTRL(*this, "staticCountRemembered", wxStaticText);
    m_staticForgotten       = XRCCTRL(*this, "staticCountForgotten", wxStaticText);
    m_staticBungled         = XRCCTRL(*this, "staticCountBungled", wxStaticText);
    m_staticAdded           = XRCCTRL(*this, "staticTimeAdded", wxStaticText);
    m_staticReviewPrevious  = XRCCTRL(*this, "staticTimeReviewPrevious", wxStaticText);
    m_staticReviewNext      = XRCCTRL(*this, "staticTimeReviewNext", wxStaticText);

    m_htmlQuestion->SetFonts(options.fontNameNormal, options.fontNameFixed, options.fontSizes);
    m_htmlAnswer->SetFonts(options.fontNameNormal, options.fontNameFixed, options.fontSizes);
    m_listCards->Connect(wxEVT_KEY_DOWN, wxKeyEventHandler(DialogCardManager::OnCheckListCardsKeyDown), NULL, this);
    m_listCards->Connect(wxEVT_CONTEXT_MENU, wxContextMenuEventHandler(DialogCardManager::OnCheckListCardsContextMenu), NULL, this);
    m_textFilter->SetFocus();

    SetSize(800, 600);
    UpdateCards();
}

void DialogCardManager::OnMenuCardAdd(wxCommandEvent& event)
{
    AddCard();
}

void DialogCardManager::OnMenuCardRemove(wxCommandEvent& event)
{
    RemoveCard();
}

void DialogCardManager::OnMenuCardEnable(wxCommandEvent& event)
{
    wxArrayInt selections;
    m_listCards->GetSelections(selections);

    const bool checked = IsSelectionChecked();
    for (unsigned i = 0; i < selections.Count(); ++i)
    {
        m_listCards->Check(selections[i], !checked);
    }
}

void DialogCardManager::OnNotebookCardPageChanged(wxNotebookEvent& event)
{
    if (m_htmlQuestion != NULL && m_htmlAnswer != NULL)
    {
        m_htmlQuestion->SetPage(m_textQuestion->GetValue());
        m_htmlAnswer->SetPage(m_textAnswer->GetValue());
    }
}

void DialogCardManager::OnCheckListCardsIndexChanged(wxCommandEvent& event)
{
    UpdateCard();
}

void DialogCardManager::OnCheckListCardsChecked(wxCommandEvent& event)
{
    const int selection = event.GetSelection();
    FlashCard* const card = m_cardMap.find(selection)->second;
    card->Enable(m_listCards->IsChecked(selection));
}

void DialogCardManager::OnCheckListCardsContextMenu(wxContextMenuEvent& event)
{
    wxArrayInt selections;
    const int selectedCount = m_listCards->GetSelections(selections);

    wxMenu* const menu = new wxMenu();
    menu->Append(ID_MENU_CARD_ADD, wxT("&Add new card"));
    if (selectedCount > 0)
    {
        menu->Append(ID_MENU_CARD_REMOVE, wxT("&Remove card(s)"));
        menu->AppendSeparator();
        menu->AppendCheckItem(ID_MENU_CARD_ENABLE, wxT("&Enable card(s)"))->Check(IsSelectionChecked());
    }

    PopupMenu(menu);
    delete menu;
}

void DialogCardManager::OnCheckListCardsKeyDown(wxKeyEvent& event)
{
    switch (event.GetKeyCode())
    {
    case WXK_DELETE:
        RemoveCard();
        break;
    case WXK_INSERT:
        AddCard();
        break;
    default:
        event.Skip();
        break;
    }
}

void DialogCardManager::OnCardSummaryChanged(wxCommandEvent& event)
{
    UpdateCards();
}

void DialogCardManager::UpdateCards()
{
    const wxString filterText = m_textFilter->GetValue().Strip().Lower();
    const FilterMode filterMode = static_cast<FilterMode>(m_choiceSearch->GetSelection());
    const DeckSortType sortType = static_cast<DeckSortType>(m_choiceSort->GetSelection());

    std::vector<FlashCard*> cards;
    m_manager->EnumerateCards(&cards, static_cast<unsigned>(-1), true, true, sortType);

    m_cardMap.clear();

    std::vector<bool> states;
    wxArrayString questions;
    int index = 0;

    for (std::vector<FlashCard*>::const_iterator iter = cards.begin(); iter != cards.end(); ++iter)
    {
        FlashCard* const card = *iter;
        const wxString question = card->GetQuestion();
        const wxString answer = card->GetAnswer();

        if (!filterText.IsEmpty())
        {
            const wxString questionTemp = question.Strip().Lower();
            const wxString answerTemp = answer.Strip().Lower();
            const bool filtered =
                (filterMode == FILTER_MODE_QUESTION && questionTemp.Find(filterText) == wxNOT_FOUND) ||
                (filterMode == FILTER_MODE_ANSWER && answerTemp.Find(filterText) == wxNOT_FOUND);

            if (filtered)
            {
                continue;
            }
        }

        states.push_back(card->GetEnabled());
        questions.Add(question);

        m_cardMap.insert(std::make_pair(index++, card));
    }

    m_listCards->SetSelection(wxNOT_FOUND);
    m_listCards->Set(questions);
    for (size_t i = 0; i < states.size(); ++i)
    {
        m_listCards->Check(i, states[i]);
    }

    if (m_listCards->GetCount() > 0)
    {
        m_listCards->Select(0);
    }

    UpdateCard();
}

void DialogCardManager::UpdateCard()
{
    wxArrayInt selections;
    const bool canDisplaySelection = m_listCards->GetSelections(selections) == 1;
    m_notebookCard->Enable(canDisplaySelection);

    const wxString unspecified  = wxT("-");
    wxString question           = wxEmptyString;
    wxString answer             = wxEmptyString;
    wxString deck               = unspecified;
    wxString countRemembered    = unspecified;
    wxString countForgotten     = unspecified;
    wxString countBungled       = unspecified;
    wxString timeAdded          = unspecified;
    wxString timeReviewPrevious = unspecified;
    wxString timeReviewNext     = unspecified;

    if (canDisplaySelection)
    {
        const FlashCard* const card = m_cardMap.find(selections[0])->second;

        question        = card->GetQuestion();
        answer          = card->GetAnswer();
        deck            = wxString::FromAscii(DeckTypeToString(card->GetDeck()).c_str());
        countRemembered = wxString::Format(wxT("%d"), card->GetCountRemembered());
        countForgotten  = wxString::Format(wxT("%d"), card->GetCountForgotten());
        countBungled    = wxString::Format(wxT("%d"), card->GetCountBungled());
        timeAdded       = TimeToStringRel(card->GetTimeAdded());

        if (card->GetDeck() != DECK_TYPE_UNTESTED)
        {
            timeReviewPrevious = TimeToStringRel(card->GetTimeReviewPrevious());
        }
        if (card->GetDeck() == DECK_TYPE_EXPIRED || card->GetDeck() == DECK_TYPE_PENDING)
        {
            timeReviewNext = TimeToStringRel(card->GetTimeReviewNext());
        }
    }

    m_textQuestion->ChangeValue(question);
    m_htmlQuestion->SetPage(question);
    m_textAnswer->ChangeValue(answer);
    m_htmlAnswer->SetPage(answer);
    m_staticDeck->SetLabel(deck);
    m_staticRemembered->SetLabel(countRemembered);
    m_staticForgotten->SetLabel(countForgotten);
    m_staticBungled->SetLabel(countBungled);
    m_staticAdded->SetLabel(timeAdded);
    m_staticReviewPrevious->SetLabel(timeReviewPrevious);
    m_staticReviewNext->SetLabel(timeReviewNext);
}

void DialogCardManager::AddCard()
{
    FlashCard* const card = m_manager->AddCard(wxEmptyString, wxEmptyString, true);

    m_listCards->Append(card->GetQuestion());
    const int selectionIndex = m_listCards->GetCount() - 1;
    m_listCards->Check(selectionIndex, card->GetEnabled());
    m_listCards->SetSelection(wxNOT_FOUND);
    m_listCards->SetSelection(selectionIndex);

    m_cardMap.insert(std::make_pair(selectionIndex, card));

    m_notebookCard->SetSelection(1);
    m_textQuestion->SetFocus();

    UpdateCard();
}

void DialogCardManager::RemoveCard()
{
    wxArrayInt selections;
    m_listCards->GetSelections(selections);

    const bool remove =
        selections.Count() > 0 &&
        wxMessageBox(wxT("Are you sure you want to remove the selected card(s)?"), wxT("Meganekko"), wxYES_NO) == wxYES;

    if (remove)
    {
        for (unsigned i = 0; i < selections.Count(); ++i)
        {
            m_manager->RemoveCard(m_cardMap.find(selections[i])->second);
        }

        UpdateCards();
    }
}

void DialogCardManager::OnCardTextChanged(wxCommandEvent& event)
{
    wxArrayInt selections;
    if (m_listCards->GetSelections(selections) == 1)
    {
        const int selection = selections[0];

        FlashCard* const card = m_cardMap.find(selection)->second;
        card->SetQuestion(m_textQuestion->GetValue().c_str());
        card->SetAnswer(m_textAnswer->GetValue().c_str());

        m_listCards->SetString(selection, card->GetQuestion());
        m_listCards->Check(selection, card->GetEnabled());
    }
}

bool DialogCardManager::IsSelectionChecked() const
{
    wxArrayInt selections;
    m_listCards->GetSelections(selections);

    unsigned checked = 0;
    for (unsigned i = 0; i < selections.GetCount(); ++i)
    {
        if (m_listCards->IsChecked(selections[i]))
        {
            ++checked;
        }
    }

    return checked == selections.GetCount() || checked > selections.GetCount() / 2;
}
