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
#include "DialogOptions.h"
#include "FlashCardManager.h"

BEGIN_EVENT_TABLE(DialogOptions, wxDialog)
    EVT_BUTTON(wxID_OK, DialogOptions::OnButtonOk)
END_EVENT_TABLE()

DialogOptions::DialogOptions(wxWindow* parent, FlashCardManager* manager) :
    m_manager(manager),
    m_spinTimeReviewMin(NULL),
    m_spinTimeReviewMax(NULL),
    m_sliderTimeReviewEntropy(NULL),
    m_checkAutoSave(NULL),
    m_textFontNameNormal(NULL),
    m_textFontNameFixed(NULL)
{
    wxXmlResource::Get()->LoadDialog(this, parent, wxT("DialogOptions"));

    m_spinTimeReviewMin         = XRCCTRL(*this, "spinTimeReviewMin", wxSpinCtrl);
    m_spinTimeReviewMax         = XRCCTRL(*this, "spinTimeReviewMax", wxSpinCtrl);
    m_sliderTimeReviewEntropy   = XRCCTRL(*this, "sliderTimeReviewEntropy", wxSlider);
    m_checkAutoSave             = XRCCTRL(*this, "checkAutoSave", wxCheckBox);
    m_textFontNameNormal        = XRCCTRL(*this, "textFontNameNormal", wxTextCtrl);
    m_textFontNameFixed         = XRCCTRL(*this, "textFontNameFixed", wxTextCtrl);
    m_spinFontSizes[0]          = XRCCTRL(*this, "spinFontSize0", wxSpinCtrl);
    m_spinFontSizes[1]          = XRCCTRL(*this, "spinFontSize1", wxSpinCtrl);
    m_spinFontSizes[2]          = XRCCTRL(*this, "spinFontSize2", wxSpinCtrl);
    m_spinFontSizes[3]          = XRCCTRL(*this, "spinFontSize3", wxSpinCtrl);
    m_spinFontSizes[4]          = XRCCTRL(*this, "spinFontSize4", wxSpinCtrl);
    m_spinFontSizes[5]          = XRCCTRL(*this, "spinFontSize5", wxSpinCtrl);
    m_spinFontSizes[6]          = XRCCTRL(*this, "spinFontSize6", wxSpinCtrl);

    MoveOptionsToUi();
    Fit();
}

void DialogOptions::OnButtonOk(wxCommandEvent& event)
{
    MoveUiToOptions();
    event.Skip();
}

void DialogOptions::MoveOptionsToUi()
{
    const FlashCardOptions& options = m_manager->GetOptions();
    m_spinTimeReviewMin->SetValue(SECONDS_TO_DAYS(options.timeReviewMin));
    m_spinTimeReviewMax->SetValue(SECONDS_TO_DAYS(options.timeReviewMax));
    m_sliderTimeReviewEntropy->SetValue(options.timeReviewEntropy);
    m_checkAutoSave->SetValue(options.autoSave);
    m_textFontNameNormal->SetValue(options.fontNameNormal);
    m_textFontNameFixed->SetValue(options.fontNameFixed);
    for (size_t i = 0; i < 7; ++i)
    {
        m_spinFontSizes[i]->SetValue(options.fontSizes[i]);
    }
}

void DialogOptions::MoveUiToOptions()
{
    int fontSizes[7] = {0};
    for (int i = 0; i < 7; ++i)
    {
        fontSizes[i] = m_spinFontSizes[i]->GetValue();
    }

    const FlashCardOptions options(
        DAYS_TO_SECONDS(m_spinTimeReviewMin->GetValue()),
        DAYS_TO_SECONDS(m_spinTimeReviewMax->GetValue()),
        m_sliderTimeReviewEntropy->GetValue(),
        m_checkAutoSave->GetValue(),
        m_textFontNameNormal->GetValue().c_str(),
        m_textFontNameFixed->GetValue().c_str(),
        fontSizes
    );

    m_manager->SetOptions(options);
}
