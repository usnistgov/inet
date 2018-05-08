//
// Copyright (C) 2004 Andras Varga
// Copyright (C) 2005 Christian Dankbar, Irene Ruengeler, Michael Tuexen
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, see <http://www.gnu.org/licenses/>.
//

// This file is based on the Ppp.cc of INET written by Andras Varga.

#define WANT_WINSOCK2

#include <stdio.h>
#include <string.h>

#include "inet/common/INETDefs.h"

#include <omnetpp/platdep/sockets.h>

#include <net/if.h>

#include "inet/common/INETUtils.h"
#include "inet/common/ModuleAccess.h"
#include "inet/common/ProtocolTag_m.h"
#include "inet/common/packet/Packet.h"
#include "inet/common/packet/chunk/BytesChunk.h"
#include "inet/common/serializer/headers/ethernethdr.h"

#include "inet/linklayer/common/EtherType_m.h"
#include "inet/linklayer/common/Ieee802Ctrl_m.h"
#include "inet/linklayer/common/InterfaceTag_m.h"
#include "inet/linklayer/tap/Tap.h"

#include "inet/networklayer/common/InterfaceEntry.h"
#include "inet/networklayer/common/InterfaceTable.h"
#include "inet/networklayer/common/IpProtocolId_m.h"
#include "inet/networklayer/ipv4/Ipv4Header_m.h"

#include <arpa/inet.h>

namespace inet {

Define_Module(Tap);

bool Tap::notify(int fd)
{
    ASSERT(fd == tapSocket);
    bool found = false;

    ssize_t n = read(tapSocket, buffer, sizeof(buffer));
    if (n < 0)
        throw cRuntimeError("Tap::notify(): An error occured: %d", n);
    if (n > 0) {
        found = true;
        //FIXME Why filtering IPv4 only on eth interface? why not filtering on PPP or other interfaces?
        // skip ethernet frames not encapsulating an IP packet.
        // TODO: how about ipv6 and other protocols?
        //TODO for decapsulate, using code from EtherEncap
        uint16_t etherType = (uint16_t)(buffer[ETHER_ADDR_LEN * 2]) << 8 | buffer[ETHER_ADDR_LEN * 2 + 1];
        //TODO get ethertype from snap header when packet has snap header
        if (etherType == ETHERTYPE_IPv4) { // ipv4
            //TODO use EtherEncap

            // put the IP packet from wire into Packet
            uint32_t pklen = n - ETHER_HDR_LEN;
            Packet *packet = new Packet("rtEvent");
            const auto& bytesChunk = makeShared<BytesChunk>(&buffer[ETHER_HDR_LEN], pklen);
            packet->insertAtBack(bytesChunk);

            // signalize new incoming packet to the interface via cMessage
            EV << "Captured " << pklen << " bytes for an IP packet.\n";
            rtScheduler->scheduleMessage(this, packet);
        }
    }
    return found;
}

Tap::~Tap()
{
    //close tap:
    rtScheduler->removeCallback(tapSocket, this);
    closeTap();
}

void Tap::initialize(int stage)
{
    MacBase::initialize(stage);

    // subscribe at scheduler for external messages
    if (stage == INITSTAGE_LOCAL) {
        numSent = numRcvd = numDropped = 0;

        if (auto scheduler = dynamic_cast<RealTimeScheduler *>(getSimulation()->getScheduler())) {
            rtScheduler = scheduler;
            device = par("device");
            openTap(device);
            connected = true;
        }
        else {
            // this simulation run works without external interface
            connected = false;
        }

        WATCH(numSent);
        WATCH(numRcvd);
        WATCH(numDropped);
    }
    else if (stage == INITSTAGE_LINK_LAYER) {
        registerInterface();
    }
}

void Tap::openTap(const char *device)
{
    if (!device || !*device)
        throw cRuntimeError("device must be specified");

    //FIXME implementation needed
    tapSocket = INVALID_SOCKET;

    EV << "Opened tap device " << device << ".\n";
}

void Tap::closeTap()
{
    //FIXME implementation needed
    tapSocket = INVALID_SOCKET;
}

InterfaceEntry *Tap::createInterfaceEntry()
{
    InterfaceEntry *e = getContainingNicModule(this);

    e->setMtu(par("mtu"));      //TODO get mtu from real interface / or set mtu in real interface
    e->setMulticast(true);      //TODO
    e->setPointToPoint(true);   //TODO

    return e;
}

void Tap::handleMessage(cMessage *msg)
{
    using namespace serializer;
    if (!isOperational) {
        handleMessageWhenDown(msg);
        return;
    }

    Packet *packet = check_and_cast<Packet *>(msg);

    if (msg->isSelfMessage()) {
        // incoming real packet from wire (captured by pcap)
        const auto& nwHeader = packet->peekAtFront<Ipv4Header>();
        EV << "Delivering a packet from "
           << nwHeader->getSourceAddress()
           << " to "
           << nwHeader->getDestinationAddress()
           << " and length of"
           << packet->getByteLength()
           << " bytes to networklayer.\n";
        packet->addTagIfAbsent<InterfaceInd>()->setInterfaceId(interfaceEntry->getInterfaceId());
        packet->addTagIfAbsent<DispatchProtocolReq>()->setProtocol(&Protocol::ipv4);
        packet->addTagIfAbsent<PacketProtocolTag>()->setProtocol(&Protocol::ipv4);
        send(packet, "upperLayerOut");
        numRcvd++;
    }
    else {
        auto protocol = packet->getTag<PacketProtocolTag>()->getProtocol();
        if (protocol != &Protocol::ipv4)
            throw cRuntimeError("ExtInterface accepts ipv4 packets only");

        const auto& ipv4Header = packet->peekAtFront<Ipv4Header>();

        if (connected) {
            struct sockaddr_in addr;
            addr.sin_family = AF_INET;
#if !defined(linux) && !defined(__linux) && !defined(_WIN32)
            addr.sin_len = sizeof(struct sockaddr_in);
#endif // if !defined(linux) && !defined(__linux) && !defined(_WIN32)
            addr.sin_port = htons(0);
            addr.sin_addr.s_addr = htonl(ipv4Header->getDestAddress().getInt());
            auto bytesChunk = packet->peekAllAsBytes();
            size_t packetLength = bytesChunk->copyToBuffer(buffer, sizeof(buffer));
            ASSERT(packetLength == packet->getByteLength());

            EV << "Delivering an IPv4 packet from "
               << ipv4Header->getSrcAddress()
               << " to "
               << ipv4Header->getDestAddress()
               << " and length of "
               << packet->getByteLength()
               << " bytes to link layer.\n";
            sendBytes(buffer, packetLength, (struct sockaddr *)&addr, sizeof(struct sockaddr_in));
            numSent++;
            delete packet;
        }
        else {
            EV << "Interface is not connected, dropping packet " << msg << endl;
            numDropped++;
        }
    }
}

void Tap::sendBytes(uint8 *buf, size_t numBytes, struct sockaddr *to, socklen_t addrlen)
{
    //TODO check: is this an IPv4 packet --OR-- is this packet acceptable by fd socket?
    if (tapSocket == INVALID_SOCKET)
        throw cRuntimeError("no tap socket.");

    int sent = sendto(tapSocket, buf, numBytes, 0, to, addrlen);    //note: no ssize_t on MSVC

    if ((size_t)sent == numBytes)
        EV << "Sent an IP packet with length of " << sent << " bytes.\n";
    else
        EV << "Sending of an IP packet FAILED! (sendto returned " << sent << " (" << strerror(errno) << ") instead of " << numBytes << ").\n";
    return;
}


void Tap::displayBusy()
{
    getDisplayString().setTagArg("i", 1, "yellow");
}

void Tap::displayIdle()
{
    getDisplayString().setTagArg("i", 1, "");
}

void Tap::refreshDisplay() const
{
    if (connected) {
        char buf[80];
        sprintf(buf, "pcap device: %s\nrcv:%d snt:%d", device, numRcvd, numSent);
        getDisplayString().setTagArg("t", 0, buf);
    }
    else {
        getDisplayString().setTagArg("i", 1, "#707070");
        getDisplayString().setTagArg("i", 2, "100");
        getDisplayString().setTagArg("t", 0, "not connected");
    }
}

void Tap::finish()
{
    std::cout << getFullPath() << ": " << numSent << " packets sent, "
              << numRcvd << " packets received, " << numDropped << " packets dropped.\n";

    // close tap:
    rtScheduler->removeCallback(tapSocket, this);
    closeTap();
}

void Tap::flushQueue()
{
    // does not have a queue, do nothing
}

void Tap::clearQueue()
{
    // does not have a queue, do nothing
}

} // namespace inet

