
import ipaddress
import math

# Main network assigned to the college
NETWORK = ipaddress.ip_network("192.168.10.0/24")

# Number of required host IP addresses for each department
departments = {
    "Computer Science": 100,
    "Electronics": 50,
    "Administration": 25,
    "Library": 12,
    "Research Lab": 10
}


def required_prefix(hosts):
    """Find the smallest subnet that supports the required hosts."""
    prefix = 32

    while (2 ** (32 - prefix) - 2) < hosts:
        prefix -= 1

    return prefix


def allocate_subnets(network, departments):
    """Allocate subnets using VLSM, starting with the largest department."""
    allocations = []
    current_ip = int(network.network_address)
    last_ip = int(network.broadcast_address)

    # Largest departments receive addresses first
    sorted_departments = sorted(
        departments.items(),
        key=lambda item: item[1],
        reverse=True
    )

    for name, hosts in sorted_departments:
        prefix = required_prefix(hosts)
        block_size = 2 ** (32 - prefix)

        # Move to the next correctly aligned subnet boundary
        start_ip = (
            (current_ip + block_size - 1) // block_size
        ) * block_size

        end_ip = start_ip + block_size - 1

        # Check that the subnet fits inside the main network
        if end_ip > last_ip:
            raise ValueError(
                f"Not enough IP addresses for {name}"
            )

        subnet = ipaddress.ip_network(
            (start_ip, prefix)
        )

        allocations.append({
            "department": name,
            "required_hosts": hosts,
            "network": subnet,
            "subnet_mask": subnet.netmask,
            "first_host": subnet.network_address + 1,
            "last_host": subnet.broadcast_address - 1,
            "broadcast": subnet.broadcast_address,
            "total_ips": subnet.num_addresses,
            "usable_hosts": subnet.num_addresses - 2
        })

        # Start looking after the allocated subnet
        current_ip = end_ip + 1

    return allocations


def validate_allocations(network, allocations):
    """Check containment, host capacity, and overlapping subnets."""
    valid = True

    for i, item in enumerate(allocations):
        subnet = item["network"]

        if not subnet.subnet_of(network):
            print(f"ERROR: {subnet} is outside {network}")
            valid = False

        if item["usable_hosts"] < item["required_hosts"]:
            print(f"ERROR: Not enough hosts for {item['department']}")
            valid = False

        for other in allocations[i + 1:]:
            if subnet.overlaps(other["network"]):
                print(
                    f"ERROR: {subnet} overlaps "
                    f"{other['network']}"
                )
                valid = False

    if valid:
        print("Validation successful: all subnets are valid.")
        print("No overlapping subnets found.")

    return valid


def generate_report(allocations):
    """Print the network configuration report."""
    print("\nCOLLEGE NETWORK CONFIGURATION REPORT")
    print("=" * 110)

    print(
        f"{'Department':<20}"
        f"{'Network/CIDR':<22}"
        f"{'Subnet Mask':<18}"
        f"{'Usable IP Range':<32}"
        f"{'Broadcast':<18}"
        f"{'Hosts':<8}"
    )

    print("-" * 110)

    for item in allocations:
        print(
            f"{item['department']:<20}"
            f"{str(item['network']):<22}"
            f"{str(item['subnet_mask']):<18}"
            f"{str(item['first_host']) + ' - ' + str(item['last_host']):<32}"
            f"{str(item['broadcast']):<18}"
            f"{item['usable_hosts']:<8}"
        )

    allocated = sum(item["total_ips"] for item in allocations)

    print("-" * 110)
    print(f"Main network: {NETWORK}")
    print(f"Total IP addresses available: {NETWORK.num_addresses}")
    print(f"Total IP addresses allocated: {allocated}")
    print(f"Total IP addresses remaining: {NETWORK.num_addresses - allocated}")


# Main program
try:
    subnets = allocate_subnets(NETWORK, departments)

    if validate_allocations(NETWORK, subnets):
        generate_report(subnets)

except ValueError as error:
    print("Allocation failed:", error)

