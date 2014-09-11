zpserver
========
This solution is a sub-branch of Zoom-Pipeline Server , a Qt based tcp-ip c/s solution.:
  * Multithread listening-accept ability on different addresses.
  * Support both SSL and Plain TCP , clients using Plain or SSL can directly exchange data.
  * Provides a thread pool for TCP/IP data transfer. Clients' socket will run in thread pool, instead of main thread.
  * Provides a pipeline-style task engine, designed for heavily data process.
  * Provides a database resource class, enable mutithread db Access.
  * Provides a Server-to-Server Cluster system, enable cross server communication.
  * For research only.
