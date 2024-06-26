/*
 * Copyright (c) 2014 Wireless Communications and Networking Group (WCNG),
 * University of Rochester, Rochester, NY, USA.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * Author: Cristiano Tapparello <cristiano.tapparello@rochester.edu>
 */

#include "basic-energy-harvester-helper.h"

#include "ns3/energy-harvester.h"

namespace ns3
{

BasicEnergyHarvesterHelper::BasicEnergyHarvesterHelper()
{
    m_basicEnergyHarvester.SetTypeId("ns3::BasicEnergyHarvester");
}

BasicEnergyHarvesterHelper::~BasicEnergyHarvesterHelper()
{
}

void
BasicEnergyHarvesterHelper::Set(std::string name, const AttributeValue& v)
{
    m_basicEnergyHarvester.Set(name, v);
}

Ptr<energy::EnergyHarvester>
BasicEnergyHarvesterHelper::DoInstall(Ptr<energy::EnergySource> source) const
{
    NS_ASSERT(source);
    Ptr<Node> node = source->GetNode();

    // Create a new Basic Energy Harvester
    Ptr<energy::EnergyHarvester> harvester =
        m_basicEnergyHarvester.Create<energy::EnergyHarvester>();
    NS_ASSERT(harvester);

    // Connect the Basic Energy Harvester to the Energy Source
    source->ConnectEnergyHarvester(harvester);
    harvester->SetNode(node);
    harvester->SetEnergySource(source);
    return harvester;
}

} // namespace ns3
