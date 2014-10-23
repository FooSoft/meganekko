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
#include "FlashCard.h"

#define COMPARE_FUNC(name) \
    static int CompareBy##name(const void* data1, const void* data2) \
    { \
        const FlashCard* const card1 = *static_cast<const FlashCard* const *>(data1); \
        const FlashCard* const card2 = *static_cast<const FlashCard* const *>(data2); \
        if (card1->Get##name() > card2->Get##name()) \
        { \
            return 1; \
        } \
        if (card1->Get##name() < card2->Get##name()) \
        { \
            return -1; \
        } \
        return 0;\
    }

struct FlashCardOptions
{
    FlashCardOptions(
        time_t              timeReviewMin,
        time_t              timeReviewMax,
        int                 timeReviewEntropy,
        bool                autoSave,
        const std::wstring& fontNameNormal,
        const std::wstring  fontNameFixed,
        const int           fontSizes[]
    ) :
            timeReviewMin(timeReviewMin),
            timeReviewMax(timeReviewMax),
            timeReviewEntropy(timeReviewEntropy),
            autoSave(autoSave),
            fontNameNormal(fontNameNormal),
            fontNameFixed(fontNameFixed)
    {
        memcpy(this->fontSizes, fontSizes, sizeof(this->fontSizes));
    }

    FlashCardOptions() :
            timeReviewMin(DAYS_TO_SECONDS(1)),
            timeReviewMax(DAYS_TO_SECONDS(365)),
            timeReviewEntropy(25),
            autoSave(false)
    {
        fontSizes[0] = 9;
        fontSizes[1] = 12;
        fontSizes[2] = 14;
        fontSizes[3] = 18;
        fontSizes[4] = 24;
        fontSizes[5] = 30;
        fontSizes[6] = 36;
    }

    static const FlashCardOptions   DEFAULT;

    time_t                          timeReviewMin;
    time_t                          timeReviewMax;
    int                             timeReviewEntropy;
    bool                            autoSave;
    std::wstring                    fontNameNormal;
    std::wstring                    fontNameFixed;
    int                             fontSizes[7];
};

class FlashCardManager
{
public:
    FlashCardManager();

    bool                    Open(const std::wstring& filename);
    bool                    Save(const std::wstring& filename) const;
    void                    Close();
    void                    FlagModified();
    bool                    IsModified() const;

    const FlashCardOptions& GetOptions() const;
    void                    SetOptions(const FlashCardOptions& options);

    FlashCard*              AddCard(const std::wstring& question, const std::wstring& answer, bool enabled);
    void                    EnumerateCards(std::vector<FlashCard*>* cards, unsigned decks, bool allowEnabled, bool allowDisabled, DeckSortType sort);
    int                     GetDeckSize(DeckType deck, bool allowDisabled) const;
    bool                    RemoveCard(FlashCard* card);
    int                     ExpireCards();

private:
    typedef                 std::list<FlashCard> CardList;

    void                    XmlExportRoot(TiXmlDocument* parent) const;
    void                    XmlExportOptions(TiXmlElement* parent) const;
    void                    XmlExportDecks(TiXmlElement* parent) const;
    void                    XmlExportDeck(TiXmlElement* parent, DeckType deck) const;
    void                    XmlExportCard(TiXmlElement* parent, const FlashCard& card) const;
    void                    XmlImportRoot(const TiXmlDocument* parent);
    void                    XmlImportOptions(const TiXmlElement* parent);
    void                    XmlImportDecks(const TiXmlElement* parent);
    void                    XmlImportDeck(const TiXmlElement* parent, DeckType deck);
    void                    XmlImportCard(const TiXmlElement* parent, DeckType deck);

    COMPARE_FUNC(Question);
    COMPARE_FUNC(Answer);
    COMPARE_FUNC(Enabled);
    COMPARE_FUNC(Deck);
    COMPARE_FUNC(CountRemembered);
    COMPARE_FUNC(CountForgotten);
    COMPARE_FUNC(CountBungled);
    COMPARE_FUNC(TimeReviewPrevious);
    COMPARE_FUNC(TimeReviewNext);
    COMPARE_FUNC(TimeAdded);

    CardList                m_cards;
    FlashCardOptions        m_options;
    mutable bool            m_modified;
};

#undef COMPARE_FUNC
