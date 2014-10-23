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

class FlashCardManager;
class FlashCard;

class DialogCardManager : public wxDialog
{
public:
    DialogCardManager(wxWindow* parent, FlashCardManager* manager);

    void                OnMenuCardAdd(wxCommandEvent& event);
    void                OnMenuCardRemove(wxCommandEvent& event);
    void                OnMenuCardEnable(wxCommandEvent& event);
    void                OnCheckListCardsIndexChanged(wxCommandEvent& event);
    void                OnCheckListCardsChecked(wxCommandEvent& event);
    void                OnCheckListCardsKeyDown(wxKeyEvent& event);
    void                OnCheckListCardsContextMenu(wxContextMenuEvent& event);
    void                OnNotebookCardPageChanged(wxNotebookEvent& event);
    void                OnCardSummaryChanged(wxCommandEvent& event);
    void                OnCardTextChanged(wxCommandEvent& event);

private:
    void                AddCard();
    void                RemoveCard();
    void                UpdateCards();
    void                UpdateCard();

    bool                IsSelectionChecked() const;

    enum FilterMode
    {
        FILTER_MODE_QUESTION,
        FILTER_MODE_ANSWER
    };

    enum
    {
        ID_MENU_CARD_ADD,
        ID_MENU_CARD_REMOVE,
        ID_MENU_CARD_ENABLE
    };

    typedef             std::map<int, FlashCard*> CardMap;

    DECLARE_EVENT_TABLE()

    FlashCardManager*   m_manager;
    CardMap             m_cardMap;

    wxTextCtrl*         m_textQuestion;
    wxHtmlWindow*       m_htmlQuestion;
    wxTextCtrl*         m_textAnswer;
    wxHtmlWindow*       m_htmlAnswer;
    wxTextCtrl*         m_textFilter;
    wxChoice*           m_choiceSearch;
    wxChoice*           m_choiceSort;
    wxCheckListBox*     m_listCards;
    wxNotebook*         m_notebookCard;
    wxStaticText*       m_staticDeck;
    wxStaticText*       m_staticRemembered;
    wxStaticText*       m_staticForgotten;
    wxStaticText*       m_staticBungled;
    wxStaticText*       m_staticAdded;
    wxStaticText*       m_staticReviewPrevious;
    wxStaticText*       m_staticReviewNext;
};
