#include "Fonts.hpp"

#ifdef FONTCONFIG
#include <fontconfig/fontconfig.h>
#elif defined(_WIN32)
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <sstream>
#else
#error "No way to find fonts on your system, that might be a problem."
#endif

#include <iostream>

namespace
{
	sf::Font* sDefaultFont;

#ifdef FONTCONFIG
	struct
	{
		FcConfig* config;
	} FontConfigData;
#endif

	bool findFont(sf::Font& font, const std::string& family, const std::string& style = "")
	{
		bool loaded = false;

#ifdef FONTCONFIG
		FcPattern* pat = FcPatternCreate();
		FcObjectSet* os = FcObjectSetBuild (FC_FAMILY, FC_STYLE, FC_FILE, (char *) 0);
		FcFontSet* fs = FcFontList(FontConfigData.config, pat, os);

		for (int i=0; fs && i < fs->nfont; ++i)
		{
			FcPattern* fcfont = fs->fonts[i];
			FcChar8 *fcfile, *fcstyle, *fcfamily;
			if (FcPatternGetString(fcfont, FC_FILE, 0, &fcfile) == FcResultMatch &&
				FcPatternGetString(fcfont, FC_FAMILY, 0, &fcfamily) == FcResultMatch &&
				FcPatternGetString(fcfont, FC_STYLE, 0, &fcstyle) == FcResultMatch)
			{
				std::string tmp((char*)fcfile);
				std::reverse(tmp.begin(), tmp.end());
				if (tmp.substr(0,4) != "ftt.")
					continue;

				if ((char*)fcfamily == family && (style.empty() || (char*)fcstyle == style))
					if (font.loadFromFile((char*)fcfile))
					{
						loaded = true;
						break;
					}
			}
		}

		if (fs) FcFontSetDestroy(fs);
		if (os) FcObjectSetDestroy(os);
		if (pat) FcPatternDestroy(pat);
#elif defined(_WIN32)
		static const LPCSTR fontRegistryPath = "Software\\Microsoft\\Windows NT\\CurrentVersion\\Fonts";
		HKEY hKey;
		LONG result;
		std::wstring wsFaceName(family.begin(), family.end());
		wsFaceName.push_back('*');

        // Open Windows font registry key
        result = RegOpenKeyEx(HKEY_LOCAL_MACHINE, fontRegistryPath, 0, KEY_READ, &hKey);
        if (result != ERROR_SUCCESS) {
            return "";
        }

        DWORD maxValueNameSize, maxValueDataSize;
        result = RegQueryInfoKey(hKey, 0, 0, 0, 0, 0, 0, 0, &maxValueNameSize, &maxValueDataSize, 0, 0);
        if (result != ERROR_SUCCESS) {
            return "";
        }

        DWORD valueIndex = 0;
        LPSTR valueName = new CHAR[maxValueNameSize];
        LPBYTE valueData = new BYTE[maxValueDataSize];
        DWORD valueNameSize, valueDataSize, valueType;
        std::string wsFontFile;

        // Look for a matching font name
        do {
            wsFontFile.clear();
            valueDataSize = maxValueDataSize;
            valueNameSize = maxValueNameSize;

            result = RegEnumValue(hKey, valueIndex, valueName, &valueNameSize, 0, &valueType, valueData, &valueDataSize);

            valueIndex++;

            if (result != ERROR_SUCCESS || valueType != REG_SZ) {
                continue;
            }

            std::string wsValueName(valueName, valueNameSize);

            // Found a match
			wsFontFile.assign((LPSTR)valueData, valueDataSize);
			CHAR winDir[MAX_PATH];
			GetWindowsDirectory(winDir, MAX_PATH);

			std::stringstream ss;
			ss << winDir << "\\Fonts\\" << wsFontFile;

			if (font.loadFromFile(ss.str()))
				break;
        } while (result != ERROR_NO_MORE_ITEMS);

        delete[] valueName;
        delete[] valueData;

        RegCloseKey(hKey);
#endif

		return loaded;
	}
}

sf::Font& sf::getDefaultFont()
{
	if (!sDefaultFont)
	{
		static std::pair<std::string,std::string> toTry[] = {
#ifndef _WIN32
			{ "DejaVu Sans", "Book" },
			{ "Bitstream Vera Sans", "Roman" },
			{ "Droid Sans", "Regular" },
#endif
			{ "Arial", "Regular" }
		};

		sDefaultFont = new sf::Font;

		for (auto& font : toTry)
			if (findFont(*sDefaultFont, font.first, font.second))
			{
				std::cout << "[FF] Using font " << font.first << std::endl;
				break;
			}
	}

	return *sDefaultFont;
}

sf::Font sf::getFont(const std::string& family, const std::string& style)
{
	sf::Font toRet;
	findFont(toRet, family, style);
	return toRet;
}

