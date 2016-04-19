#ifndef __CORE_THREAD_HPP__
#define __CORE_THREAD_HPP__

#include "core/core.hpp"

#ifdef CORE_CXX11_LIBRARY_THREAD_SUPPORT
    #include <thread>
    #include <mutex>
    #include <condition_variable>
    #include <list>
    #include <algorithm>
    namespace sg
    {
        using std::thread;
        using std::mutex;
        using std::unique_lock;
        using std::condition_variable;
        namespace this_thread = std::this_thread;
        #ifdef CORE_CXX11_LIBRARY_THREAD_SHARED_MUTEX_SUPPORT
            using std::shared_mutex;
            using std::shared_lock;
        #else
            typedef std::mutex  shared_mutex;
            #define shared_lock std::unique_lock
        #endif

        class thread_group
        {
        private:
            thread_group(thread_group const&);
            thread_group& operator=(thread_group const&);

        public:
            thread_group() {}
           ~thread_group()
            {
                for (std::list<thread*>::iterator it = threads.begin(), end = threads.end();
                    it != end;
                    ++it)
                {
                    delete *it;
                }
            }

            bool is_this_thread_in()
            {
                thread::id id = this_thread::get_id();
                shared_lock<shared_mutex> lock(m);
                for (std::list<thread*>::iterator it = threads.begin(), end = threads.end();
                     it != end;
                     ++it)
                {
                    if ((*it)->get_id() == id)
                        return true;
                }
                return false;
            }

            bool is_thread_in(thread* thrd)
            {
                if (thrd)
                {
                    thread::id id = thrd->get_id();
                    shared_lock<shared_mutex> lock(m);
                    for (std::list<thread*>::iterator it = threads.begin(), end = threads.end();
                         it != end;
                         ++it)
                    {
                        if ((*it)->get_id() == id)
                            return true;
                    }
                    return false;
                }
                else
                {
                    return false;
                }
            }

            template<typename F>
            thread* create_thread(F threadfunc)
            {
                unique_lock<shared_mutex> lock(m);
                std::auto_ptr<thread> new_thread(new thread(threadfunc));
                threads.push_back(new_thread.get());
                return new_thread.release();
            }

            void add_thread(thread *thrd)
            {
                if (thrd)
                {
                    BOOST_ASSERT(! is_thread_in(thrd));

                    unique_lock<shared_mutex> lock(m);
                    threads.push_back(thrd);
                }
            }

            void remove_thread(thread* thrd)
            {
                unique_lock<shared_mutex> lock(m);
                std::list<thread*>::iterator const it = std::find(threads.begin(), threads.end(), thrd);
                if (it != threads.end())
                {
                    threads.erase(it);
                }
            }

            void join_all()
            {
                BOOST_ASSERT(! is_this_thread_in());

                shared_lock<shared_mutex> lock(m);

                for (std::list<thread*>::iterator it = threads.begin(), end = threads.end();
                     it != end;
                     ++it)
                {
                    if ((*it)->joinable())
                    {
                        (*it)->join();
                    }
                }
            }

            size_t size() const
            {
                shared_lock<shared_mutex> lock(m);
                return threads.size();
            }

        private:
            std::list<thread*> threads;
            mutable shared_mutex m;
        };
    }
#else
    #include <boost/thread.hpp>
    namespace sg
    {
        using boost::thread;
        using boost::mutex;
        using boost::unique_lock;
        using boost::condition_variable;
        namespace this_thread = boost::this_thread;
        using boost::shared_mutex;
        using boost::shared_lock;
        using boost::unique_lock;
        using boost::thread_group;
    }
#endif




#endif



