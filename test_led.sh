#!/bin/bash

echo "=== ESP32 LED Test Upload Script ==="
echo ""

# Check if a port was provided as argument
if [ $# -eq 0 ]; then
    echo "Please provide the port as an argument."
    echo "Usage: $0 /dev/cu.usbserial-XXXXXXXX"
    echo ""
    echo "Available ports:"
    ls /dev/cu.usbserial-* 2>/dev/null || echo "No USB serial ports found"
    ls /dev/cu.usbmodem* 2>/dev/null || echo "No USB modem ports found"
    exit 1
fi

PORT=$1
echo "Uploading LED test to port: $PORT"
echo ""

# Create a temporary platformio.ini for this test
cp platformio_ledtest.ini platformio_temp.ini
echo "" >> platformio_temp.ini
echo "upload_port = $PORT" >> platformio_temp.ini
echo "monitor_port = $PORT" >> platformio_temp.ini

# Upload the test code
echo "Compiling and uploading LED test..."
platformio run -c platformio_temp.ini -e esp32dev_ledtest --target upload

if [ $? -eq 0 ]; then
    echo ""
    echo "✅ Upload successful!"
    echo ""
    echo "The device should now be flashing green LEDs."
    echo "Check the serial monitor with:"
    echo "platformio device monitor -c platformio_temp.ini -e esp32dev_ledtest"
    echo ""
    echo "Or press Ctrl+C and then run the monitor command above."
    echo ""
    read -p "Start serial monitor now? (y/n): " -n 1 -r
    echo
    if [[ $REPLY =~ ^[Yy]$ ]]; then
        platformio device monitor -c platformio_temp.ini -e esp32dev_ledtest
    fi
else
    echo ""
    echo "❌ Upload failed!"
    echo "Check the connection and try again."
fi

# Clean up
rm platformio_temp.ini

echo ""
echo "=== LED Test Complete ==="
