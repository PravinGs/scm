#!/bin/bash

create_syslog_mechanism() {
    binary_path=$1
    # Check if syslog mechanism already exists
    if grep -q "$binary_path" /etc/syslog.conf; then
        echo "Syslog mechanism already exists for $binary_path"
        return 0
    fi

    # Create a syslog mechanism for the application
    syslog_conf="${binary_path}.conf"
    echo "local0.* ${binary_path}" > "$syslog_conf"
    echo "Syslog mechanism created for $binary_path"
    echo "$syslog_conf"
}

set_syslog_backup_configuration() {
    syslog_conf=$1
    # Set syslog backup configuration
    backup_conf="/etc/syslog.conf"
    echo "*.* @backup_server" >> "$backup_conf"
    echo "Syslog backup configuration set successfully."
}

main() {
    # Check if binary path argument is provided
    if [ $# -eq 0 ]; then
        echo "Usage: $0 <binary_path>"
        exit 1
    fi
    
    binary_path=$1
    
    # Check if binary path exists and is executable
    if [ ! -x "$binary_path" ]; then
        echo "Binary path does not exist or is not executable."
        exit 1
    fi

    # Create syslog mechanism if not already configured
    syslog_conf=$(create_syslog_mechanism "$binary_path")
    if [ -z "$syslog_conf" ]; then
        # Syslog mechanism already configured
        exit 0
    fi

    # Set syslog backup configuration
    set_syslog_backup_configuration "$syslog_conf"
}

main "$@"