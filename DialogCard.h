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

struct  FlashCardOptions;
class   FlashCard;

class DialogCard : public wxDialog
{
public:
    enum
    {
        CARD_CTRL_LEARNED,
        CARD_CTRL_ENABLED,
        CARD_CTRL_NAVIGATE,
        CARD_CTRL_QUIZ
    };

    typedef             std::vector<FlashCard*> CardDeck;

    DialogCard(wxWindow* parent, unsigned controls, const CardDeck& cards, const FlashCardOptions& options);

    void                OnButtonShow(wxCommandEvent& event);
    void                OnButtonYes(wxCommandEvent& event);
    void                OnButtonPartially(wxCommandEvent& event);
    void                OnButtonNo(wxCommandEvent& event);
    void                OnButtonNext(wxCommandEvent& event);
    void                OnButtonPrevious(wxCommandEvent& event);
    void                OnCheckboxLearned(wxCommandEvent& event);
    void                OnCheckboxEnabled(wxCommandEvent& event);
    void                OnHtmlQuestionDblClick(wxMouseEvent& event);
    void                OnHtmlAnswerDblClick(wxMouseEvent& event);

private:
    bool                AdvanceCard();
    bool                RewindCard();
    void                UpdateCard();
    void                ShowAnswer(bool show = true);
    void                HideAnswer();
    FlashCard*          GetActiveCard();

    DECLARE_EVENT_TABLE()

    const CardDeck&     m_cards;
    size_t              m_cardIndex;

    wxPanel*            m_panelConceal;
    wxPanel*            m_panelAnswer;
    wxHtmlWindow*       m_htmlQuestion;
    wxHtmlWindow*       m_htmlAnswer;
    wxButton*           m_buttonYes;
    wxButton*           m_buttonPartially;
    wxButton*           m_buttonNo;
    wxButton*           m_buttonNext;
    wxButton*           m_buttonPrevious;
    wxCheckBox*         m_checkboxLearned;
    wxCheckBox*         m_checkboxEnabled;
    wxStaticText*       m_staticRemember;
};
