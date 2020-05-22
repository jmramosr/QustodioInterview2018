/** @file
 * @brief Composer Pool Class
 *
 * This file contains the Composer Pool class to create a Threading Pool to the different CommonStorageComponent filter and values
 * @author José Manuel Ramos Ruiz
 * @date 23 Dic 2018 - Revisión 1.0
 *
 * @see @ref ComposerPool_legal_note_sec
 *
 * @section ComposerPool_legal_note_sec Legal Note
 * Only for Qustodio hiring purposes
 *
 * @section ComposerPool_intro_sec Introduction
 *
 * This is the model to represents a Browsing Element
 *
 * @section ComposerPool_revision_sec Versions
 * Versión | Fecha      | Autor                        | Comentarios adicionales
 * ------: | :--------: | :--------------------------  | -----------------------
 *     1.0 | 2018-12-23 | José Manuel Ramos Ruiz       | Initial Release
 *
 * @section ComposerPool_install_sec Use
 *
 * @subsection ComposerPool_step1 Requirements
 * Requires C++11 to use it correctly
 */

#ifndef COMPOSERPOOL_HPP
#define COMPOSERPOOL_HPP

#include <vector>
#include <queue>
#include <memory>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <future>
#include <atomic>
#include <functional>
#include <stdexcept>
#include <algorithm>
#include <cassert>

namespace Qustodio
{

/*!
 * @class ComposerPool ComposerPool.hpp "ComposerPool.hpp"
 * @author José Manuel Ramos Ruiz
 * @date 23/12/2018
 * @brief Basic thread pool to handle filters concurrently
 * 
 * @see https://github.com/log4cplus/ThreadPool/blob/master/ThreadPool.h
 */
  class ComposerPool
  {
    public:
    explicit ComposerPool(std::size_t threads
        = (std::max)(2u, std::thread::hardware_concurrency()));
    template<class F, class... Args>
    auto enqueue(F&& f, Args&&... args)
        -> std::future<typename std::result_of<F(Args...)>::type>;
    void wait_until_empty();
    void wait_until_nothing_in_flight();
    void set_queue_size_limit(std::size_t limit);
    void set_pool_size(std::size_t limit);
    ~ComposerPool ();

    private:
    void emplace_back_worker (std::size_t worker_number);

    // need to keep track of threads so we can join them
    std::vector< std::thread > workers;
    // target pool size
    std::size_t pool_size;
    // the task queue
    std::queue< std::function<void()> > tasks;
    // queue length limit
    std::size_t max_queue_size = 100000;
    // stop signal
    bool stop = false;

    // synchronization
    std::mutex queue_mutex;
    std::condition_variable condition_producers;
    std::condition_variable condition_consumers;

    std::mutex in_flight_mutex;
    std::condition_variable in_flight_condition;
    std::atomic<std::size_t> in_flight;

    struct handle_in_flight_decrement
    {
        ComposerPool & tp;

        handle_in_flight_decrement(ComposerPool & tp_)
            : tp(tp_)
        { }

        ~handle_in_flight_decrement()
        {
            std::size_t prev
                = std::atomic_fetch_sub_explicit(&tp.in_flight,
                    std::size_t(1),
                    std::memory_order_acq_rel);
            if (prev == 1)
            {
                std::unique_lock<std::mutex> guard(tp.in_flight_mutex);
                tp.in_flight_condition.notify_all();
            }
        }
    };
};

// the constructor just launches some amount of workers
  inline ComposerPool::ComposerPool ( std::size_t threads )
          : pool_size( threads ), in_flight( 0 )
  {
    for ( std::size_t i = 0; i != threads; ++i )
      emplace_back_worker( i );
  }

// add new work item to the pool
  template < class F, class... Args >
  auto ComposerPool::enqueue ( F &&f, Args &&... args ) -> std::future< typename std::result_of< F( Args... ) >::type >
  {
    using return_type = typename std::result_of< F ( Args... ) >::type;

    auto task = std::make_shared < std::packaged_task < return_type() > > (
            std::bind( std::forward< F >( f ), std::forward< Args >( args )... )
    );

    std::future <return_type> res = task->get_future();

    std::unique_lock <std::mutex> lock( queue_mutex );
    if ( tasks.size() >= max_queue_size )
      // wait for the queue to empty or be stopped
      condition_producers.wait( lock,
                                [ this ]
                                {
                                  return tasks.size() < max_queue_size
                                         || stop;
                                } );

    // don't allow enqueueing after stopping the pool
    if ( stop )
      throw std::runtime_error( "enqueue on stopped ComposerPool" );

    tasks.emplace( [ task ] () { ( * task )(); } );
    std::atomic_fetch_add_explicit( & in_flight,
                                    std::size_t( 1 ),
                                    std::memory_order_relaxed );
    condition_consumers.notify_one();

    return res;
  }


// the destructor joins all threads
  inline ComposerPool::~ComposerPool ()
  {
    std::unique_lock <std::mutex> lock( queue_mutex );
    stop = true;
    condition_consumers.notify_all();
    condition_producers.notify_all();
    pool_size = 0;
    condition_consumers.wait( lock, [ this ] { return this->workers.empty(); } );
    assert( in_flight == 0 );
  }

  inline void ComposerPool::wait_until_empty ()
  {
    std::unique_lock <std::mutex> lock( this->queue_mutex );
    this->condition_producers.wait( lock,
                                    [ this ] { return this->tasks.empty(); } );
  }

  inline void ComposerPool::wait_until_nothing_in_flight ()
  {
    std::unique_lock <std::mutex> lock( this->in_flight_mutex );
    this->in_flight_condition.wait( lock,
                                    [ this ] { return this->in_flight == 0; } );
  }

  inline void ComposerPool::set_queue_size_limit ( std::size_t limit )
  {
    std::unique_lock <std::mutex> lock( this->queue_mutex );

    if ( stop )
      return;

    std::size_t const old_limit = max_queue_size;
    max_queue_size = ( std::max )( limit, std::size_t( 1 ) );
    if ( old_limit < max_queue_size )
      condition_producers.notify_all();
  }

  inline void ComposerPool::set_pool_size ( std::size_t limit )
  {
    if ( limit < 1 )
      limit = 1;

    std::unique_lock <std::mutex> lock( this->queue_mutex );

    if ( stop )
      return;

    pool_size = limit;
    std::size_t const old_size = this->workers.size();
    if ( pool_size > old_size )
    {
      // create new worker threads
      for ( std::size_t i = old_size; i != pool_size; ++i )
        emplace_back_worker( i );
    } else if ( pool_size < old_size )
      // notify all worker threads to start downsizing
      this->condition_consumers.notify_all();
  }

  inline void ComposerPool::emplace_back_worker ( std::size_t worker_number )
  {
    workers.emplace_back(
            [ this, worker_number ]
            {
              for ( ;; )
              {
                std::function< void () > task;
                bool notify;

                {
                  std::unique_lock <std::mutex> lock( this->queue_mutex );
                  this->condition_consumers.wait( lock,
                                                  [ this, worker_number ]
                                                  {
                                                    return this->stop || !this->tasks.empty()
                                                           || pool_size < worker_number + 1;
                                                  } );

                  // deal with downsizing of thread pool or shutdown
                  if ( ( this->stop && this->tasks.empty() )
                       || ( !this->stop && pool_size < worker_number + 1 ) )
                  {
                    std::thread &last_thread = this->workers.back();
                    std::thread::id this_id = std::this_thread::get_id();
                    if ( this_id == last_thread.get_id() )
                    {
                      // highest number thread exits, resizes the workers
                      // vector, and notifies others
                      last_thread.detach();
                      this->workers.pop_back();
                      this->condition_consumers.notify_all();
                      return;
                    } else
                      continue;
                  } else if ( !this->tasks.empty() )
                  {
                    task = std::move( this->tasks.front() );
                    this->tasks.pop();
                    notify = this->tasks.size() + 1 == max_queue_size
                             || this->tasks.empty();
                  } else
                    continue;
                }

                handle_in_flight_decrement guard( * this );

                if ( notify )
                {
                  std::unique_lock <std::mutex> lock( this->queue_mutex );
                  condition_producers.notify_all();
                }

                task();
              }
            }
    );
  }
} // namespace Qustodio

#endif // COMPOSERPOOL_HPP
