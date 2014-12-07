/***************************************************************************
 *
 * This file is covered by a dual licence. You can choose whether you
 * want to use it according to the terms of the GNU GPL version 2, or
 * under the terms of Zorp Professional Firewall System EULA located
 * on the Zorp installation CD.
 *
 * $Id: thread.c,v 1.20 2004/02/11 13:26:36 sasa Exp $
 *
 * Author  : Bazsi
 * Auditor :
 * Last audited version:
 * Notes:
 *
 ***************************************************************************/

#include <zorp/thread.h>
#include <zorp/log.h>

#include <string.h>
#include <time.h>
#include <stdlib.h>
#if HAVE_SYS_RESOURCE_H
#include <sys/resource.h>
#endif
#include <stdio.h>

gint max_threads = 100;
static gint num_threads = 0;
static gint max_stack_size = 512 * 1024;
static GPrivate *current_thread;

/**
 * Linked list of callback functions with a user data pointer for each.
 **/
typedef struct _ZThreadCallback
{
  struct _ZThreadCallback *next;
  GFunc cb;                             /**< callback function */
  gpointer user_data;
} ZThreadCallback;

static ZThreadCallback *start_callbacks;
static ZThreadCallback *stop_callbacks;

/**
 * Frees a given Zorp thread state.
 *
 * @param[in] self Zorp thread state
 **/
static void
z_thread_free(ZThread *self)
{
  g_free(self);
}

/**
 * Returns the current Zorp specific thread state.
 **/
ZThread *
z_thread_self(void)
{
  return (ZThread *) g_private_get(current_thread);
}

/**
 * Use this function to register a start-callback. Start callbacks are called
 * when a given thread starts.
 *
 * @param[in] func callback to add to the set of start-callbacks
 * @param[in] user_data pointer to pass to this function
 **/
void
z_thread_register_start_callback(GFunc func, gpointer user_data)
{
  ZThreadCallback *cb = g_new0(ZThreadCallback, 1);

  cb->cb = func;
  cb->user_data = user_data;
  cb->next = start_callbacks;
  start_callbacks = cb;
}

/**
 * Use this function to register a stop-callback. Stop callbacks are called
 * when a given thread terminates.
 *
 * @param[in] func callback to add to the set of stop-callbacks
 * @param[in] user_data pointer to pass to this function
 **/
void
z_thread_register_stop_callback(GFunc func, gpointer user_data)
{
  ZThreadCallback *cb = g_new0(ZThreadCallback, 1);

  cb->cb = func;
  cb->user_data = user_data;
  cb->next = stop_callbacks;
  stop_callbacks = cb;
}


/**
 * This function iterates on a linked list of registered callbacks and calls
 * each function.
 *
 * @param[in] self thread state
 * @param[in] p linked list to iterate through
 **/
static void
z_thread_iterate_callbacks(ZThread *self, ZThreadCallback *p)
{
  for (; p; p = p->next)
    p->cb(self, p->user_data);
}

/**
 * This function is called upon thread startup and performs thread specific
 * initialization. It calls thread-start and thread-exit callbacks.
 *
 * @param[in] self thread specific variables
 * @param     user_data pointer to pass to real thread function (unused)
 **/
static void
z_thread_func_core(ZThread *self, gpointer user_data G_GNUC_UNUSED)
{
  g_private_set(current_thread, self);
  self->thread = g_thread_self();

  z_thread_iterate_callbacks(self, start_callbacks);

  /*LOG
    This message indicates that a thread is starting.
  */
  z_log(self->name, CORE_DEBUG, 6, "thread starting;");
  (*self->func)(self->arg);
  /*LOG
    This message indicates that a thread is ending.
  */
  z_log(self->name, CORE_DEBUG, 6, "thread exiting;");

  z_thread_iterate_callbacks(self, stop_callbacks);  
  z_thread_free(self);

}

/* simple PThread based implementation */

static GAsyncQueue *queue;

/**
 * This function wrapped around the real thread function logging two
 * events when the thread starts & stops 
 *
 * @param[in] st thread state
 **/
static gpointer
z_thread_func(gpointer st)
{
  ZThread *self = (ZThread *) st;
  
  do
    {
      z_thread_func_core(self, NULL);
      self = NULL;
      g_async_queue_lock(queue);
      self = (ZThread *) g_async_queue_try_pop_unlocked(queue);
      if (!self)
        {
          num_threads--;
          g_async_queue_unref(queue);
          g_async_queue_unlock(queue);
        }
      else
        g_async_queue_unlock(queue);
    }
  while (self != NULL);

  return NULL;
}

/**
 * Allocate and initialize a Zorp thread identified by a name, and
 * using the given thread function.
 *
 * @param[in] name name to identify this thread with
 * @param[in] func thread function
 * @param[in] arg pointer to pass to thread function
 *
 * @returns TRUE to indicate success
 **/
gboolean
z_thread_new(const gchar *name, GThreadFunc func, gpointer arg)
{
  ZThread *self = g_new0(ZThread, 1);
  GError *error = NULL;
  static gint thread_id = 1;
  
  self->thread_id = thread_id++;
  self->func = func;
  self->arg = arg;
  strncpy(self->name, name, sizeof(self->name) - 1);
  
  g_async_queue_lock(queue);
  if (num_threads >= max_threads)
    {
      /*LOG
        This message reports that the maximal thread limit is reached. Try to increase
	the maximal thread limit.
       */
      z_log(NULL, CORE_ERROR, 3, "Too many running threads, waiting for one to become free; num_threads='%d', max_threads='%d'", num_threads, max_threads);
      g_async_queue_push_unlocked(queue, self);
      g_async_queue_unlock(queue);
    }
  else
    {
      num_threads++;
      g_async_queue_ref(queue);
      g_async_queue_unlock(queue);
      if (!g_thread_create_full(z_thread_func, self, max_stack_size, FALSE, TRUE, G_THREAD_PRIORITY_NORMAL, &error))
        {
	  /*LOG
	    This message indicates that creating a new thread failed. It is likely that
	    the system is running low on some resources or some limit is reached.
	   */
          z_log(NULL, CORE_ERROR, 2, "Error starting new thread; error='%s'", error->message);
          g_async_queue_lock(queue);
          num_threads--;
          g_async_queue_unlock(queue);
          return FALSE;
        }
    }
  
  return TRUE;
}

/**
 * This function should be called before calling z_thread_init() to specify
 * the maximum number of threads.
 *
 * @param[in] max maximum number of threads
 **/
void
z_thread_set_max_threads(gint max)
{
  max_threads = max;
}

/**
 * This function should be called before calling z_thread_init() to specify
 * the maximum size of thread stacks.
 *
 * @param[in] stack_size maximum size for thread stacks
 **/
void
z_thread_set_max_stack_size(gint stack_size)
{
  max_stack_size = stack_size;
}

/**
 * This function should be called after specifying various threading
 * parameters using z_thread_*() functions and _before_ creating any threads
 * using z_thread_new().
 **/
void
z_thread_init(void)
{
#if HAVE_SETRLIMIT
  struct rlimit limit;

  /* NOTE: in addition to specifying the stack size where possible, we set
   * the stack limit, which sets per-thread stack limit for threadpools in
   * which case we are unable to set the stack limit any other way */

  memset(&limit, 0, sizeof(limit));
  limit.rlim_cur = max_stack_size;
  limit.rlim_max = max_stack_size;

  setrlimit(RLIMIT_STACK, &limit);
#endif

  g_thread_init(NULL);
  queue = g_async_queue_new();
  current_thread = g_private_new(NULL);
}

/**
 * Call this function when exiting your program to global thread related
 * data structures.
 **/
void
z_thread_destroy(void)
{
  g_async_queue_unref(queue);
}

/**
 * Set max_stack_size from command line argument.
 *
 * @param      option_name unused
 * @param[in]  value string value for stack size to set
 * @param      data unused
 * @param[out] error
 *
 * @returns TRUE if parsing of the stack-size argument was successful
 **/
static gboolean
z_thread_stack_size_arg(const gchar *option_name G_GNUC_UNUSED, const gchar *value, gpointer data G_GNUC_UNUSED, GError **error)
{
  gchar *end;
  
  max_stack_size = strtol(value, &end, 10) * 1024;
  if (*end != '\0')
    {
      g_set_error(error, G_OPTION_ERROR, G_OPTION_ERROR_BAD_VALUE, "Error parsing stack-size argument");
      return FALSE;
    }

  if (max_stack_size > 8192 * 1024)
    {
      fprintf(stderr, "Stack size limit exceeded, set default value 8MB;\n");
      max_stack_size = 8192 * 1024;
    }
  return TRUE;
}

/**
 * Command line options for ZThread.
 **/
static GOptionEntry z_thread_option_entries[] =
{
  { "threads",           't', 0, G_OPTION_ARG_INT,      &max_threads,            "Set the maximum number of threads", "<thread limit>" },
  { "stack-size",        'S', 0, G_OPTION_ARG_CALLBACK, z_thread_stack_size_arg, "Set the stack size in kBytes", "<stacksize>" },
  { NULL, 0, 0, 0, NULL, NULL, NULL },
};

/**
 * Add the command line options of ZThread to the option context.
 *
 * @param[in] ctx GOptionContext instance
 **/
void
z_thread_add_option_group(GOptionContext *ctx)
{
  GOptionGroup *group;
  
  group = g_option_group_new("thread", "Thread options", "Thread options", NULL, NULL);
  g_option_group_add_entries(group, z_thread_option_entries);
  g_option_context_add_group(ctx, group);
}
