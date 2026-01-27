# MyRTOS - Custom Real Time Operating System
   MyRTOS is a simple real time operating system built as a personal hobby project to explore the inner workings of real time operating systems.
   
   This is a relatively simple implementation with three different scheduling algorithms (round-robin, priority-based, and dynamic scheduling)

   This operating system operates with its own custom heap allocator as well as a clearly defined abstraction layer to allow for future implementation accross multiple computer architectures

## Heap
   MyRTOS uses a heap structure that is allocated at compile time (static array in data section of code)

   The allocation algorithm is a simple greedy algorithm that naively searches from top to bottom of heap for a suitable free block of the requested length

   This heap also supports realloc and free operations.  Adjacent free blocks are combined to reduce memory fragmentation on a call to the free function.

   MyRTOS uses a separate heap (same algorithm, different memory) for internal use (locks, etc.) and for users.

## Task Array and Scheduling
   MyRTOS allocates a specific region of memory for storage of task data during compile time.  The maximum number of tasks can be changed in the config file.

   Each task has its own dedicated stack where register values and passed parameters are held.  On a context switch register values are pushed to this stack.

   The scheduling array is an array of queues.  Each array corresponds to a specific priority level.  The number of possible priority levels are also fixed at compile time.  This allows fast access of the highest priority tasks and no sorting overhead for re-scheduling a task.

   Each queue in the structure operates as a FIFO circular array of pointers to the task memory.  While this approach uses more memory overall, it is faster than a normal priority queue.

   The scheudling array also contains a blocked level.  When tasks are blocked, an internal flag is set such that if that task is picked by the scheduler while still being flagged, it will be moved to the blocked list to avoid overhead of sorting through that task.  Any task can be blocked by any other task at any time with minimal overhead simply by setting its blocked flag.

   There are three different scheudling methods that are implemented in MyRTOS.
   
   - Round-Robin
      
      - The most simple scheduler where every task has the same priority.  Every task is given equal attention.
   
   - Priority-Based

      - This scheudler consists of various priority levels where the highest priority task is always chosen.  If multiple tasks have the same priority, the round-robin scheduling algorithm is applied.

   - Dynamic-Priority

      - This scheduler is similar to the Priority-Based scheduler, however task priority can change.  If a task uses a full time slice a set amount of times (defined in the config) the priority of that task will be lowered by one level.  If at any point this task is blocked and unblocked, the priority will return to its original value.  This method allows the scheduler to avoid starving lower priority tasks.

## Inter-Task Communication
   MyRTOS implements mutex, semaphore, and queue structures.

   It is important to note that global interrupts are disabled in MyRTOS whenever modification to lock structures or the scheduling array occur.  This means the scheduler will not be called during a lock take or give.

   The mutex and semaphore structures also implement priority inheritance.  If a low priority task holds a resource that a high priority task needs, the low priority task will temporarily get its priority bumped.  In the case of the semaphore, all holding tasks will get their priority bumped until they release the resource regardless of if the higher priority tasks remains blocked or not.

   In all structures when an attempt at aquistion fails, the attempting task will be blocked indefinitely or until the resource has been freed.

   Tasks that are waiting on a resource are kept track of using a dynamically allocated waiting list.  This allows for fast access of waiting tasks for unblocking.  This waiting list can be initialized to a specific size to avoid the overhead of dynamic re-allocation.  If more size is needed for this list, the list will re-allocate memory equal to double its previous size and remain at that even if no tasks are in the list anymore.

   A similar mechanism is used for keeping track of holding tasks, however, dynamic reallocation is not necessary in this case.

## Testing
   I have written some basic unit tests using the Unity framework.  These tests were built and run on a PSoC6 Microcontroller (Check out the ARM-CORTE-M4 branch), however they were written to be able to be used regardless of the specific architecture.

## Future Improvements
   This project was designed to be able to be easily ported to different computer architectures.  Currently it has only been implemented for the PSoC6 which uses the ARM-CORTEX-M4 architecture.

   Eventually I may use this is a real-world project as well so future improvements may come as a result as bugs are caught.
