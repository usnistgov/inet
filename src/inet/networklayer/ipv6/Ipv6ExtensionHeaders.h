//
// Copyright (C) 2010 Zoltan Bojthe
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this program; if not, see <http://www.gnu.org/licenses/>.
//

#ifndef __INET_IPV6EXTENSIONHEADERS_H
#define __INET_IPV6EXTENSIONHEADERS_H

#include "inet/networklayer/ipv6/Ipv6ExtensionHeaders_m.h"

namespace inet {

class INET_API Ipv6RoutingHeader : public Ipv6RoutingHeader_Base
{
  public:
    Ipv6RoutingHeader() : Ipv6RoutingHeader_Base() {}
    Ipv6RoutingHeader(const Ipv6RoutingHeader& other) : Ipv6RoutingHeader_Base(other) {}
    Ipv6RoutingHeader& operator=(const Ipv6RoutingHeader& other) { Ipv6RoutingHeader_Base::operator=(other); return *this; }
    virtual Ipv6RoutingHeader *dup() const override { return new Ipv6RoutingHeader(*this); }
    // ADD CODE HERE to redefine and implement pure virtual functions from Ipv6RoutingHeader_Base
    virtual void setAddressArraySize(size_t size) override;
};

} // namespace inet

#endif    // __INET_IPV6EXTENSIONHEADERS_H_
