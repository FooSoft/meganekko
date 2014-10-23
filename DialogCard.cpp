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
#include "DialogCard.h"
#include "DialogCardEditor.h"
#include "FlashCardManager.h"

BEGIN_EVENT_TABLE(DialogCard, wxDialog)
    EVT_BUTTON(XRCID("buttonShow"),             DialogCard::OnButtonShow)
    EVT_BUTTON(XRCID("buttonYes"),              DialogCard::OnButtonYes)
    EVT_BUTTON(XRCID("buttonPartially"),        DialogCard::OnButtonPartially)
    EVT_BUTTON(XRCID("buttonNo"),               DialogCard::OnButtonNo)
    EVT_BUTTON(XRCID("buttonNext"),             DialogCard::OnButtonNext)
    EVT_BUTTON(XRCID("buttonPrevious"),         DialogCard::OnButtonPrevious)
    EVT_CHECKBOX(XRCID("checkboxLearned"),      DialogCard::OnCheckboxLearned)
    EVT_CHECKBOX(XRCID("checkboxEnabled"),      DialogCard::OnCheckboxEnabled)
END_EVENT_TABLE()

DialogCard::DialogCard(wxWindow* parent, unsigned controls, const CardDeck& cards, const FlashCardOptions& options) :
        m_cards(cards),
        m_cardIndex(0),
        m_panelConceal(NULL),
        m_panelAnswer(NULL),
        m_htmlQuestion(NULL),
        m_htmlAnswer(NULL),
        m_buttonYes(NULL),
        m_buttonPartially(NULL),
        m_buttonNo(NULL),
        m_buttonNext(NULL),
        m_buttonPrevious(NULL),
        m_checkboxLearned(NULL),
        m_checkboxEnabled(NULL),
        m_staticRemember(NULL)
{
    wxXmlResource::Get()->LoadDialog(this, parent, wxT("DialogCard"));

    m_panelConceal      = XRCCTRL(*this, "panelConceal", wxPanel);
    m_panelAnswer       = XRCCTRL(*this, "panelAnswer", wxPanel);
    m_htmlQuestion      = XRCCTRL(*this, "htmlQuestion", wxHtmlWindow);
    m_htmlAnswer        = XRCCTRL(*this, "htmlAnswer", wxHtmlWindow);
    m_buttonYes         = XRCCTRL(*this, "buttonYes", wxButton);
    m_buttonPartially   = XRCCTRL(*this, "buttonPartially", wxButton);
    m_buttonNo          = XRCCTRL(*this, "buttonNo", wxButton);
    m_buttonNext        = XRCCTRL(*this, "buttonNext", wxButton);
    m_buttonPrevious    = XRCCTRL(*this, "buttonPrevious", wxButton);
    m_checkboxLearned   = XRCCTRL(*this, "checkboxLearned", wxCheckBox);
    m_checkboxEnabled   = XRCCTRL(*this, "checkboxEnabled", wxCheckBox);
    m_staticRemember    = XRCCTRL(*this, "staticRemember", wxStaticText);

    m_htmlQuestion->SetFonts(options.fontNameNormal, options.fontNameFixed, options.fontSizes);
    m_htmlQuestion->Connect(wxEVT_LEFT_DCLICK, wxMouseEventHandler(DialogCard::OnHtmlQuestionDblClick), NULL, this);
    m_htmlAnswer->SetFonts(options.fontNameNormal, options.fontNameFixed, options.fontSizes);
    m_htmlAnswer->Connect(wxEVT_LEFT_DCLICK, wxMouseEventHandler(DialogCard::OnHtmlAnswerDblClick), NULL, this);

    if (IS_TRUE(controls & BIT(CARD_CTRL_LEARNED)))
    {
        m_checkboxLearned->Show();
    }
    if (IS_TRUE(controls & BIT(CARD_CTRL_ENABLED)))
    {
        m_checkboxEnabled->Show();
    }
    if (IS_TRUE(controls & BIT(CARD_CTRL_NAVIGATE)))
    {
        m_buttonPrevious->Show();
        m_buttonNext->Show();
    }
    if (IS_TRUE(controls & BIT(CARD_CTRL_QUIZ)))
    {
        m_staticRemember->Show();
        m_buttonYes->Show();
        m_buttonPartially->Show();
        m_buttonNo->Show();
        HideAnswer();
    }

    UpdateCard();
    SetSize(640, 480);
}

void DialogCard::OnButtonShow(wxCommandEvent& event)
{
    ShowAnswer();
}

void DialogCard::OnButtonYes(wxCommandEvent& event)
{
    GetActiveCard()->ScheduleReview(GRADE_TYPE_REMEMBER);
    HideAnswer();
    if (!AdvanceCard())
    {
        EndModal(0);
    }
}

void DialogCard::OnButtonPartially(wxCommandEvent& event)
{
    GetActiveCard()->ScheduleReview(GRADE_TYPE_BUNGLE);
    HideAnswer();
    if (!AdvanceCard())
    {
        EndModal(0);
    }
}

void DialogCard::OnButtonNo(wxCommandEvent& event)
{
    GetActiveCard()->ScheduleReview(GRADE_TYPE_FORGET);
    HideAnswer();
    if (!AdvanceCard())
    {
        EndModal(0);
    }
}

void DialogCard::OnButtonNext(wxCommandEvent& event)
{
    AdvanceCard();
}

void DialogCard::OnButtonPrevious(wxCommandEvent& event)
{
    RewindCard();
}

void DialogCard::OnCheckboxLearned(wxCommandEvent& event)
{
    GetActiveCard()->ScheduleReview(event.IsChecked() ? GRADE_TYPE_LEARN : GRADE_TYPE_UNLEARN);
}

void DialogCard::OnCheckboxEnabled(wxCommandEvent& event)
{
    GetActiveCard()->Enable(event.IsChecked());
}

void DialogCard::OnHtmlQuestionDblClick(wxMouseEvent& event)
{
    FlashCard* const card = GetActiveCard();
    wxString question = card->GetQuestion();

    DialogCardEditor* const dialog = new DialogCardEditor(this, &question);
    if (dialog->ShowModal() == wxID_OK)
    {
        card->SetQuestion(question.c_str());
        UpdateCard();
    }
}

void DialogCard::OnHtmlAnswerDblClick(wxMouseEvent& event)
{
    FlashCard* const card = GetActiveCard();
    wxString answer = card->GetAnswer();

    DialogCardEditor* const dialog = new DialogCardEditor(this, &answer);
    if (dialog->ShowModal() == wxID_OK)
    {
        card->SetAnswer(answer.c_str());
        UpdateCard();
    }
}

void DialogCard::UpdateCard()
{
    m_htmlQuestion->SetPage(GetActiveCard()->GetQuestion());
    m_htmlAnswer->SetPage(GetActiveCard()->GetAnswer());

    m_buttonYes->SetToolTip(wxString::Format(wxT("Expire about %s"), TimeToStringRel(GetActiveCard()->ComputeReview(GRADE_TYPE_REMEMBER, false)).c_str()));
    m_buttonPartially->SetToolTip(wxString::Format(wxT("Expire about %s"), TimeToStringRel(GetActiveCard()->ComputeReview(GRADE_TYPE_BUNGLE, false)).c_str()));
    m_buttonNo->SetToolTip(wxT("Move to failed deck"));

    m_buttonNext->Enable(m_cardIndex < m_cards.size() - 1);
    m_buttonPrevious->Enable(m_cardIndex > 0);
    m_checkboxLearned->SetValue(GetActiveCard()->IsLearned());
    m_checkboxEnabled->SetValue(GetActiveCard()->GetEnabled());

    SetTitle(wxString::Format(wxT("Flash card %d of %d"), m_cardIndex + 1, m_cards.size()));
}

bool DialogCard::AdvanceCard()
{
    if (m_cardIndex + 1 < m_cards.size())
    {
        ++m_cardIndex;
        UpdateCard();
        return true;
    }

    return false;
}

bool DialogCard::RewindCard()
{
    if (m_cardIndex > 0)
    {
        --m_cardIndex;
        UpdateCard();
        return true;
    }

    return false;
}

void DialogCard::ShowAnswer(bool show)
{
    m_panelConceal->Show(!show);
    m_htmlAnswer->Show(show);
    m_buttonYes->Enable(show);
    m_buttonPartially->Enable(show);
    m_buttonNo->Enable(show);
    m_panelAnswer->Layout();
}

void DialogCard::HideAnswer()
{
    ShowAnswer(false);
}

FlashCard* DialogCard::GetActiveCard()
{
    return m_cards[m_cardIndex];
}
