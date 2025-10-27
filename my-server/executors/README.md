# Executors

Executors are callback functions that process client connections in the thread pool.

## Overview

An executor is a function that receives a socket file descriptor and implements your business logic for handling client connections. This pattern allows you to plug in custom request processing without modifying the core server framework.
