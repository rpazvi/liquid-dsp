/*
 * Copyright (c) 2007, 2009, 2010 Joseph Gaeddert
 * Copyright (c) 2007, 2009, 2010 Virginia Polytechnic Institute & State University
 *
 * This file is part of liquid.
 *
 * liquid is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * liquid is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with liquid.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "autotest/autotest.h"
#include "liquid.h"

// AUTOTEST : test packet length computation
void autotest_packetizer_length()
{
    // coding schemes
    fec_scheme fec0 = FEC_CONV_V27;
    fec_scheme fec1 = FEC_REP3;
    crc_scheme check = CRC_32;
    unsigned int n = 10;  // decoded message length

    // compute encoded message length
    unsigned int k = packetizer_compute_enc_msg_len(n, check, fec0, fec1);

    // estimate decoded message length
    unsigned int m = packetizer_compute_dec_msg_len(k, check, fec0, fec1);

    // ensure n==m
    CONTEND_EQUALITY(n, m);
}

// Help function to keep code base small
void packetizer_test_codec(unsigned int _n,
                           crc_scheme _crc,
                           fec_scheme _fec0,
                           fec_scheme _fec1)
{
    unsigned char msg_tx[_n];
    unsigned char msg_rx[_n];
    unsigned int pkt_len = packetizer_compute_enc_msg_len(_n,_crc,_fec0,_fec1);
    unsigned char packet[pkt_len];

    // create object
    packetizer p = packetizer_create(_n,_crc,_fec0,_fec1);

    if (liquid_autotest_verbose)
        packetizer_print(p);

    // initialize data
    unsigned int i;
    for (i=0; i<_n; i++) {
        msg_tx[i] = i % 256;
        msg_rx[i] = 0;
    }

    bool crc_pass;

    // encode/decode packet
    packetizer_encode(p, msg_tx, packet);
    crc_pass = packetizer_decode(p, packet, msg_rx);

    CONTEND_SAME_DATA(msg_tx, msg_rx, _n);
    CONTEND_EQUALITY(crc_pass, true);

    // clean up objects
    packetizer_destroy(p);
}

//
// AUTOTESTS
//
void autotest_packetizer_n16_0_0()  { packetizer_test_codec(16, CRC_32, FEC_NONE,      FEC_NONE);       }
void autotest_packetizer_n16_0_1()  { packetizer_test_codec(16, CRC_32, FEC_NONE,      FEC_REP3);       }
void autotest_packetizer_n16_0_2()  { packetizer_test_codec(16, CRC_32, FEC_NONE,      FEC_HAMMING74);  }

