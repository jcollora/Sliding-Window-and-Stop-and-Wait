#include "udphw3.h"

// Timeout time
#define TIMEOUT 1500

// Client waits to recieve ackn. after every send or timeout
// upon which, it backs up the ptr, counter++, and resends the message and repeats
int hw::clientStopWait(UdpSocket &sock, const int max, int message[]) {
    cerr << "inside case 2 client" << endl;
    int retransmits = 0;
    
    // Full # of sends, defined in calling func 
    for(int i = 0; i < max; i++) {
        // store sequence num for sending message
        message[0] = i;
        // send message
        sock.sendTo((char *) message, MSGSIZE);
        cerr << "msg sent" << endl;
        
        // repeat until timeout, start tracking
        Timer time;
        time.start();
        bool timedout = false;

        // wait for response until timeout
        while(1) {
            // is a response available?
            int status = sock.pollRecvFrom();
            if(status > 0) 
                break;
            // if timout has occured, count it, and back up i to resend
            if(time.lap() > TIMEOUT && !timedout)
                timedout = true; retransmits++; i--; break;
        }
        // retransmit already accounted for, skip 
        if(timedout)
            continue;
        
        sock.recvFrom((char *) message, MSGSIZE);
        cerr << "msg recv: " << message[0] << endl;
        // check sequence number
        if(message[0] != i) 
            retransmits++; i--; continue; cerr << "success ack" << endl;
    }
    return retransmits;
}

// Stop n' wait's server counterpart
// simply ackn. when sequence number is desired
void hw::serverReliable(UdpSocket &sock, const int max, int message[]) {
    cerr << "inside case 2 server" << endl;
    // Check full # of sends
    for(int i = 0; i < max; i++) {
        // wait to recieve a message
        while(1) {
            if(sock.pollRecvFrom() > 0) {
                sock.recvFrom((char *) message, MSGSIZE);
                cerr << "msg recv: " << message[0] << endl;
                // if sequences match, send ackn. (same as seq. #)
                if(message[0] == i)
                    sock.ackTo((char *) &i, sizeof(i)); break; cerr << "ack sent" << endl;
            }
        }
    }
}

// slide is like a scope on message data within range sequence #s used
// to define the scope of the slide moved along message
// slide also exists for the ack waiting, defined by the lastack recieved
int hw::clientSlidingWindow(UdpSocket &sock, const int max, int message[], 
			  int windowSize) {
                  
    cerr << "inside test 3 client" << endl;
    int retransmits = 0;
    int unacked = 0;
    int lastack = 0;

    // Full # of sends, defined in calling func
    for(int i = 0; i < max; i++) {
        // send windowsize # of packets
        if (unacked < windowSize) {
            // store sequence num for sending message
            message[0] = i;
            // send message
            sock.sendTo((char *) message, MSGSIZE);
            cerr << "msg sent" << endl;
            unacked++;
        }

        // Determines resend
        // once windowsize full
        if (unacked == windowSize) {
            // Wait for ackn. start timers for timeout
            Timer time;
            time.start();

            // wait for all responses until timeout
            // move ptr back and resend
            while(1) {
                if(sock.pollRecvFrom() > 0) {
                    sock.recvFrom((char *) message, MSGSIZE);
                    cerr << "msg recv: " << message[0] << endl;
                    if(message[0] < 0)
                        i = max; break;
                    if(message[0] == lastack)
                        unacked--; lastack++; break; cerr << "successful ack" << endl;
                }
                if(time.lap() > TIMEOUT && unacked == windowSize) {
                    cerr << "resending" << endl;
                    retransmits = retransmits + (i + windowSize - lastack);
                    unacked = 0; 
                    i = lastack; 
                    break;
                }
            }
        }
    } 

    return retransmits;
}

// Sliding window;s server counterpart
void hw::serverEarlyRetrans( UdpSocket &sock, const int max, int message[], 
			 int windowSize ) {
    // loops if data is in right order
    // no need for array when returning 
    for(int i = 0; i < max; i++) {
        // ackn. loop
        while(1) {
            if(sock.pollRecvFrom() > 0) {
                sock.recvFrom((char *) message, MSGSIZE);
                cerr << "msg recv: " << message[0] << endl;
                sock.ackTo((char *) &i, sizeof(i));
                cerr << "ack sent" << endl;
                // ackn. advances when sequences are in correct order
                if (message[0] == i) break;
            }
        }
    }
}   