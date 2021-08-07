/**************************************************************************************************
 * Copyright (c) 2021 Calypso Networks Association https://calypsonet.org/                        *
 *                                                                                                *
 * See the NOTICE file(s) distributed with this work for additional information regarding         *
 * copyright ownership.                                                                           *
 *                                                                                                *
 * This program and the accompanying materials are made available under the terms of the Eclipse  *
 * Public License 2.0 which is available at http://www.eclipse.org/legal/epl-2.0                  *
 *                                                                                                *
 * SPDX-License-Identifier: EPL-2.0                                                               *
 **************************************************************************************************/

#pragma once

#include <chrono>
#include <thread>

namespace keyple {
namespace core {
namespace service {
namespace cpp {

class Thread {
private:
    /**
     *
     */
    class UncaughtExceptionHandler {

    };

public:
    /**
     * Constructor
     *
     * Allocates a new Thread object. This constructor has the same effect as Thread (null, null,
     * gname), where gname is a newly generated name. Automatically generated names are of the form
     * "Thread-"+n, where n is an integer.
     */
    Thread()
    : mAlive(false), mName("Thread-x"), mInterrupted(false), mThread(nullptr),
        mDetached(true) {}

    /**
     * Constructor
     *
     * Allocates a new Thread object. This constructor has the same effect as
     * Thread (null, null, name).
     *
     * @param name the name of the new thread
     */
    Thread(const std::string& name)
    : mAlive(false), mName(name), mInterrupted(false), mThread(nullptr),
        mDetached(true) {}

    /**
     * Destructor
     */
    virtual ~Thread()
    {
        if (mAlive == 1 && mDetached == 0) {
            mThread->detach();
        }
    }

    /**
     *
     */
    void setName(const std::string& name)
    {
        mName = name;
    }

    /**
     * Causes this thread to begin execution.
     *
     * The result is that two threads are running concurrently: the current thread (which returns
     * from the call to the start method) and the other thread (which executes its run method).
     *
     * It is never legal to start a thread more than once. In particular, a thread may not be
     * restarted once it has completed execution.
     *
     * @throw IllegalThreadStateException if the thread was already started.
     */
    void start()
    {
        int result;

        mInterrupted = false;

        mThread = new std::thread(runThread, this);
        result = mThread ? 0 : -1;

        if (result == 0)
            mAlive = 1;
    }

    /**
     * In the call to pthread_create() the last argument is a void pointer to a data structure which
     * will be passed to the runThread() function when it is called. Since the input argument to the
     * runThread() is the Thread class this pointer, we can cast it to a Thread pointer then use it
     * to call the Thread::run() method. Due to polymorphism, the Thread subclass run() method will
     * be called to carry out the thread’s action.
     */
    static void runThread(void* arg)
    {
        ((Thread*)arg)->run();
    }

    /**
     * By default Pthreads are joinable. meaning you can wait for them to complete with a call to
     * pthread_join(). The Thread class join method checks to see if the thread is running, then
     * calls this function to wait for the thread to complete. If the call is successful the thread
     * is marked as detached since pthread_join() automatically detatches a thread.
     */
    int join()
    {
        int result = -1;

        if (mAlive == 1) {
            mThread->join();
            mDetached = 1;
        }

        return result;
    }

    /**
     * This is a utility method that detaches a thread when the caller doesn’t want to wait for the
     * thread to complete. If the thread is running and not detached, pthread_detach() is called and
     * the thread is flagged as detached if the call is successful.
     */
    int detach()
    {
        int result = -1;

        if (mAlive == 1 && mDetached == 0) {
            mThread->detach();
            mDetached = 1;
        }

        return result;
    }

    /**
     *
     */
    bool isAlive() const
    {
        return mAlive;
    }

    /**
     * This is another utility method that returns the thread ID for display or logging purposes.
     */
    std::thread::id selfId()
    {
        return mThread->get_id();
    }

    /**
     *
     */
    std::thread* self()
    {
        return mThread;
    }

    /**
     *
     */
    void setDaemon(bool on)
    {
        (void)on;
    }

    /**
     * Causes the currently executing thread to sleep (temporarily cease execution) for the
     * specified number of milliseconds, subject to the precision and accuracy of system timers and
     * schedulers. The thread does not lose ownership of any monitors.
     *
     * @param millis the length of time to sleep in milliseconds
     *
     * @throws IllegalArgumentException if the value of millis is negative
     * @throws InterruptedException if any thread has interrupted the current thread. The
     *         interrupted status of the current thread is cleared when this exception is thrown.
     *         static void sleep(long millis) throw(InterruptedException)
     */
    static void sleep(long millis)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(millis));
    }

    /**
     * Interrupts this thread.
     *
     * Unless the current thread is interrupting itself, which is always permitted, the checkAccess
     * method of this thread is invoked, which may cause a SecurityException to be thrown.
     *
     * If this thread is blocked in an invocation of the wait(), wait(long), or wait(long, int)
     * methods of the Object class, or of the join(), join(long), join(long, int), sleep(long), or
     * sleep(long, int), methods of this class, then its interrupt status will be cleared and it
     * will receive an InterruptedException.
     *
     * If this thread is blocked in an I/O operation upon an interruptible channel then the channel
     * will be closed, the thread's interrupt status will be set, and the thread will receive a
     * ClosedByInterruptException.
     *
     * If this thread is blocked in a Selector then the thread's interrupt status will be set and it
     * will return immediately from the selection operation, possibly with a non-zero value, just as
     * if the selector's wakeup method were invoked.
     *
     * If none of the previous conditions hold then this thread's interrupt status will be set.
     *
     * Interrupting a thread that is not alive need not have any effect.
     *
     * @throws SecurityException if the current thread cannot modify this thread
     */
    void interrupt()
    {
        mInterrupted = true;
    }

    /**
     *
     */
    bool isInterrupted() const
    {
        return mInterrupted;
    }

    /**
     *
     */
    virtual void* run() = 0;

    /**
     *
     */
    static std::thread::id currentThreadId()
    {
        return std::this_thread::get_id();
    }

    /**
     *
     */
    std::string getName()
    {
        return mName;
    }

    /**
     *
     */
    void setThread(std::thread* t)
    {
        mThread = t;
    }

    /**
     *
     */
    void setUncaughtExceptionHandler(std::shared_ptr<UncaughtExceptionHandler> eh)
    {
        mUncaughtExceptionHandler = eh;
    }

private:
    /**
     *
     */
    int mAlive;

    /**
     *
     */
    std::string mName;

    /**
     *
     */
    bool mInterrupted;

    /**
     *
     */
    std::thread* mThread;

    /**
     *
     */
    int mDetached;

    /**
     *
     */
    std::shared_ptr<UncaughtExceptionHandler> mUncaughtExceptionHandler;
};

}
}
}
}
