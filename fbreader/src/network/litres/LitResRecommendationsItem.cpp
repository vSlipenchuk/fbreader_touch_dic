/*
 * Copyright (C) 2004-2012 Geometer Plus <contact@geometerplus.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301, USA.
 */

#include <ZLStringUtil.h>
#include <ZLNetworkUtil.h>

#include "../NetworkLink.h"
#include "../authentication/litres/LitResAuthenticationManager.h"

#include "LitResRecommendationsItem.h"

LitResRecommendationsItem::LitResRecommendationsItem(
	const OPDSLink &link,
	const std::string &title,
	const std::string &summary,
	const UrlInfoCollection &urlByType,
	AccessibilityType accessibility
) : OPDSCatalogItem(
	link,
	title,
	summary,
	urlByType,
	accessibility
) { }

std::string LitResRecommendationsItem::getCatalogUrl() {
	LitResAuthenticationManager &mgr = (LitResAuthenticationManager&)*Link.authenticationManager();
	std::string catalogUrl = OPDSCatalogItem::getCatalogUrl();
	if (mgr.isAuthorised().Status == B3_FALSE) {
		return catalogUrl;
	}
	std::string query = ZLStringUtil::join(mgr.getPurchasedIds(), ",");
	ZLNetworkUtil::appendParameter(catalogUrl, "ids", query);
	return catalogUrl;
}
