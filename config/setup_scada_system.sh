#!/bin/bash

# SCADA Core Service Setup Script
# This script sets up the complete SCADA system with InfluxDB integration

set -e  # Exit on any error

echo "=== SCADA Core Service Setup ==="
echo "Setting up data acquisition system with InfluxDB integration"
echo

# Configuration
TELEGRAF_SOCKET="/tmp/telegraf.sock"
TELEGRAF_CONFIG="./telegraf.conf"
SCADA_EXECUTABLE="./scada_service_test"
INFLUXDB_URL="http://localhost:8086"
INFLUXDB_TOKEN="your-token-here"
INFLUXDB_ORG="scada_org"
INFLUXDB_BUCKET="scada_data"

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Function to print colored output
print_status() {
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

# Function to check if a command exists
command_exists() {
    command -v "$1" >/dev/null 2>&1
}

# Function to check if a service is running
service_running() {
    pgrep -f "$1" >/dev/null 2>&1
}

# Check prerequisites
print_status "Checking prerequisites..."

# Check if Qt is available
if ! command_exists qmake; then
    print_error "Qt development tools not found. Please install Qt 6.6.3 or later."
    exit 1
fi

# Check if Telegraf is available
if ! command_exists telegraf; then
    print_warning "Telegraf not found. Please install Telegraf for InfluxDB integration."
    print_status "You can install Telegraf from: https://docs.influxdata.com/telegraf/v1.25/install/"
else
    print_success "Telegraf found: $(telegraf --version | head -n1)"
fi

# Check if InfluxDB is available
if ! command_exists influx; then
    print_warning "InfluxDB CLI not found. Please install InfluxDB 2.x for data storage."
    print_status "You can install InfluxDB from: https://docs.influxdata.com/influxdb/v2.6/install/"
else
    print_success "InfluxDB CLI found: $(influx version | head -n1)"
fi

echo

# Build SCADA service if not exists
if [ ! -f "$SCADA_EXECUTABLE" ]; then
    print_status "Building SCADA Core Service..."
    
    if [ ! -f "scada_service_test.pro" ]; then
        print_error "Project file scada_service_test.pro not found!"
        exit 1
    fi
    
    # Clean previous build
    if [ -f "Makefile" ]; then
        make clean >/dev/null 2>&1 || true
    fi
    
    # Build project
    qmake scada_service_test.pro
    if ! make; then
        print_error "Failed to build SCADA service"
        exit 1
    fi
    
    print_success "SCADA service built successfully"
else
    print_success "SCADA service executable found"
fi

echo

# Setup Telegraf socket directory
print_status "Setting up Telegraf socket..."

# Create socket directory if it doesn't exist
SOCKET_DIR=$(dirname "$TELEGRAF_SOCKET")
if [ ! -d "$SOCKET_DIR" ]; then
    sudo mkdir -p "$SOCKET_DIR"
    print_success "Created socket directory: $SOCKET_DIR"
fi

# Set permissions
sudo chmod 777 "$SOCKET_DIR"
print_success "Socket directory permissions set"

# Remove existing socket if it exists
if [ -S "$TELEGRAF_SOCKET" ]; then
    sudo rm -f "$TELEGRAF_SOCKET"
    print_status "Removed existing socket file"
fi

echo

# Check Telegraf configuration
if [ -f "$TELEGRAF_CONFIG" ]; then
    print_success "Telegraf configuration found: $TELEGRAF_CONFIG"
    
    # Validate configuration
    if command_exists telegraf; then
        if telegraf --config "$TELEGRAF_CONFIG" --test >/dev/null 2>&1; then
            print_success "Telegraf configuration is valid"
        else
            print_warning "Telegraf configuration validation failed"
            print_status "Please check the configuration file: $TELEGRAF_CONFIG"
        fi
    fi
else
    print_warning "Telegraf configuration not found: $TELEGRAF_CONFIG"
    print_status "Using default configuration. Please customize for your environment."
fi

echo

# Function to start Telegraf
start_telegraf() {
    if service_running "telegraf.*$TELEGRAF_CONFIG"; then
        print_status "Telegraf is already running"
        return 0
    fi
    
    if [ -f "$TELEGRAF_CONFIG" ] && command_exists telegraf; then
        print_status "Starting Telegraf..."
        telegraf --config "$TELEGRAF_CONFIG" &
        TELEGRAF_PID=$!
        
        # Wait a moment for Telegraf to start
        sleep 2
        
        if kill -0 $TELEGRAF_PID 2>/dev/null; then
            print_success "Telegraf started successfully (PID: $TELEGRAF_PID)"
            echo $TELEGRAF_PID > telegraf.pid
            return 0
        else
            print_error "Failed to start Telegraf"
            return 1
        fi
    else
        print_warning "Telegraf not available or configuration missing"
        return 1
    fi
}

# Function to stop Telegraf
stop_telegraf() {
    if [ -f "telegraf.pid" ]; then
        TELEGRAF_PID=$(cat telegraf.pid)
        if kill -0 $TELEGRAF_PID 2>/dev/null; then
            print_status "Stopping Telegraf (PID: $TELEGRAF_PID)..."
            kill $TELEGRAF_PID
            rm -f telegraf.pid
            print_success "Telegraf stopped"
        fi
    fi
    
    # Kill any remaining telegraf processes
    pkill -f "telegraf.*$TELEGRAF_CONFIG" 2>/dev/null || true
}

# Function to start SCADA service
start_scada_service() {
    if [ ! -f "$SCADA_EXECUTABLE" ]; then
        print_error "SCADA service executable not found: $SCADA_EXECUTABLE"
        return 1
    fi
    
    print_status "Starting SCADA Core Service..."
    print_status "Data acquisition will begin shortly..."
    print_status "Press Ctrl+C to stop the service"
    echo
    
    # Run SCADA service
    "$SCADA_EXECUTABLE"
}

# Function to cleanup on exit
cleanup() {
    echo
    print_status "Cleaning up..."
    stop_telegraf
    print_success "Cleanup completed"
}

# Set trap for cleanup
trap cleanup EXIT INT TERM

# Main execution
case "${1:-start}" in
    "start")
        print_status "Starting SCADA system..."
        echo
        
        # Start Telegraf if available
        start_telegraf
        
        # Wait a moment for socket to be created
        sleep 1
        
        # Check if socket was created
        if [ -S "$TELEGRAF_SOCKET" ]; then
            print_success "Telegraf socket created: $TELEGRAF_SOCKET"
        else
            print_warning "Telegraf socket not found. Data will not be sent to InfluxDB."
            print_status "SCADA service will still perform data acquisition and logging."
        fi
        
        echo
        
        # Start SCADA service
        start_scada_service
        ;;
        
    "stop")
        print_status "Stopping SCADA system..."
        stop_telegraf
        pkill -f "$SCADA_EXECUTABLE" 2>/dev/null || true
        print_success "SCADA system stopped"
        ;;
        
    "status")
        print_status "SCADA System Status:"
        echo
        
        # Check SCADA service
        if service_running "$SCADA_EXECUTABLE"; then
            print_success "SCADA service is running"
        else
            print_warning "SCADA service is not running"
        fi
        
        # Check Telegraf
        if service_running "telegraf.*$TELEGRAF_CONFIG"; then
            print_success "Telegraf is running"
        else
            print_warning "Telegraf is not running"
        fi
        
        # Check socket
        if [ -S "$TELEGRAF_SOCKET" ]; then
            print_success "Telegraf socket exists: $TELEGRAF_SOCKET"
        else
            print_warning "Telegraf socket not found: $TELEGRAF_SOCKET"
        fi
        
        # Check InfluxDB connectivity
        if command_exists curl; then
            if curl -s "$INFLUXDB_URL/ping" >/dev/null 2>&1; then
                print_success "InfluxDB is accessible at $INFLUXDB_URL"
            else
                print_warning "InfluxDB is not accessible at $INFLUXDB_URL"
            fi
        fi
        ;;
        
    "build")
        print_status "Building SCADA service..."
        
        # Clean and rebuild
        if [ -f "Makefile" ]; then
            make clean
        fi
        
        qmake scada_service_test.pro
        make
        
        print_success "Build completed"
        ;;
        
    "test")
        print_status "Running SCADA service test (10 seconds)..."
        
        # Start Telegraf
        start_telegraf
        sleep 1
        
        # Run SCADA service for 10 seconds
        timeout 10s "$SCADA_EXECUTABLE" || true
        
        print_success "Test completed"
        ;;
        
    "help")
        echo "SCADA Core Service Setup Script"
        echo
        echo "Usage: $0 [command]"
        echo
        echo "Commands:"
        echo "  start   - Start the complete SCADA system (default)"
        echo "  stop    - Stop all SCADA services"
        echo "  status  - Show system status"
        echo "  build   - Build the SCADA service"
        echo "  test    - Run a 10-second test"
        echo "  help    - Show this help message"
        echo
        echo "Configuration:"
        echo "  Telegraf socket: $TELEGRAF_SOCKET"
        echo "  Telegraf config: $TELEGRAF_CONFIG"
        echo "  SCADA executable: $SCADA_EXECUTABLE"
        echo "  InfluxDB URL: $INFLUXDB_URL"
        echo
        ;;
        
    *)
        print_error "Unknown command: $1"
        print_status "Use '$0 help' for usage information"
        exit 1
        ;;
esac