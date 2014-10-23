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

void FlashCardManager::XmlExportRoot(TiXmlDocument* parent) const
{
    TiXmlElement* const rootElement = new TiXmlElement("Meganekko");
    XmlExportOptions(rootElement);
    XmlExportDecks(rootElement);
    parent->LinkEndChild(rootElement);
}

void FlashCardManager::XmlExportOptions(TiXmlElement* parent) const
{
    TiXmlElement* const optionsElement = new TiXmlElement("Options");

    optionsElement->SetAttribute("timeReviewMin", m_options.timeReviewMin);
    optionsElement->SetAttribute("timeReviewMax", m_options.timeReviewMax);
    optionsElement->SetAttribute("timeReviewEntropy", m_options.timeReviewEntropy);
    optionsElement->SetAttribute("autoSave", m_options.autoSave);
    optionsElement->SetAttribute("fontNameNormal", wstrToUtf8(m_options.fontNameNormal).c_str());
    optionsElement->SetAttribute("fontNameFixed", wstrToUtf8(m_options.fontNameFixed).c_str());
    for (size_t i = 0; i < ARRAY_SIZE(m_options.fontSizes); ++i)
    {
        char attribute[16] = {0};
        sprintf(attribute, "fontSize%d", static_cast<int>(i));
        optionsElement->SetAttribute(attribute, m_options.fontSizes[i]);
    }

    parent->LinkEndChild(optionsElement);
}

void FlashCardManager::XmlExportDecks(TiXmlElement* parent) const
{
    TiXmlElement* const decksElement = new TiXmlElement("Decks");

    for (size_t i = 0; i < DECK_TYPES; ++i)
    {
        XmlExportDeck(decksElement, static_cast<DeckType>(i));
    }

    parent->LinkEndChild(decksElement);
}

void FlashCardManager::XmlExportDeck(TiXmlElement* parent, DeckType deck) const
{
    TiXmlElement* const deckElement = new TiXmlElement("Deck");
    deckElement->SetAttribute("type", DeckTypeToString(deck).c_str());

    for (CardList::const_iterator iter = m_cards.begin(); iter != m_cards.end(); ++iter)
    {
        if (iter->GetDeck() == deck)
        {
            XmlExportCard(deckElement, *iter);
        }
    }

    parent->LinkEndChild(deckElement);
}

void FlashCardManager::XmlExportCard(TiXmlElement* parent, const FlashCard& card) const
{
    TiXmlElement* const cardElement = new TiXmlElement("Card");

    TiXmlElement* const questionElement = new TiXmlElement("Question");
    TiXmlText* const questionText = new TiXmlText(wstrToUtf8(card.GetQuestion()).c_str());
    questionText->SetCDATA(true);
    questionElement->LinkEndChild(questionText);
    cardElement->LinkEndChild(questionElement);

    TiXmlElement* const answerElement = new TiXmlElement("Answer");
    TiXmlText* const answerText = new TiXmlText(wstrToUtf8(card.GetAnswer()).c_str());
    answerText->SetCDATA(true);
    answerElement->LinkEndChild(answerText);
    cardElement->LinkEndChild(answerElement);

    cardElement->SetAttribute("enabled", card.GetEnabled());
    cardElement->SetAttribute("countRemembered", card.GetCountRemembered());
    cardElement->SetAttribute("countForgotten", card.GetCountForgotten());
    cardElement->SetAttribute("countBungled", card.GetCountBungled());
    cardElement->SetAttribute("timeReviewNext", card.GetTimeReviewNext());
    cardElement->SetAttribute("timeReviewPrevious", card.GetTimeReviewPrevious());
    cardElement->SetAttribute("timeAdded", card.GetTimeAdded());

    parent->LinkEndChild(cardElement);
}

void FlashCardManager::XmlImportRoot(const TiXmlDocument* parent)
{
    const TiXmlElement* const rootElement = parent->RootElement();
    if (rootElement == NULL || strcmp(rootElement->Value(), "Meganekko") != 0)
    {
        return;
    }

    for (const TiXmlElement* baseElement = rootElement->FirstChildElement(); baseElement != NULL; baseElement = baseElement->NextSiblingElement())
    {
        const char* const value = baseElement->Value();
        if (strcmp(value, "Options") == 0)
        {
            XmlImportOptions(baseElement);
        }
        else if (strcmp(value, "Decks") == 0)
        {
            XmlImportDecks(baseElement);
        }
    }
}

void FlashCardManager::XmlImportOptions(const TiXmlElement* parent)
{
    int timeReviewMin = FlashCardOptions::DEFAULT.timeReviewMin;
    parent->Attribute("timeReviewMin", &timeReviewMin);

    int timeReviewMax = FlashCardOptions::DEFAULT.timeReviewMax;
    parent->Attribute("timeReviewMax", &timeReviewMax);

    int timeReviewEntropy = FlashCardOptions::DEFAULT.timeReviewEntropy;;
    parent->Attribute("timeReviewEntropy", &timeReviewEntropy);

    int autoSave = FlashCardOptions::DEFAULT.autoSave;;
    parent->Attribute("autoSave", &autoSave);

    const char* const fontNameNormal = parent->Attribute("fontNameNormal");
    const char* const fontNameFixed = parent->Attribute("fontNameFixed");

    for (size_t i = 0; i < ARRAY_SIZE(m_options.fontSizes); ++i)
    {
        char attribute[16] = {0};
        sprintf(attribute, "fontSize%d", static_cast<int>(i));
        m_options.fontSizes[i] = FlashCardOptions::DEFAULT.fontSizes[i];
        parent->Attribute(attribute, m_options.fontSizes + i);
    }

    m_options.timeReviewMin = timeReviewMin;
    m_options.timeReviewMax = timeReviewMax;
    m_options.timeReviewEntropy = timeReviewEntropy;
    m_options.autoSave = IS_TRUE(autoSave);
    m_options.fontNameNormal = fontNameNormal == NULL ? FlashCardOptions::DEFAULT.fontNameNormal : utf8toWStr(fontNameNormal);
    m_options.fontNameFixed = fontNameFixed == NULL ? FlashCardOptions::DEFAULT.fontNameFixed : utf8toWStr(fontNameFixed);
}

void FlashCardManager::XmlImportDecks(const TiXmlElement* parent)
{
    for (const TiXmlElement* decksElement = parent->FirstChildElement(); decksElement != NULL; decksElement = decksElement->NextSiblingElement())
    {
        const char* const value = decksElement->Value();
        if (strcmp(value, "Deck") == 0)
        {
            const char* const deckString = decksElement->Attribute("type");
            const DeckType deck = deckString == NULL ? DECK_TYPE_UNTESTED : StringToDeckType(deckString);
            XmlImportDeck(decksElement, deck);
        }
    }
}

void FlashCardManager::XmlImportDeck(const TiXmlElement* parent, DeckType deck)
{
    for (const TiXmlElement* deckElement = parent->FirstChildElement(); deckElement != NULL; deckElement = deckElement->NextSiblingElement())
    {
        const char* const value = deckElement->Value();
        if (strcmp(value, "Card") == 0)
        {
            XmlImportCard(deckElement, deck);
        }
    }
}

void FlashCardManager::XmlImportCard(const TiXmlElement* parent, DeckType deck)
{
    std::wstring question;
    std::wstring answer;

    for (const TiXmlElement* cardElement = parent->FirstChildElement(); cardElement != NULL; cardElement = cardElement->NextSiblingElement())
    {
        const char* const value = cardElement->Value();
        if (strcmp(value, "Question") == 0)
        {
            question = utf8toWStr(cardElement->GetText());
        }
        else if (strcmp(value, "Answer") == 0)
        {
            answer = utf8toWStr(cardElement->GetText());
        }
    }

    int enabled = 0;
    parent->Attribute("enabled", &enabled);

    int countRemembered = 0;
    parent->Attribute("countRemembered", &countRemembered);

    int countForgotten = 0;
    parent->Attribute("countForgotten", &countForgotten);

    int countBungled = 0;
    parent->Attribute("countBungled", &countBungled);

    int timeReviewNext = 0;
    parent->Attribute("timeReviewNext", &timeReviewNext);

    int timeReviewPrevious = 0;
    parent->Attribute("timeReviewPrevious", &timeReviewPrevious);

    int timeAdded = 0;
    parent->Attribute("timeAdded", &timeAdded);

    const bool valid =
        timeAdded > 0 &&
        timeAdded <= timeReviewPrevious &&
        timeReviewPrevious <= timeReviewNext;

    if (!valid)
    {
        timeAdded = timeReviewPrevious = timeReviewNext = time(NULL);
    }

    const FlashCard card(
        this,
        deck,
        question,
        answer,
        IS_TRUE(enabled),
        countRemembered,
        countForgotten,
        countBungled,
        timeReviewPrevious,
        timeReviewNext,
        timeAdded
    );

    m_cards.push_front(card);
}
