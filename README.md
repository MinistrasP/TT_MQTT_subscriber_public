# TT_MQTT_subscriber_public

MQTT subscriber program written in C for openwrt-based OS

Program subscribes to chosen topics with the ability to add events, which can send 
emails if chosen parameters meet certain conditions.

Program uses uci for configuration. In mqtt_sub.config you can specify:

1. enable: 1 - autostarts your program, 0 - turns it off
2. username: username for MQTT broker connection
3. password: password for MQTT broker connection
4. tls: 1 - enables TLS, 0 - disables TLS. TLS files are searched in default location
5. port: specify port number for MQTT broker connection
6. ip_address: specify ip adress for MQTT broker connection
7. user_email: SMTP enabled email to send data FROM
8. email_pass: SMTP enabled email password to send data FROM
9. destination_email: email where data is sent to

You can also add subsribed topics here, example is already pre written.
Topics only need one option

Events require multiple options:
1. topic - typical topic format e.g. '/topic1'
2. parameter_name - name to check JSON string
3. parameter_type - either a 'string' or 'int' literal. All other options will fail
4. condition - numerical value where:
	a) 1 -
	b) 2 - 
	c) 3 -
	d) 4 - 
	e) 5 -
	f) 6 - 
5. expectedValue - a string/int that gets compared to message value


!!!Program only sends emails if all required conditions are set!!!:

1. All email data is not empty
2. Input data is a valid JSON format
3. Chosen topic has a matching event
4. Matching event meets required conditions 
