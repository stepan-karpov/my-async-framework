# Queues

## Unbounded vs Bounded 

## Lvl. 1 Unbounded Sync Single Producer Multiple Consumers Queue
## Lvl. 2 Bounded Async Single Producer Multiple Consumers Queue

- Bounded is overkill here, in a proper realization bounding should be implemented in an entry microservice point (TODO: rate-limiter)
- Can be used as MPMC queue