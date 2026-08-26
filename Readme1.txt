Task 3.1P - IFTTT Trigger and Notification

Hardware Used:
- Arduino Nano 33 IoT
- BH1750 Light Sensor
- Breadboard and Jumper Wires

Software/Services Used:
- Arduino IDE
- BH1750 Library
- Wi-Fi
- IFTTT Webhooks
- Email Notifications

Working:
The BH1750 measures the light level in lux. Arduino checks the light level against the defined conditions. When sunlight starts or stops, Arduino triggers an IFTTT Webhook, which sends an email notification.

Testing:
The system was tested by changing the light level on the BH1750 and verifying that both start and stop email notifications were received.