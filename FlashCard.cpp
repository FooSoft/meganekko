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
#include "FlashCard.h"
#include "FlashCardManager.h"

FlashCard::FlashCard(
    FlashCardManager*   manager,
    DeckType            deck,
    const std::wstring& question,
    const std::wstring& answer,
    bool                enabled,
    int                 countRemembered,
    int                 countForgotten,
    int                 countBungled,
    time_t              timeReviewPrevious,
    time_t              timeReviewNext,
    time_t              timeAdded
) :
        m_manager(manager),
        m_deck(deck),
        m_question(question),
        m_answer(answer),
        m_enabled(enabled),
        m_countRemembered(countRemembered),
        m_countForgotten(countForgotten),
        m_countBungled(countBungled),
        m_timeReviewPrevious(timeReviewPrevious),
        m_timeReviewNext(timeReviewNext),
        m_timeAdded(timeAdded)
{
    ASSERT(m_timeAdded > 0);
    ASSERT(m_timeReviewPrevious > 0 && m_timeReviewPrevious >= m_timeAdded);
    ASSERT(m_timeReviewNext > 0 && m_timeReviewNext >= m_timeReviewPrevious);
}

void FlashCard::SetDeck(DeckType deck)
{
    if (m_deck != deck)
    {
        m_deck = deck;
        m_manager->FlagModified();
    }
}

DeckType FlashCard::GetDeck() const
{
    return m_deck;
}

void FlashCard::SetQuestion(const std::wstring& question)
{
    if (m_question != question)
    {
        m_question = question;
        m_manager->FlagModified();
    }
}

const std::wstring& FlashCard::GetQuestion() const
{
    return m_question;
}

void FlashCard::SetAnswer(const std::wstring& answer)
{
    if (m_answer != answer)
    {
        m_manager->FlagModified();
        m_answer = answer;
    }
}

const std::wstring& FlashCard::GetAnswer() const
{
    return m_answer;
}

bool FlashCard::GetEnabled() const
{
    return m_enabled;
}

bool FlashCard::IsLearned() const
{
    return m_deck != DECK_TYPE_FAILED;
}

void FlashCard::Enable(bool enable)
{
    if (m_enabled != enable)
    {
        m_enabled = enable;
        m_manager->FlagModified();
    }
}

void FlashCard::Disable()
{
    Enable(false);
}

bool FlashCard::Expire()
{
    if (m_deck == DECK_TYPE_PENDING && m_timeReviewNext <= GetTimeCurrent())
    {
        SetDeck(DECK_TYPE_EXPIRED);
        return true;
    }

    return false;
}

time_t FlashCard::GetTimeReviewPrevious() const
{
    return m_timeReviewPrevious;
}

time_t FlashCard::GetTimeReviewNext() const
{
    return m_timeReviewNext;
}

time_t FlashCard::GetTimeAdded() const
{
    return m_timeAdded;
}

int FlashCard::GetCountRemembered() const
{
    return m_countRemembered;
}

int FlashCard::GetCountForgotten() const
{
    return m_countForgotten;
}

int FlashCard::GetCountBungled() const
{
    return m_countBungled;
}

void FlashCard::SetCountRemembered(int count)
{
    if (count != m_countRemembered)
    {
        m_countRemembered = count;
        m_manager->FlagModified();
    }
}

void FlashCard::SetCountForgotten(int count)
{
    if (count != m_countForgotten)
    {
        m_countForgotten = count;
        m_manager->FlagModified();
    }
}

void FlashCard::SetCountBungled(int count)
{
    if (count != m_countBungled)
    {
        m_countBungled = count;
        m_manager->FlagModified();
    }
}

void FlashCard::SetTimeReviewPrevious(time_t time)
{
    if (time != m_timeReviewPrevious)
    {
        m_timeReviewPrevious = time;
        m_manager->FlagModified();
    }
}

void FlashCard::SetTimeReviewNext(time_t time)
{
    if (time != m_timeReviewNext)
    {
        m_timeReviewNext = time;
        m_manager->FlagModified();
    }
}

void FlashCard::ScheduleReview(GradeType grade)
{
    SetTimeReviewNext(ComputeReview(grade, true));
    SetTimeReviewPrevious(GetTimeCurrent());

    switch (grade)
    {
    case GRADE_TYPE_REMEMBER:
        SetDeck(DECK_TYPE_PENDING);
        SetCountRemembered(m_countRemembered + 1);
        break;
    case GRADE_TYPE_BUNGLE:
        SetDeck(DECK_TYPE_PENDING);
        SetCountBungled(m_countBungled + 1);
        break;
    case GRADE_TYPE_FORGET:
        SetDeck(DECK_TYPE_FAILED);
        SetCountForgotten(m_countForgotten + 1);
        break;
    case GRADE_TYPE_LEARN:
        SetDeck(DECK_TYPE_PENDING);
        break;
    case GRADE_TYPE_UNLEARN:
        SetDeck(DECK_TYPE_FAILED);
        break;
    }
}

time_t FlashCard::ComputeReview(GradeType grade, bool scatter) const
{
    const FlashCardOptions& options = m_manager->GetOptions();
    const int timeReviewEntropy = scatter ? options.timeReviewEntropy : 0;
    const time_t timeNow = GetTimeCurrent();

    time_t timeNext = 0;
    if (grade == GRADE_TYPE_REMEMBER)
    {
        const time_t timeDelta = (std::min(m_timeReviewNext, timeNow) - m_timeReviewPrevious) * 2;
        timeNext = std::max(m_timeReviewNext, timeNow) + ScatterTime(timeDelta, timeReviewEntropy);
    }
    else if (grade == GRADE_TYPE_BUNGLE)
    {
        const time_t timeDelta = (std::min(m_timeReviewNext, timeNow) - m_timeReviewPrevious) / 2;
        timeNext = std::max(m_timeReviewNext, timeNow) + ScatterTime(timeDelta, timeReviewEntropy);
    }
    else
    {
        timeNext = timeNow + ScatterTime(options.timeReviewMin, timeReviewEntropy);
    }

    const time_t timeDeltaNext = timeNext - timeNow;
    if (timeDeltaNext < options.timeReviewMin)
    {
        timeNext = timeNow + options.timeReviewMin;
    }
    else if (timeDeltaNext > options.timeReviewMax)
    {
        timeNext = timeNow + options.timeReviewMax;
    }

    return timeNext;
}

time_t FlashCard::GetTimeCurrent()
{
    return time(NULL);
}

time_t FlashCard::ScatterTime(time_t time, int percent)
{
    if (percent == 0)
    {
        return time;
    }

    const double multiplier = 1.0 + static_cast<double>(percent / 2 - rand() % percent) / 100.0;
    const time_t result = static_cast<time_t>(multiplier * static_cast<double>(time));

    return result;
}
