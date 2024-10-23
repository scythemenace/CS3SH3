## Documentation for the solution

### Rough (initial) idea

- Can use a lock with waiting as taught in the lectures
- Cap the queue to 3 as specified in the prompt
- Put any student thread to sleep which is not able to sit in the queue or any TA thread to sleep when he is not available

### Certain resources

- **Sleeping barber problem**
  - Very similar to our prompt
  - Need three semaphores apparently
    - One for the customers present in the waiting room
    - One for the barber (0 or 1) to indicate whether he is idle or working
    - One to provide mutual exclusion (this can be replaced with a mutex)
