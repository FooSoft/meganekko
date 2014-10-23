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

class FlashCard
{
public:
    FlashCard(
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
    );

    void                SetDeck(DeckType deck);
    DeckType            GetDeck() const;
    void                SetQuestion(const std::wstring& question);
    const std::wstring& GetQuestion() const;
    void                SetAnswer(const std::wstring& answer);
    const std::wstring& GetAnswer() const;
    bool                GetEnabled() const;
    bool                IsLearned() const;
    void                Enable(bool enable = true);
    void                Disable();
    bool                Expire();

    time_t              GetTimeReviewPrevious() const;
    time_t              GetTimeReviewNext() const;
    time_t              GetTimeAdded() const;
    int                 GetCountRemembered() const;
    int                 GetCountForgotten() const;
    int                 GetCountBungled() const;

    void                ScheduleReview(GradeType grade);
    time_t              ComputeReview(GradeType grade, bool scatter) const;

private:
    void                SetCountRemembered(int count);
    void                SetCountForgotten(int count);
    void                SetCountBungled(int count);
    void                SetTimeReviewPrevious(time_t time);
    void                SetTimeReviewNext(time_t time);

    static time_t       GetTimeCurrent();
    static time_t       ScatterTime(time_t time, int percent);

    FlashCardManager*   m_manager;
    DeckType            m_deck;
    std::wstring       	m_question;
    std::wstring        m_answer;
    bool                m_enabled;
    int                 m_countRemembered;
    int                 m_countForgotten;
    int                 m_countBungled;
    time_t              m_timeReviewPrevious;
    time_t              m_timeReviewNext;
    time_t              m_timeAdded;
};
