# TT_MQTT_subscriber

MQTT subscriber program written in C for openwrt-system.

This program subscribes to MQTT broker topics, whose payload gets stored inside a local database.
Its possible to add event's which track input value types, and sizes. On chosen conditions it can send emails.

Program uses uci for configuration. 
https://openwrt.org/docs/guide-user/base-system/uci

In mqtt_sub.config you can specify:

1. enable:            1 - autostarts your program, 0 - turns it off
2. username:          username for MQTT broker connection
3. password:          password for MQTT broker connection
4. tls:               1 - enables TLS, 0 - disables TLS. TLS files are searched in default location
5. port:              specify port number for MQTT broker connection
6. ip_address:        specify ip adress for MQTT broker connection
7. user_email:        SMTP enabled email to send data FROM
8. email_pass: 		  SMTP enabled email password to send data FROM
9. destination_email: email where data is sent to

You can also add subsribed topics here, few example's are already pre-written.

Topics only need one option.

Events require multiple options:

1. topic -          typical topic format e.g. '/topic1'
2. parameter_name - name to check JSON string
3. parameter_type - either a 'string' or 'int' literal. All other options will fail
4. condition -      numerical value where:
	a) 1 - input int higher than expected value
	b) 2 - input int lower than expected value
	c) 3 - input int/string equal to expected value     (works for string and int)
	d) 4 - input int/string not equal to expected value (works for string and int)
	e) 5 - input int is lower or equal to expected value
	f) 6 - input int is higher or equal to expected value
5. expectedValue -  a string/int that gets compared to message value


!!!Program only sends emails if all required conditions are set!!!:

1. All email data is not empty
2. Input data is a valid JSON format
3. Chosen topic has a matching event
4. Matching event meets required conditions 
