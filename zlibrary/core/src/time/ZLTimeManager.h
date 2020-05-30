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

#ifndef __ZLTIMEMANAGER_H__
#define __ZLTIMEMANAGER_H__

#include <map>

#include <shared_ptr.h>
#include <ZLRunnable.h>

class ZLTime;

class ZLTimeManager {

private:
	class AutoRemovableTask;

protected:
	static ZLTimeManager *ourInstance;

public:
	static ZLTimeManager &Instance();
	static void deleteInstance();

protected:
	ZLTimeManager();
	virtual ~ZLTimeManager();

public:
	virtual void addTask(shared_ptr<ZLRunnable> task, int interval) = 0;
	void addAutoRemovableTask(shared_ptr<ZLRunnable> task, int delay = 0);
	void removeTask(shared_ptr<ZLRunnable> task);

protected:
	virtual void removeTaskInternal(shared_ptr<ZLRunnable> task) = 0;

	virtual ZLTime currentTime() const = 0;
	virtual short hoursBySeconds(long seconds) const = 0;
	virtual short minutesBySeconds(long seconds) const = 0;

	virtual short yearBySeconds(long seconds) const = 0;
	virtual short monthBySeconds(long seconds) const = 0;
	virtual short dayOfMonthBySeconds(long seconds) const = 0;

protected:
	typedef void (*DeleteFunc)(void*);
	template <typename T> struct DeleteHelper {
		static void func(T *object) { delete object; }
	};

	class DeleteTask : public ZLRunnable {

	public:
		DeleteTask(void *object, DeleteFunc func) : myObject(object), myFunc(func) {}
	private:
		void run() { myFunc(myObject); }

		void *myObject;
		DeleteFunc myFunc;
	};

public:
	template <typename T> static void deleteLater(shared_ptr<T> object, int delay = 0) {
		deleteLater(new shared_ptr<T>(object), delay);
	}
	template <typename T> static void deleteLater(T *object, int delay = 0) {
		DeleteFunc func = reinterpret_cast<DeleteFunc>(&DeleteHelper<T>::func);
		ZLTimeManager::Instance().addAutoRemovableTask(new DeleteTask(object, func), delay);
	}

private:
	std::map<shared_ptr<ZLRunnable>,shared_ptr<ZLRunnable> > myAutoRemovableTasks;

friend class ZLTime;
};

#endif /* __ZLTIMEMANAGER_H__ */
