# P1---Threaded-Prime-Number-Search

A C++ project demonstrating multi-threading by computing prime numbers using different concurrent programming strategies.

## Project Structure

The repository contains four distinct implementations, organized into their own folders:

* `variant1/`
* `variant2/`
* `variant3/`
* `variant4/`

Inside each `variant` folder, you will find:
* `main.cpp`: The source code for that specific threading variant.
* `config.txt`: A configuration file to dynamically set the execution parameters.
* Pre-compiled `.exe` files for immediate testing.

## How to Use

1. Navigate to the folder of the variant you want to test.
2. Open the `config.txt` file in that folder and adjust the parameters. For example:
```text
   threads=4
   max_number=256
