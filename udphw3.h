#ifndef UDPHW3_H
#define UDPHW3_H

#include "timer.h"
#include "UdpSocket.h"

class hw {
    public:
        int clientStopWait( UdpSocket &sock, const int max, int message[] );
        void serverReliable( UdpSocket &sock, const int max, int message[] );
        int clientSlidingWindow( UdpSocket &sock, const int max, int message[], int windowSize );
        void serverEarlyRetrans( UdpSocket &sock, const int max, int message[], int windowSize );     
};

#endif