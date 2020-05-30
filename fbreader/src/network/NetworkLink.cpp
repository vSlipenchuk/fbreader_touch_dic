/*
 * Copyright (C) 2008-2012 Geometer Plus <contact@geometerplus.com>
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

#include <ZLNetworkRequest.h>

#include <ZLibrary.h>
#include <ZLFile.h>
#include <ZLStringUtil.h>

#include "NetworkLink.h"
#include "NetworkOperationData.h"

const std::string NetworkLink::URL_MAIN = "main";
const std::string NetworkLink::URL_SEARCH = "search";
const std::string NetworkLink::URL_SIGN_IN = "signIn";
const std::string NetworkLink::URL_SIGN_OUT = "signOut";
const std::string NetworkLink::URL_SIGN_UP = "signUp";
const std::string NetworkLink::URL_TOPUP = "topup";
const std::string NetworkLink::URL_RECOVER_PASSWORD = "recoverPassword";

NetworkLink::NetworkLink(
	const std::string &siteName
) :
	mySiteName(ZLStringUtil::stringStartsWith(siteName, "www.") ? siteName.substr(std::string("www.").length()) : siteName),
	myEnabled(true),
	myUpdated(0) {
}

NetworkLink::~NetworkLink() {
}

std::string NetworkLink::url(const std::string &urlId) const {
	std::map<std::string,std::string>::const_iterator it = myLinks.find(urlId);
	return (it != myLinks.end()) ? it->second : std::string();
}

shared_ptr<ZLNetworkRequest> NetworkLink::resume(NetworkOperationData &result) const {
	result.clear();
	return 0;
}

void NetworkLink::setTitle(const std::string& title) {
	myTitle = title;
}
void NetworkLink::setSummary(const std::string& summary) {
	mySummary = summary;
}

void NetworkLink::setLanguage(const std::string &language) {
	myLanguage = language;
}

void NetworkLink::setIcon(const std::string& icon) {
	myIcon = icon;
}

void NetworkLink::setPredefinedId(const std::string& id) {
	myPredefinedId = id;
}

void NetworkLink::setLinks(const std::map<std::string,std::string>& links) {
	myLinks = links;
}

void NetworkLink::setUpdated(shared_ptr<ATOMUpdated> u) {
	myUpdated = u;
}

std::string NetworkLink::getSiteName() const {
	return mySiteName;
}

void NetworkLink::setEnabled(bool enabled) {
	myEnabled = enabled;
}

std::string NetworkLink::getTitle() const {
	return myTitle;
}
std::string NetworkLink::getSummary() const {
	return mySummary;
}

std::string NetworkLink::getLanguage() const {
	return myLanguage;
}

std::string NetworkLink::getPredefinedId() const {
	return myPredefinedId;
}

const std::map<std::string,std::string>& NetworkLink::getLinks() const {
	return myLinks;
}

shared_ptr<ATOMUpdated> NetworkLink::getUpdated() const {
	return myUpdated;
}

bool NetworkLink::isPredefined() const {
	return !myPredefinedId.empty();
}

std::string NetworkLink::getIcon() const {
	return myIcon;
}
bool NetworkLink::isEnabled() const {
	return myEnabled;
}

void NetworkLink::loadFrom(const NetworkLink & link) {
	myTitle = link.myTitle;
	myIcon = link.myIcon;
	mySummary = link.mySummary;
	myLanguage = link.myLanguage;
	myLinks = link.myLinks;
	myPredefinedId = link.myPredefinedId;
	myUpdated = link.myUpdated;
}

void NetworkLink::loadLinksFrom(const NetworkLink & link) {
	myIcon = link.myIcon;
	myLinks = link.myLinks;
	myUpdated = link.myUpdated;
}

void NetworkLink::loadSummaryFrom(const NetworkLink & link) {
	myTitle = link.myTitle;
	mySummary = link.mySummary;
}
