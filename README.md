# Message Listener and Data Sorting

This C++ project reads messages from a specified port and saves them to an SQLite database with a timestamp. Messages with specific prefixes are differentiated based on their type. Additionally, the project can sort and display the integer data at regular intervals. this project has been completed with VS code.

## Overview

The primary purpose of this project is to demonstrate message listening and data handling. It reads messages from a port and saves them to an SQLite database. These messages are categorized based on their prefixes and then sorted in ascending order for display.

## Prerequisites

Before using this project, ensure that you have the following prerequisites installed:
- CMake
- SQLite

## Configuration

- To configure the project, edit the `.ini` file with your desired `port`, `p1`, and `p2` values.

## RESULT

![save_data](image/save_data.png)
![save_data](image/sorting.png)




