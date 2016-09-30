SSLHandshakeTool
================

Uses the SSL-C's SSL_do_handshake to initiate an SSL handshake with an SSL server.  Doesn't send any data, just performs
handshakes in a loop.  If configured, will terminate the handshake at the specified step in order to test server resiliency.

Usage: <Note: pressing any key during execution will terminate the infinite loop>

SSLHandshake		
	performs infinite handshakes with local server on 5100, pausing 1 sec between each one
SSLHandshake 155.64.153.129 443
	performs infinite handshakes with 155.64.153.129 on 443, pausing 1 sec between each one
SSLHandshake 155.64.153.129 443 5
	performs infinite handshakes with 155.64.153.129 on 443, pausing 5 sec between each one
SSLHandshake 155.64.153.129 443 5 3
	performs infinite handshakes with 155.64.153.129 on 443, pausing 5 sec between each one,
	and failing out of the handshake at step 3