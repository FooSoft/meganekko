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
#include "FlashCardManager.h"

const FlashCardOptions FlashCardOptions::DEFAULT;

FlashCardManager::FlashCardManager() :
        m_modified(false)
{
}

bool FlashCardManager::Open(const std::wstring& filename)
{
    TiXmlDocument document;
    if (document.LoadFile(wstrToUtf8(filename).c_str()))
    {
        Close();
        XmlImportRoot(&document);
        ExpireCards();
        return true;
    }
    return false;
}

bool FlashCardManager::Save(const std::wstring& filename) const
{
    TiXmlDocument document;
    XmlExportRoot(&document);
    if (document.SaveFile(wstrToUtf8(filename).c_str()))
    {
        m_modified = false;
        return true;
    }
    return false;
}

void FlashCardManager::Close()
{
    m_cards.clear();
    m_options = FlashCardOptions::DEFAULT;
    m_modified = false;
}

bool FlashCardManager::IsModified() const
{
    return m_modified;
}

void FlashCardManager::FlagModified()
{
    m_modified = true;
}

const FlashCardOptions& FlashCardManager::GetOptions() const
{
    return m_options;
}

void FlashCardManager::SetOptions(const FlashCardOptions& options)
{
    m_modified |= (
        m_options.timeReviewMin != options.timeReviewMin ||
        m_options.timeReviewMax != options.timeReviewMax ||
        m_options.timeReviewEntropy != options.timeReviewEntropy ||
        m_options.autoSave != options.autoSave ||
        m_options.fontNameNormal != options.fontNameNormal ||
        m_options.fontNameFixed != options.fontNameFixed ||
        memcmp(m_options.fontSizes, options.fontSizes, sizeof(m_options.fontSizes)) != 0
    );
    m_options = options;
}

FlashCard* FlashCardManager::AddCard(const std::wstring& question, const std::wstring& answer, bool enabled)
{
    const time_t timeNow = time(NULL);
    const FlashCard card(
        this,
        DECK_TYPE_UNTESTED,
        question,
        answer,
        enabled,
        0,
        0,
        0,
        timeNow,
        timeNow,
        timeNow
    );
    m_cards.push_front(card);
    FlagModified();
    return &*m_cards.begin();
}

void FlashCardManager::EnumerateCards(std::vector<FlashCard*>* cards, unsigned decks, bool allowEnabled, bool allowDisabled, DeckSortType sort)
{
    for (CardList::iterator iter = m_cards.begin(); iter != m_cards.end(); ++iter)
    {
        if ((allowEnabled || !iter->GetEnabled()) && (allowDisabled || iter->GetEnabled()) && IS_TRUE(decks & BIT(iter->GetDeck())))
        {
            cards->push_back(&*iter);
        }
    }

    if (cards->size() == 0)
    {
        return;
    }

    if (sort == DECK_SORT_TYPE_SHUFFLE)
    {
        for (size_t i = 0; i < cards->size(); ++i)
        {
            std::swap(cards->at(i), cards->at(rand() % cards->size()));
        }
    }
    else
    {
        static int (*const s_compareFuncs[])(const void*, const void*) =
        {
            CompareByTimeAdded,
            CompareByTimeReviewPrevious,
            CompareByTimeReviewNext,
            CompareByDeck,
            CompareByEnabled,
            CompareByQuestion,
            CompareByAnswer,
            CompareByCountRemembered,
            CompareByCountForgotten,
            CompareByCountBungled
        };

        qsort(&cards->at(0), cards->size(), sizeof(FlashCard*), s_compareFuncs[sort]);
    }
}

int FlashCardManager::GetDeckSize(DeckType deck, bool allowDisabled) const
{
    int count = 0;
    for (CardList::const_iterator iter = m_cards.begin(); iter != m_cards.end(); ++iter)
    {
        if (iter->GetDeck() == deck && (allowDisabled || iter->GetEnabled()))
        {
            ++count;
        }
    }
    return count;
}

bool FlashCardManager::RemoveCard(FlashCard* card)
{
    for (CardList::iterator iter = m_cards.begin(); iter != m_cards.end(); ++iter)
    {
        if (&*iter == card)
        {
            m_cards.erase(iter);
            FlagModified();
            return true;
        }
    }
    return false;
}

int FlashCardManager::ExpireCards()
{
    int count = 0;
    for (CardList::iterator iter = m_cards.begin(); iter != m_cards.end(); ++iter)
    {
        if (iter->Expire())
        {
            ++count;
        }
    }
    return count;
}
