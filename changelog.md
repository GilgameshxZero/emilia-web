# Changelog

## 7.1.4

* Revamp `makefile` to pass correct linker and compiler flags.
* Setup Visual Studio Code debugging workflows.

## 7.1.3

* Fix a bug where 404 `Content-Length` was not being sent correctly.

## 7.1.2

* Fix `make noinc`.
* Don't sort `using` in `.clang-format`.
* Use `rain v6.4.0`.

## 7.1.1

* Update `noinc` make command.

## 7.1.0

* Fix the RAII `Socket` implementation and update inheritance model to match.
* Fix `MacOS` builds.

## 7.0.14

* Update to RAII implementation of `Rain::Socket`.
* Add additional `make` options and switch to WSL from Cygwin.

## 7.0.13

* Shorten default timeouts.

## 7.0.12

* Update to use the new templates for `ServerSlave` with custom data types.

## 7.0.11

* Fix a bug where the file cache was wiped on every new request. This should improve performance for smaller files and minimize disk reads.

## 7.0.10

* Use `-lstdc++fs` flag for `g++` to support `g++-8`.
* Add `data` directory to version control.

## 7.0.9

Using `rain 6.0.20`, implement similar server functionality to `emilia-web 6.x.x`.

## 7.0.8

Modify makefile to increment build number correctly. Remove dependency temporarily on higher-order functionality of `rain` while `rain` is undergoing refactoring.

## 7.0.7

Change makefile to recompile when `rain` headers are modified.

## 7.0.6

Asynchronous server now parses incoming requests and prints them.

## 7.0.5

Create an asynchronous server with `ThreadPool` and "handle" incoming socket connections.

## 7.0.4

The simplest web server which returns a 200 saying "i love you" regardless of the request.

## 7.0.3

Switch to header-only `rain`. Use `HttpSocket` to setup a server and client. Piggyback off of shared code for property files in Visual Studio project.

## 7.0.2

Implement basic example for socket client.

## 7.0.1

Implemented basic command line parsing via `Rain::CommandLineParser`.

## 7.0.0

Setup cross-platform build and separate SMTP server from HTTP server of `emilia`.
