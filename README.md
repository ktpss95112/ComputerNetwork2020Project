# Computer Network 2020 Fall Final Project
Author: B07902123 蔡奇夆


### How to run text server

This server uses socket to receive clients' text messages. It allows multiple clients simultaneously.

To run the server, please `cd` to `src/text_server/` and run `make`.

Run `./text_server` afterwards to see how to set command line arguments.

(I use `./text_server 0.0.0.0 9999` in my environment and it works just fine)


### How to run http server

This server uses socket to receive clients' HTTP request. It allows multiple clients simultaneously.

To run the server, please `cd` to `src/http_server/` and run `make`.

Run `./http_server` afterwards to see how to set command line arguments.

(I use `./http_server 0.0.0.0 8888 ../../website` in my environment and it works just fine)
