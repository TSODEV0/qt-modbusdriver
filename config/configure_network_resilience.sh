#!/bin/bash

# SCADA Network Resilience Configuration Script
# This script helps configure the SCADA system for different network conditions

set -e

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Configuration file path
CONFIG_FILE="config.ini"
BACKUP_FILE="config.ini.backup"

# Function to print colored output
print_info() {
    echo -e "${BLUE}[INFO]${NC} $1"
}

print_success() {
    echo -e "${GREEN}[SUCCESS]${NC} $1"
}

print_warning() {
    echo -e "${YELLOW}[WARNING]${NC} $1"
}

print_error() {
    echo -e "${RED}[ERROR]${NC} $1"
}

# Function to backup existing configuration
backup_config() {
    if [ -f "$CONFIG_FILE" ]; then
        cp "$CONFIG_FILE" "$BACKUP_FILE"
        print_info "Configuration backed up to $BACKUP_FILE"
    fi
}

# Function to restore configuration
restore_config() {
    if [ -f "$BACKUP_FILE" ]; then
        cp "$BACKUP_FILE" "$CONFIG_FILE"
        print_success "Configuration restored from backup"
    else
        print_error "No backup file found"
        exit 1
    fi
}

# Function to update configuration section
update_config_section() {
    local section="$1"
    local key="$2"
    local value="$3"
    
    # Create section if it doesn't exist
    if ! grep -q "\[$section\]" "$CONFIG_FILE" 2>/dev/null; then
        echo "" >> "$CONFIG_FILE"
        echo "[$section]" >> "$CONFIG_FILE"
    fi
    
    # Update or add the key-value pair
    if grep -q "^$key=" "$CONFIG_FILE"; then
        sed -i "s/^$key=.*/$key=$value/" "$CONFIG_FILE"
    else
        # Add after the section header
        sed -i "/\[$section\]/a $key=$value" "$CONFIG_FILE"
    fi
}

# Function to configure for excellent network (LAN/Ethernet)
configure_excellent() {
    print_info "Configuring for excellent network conditions (LAN/Ethernet)..."
    
    update_config_section "ConnectionResilience" "network_type" "ethernet"
    update_config_section "ConnectionResilience" "connection_timeout" "5000"
    update_config_section "ConnectionResilience" "request_timeout" "3000"
    update_config_section "ConnectionResilience" "max_retries" "3"
    update_config_section "ConnectionResilience" "retry_delay" "1000"
    update_config_section "ConnectionResilience" "heartbeat_interval" "60000"
    update_config_section "ConnectionResilience" "auto_adjust" "true"
    
    # Update system timeouts
    update_config_section "System" "timeout" "5000"
    
    print_success "Excellent network configuration applied"
    print_info "Settings: 5s connection timeout, 3s request timeout, 3 retries"
}

# Function to configure for good network (WiFi)
configure_good() {
    print_info "Configuring for good network conditions (WiFi)..."
    
    update_config_section "ConnectionResilience" "network_type" "wifi"
    update_config_section "ConnectionResilience" "connection_timeout" "8000"
    update_config_section "ConnectionResilience" "request_timeout" "5000"
    update_config_section "ConnectionResilience" "max_retries" "5"
    update_config_section "ConnectionResilience" "retry_delay" "1500"
    update_config_section "ConnectionResilience" "heartbeat_interval" "45000"
    update_config_section "ConnectionResilience" "auto_adjust" "true"
    
    # Update system timeouts
    update_config_section "System" "timeout" "8000"
    
    print_success "Good network configuration applied"
    print_info "Settings: 8s connection timeout, 5s request timeout, 5 retries"
}

# Function to configure for fair network (4G Cellular)
configure_fair() {
    print_info "Configuring for fair network conditions (4G Cellular)..."
    
    update_config_section "ConnectionResilience" "network_type" "cellular_4g"
    update_config_section "ConnectionResilience" "connection_timeout" "15000"
    update_config_section "ConnectionResilience" "request_timeout" "12000"
    update_config_section "ConnectionResilience" "max_retries" "8"
    update_config_section "ConnectionResilience" "retry_delay" "3000"
    update_config_section "ConnectionResilience" "heartbeat_interval" "30000"
    update_config_section "ConnectionResilience" "auto_adjust" "true"
    
    # Update system timeouts
    update_config_section "System" "timeout" "15000"
    
    print_success "Fair network configuration applied"
    print_info "Settings: 15s connection timeout, 12s request timeout, 8 retries"
}

# Function to configure for poor network (3G/Satellite)
configure_poor() {
    print_info "Configuring for poor network conditions (3G/Satellite)..."
    
    update_config_section "ConnectionResilience" "network_type" "cellular_3g"
    update_config_section "ConnectionResilience" "connection_timeout" "25000"
    update_config_section "ConnectionResilience" "request_timeout" "20000"
    update_config_section "ConnectionResilience" "max_retries" "15"
    update_config_section "ConnectionResilience" "retry_delay" "5000"
    update_config_section "ConnectionResilience" "heartbeat_interval" "20000"
    update_config_section "ConnectionResilience" "auto_adjust" "true"
    
    # Update system timeouts
    update_config_section "System" "timeout" "25000"
    
    print_success "Poor network configuration applied"
    print_info "Settings: 25s connection timeout, 20s request timeout, 15 retries"
}

# Function to configure custom settings
configure_custom() {
    print_info "Configuring custom network settings..."
    
    echo "Enter custom configuration values:"
    
    read -p "Network type (ethernet/wifi/cellular_4g/cellular_3g/satellite): " network_type
    read -p "Connection timeout (ms) [default: 15000]: " connection_timeout
    read -p "Request timeout (ms) [default: 12000]: " request_timeout
    read -p "Max retries [default: 8]: " max_retries
    read -p "Retry delay (ms) [default: 3000]: " retry_delay
    read -p "Heartbeat interval (ms) [default: 30000]: " heartbeat_interval
    
    # Set defaults if empty
    connection_timeout=${connection_timeout:-15000}
    request_timeout=${request_timeout:-12000}
    max_retries=${max_retries:-8}
    retry_delay=${retry_delay:-3000}
    heartbeat_interval=${heartbeat_interval:-30000}
    
    update_config_section "ConnectionResilience" "network_type" "$network_type"
    update_config_section "ConnectionResilience" "connection_timeout" "$connection_timeout"
    update_config_section "ConnectionResilience" "request_timeout" "$request_timeout"
    update_config_section "ConnectionResilience" "max_retries" "$max_retries"
    update_config_section "ConnectionResilience" "retry_delay" "$retry_delay"
    update_config_section "ConnectionResilience" "heartbeat_interval" "$heartbeat_interval"
    update_config_section "ConnectionResilience" "auto_adjust" "true"
    
    # Update system timeout
    update_config_section "System" "timeout" "$connection_timeout"
    
    print_success "Custom network configuration applied"
    print_info "Settings: ${connection_timeout}ms connection timeout, ${request_timeout}ms request timeout, $max_retries retries"
}

# Function to show current configuration
show_config() {
    print_info "Current network resilience configuration:"
    echo ""
    
    if [ -f "$CONFIG_FILE" ]; then
        if grep -q "\[ConnectionResilience\]" "$CONFIG_FILE"; then
            echo "[ConnectionResilience]"
            sed -n '/\[ConnectionResilience\]/,/^\[/p' "$CONFIG_FILE" | grep -v '^\[' | grep -v '^$' || echo "No settings found"
        else
            print_warning "No ConnectionResilience section found in configuration"
        fi
        echo ""
        
        if grep -q "\[System\]" "$CONFIG_FILE"; then
            echo "[System]"
            sed -n '/\[System\]/,/^\[/p' "$CONFIG_FILE" | grep -v '^\[' | grep -v '^$' || echo "No settings found"
        else
            print_warning "No System section found in configuration"
        fi
    else
        print_error "Configuration file not found: $CONFIG_FILE"
    fi
}

# Function to test network connectivity
test_connectivity() {
    print_info "Testing network connectivity to Modbus server..."
    
    # Get server address from config
    if [ -f "$CONFIG_FILE" ]; then
        server_ip=$(grep "^server_ip=" "$CONFIG_FILE" | cut -d'=' -f2 || echo "10.72.2.215")
        server_port=$(grep "^server_port=" "$CONFIG_FILE" | cut -d'=' -f2 || echo "502")
    else
        server_ip="10.72.2.215"
        server_port="502"
    fi
    
    print_info "Testing connection to $server_ip:$server_port"
    
    # Test ping
    if ping -c 3 -W 5 "$server_ip" > /dev/null 2>&1; then
        print_success "Ping test successful"
        
        # Measure response time
        response_time=$(ping -c 3 -W 5 "$server_ip" | tail -1 | awk -F'/' '{print $5}' | cut -d' ' -f1)
        print_info "Average response time: ${response_time}ms"
        
        # Recommend configuration based on response time
        if (( $(echo "$response_time < 50" | bc -l) )); then
            print_info "Recommendation: Use 'excellent' configuration"
        elif (( $(echo "$response_time < 200" | bc -l) )); then
            print_info "Recommendation: Use 'good' configuration"
        elif (( $(echo "$response_time < 1000" | bc -l) )); then
            print_info "Recommendation: Use 'fair' configuration"
        else
            print_info "Recommendation: Use 'poor' configuration"
        fi
    else
        print_error "Ping test failed - server may be unreachable"
    fi
    
    # Test TCP connection
    if timeout 10 bash -c "</dev/tcp/$server_ip/$server_port" 2>/dev/null; then
        print_success "TCP connection test successful"
    else
        print_error "TCP connection test failed - Modbus service may be down"
    fi
}

# Function to show help
show_help() {
    echo "SCADA Network Resilience Configuration Script"
    echo ""
    echo "Usage: $0 [OPTION]"
    echo ""
    echo "Options:"
    echo "  excellent    Configure for excellent network (LAN/Ethernet)"
    echo "  good         Configure for good network (WiFi)"
    echo "  fair         Configure for fair network (4G Cellular)"
    echo "  poor         Configure for poor network (3G/Satellite)"
    echo "  custom       Configure with custom settings"
    echo "  show         Show current configuration"
    echo "  test         Test network connectivity"
    echo "  backup       Backup current configuration"
    echo "  restore      Restore configuration from backup"
    echo "  help         Show this help message"
    echo ""
    echo "Network Quality Guidelines:"
    echo "  Excellent: < 50ms latency, stable connection (LAN/Ethernet)"
    echo "  Good:      < 200ms latency, occasional drops (WiFi)"
    echo "  Fair:      < 1000ms latency, regular drops (4G Cellular)"
    echo "  Poor:      > 1000ms latency, frequent drops (3G/Satellite)"
    echo ""
}

# Main script logic
case "${1:-help}" in
    "excellent")
        backup_config
        configure_excellent
        ;;
    "good")
        backup_config
        configure_good
        ;;
    "fair")
        backup_config
        configure_fair
        ;;
    "poor")
        backup_config
        configure_poor
        ;;
    "custom")
        backup_config
        configure_custom
        ;;
    "show")
        show_config
        ;;
    "test")
        test_connectivity
        ;;
    "backup")
        backup_config
        print_success "Configuration backed up"
        ;;
    "restore")
        restore_config
        ;;
    "help")
        show_help
        ;;
    *)
        print_error "Unknown option: $1"
        echo ""
        show_help
        exit 1
        ;;
esac

print_info "Configuration script completed"
print_info "Restart the SCADA service to apply changes"