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

#ifndef __ZLTEXTPARAGRAPH_H__
#define __ZLTEXTPARAGRAPH_H__

#include <map>
#include <vector>
#include <string>

#include <shared_ptr.h>

#include <ZLHyperlinkType.h>
#include <ZLTextKind.h>
#include <ZLTextAlignmentType.h>

class ZLImage;
typedef std::map<std::string,shared_ptr<const ZLImage> > ZLImageMap;

class ZLTextParagraphEntry {

public:
	enum Kind {
		TEXT_ENTRY = 1,
		IMAGE_ENTRY = 2,
		CONTROL_ENTRY = 3,
		HYPERLINK_CONTROL_ENTRY = 4,
		STYLE_CSS_ENTRY = 5,
		STYLE_OTHER_ENTRY = 6,
		STYLE_CLOSE_ENTRY = 7, //TODO implement handling of style close entry
		FIXED_HSPACE_ENTRY = 8,
		RESET_BIDI_ENTRY = 9,
	};

protected:
	ZLTextParagraphEntry();

public:
	virtual ~ZLTextParagraphEntry();

private: // disable copying
	ZLTextParagraphEntry(const ZLTextParagraphEntry &entry);
	const ZLTextParagraphEntry &operator = (const ZLTextParagraphEntry &entry);
};

class ZLTextControlEntry : public ZLTextParagraphEntry {

protected:
	ZLTextControlEntry(ZLTextKind kind, bool isStart);

public:
	virtual ~ZLTextControlEntry();
	ZLTextKind kind() const;
	bool isStart() const;
	virtual bool isHyperlink() const;

private:
	ZLTextKind myKind;
	bool myStart;

friend class ZLTextControlEntryPool;
};

class ZLTextFixedHSpaceEntry : public ZLTextParagraphEntry {

public:
	ZLTextFixedHSpaceEntry(unsigned char length);
	unsigned char length() const;

private:
	const unsigned char myLength;
};

class ZLTextControlEntryPool {

public:
	static ZLTextControlEntryPool Pool;

public:
	ZLTextControlEntryPool();
	~ZLTextControlEntryPool();
	shared_ptr<ZLTextParagraphEntry> controlEntry(ZLTextKind kind, bool isStart);

private:
	std::map<ZLTextKind, shared_ptr<ZLTextParagraphEntry> > myStartEntries;
	std::map<ZLTextKind, shared_ptr<ZLTextParagraphEntry> > myEndEntries;
};

class ZLTextHyperlinkControlEntry : public ZLTextControlEntry {

public:
	ZLTextHyperlinkControlEntry(const char *address);
	~ZLTextHyperlinkControlEntry();
	const std::string &label() const;
	ZLHyperlinkType hyperlinkType() const;
	bool isHyperlink() const;

private:
	std::string myLabel;
	ZLHyperlinkType myHyperlinkType;
};

class ZLTextEntry : public ZLTextParagraphEntry {

public:
	ZLTextEntry(const char *address);
	~ZLTextEntry();

	std::size_t dataLength() const;
	const char *data() const;

private:
	const char *myAddress;
};

class ImageEntry : public ZLTextParagraphEntry {

public:
	ImageEntry(const std::string &id, const ZLImageMap *imageMap, short vOffset);
	~ImageEntry();
	const std::string &id() const;
	shared_ptr<const ZLImage> image() const;
	short vOffset() const;

private:
	const std::string myId;
	const ZLImageMap *myMap;
	const short myVOffset;
};

class ResetBidiEntry : public ZLTextParagraphEntry {

public:
	static const shared_ptr<ZLTextParagraphEntry> Instance;

private:
	ResetBidiEntry();
};

class ZLTextParagraph {

public:
	class Iterator {

	public:
		Iterator(const ZLTextParagraph &paragraph);
		~Iterator();

		bool isEnd() const;
		void next();
		const shared_ptr<ZLTextParagraphEntry> entry() const;
		ZLTextParagraphEntry::Kind entryKind() const;

	private:
		char *myPointer;
		std::size_t myIndex;
		std::size_t myEndIndex;
		mutable shared_ptr<ZLTextParagraphEntry> myEntry;
	};

	enum Kind {
		TEXT_PARAGRAPH,
		TREE_PARAGRAPH,
		EMPTY_LINE_PARAGRAPH,
		BEFORE_SKIP_PARAGRAPH,
		AFTER_SKIP_PARAGRAPH,
		END_OF_SECTION_PARAGRAPH,
		END_OF_TEXT_PARAGRAPH,
	};

protected:
	ZLTextParagraph();

public:
	virtual ~ZLTextParagraph();
	virtual Kind kind() const;

	std::size_t entryNumber() const;

	std::size_t textDataLength() const;
	std::size_t characterNumber() const;

private:
	void addEntry(char *address);

private:
	char *myFirstEntryAddress;
	std::size_t myEntryNumber;

friend class Iterator;
friend class ZLTextModel;
friend class ZLTextPlainModel;
};

class ZLTextSpecialParagraph : public ZLTextParagraph {

private:
	ZLTextSpecialParagraph(Kind kind);

public:
	~ZLTextSpecialParagraph();
	Kind kind() const;

private:
	Kind myKind;

friend class ZLTextPlainModel;
};

class ZLTextTreeParagraph : public ZLTextParagraph {

public:
	ZLTextTreeParagraph(ZLTextTreeParagraph *parent = 0);
	~ZLTextTreeParagraph();
	Kind kind() const;

	bool isOpen() const;
	void open(bool o);
	void openTree();
	int depth() const;
	ZLTextTreeParagraph *parent();
	const ZLTextTreeParagraph *parent() const;
	const std::vector<ZLTextTreeParagraph*> &children() const;
	int fullSize() const;

	void removeFromParent();

private:
	void addChild(ZLTextTreeParagraph *child);

private:
	bool myIsOpen;
	int myDepth;
	ZLTextTreeParagraph *myParent;
	std::vector<ZLTextTreeParagraph*> myChildren;
};

inline ZLTextParagraphEntry::ZLTextParagraphEntry() {}
inline ZLTextParagraphEntry::~ZLTextParagraphEntry() {}

inline ZLTextControlEntry::ZLTextControlEntry(ZLTextKind kind, bool isStart) : myKind(kind), myStart(isStart) {}
inline ZLTextControlEntry::~ZLTextControlEntry() {}
inline ZLTextKind ZLTextControlEntry::kind() const { return myKind; }
inline bool ZLTextControlEntry::isStart() const { return myStart; }
inline bool ZLTextControlEntry::isHyperlink() const { return false; }

inline ZLTextFixedHSpaceEntry::ZLTextFixedHSpaceEntry(unsigned char length) : myLength(length) {}
inline unsigned char ZLTextFixedHSpaceEntry::length() const { return myLength; }

inline ZLTextControlEntryPool::ZLTextControlEntryPool() {}
inline ZLTextControlEntryPool::~ZLTextControlEntryPool() {}

inline ZLTextHyperlinkControlEntry::ZLTextHyperlinkControlEntry(const char *address) : ZLTextControlEntry((ZLTextKind)*address, true), myLabel(address + 2), myHyperlinkType((ZLHyperlinkType)*(address + 1)) {}
inline ZLTextHyperlinkControlEntry::~ZLTextHyperlinkControlEntry() {}
inline const std::string &ZLTextHyperlinkControlEntry::label() const { return myLabel; }
inline ZLHyperlinkType ZLTextHyperlinkControlEntry::hyperlinkType() const { return myHyperlinkType; }
inline bool ZLTextHyperlinkControlEntry::isHyperlink() const { return true; }

inline ZLTextEntry::ZLTextEntry(const char *address) : myAddress(address) {}
inline ZLTextEntry::~ZLTextEntry() {}
inline const char *ZLTextEntry::data() const { return myAddress + sizeof(std::size_t); }

inline ImageEntry::ImageEntry(const std::string &id, const ZLImageMap *imageMap, short vOffset) : myId(id), myMap(imageMap), myVOffset(vOffset) {}
inline ImageEntry::~ImageEntry() {}
inline const std::string &ImageEntry::id() const { return myId; }
inline short ImageEntry::vOffset() const { return myVOffset; }

inline ZLTextParagraph::ZLTextParagraph() : myEntryNumber(0) {}
inline ZLTextParagraph::~ZLTextParagraph() {}
inline ZLTextParagraph::Kind ZLTextParagraph::kind() const { return TEXT_PARAGRAPH; }
inline std::size_t ZLTextParagraph::entryNumber() const { return myEntryNumber; }
inline void ZLTextParagraph::addEntry(char *address) { if (myEntryNumber == 0) myFirstEntryAddress = address; ++myEntryNumber; }

inline ZLTextParagraph::Iterator::Iterator(const ZLTextParagraph &paragraph) : myPointer(paragraph.myFirstEntryAddress), myIndex(0), myEndIndex(paragraph.entryNumber()) {}
inline ZLTextParagraph::Iterator::~Iterator() {}
inline bool ZLTextParagraph::Iterator::isEnd() const { return myIndex == myEndIndex; }
inline ZLTextParagraphEntry::Kind ZLTextParagraph::Iterator::entryKind() const { return (ZLTextParagraphEntry::Kind)*myPointer; }

inline ZLTextSpecialParagraph::ZLTextSpecialParagraph(Kind kind) : myKind(kind) {}
inline ZLTextSpecialParagraph::~ZLTextSpecialParagraph() {}
inline ZLTextParagraph::Kind ZLTextSpecialParagraph::kind() const { return myKind; }

inline ZLTextTreeParagraph::~ZLTextTreeParagraph() {}
inline ZLTextParagraph::Kind ZLTextTreeParagraph::kind() const { return TREE_PARAGRAPH; }
inline bool ZLTextTreeParagraph::isOpen() const { return myIsOpen; }
inline void ZLTextTreeParagraph::open(bool o) { myIsOpen = o; }
inline int ZLTextTreeParagraph::depth() const { return myDepth; }
inline ZLTextTreeParagraph *ZLTextTreeParagraph::parent() { return myParent; }
inline const ZLTextTreeParagraph *ZLTextTreeParagraph::parent() const { return myParent; }
inline const std::vector<ZLTextTreeParagraph*> &ZLTextTreeParagraph::children() const { return myChildren; }
inline void ZLTextTreeParagraph::addChild(ZLTextTreeParagraph *child) { myChildren.push_back(child); }

#endif /* __ZLTEXTPARAGRAPH_H__ */
