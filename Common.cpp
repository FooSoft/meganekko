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
#include "Common.h"

std::string DeckTypeToString(DeckType type)
{
    switch (type)
    {
    case DECK_TYPE_EXPIRED:
        return "expired";
    case DECK_TYPE_FAILED:
        return "failed";
    case DECK_TYPE_PENDING:
        return "pending";
    default:
        return "untested";
    }
}

DeckType StringToDeckType(const std::string& string)
{
    if (string == "expired")
    {
        return DECK_TYPE_EXPIRED;
    }
    else if (string == "failed")
    {
        return DECK_TYPE_FAILED;
    }
    else if (string == "pending")
    {
        return DECK_TYPE_PENDING;
    }
    return DECK_TYPE_UNTESTED;
}

std::wstring TimeToString(time_t time)
{
    char* const string = ctime(&time);

    for (char* iter = string; *iter != 0; ++iter)
    {
        switch (*iter)
        {
            case 0x0d:
            case 0x0a:
                *iter = 0;
        }
    }

    return utf8toWStr(string);
}

std::wstring TimeToStringRel(time_t timeValue, time_t timeNow)
{
    if (timeNow == 0)
    {
        timeNow = time(NULL);
    }

    static const char* s_suffixes[] = { "ago", "from now" };

    const time_t timeDelta = std::max(timeValue, timeNow) - std::min(timeValue, timeNow);
    const char* suffix = s_suffixes[timeValue > timeNow];
    char buffer[256] = {0};

    const int days = timeDelta / 86400;
    const int hours = (timeDelta % 86400) / 3600;
    const int minutes = (timeDelta % 3600) / 60;
    const int seconds = timeDelta % 60;

    if (days > 0)
    {
        sprintf(buffer, "%d day(s) %s", days, suffix);
    }
    else if (hours > 0)
    {
        sprintf(buffer, "%d hour(s) %s", hours, suffix);
    }
    else if (minutes > 0)
    {
        sprintf(buffer, "%d minute(s) %s", minutes, suffix);
    }
    else
    {
        sprintf(buffer, "%d second(s) %s", seconds, suffix);
    }

    return utf8toWStr(buffer);
}
