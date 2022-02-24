#ifndef UDPHW3CASE4_H
#define UDPHW3CASE4_H

#include "Timer.h"
#include "UdpSocket.h"

class hwc4 {
    public:
        int clientStopWait( UdpSocket &sock, const int max, int message[] );
        void serverReliable( UdpSocket &sock, const int max, int message[] );
        int clientSlidingWindow( UdpSocket &sock, const int max, int message[], int windowSize );
        void serverEarlyRetrans( UdpSocket &sock, const int max, int message[], int dropP );     
};

#endif