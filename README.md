# Implementation of TCP-Jersey in ns-3

## Course Code: CS821

## Assignment: #FP7

### Overview

TCP-Jersey [1] is a variant of TCP, It makes TCP to perform better in wireless and wired-wireless hybrid networks environment by differentiating the packet losses caused by network congestions from the losses caused by wireless link errors. This
repository provides an implementation of TCP-Jersey in ns-3 [2].

### TCP-Jersey Examples

Example programs for TCP-Jersey have been provided in

`examples/tcp/`

and should be executed as

`./waf --run "tcp-variants-comparison --transport_prot=TcpJersey"`

`./waf --run jersey-fairness`

`./waf --run jersey-goodput`

`./waf --run jersey-vs-other-tcps`


### References:

[1] Xu, Kai, Ye Tian, and Nirwan Ansari. "TCP-Jersey for wireless IP communications." IEEE Journal on selected areas in communications 22.4 (2004): 747-756.

[2] https://www.nsnam.org/ns-3-dev/
