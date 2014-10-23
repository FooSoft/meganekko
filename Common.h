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

#define ASSERT                      assert
#define IS_TRUE(x)                  ((x) ? true : false)
#define ARRAY_SIZE(x)               (sizeof(x) / sizeof(x[0]))
#define BIT(x)                      (1 << (x))
#define SECONDS_TO_DAYS(seconds)    (seconds / 86400)
#define DAYS_TO_SECONDS(days)       (days * 86400)

enum DeckType
{
    DECK_TYPE_EXPIRED,
    DECK_TYPE_FAILED,
    DECK_TYPE_UNTESTED,
    DECK_TYPE_PENDING,
    DECK_TYPES
};

enum DeckSortType
{
    DECK_SORT_TYPE_TIME_ADDED,
    DECK_SORT_TYPE_TIME_REVIEW_PREVIOUS,
    DECK_SORT_TYPE_TIME_REVIEW_NEXT,
    DECK_SORT_TYPE_DECK,
    DECK_SORT_TYPE_ENABLED,
    DECK_SORT_TYPE_QUESTION,
    DECK_SORT_TYPE_ANSWER,
    DECK_SORT_TYPE_COUNT_REMEMBERED,
    DECK_SORT_TYPE_COUNT_FORGOTTEN,
    DECK_SORT_TYPE_COUNT_BUNGLED,
    DECK_SORT_TYPE_SHUFFLE
};

enum GradeType
{
    GRADE_TYPE_REMEMBER,
    GRADE_TYPE_BUNGLE,
    GRADE_TYPE_FORGET,
    GRADE_TYPE_LEARN,
    GRADE_TYPE_UNLEARN,
};

std::string     DeckTypeToString(DeckType type);
DeckType        StringToDeckType(const std::string& string);
std::wstring    TimeToStringRel(time_t timeValue, time_t timeNow = 0);
std::wstring    TimeToString(time_t time);
